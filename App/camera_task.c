#include "camera_task.h"
#include "ov2640.h"
//#include "camera_app.h"
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

// 初始化摄像头
uint8_t CameraTask_Init(void)
{
    // 初始化OV2640
    if(OV2640_Init() != 0)
    {
        return 1;  // 初始化失败
    }

    // 配置为RGB565模式
    OV2640_RGB565_Mode();

    // ⚠️ 使用 180x180（测试验证可用的最小分辨率）
    OV2640_OutSize_Set(320, 240);

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

#if CAMERA_TEST_MODE
// 测试模式：生成假图像数据
static uint8_t capture_frame_test(uint8_t* buffer)
{
    static uint8_t pattern = 0;

    // 生成简单的测试图案
    for(int i = 0; i < IMAGE_SIZE; i++)
    {
        buffer[i] = (uint8_t)((i + pattern) % 256);
    }
    pattern++;

    // 模拟采集延时
    osDelay(10);  // FreeRTOS延时

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
            if(HAL_GetTick() - start_tick > 10) {  // 10ms超时（行周期<1ms）
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

            // ⚠️ 优化读取时序：等待PCLK稳定后再读取
            // 等待PCLK上升沿
            timeout = 10000;
            while(!OV2640_PCLK_READ() && timeout > 0) timeout--;
            if(timeout == 0) {
                capture_fail_reason = 4;
                return 1;
            }

            // 短暂延迟让数据稳定（关键！）
            __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
            __NOP(); __NOP(); __NOP(); __NOP(); __NOP();

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

            // 短暂延迟让数据稳定
            __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
            __NOP(); __NOP(); __NOP(); __NOP(); __NOP();

            byte_low = OV2640_DATA_READ();

            // 组合RGB565
            rgb565 = (byte_high << 8) | byte_low;

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
                // 采集一帧到当前缓冲区
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
            vTaskDelayUntil(&pxPreviousWakeTime, pdMS_TO_TICKS(50));    // 工作时50ms (~20 FPS)
        }
    }
}
