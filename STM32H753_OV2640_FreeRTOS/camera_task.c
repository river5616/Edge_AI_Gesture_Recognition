#include "camera_task.h"
#include "ov2640.h"
#include "camera_app.h"
#include <string.h>

// 图像缓冲区（放在DTCM，不占用Heap）
static uint8_t image_buffer[2][IMAGE_SIZE] __attribute__((section(".dtcmram")));
static uint8_t current_buf = 0;

// FPS计算
static uint32_t frame_count = 0;
static uint32_t last_fps_time = 0;

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

    // 设置图像大小为96x96
    OV2640_ImageWin_Set(0, 0, IMAGE_WIDTH * 4, IMAGE_HEIGHT * 4);
    OV2640_OutSize_Set(IMAGE_WIDTH, IMAGE_HEIGHT);

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
    uint8_t r = ((rgb565 >> 11) & 0x1F) * 255 / 31;
    uint8_t g = ((rgb565 >> 5) & 0x3F) * 255 / 63;
    uint8_t b = (rgb565 & 0x1F) * 255 / 31;

    return (uint8_t)((r * 299 + g * 587 + b * 114) / 1000);
}

// 采集一帧图像
static uint8_t capture_frame(uint8_t* buffer)
{
    uint16_t x, y;
    uint16_t rgb565;

    // 等待VSYNC（帧开始）
    uint32_t timeout = 10000;
    while(OV2640_VSYNC_READ() && timeout > 0) timeout--;
    if(timeout == 0) return 1;

    timeout = 10000;
    while(!OV2640_VSYNC_READ() && timeout > 0) timeout--;
    if(timeout == 0) return 1;

    // 逐行读取
    for(y = 0; y < IMAGE_HEIGHT; y++)
    {
        // 等待HREF（行开始）
        timeout = 1000;
        while(!OV2640_HREF_READ() && timeout > 0) timeout--;
        if(timeout == 0) return 1;

        // 读取一行像素
        for(x = 0; x < IMAGE_WIDTH; x++)
        {
            // 读取RGB565像素（2个字节）
            uint8_t byte_high, byte_low;

            // 等待PCLK上升沿
            timeout = 100;
            while(!OV2640_PCLK_READ() && timeout > 0) timeout--;
            byte_high = OV2640_DATA_READ();

            // 等待PCLK下降沿
            timeout = 100;
            while(OV2640_PCLK_READ() && timeout > 0) timeout--;

            // 等待PCLK上升沿
            timeout = 100;
            while(!OV2640_PCLK_READ() && timeout > 0) timeout--;
            byte_low = OV2640_DATA_READ();

            // 组合RGB565
            rgb565 = (byte_high << 8) | byte_low;

            // 转换为灰度并存储
            buffer[y * IMAGE_WIDTH + x] = rgb565_to_gray(rgb565);
        }
    }

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
    // if(CameraTask_Init() != 0)
    // {
    //     // 初始化失败，挂起任务
    //     vTaskSuspend(NULL);
    // }

    last_fps_time = osKernelGetTickCount();

    // 无限循环
    for(;;)
    {
        WorkMode_t mode = Task_GetMode();

        if(mode != MODE_IDLE)
        {
            // 获取互斥量
            if(osMutexAcquire(cameraMutex, 100) == osOK)
            {
                // 采集一帧到当前缓冲区
                if(capture_frame(image_buffer[current_buf]) == 0)
                {
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
