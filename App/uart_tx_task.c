#include "uart_tx_task.h"
#include <stdio.h>
#include <string.h>

// 外部UART句柄
extern UART_HandleTypeDef huart1;

//int fputc(int ch, FILE* f)
//{
//	HAL_UART_Transmit(&huart1,(uint8_t*)&ch,1,50);
//	return ch;
//}

// 发送图像数据
void UartTxTask_SendImage(uint8_t* image_data, uint16_t width, uint16_t height, uint32_t data_size)
{
    char header[128];

    // 获取互斥量
    if(osMutexAcquire(uartMutex, osWaitForever) == osOK)
    {
        // 发送头部（data_size为实际数据大小）
        sprintf(header, "$IMG,%d,%d,%u\r\n", width, height, data_size);
        HAL_UART_Transmit(&huart1, (uint8_t*)header, strlen(header), 1000);

        // 发送图像数据（使用实际大小）
        HAL_UART_Transmit(&huart1, image_data, data_size, 10000);  // 增加超时时间

        // 发送结束标记
        HAL_UART_Transmit(&huart1, (uint8_t*)"$END\r\n", 6, 1000);

        // 释放互斥量
        osMutexRelease(uartMutex);
    }
}

// 发送FPS信息
void UartTxTask_SendFPS(float fps)
{
    char fps_str[32];
    sprintf(fps_str, "$FPS,%.2f\r\n", fps);

    if(osMutexAcquire(uartMutex, 100) == osOK)
    {
        HAL_UART_Transmit(&huart1, (uint8_t*)fps_str, strlen(fps_str), 1000);
        osMutexRelease(uartMutex);
    }
}

// 发送进度信息
void UartTxTask_SendProgress(uint16_t current, uint16_t total)
{
    char progress[64];
    sprintf(progress, "$PROGRESS,%d,%d\r\n", current, total);

    if(osMutexAcquire(uartMutex, 100) == osOK)
    {
        HAL_UART_Transmit(&huart1, (uint8_t*)progress, strlen(progress), 1000);
        osMutexRelease(uartMutex);
    }
}

// 发送推理结果
void UartTxTask_SendResult(uint8_t gesture_id, float confidence)
{
    char result[64];
    sprintf(result, "$RESULT,%d,%.2f\r\n", gesture_id, confidence);

    if(osMutexAcquire(uartMutex, 100) == osOK)
    {
        HAL_UART_Transmit(&huart1, (uint8_t*)result, strlen(result), 1000);
        osMutexRelease(uartMutex);
    }
}

/**
 * @brief  串口发送任务
 * @param  argument: 未使用
 * @retval None
 */
void AppTask_uart_tx(void *argument)
{
    TickType_t pxPreviousWakeTime = xTaskGetTickCount();
    uint32_t last_fps_send = 0;

    // 无限循环
    for(;;)
    {
        WorkMode_t mode = Task_GetMode();

        // 只在采集模式下处理图像
        if(mode == MODE_COLLECTING)
        {
            // 等待帧就绪信号
            if(osSemaphoreAcquire(frameReadySem, 100) == osOK)
            {
                ImageMsg_t img_msg;

                // 从队列获取图像消息
                if(osMessageQueueGet(imageQueue, &img_msg, NULL, 0) == osOK)
                {
                    // 数据采集模式：发送图像
                    UartTxTask_SendImage(img_msg.data, img_msg.width, img_msg.height, img_msg.data_size);

                    // 更新计数
                    g_current_images++;

                    // 发送进度
                    UartTxTask_SendProgress(g_current_images, g_target_images);

                    // 检查是否完成
                    if(g_current_images >= g_target_images)
                    {
                        Task_SetMode(MODE_IDLE);
                        HAL_UART_Transmit(&huart1, (uint8_t*)"$COMPLETE\r\n", 11, 1000);
                    }
                }
            }
        }

        // 每秒发送一次FPS
        uint32_t current_time = osKernelGetTickCount();
        if(current_time - last_fps_send >= 1000)
        {
            float fps = Task_GetFPS();
            if(fps > 0.01f)  // 避免发送0
            {
                UartTxTask_SendFPS(fps);
            }
            last_fps_send = current_time;
        }

        // 短延时
        vTaskDelayUntil(&pxPreviousWakeTime, pdMS_TO_TICKS(20));
    }
}
