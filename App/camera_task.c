#include "camera_task.h"
#include "ov2640.h"
#include "camera_app.h"
#include <string.h>

// 外部调试变量声明
extern uint8_t capture_status;
extern uint8_t capture_success_cnt;
extern uint8_t capture_fail_reason;
extern uint8_t href_state;  // HREF引脚状态

// 图像缓冲区（放在DTCM，不占用Heap）
static uint8_t image_buffer[2][IMAGE_SIZE] __attribute__((section(".dtcmram")));
static uint8_t current_buf = 0;

// FPS计算
static uint32_t frame_count = 0;
static uint32_t last_fps_time = 0;

// 调试：测试模式开关（没有摄像头时可以使用）
#define CAMERA_TEST_MODE  0  // 设置为1启用测试模式，生成假数据

// 图像格式选择
#define USE_JPEG_MODE  0  // 0=RGB565灰度模式(已验证), 1=JPEG模式(当前不工作)

// 初始化摄像头
uint8_t CameraTask_Init(void)
{
    // 初始化OV2640
    if(OV2640_Init() != 0)
    {
        return 1;  // 初始化失败
    }

#if USE_JPEG_MODE
    // 配置为JPEG模式
    OV2640_JPEG_Mode();

    // JPEG模式使用较大分辨率（JPEG压缩后体积小）
    OV2640_OutSize_Set(240, 240);
#else
    // 配置为RGB565模式
    OV2640_RGB565_Mode();

    // ⚠️ 使用 180x180（测试验证可用的最小分辨率）
    OV2640_OutSize_Set(IMAGE_WIDTH, IMAGE_HEIGHT);
#endif

    // ⚠️ 关键修复：调整PCLK分频，平衡速度和稳定性
    // pclkdiv: PCLK分频，值越大速度越慢但越稳定
    // clkdiv: 内部时钟分频
    uint8_t pclkdiv = 8;   // 适中的分频值
    uint8_t clkdiv = 4;    // 适中的分频值
    SCCB_WR_Reg(0xFF, 0x00);
    SCCB_WR_Reg(0xD3, pclkdiv);  // 设置PCLK分频
    SCCB_WR_Reg(0xFF, 0x01);
    SCCB_WR_Reg(0x11, clkdiv);   // 设置CLK分频

    // 设置自动曝光
    OV2640_Auto_Exposure(2);

    return 0;  // 成功
}

// 检查摄像头是否就绪
uint8_t CameraTask_IsReady(void)
{
    // 可以添加更多检查逻辑
    return 1;
}

// RGB565转灰度（内联优化）
static inline uint8_t rgb565_to_gray(uint16_t rgb565)
{
    // ⚠️ 检查RGB565字节序是否正确
    uint8_t r = ((rgb565 >> 11) & 0x1F) * 255 / 31;
    uint8_t g = ((rgb565 >> 5) & 0x3F) * 255 / 63;
    uint8_t b = (rgb565 & 0x1F) * 255 / 31;

    // ITU-R BT.601 标准加权系数
    return (uint8_t)((r * 299 + g * 587 + b * 114) / 1000);
}

#if USE_JPEG_MODE
// JPEG模式：采集JPEG图像
// buffer: 输出缓冲区
// max_size: 缓冲区最大大小
// jpeg_size: 输出JPEG数据大小
static uint8_t capture_frame_jpeg(uint8_t* buffer, uint32_t max_size, uint32_t* jpeg_size)
{
    uint32_t i = 0;
    uint8_t byte, prev_byte = 0;
    uint32_t start_tick;
    uint8_t found_start = 0;
    uint32_t bytes_read = 0;
    const uint32_t MAX_READ_BYTES = 50000;  // ⚠️ 减少到50K，防止卡死太久

    // ⚠️ 使用osKernelGetTickCount()替代HAL_GetTick()，更可靠
    // 等待VSYNC（帧开始）
    start_tick = osKernelGetTickCount();
    while(OV2640_VSYNC_READ()) {
        if(osKernelGetTickCount() - start_tick > 100) {
            capture_fail_reason = 1;
            return 1;
        }
        // ⚠️ 添加短暂延时，让出CPU给其他任务
        osDelay(1);
    }

    start_tick = osKernelGetTickCount();
    while(!OV2640_VSYNC_READ()) {
        if(osKernelGetTickCount() - start_tick > 100) {
            capture_fail_reason = 2;
            return 1;
        }
        osDelay(1);
    }

    // VSYNC检测完成后，延时让摄像头稳定
    osDelay(2);

    // 读取JPEG数据流
    // JPEG格式: 0xFFD8 (start) ... 0xFFD9 (end)
    uint32_t loop_start_tick = osKernelGetTickCount();  // ⚠️ 添加全局超时

    while(bytes_read < MAX_READ_BYTES)
    {
        // ⚠️ 添加全局超时检测（最多5秒）
        if(osKernelGetTickCount() - loop_start_tick > 5000) {
            capture_fail_reason = 8;  // 全局超时
            return 1;
        }

        uint32_t timeout;

        // ⚠️ 缩短PCLK超时到1000次，减少卡住时间
        timeout = 1000;
        while(!OV2640_PCLK_READ() && timeout > 0) timeout--;
        if(timeout == 0) {
            if(found_start && i > 100) {
                // PCLK超时但已读到数据，可能是帧结束
                break;
            }
            capture_fail_reason = 4;
            return 1;
        }

        // 延迟让数据稳定
        __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP(); __NOP();

        byte = OV2640_DATA_READ();
        bytes_read++;

        // ⚠️ 每读取1000字节让出一次CPU
        if(bytes_read % 1000 == 0) {
            taskYIELD();  // 让低优先级任务有机会运行
        }

        // 检测JPEG起始标记 0xFFD8
        if(!found_start) {
            if(prev_byte == 0xFF && byte == 0xD8) {
                found_start = 1;
                // 保存起始标记
                buffer[i++] = 0xFF;
                buffer[i++] = 0xD8;
                prev_byte = 0;  // 重置

                // 等待PCLK下降沿
                timeout = 1000;
                while(OV2640_PCLK_READ() && timeout > 0) timeout--;
                continue;
            }
            prev_byte = byte;
        }
        else {
            // 已经找到起始标记，开始保存数据
            if(i < max_size) {
                buffer[i++] = byte;

                // 检测JPEG结束标记 0xFFD9
                if(prev_byte == 0xFF && byte == 0xD9) {
                    // 找到结束标记，完成
                    *jpeg_size = i;
                    capture_fail_reason = 0;
                    return 0;
                }

                prev_byte = byte;
            } else {
                // 缓冲区满了
                capture_fail_reason = 6;
                return 1;
            }
        }

        // 等待PCLK下降沿
        timeout = 1000;
        while(OV2640_PCLK_READ() && timeout > 0) timeout--;
    }

    // 读取了太多字节仍未找到结束标记
    if(found_start && i >= 100) {
        // 返回已读取的数据（可能不完整）
        *jpeg_size = i;
        capture_fail_reason = 7;  // 警告：未找到结束标记
        return 0;  // 仍然返回部分数据
    }

    capture_fail_reason = 5;  // 未找到JPEG数据
    return 1;
}
#endif

#if CAMERA_TEST_MODE
// 测试模式：生成固定图案，验证数据传输链路
static uint8_t capture_frame_test(uint8_t* buffer)
{
    // ⚠️ 生成简单的渐变图案，便于识别问题
    // 横向渐变：每行灰度值相同，从0到255
    for(int y = 0; y < IMAGE_HEIGHT; y++)
    {
        uint8_t gray = (y * 255) / IMAGE_HEIGHT;  // 从上到下渐变
        for(int x = 0; x < IMAGE_WIDTH; x++)
        {
            buffer[y * IMAGE_WIDTH + x] = gray;
        }
    }

    // 添加一些标记点，验证方向
    // 左上角：白点
    for(int i = 0; i < 10; i++)
        for(int j = 0; j < 10; j++)
            buffer[i * IMAGE_WIDTH + j] = 255;

    // 右上角：黑点
    for(int i = 0; i < 10; i++)
        for(int j = IMAGE_WIDTH - 10; j < IMAGE_WIDTH; j++)
            buffer[i * IMAGE_WIDTH + j] = 0;

    // 模拟采集延时
    osDelay(50);  // FreeRTOS延时

    return 0;  // 成功
}
#endif

// 采集一帧图像
static uint8_t capture_frame(uint8_t* buffer)
{
    uint16_t x, y;
    uint16_t rgb565;
    uint32_t start_tick;

    // 等待VSYNC（帧开始）- 使用时间超时而不是循环计数
    // OV2640@30fps，帧周期=33ms，超时设置为100ms足够
    start_tick = HAL_GetTick();
    while(OV2640_VSYNC_READ()) {
        if(HAL_GetTick() - start_tick > 100) {  // 100ms超时
            capture_fail_reason = 1;  // VSYNC 第一次超时
            return 1;
        }
    }

    start_tick = HAL_GetTick();
    while(!OV2640_VSYNC_READ()) {
        if(HAL_GetTick() - start_tick > 100) {  // 100ms超时
            capture_fail_reason = 2;  // VSYNC 第二次超时
            return 1;
        }
    }

    // ⚠️ 调试：记录VSYNC检测成功后的HREF状态
    href_state = OV2640_HREF_READ();

    // VSYNC检测完成后，稍微延时让摄像头稳定
    osDelay(2);  // FreeRTOS延时2ms，等待第一行数据准备好

    // 逐行读取
    for(y = 0; y < IMAGE_HEIGHT; y++)
    {
        // 等待HREF（行开始）- 改用时间超时
        start_tick = HAL_GetTick();
        while(!OV2640_HREF_READ()) {
            if(HAL_GetTick() - start_tick > 20) {  // 10ms超时（行周期<1ms）
                capture_fail_reason = 3;  // HREF 超时
                return 1;
            }
        }

        // 读取一行像素
        uint32_t timeout;  // 声明 timeout 变量
        for(x = 0; x < IMAGE_WIDTH; x++)
        {
            // 读取RGB565像素（2个字节）
            uint8_t byte_high, byte_low;

            // ⚠️ 改进时序：在PCLK下降沿后读取，确保数据稳定
            // 等待PCLK下降沿
            timeout = 10000;
            while(OV2640_PCLK_READ() && timeout > 0) timeout--;
            if(timeout == 0) {
                capture_fail_reason = 4;
                return 1;
            }

            // 等待PCLK上升沿
            timeout = 10000;
            while(!OV2640_PCLK_READ() && timeout > 0) timeout--;
            if(timeout == 0) {
                capture_fail_reason = 4;
                return 1;
            }

            // ⚠️ 增加延迟：等待数据完全稳定（关键！）
            for(volatile int i = 0; i < 20; i++) __NOP();

            // 读取高字节
            byte_high = OV2640_DATA_READ();

            // 等待PCLK下降沿
            timeout = 10000;
            while(OV2640_PCLK_READ() && timeout > 0) timeout--;

            // 等待PCLK上升沿
            timeout = 10000;
            while(!OV2640_PCLK_READ() && timeout > 0) timeout--;
            if(timeout == 0) {
                capture_fail_reason = 4;
                return 1;
            }

            // ⚠️ 增加延迟：等待数据完全稳定
            for(volatile int i = 0; i < 20; i++) __NOP();

            // 读取低字节
            byte_low = OV2640_DATA_READ();

            // ⚠️ 测试字节序：交换高低字节（用户反馈：字节序与疏密有关）
            rgb565 = (byte_high << 8) | byte_low;  // 交换字节序

            // 转换为灰度并存储
            buffer[y * IMAGE_WIDTH + x] = rgb565_to_gray(rgb565);
        }
        
    }

    capture_fail_reason = 0;  // 成功，无错误
    return 0;  // 成功
}

// 更新FPS
static void update_fps(void)
{
    frame_count++;
    uint32_t current_time = osKernelGetTickCount();
    uint32_t elapsed = current_time - last_fps_time;

    if(elapsed >= 1000)  // 每秒更新一次
    {
        float fps = (float)frame_count * 1000.0f / (float)elapsed;
        Task_UpdateFPS(fps);

        frame_count = 0;
        last_fps_time = current_time;
    }
}

/**
 * @brief  摄像头采集任务
 * @param  argument: 未使用
 * @retval None
 */
void AppTask_cam_capture(void *argument)
{
    TickType_t pxPreviousWakeTime = xTaskGetTickCount();

    // 初始化摄像头
//    if(CameraTask_Init() != 0)
//    {
//        // 初始化失败，挂起任务
//        vTaskSuspend(NULL);
//    }
	//camera_init_flag = CameraTask_Init();
	if(camera_init_flag!=0)
	{
		vTaskSuspend(NULL);
	}
	
    last_fps_time = osKernelGetTickCount();

    // 无限循环
    for(;;)
    {
        WorkMode_t mode = Task_GetMode();
        cnt2++;

        if(mode != MODE_IDLE)
        {
            // 获取互斥量
            if(osMutexAcquire(cameraMutex, 100) == osOK)
            {
#if USE_JPEG_MODE
                // JPEG模式：采集JPEG图像
                uint32_t jpeg_size = 0;
                capture_status = capture_frame_jpeg(image_buffer[current_buf], IMAGE_SIZE, &jpeg_size);

                if(capture_status == 0)
                {
                    capture_success_cnt++;  // 成功计数

                    // 准备图像消息
                    ImageMsg_t img_msg = {
                        .data = image_buffer[current_buf],
                        .width = IMAGE_WIDTH,
                        .height = IMAGE_HEIGHT,
                        .data_size = jpeg_size,  // JPEG实际大小
                        .timestamp = osKernelGetTickCount(),
                        .frame_id = frame_count
                    };

                    // 发送到队列
                    if(osMessageQueuePut(imageQueue, &img_msg, 0, 0) == osOK)
                    {
                        // 通知其他任务
                        osSemaphoreRelease(frameReadySem);

                        // 切换缓冲区
                        current_buf = (current_buf + 1) % 2;

                        // 更新FPS
                        update_fps();
                    }
                }
#else
                // RGB565灰度模式：采集一帧到当前缓冲区
#if CAMERA_TEST_MODE
                capture_status = capture_frame_test(image_buffer[current_buf]);
#else
                capture_status = capture_frame(image_buffer[current_buf]);
#endif
                if(capture_status == 0)
                {
                    capture_success_cnt++;  // 成功计数

                    // 准备图像消息
                    ImageMsg_t img_msg = {
                        .data = image_buffer[current_buf],
                        .width = IMAGE_WIDTH,
                        .height = IMAGE_HEIGHT,
                        .data_size = IMAGE_SIZE,  // 灰度模式固定大小
                        .timestamp = osKernelGetTickCount(),
                        .frame_id = frame_count
                    };

                    // 发送到队列
                    if(osMessageQueuePut(imageQueue, &img_msg, 0, 0) == osOK)
                    {
                        // 通知其他任务
                        osSemaphoreRelease(frameReadySem);

                        // 切换缓冲区
                        current_buf = (current_buf + 1) % 2;

                        // 更新FPS
                        update_fps();
                    }
                }
#endif

                // 释放互斥量
                osMutexRelease(cameraMutex);
            }
        }

        // 根据模式调整延时
        if(mode == MODE_IDLE)
        {
            vTaskDelayUntil(&pxPreviousWakeTime, pdMS_TO_TICKS(1000));  // 空闲时1秒
        }
        else
        {
            vTaskDelayUntil(&pxPreviousWakeTime, pdMS_TO_TICKS(30));    // 工作时50ms (~20 FPS)
        }
    }
}
