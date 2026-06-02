#include "uart_tx_task.h"
#include <stdio.h>
#include <string.h>

// OneNet相关
#include "esp8266.h"

// 传感器数据
#include "disp.h"  // 包含SensorData_t和g_sensorData定义

// 外部UART句柄
extern UART_HandleTypeDef huart1;

// OneNet传感器数据（全局变量，供onenet.c使用）
float g_sensor_temperature = 25.0f;
float g_sensor_humidity = 60.0f;
float g_sensor_light = 0.0f;       // 光照强度(Lux)
uint16_t g_sensor_distance = 0;    // 距离(cm)
float g_sensor_mq2 = 0.0f;         // MQ2 PPM值
int32_t g_sensor_heartRate = 0;    // 心率(BPM)
int32_t g_sensor_spo2 = 0;         // 血氧饱和度(%)
uint8_t g_last_gesture = 0;        // 保留，但不上传OneNet

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

    // 更新最后的手势识别结果
    g_last_gesture = gesture_id;
}

// 更新传感器数据（用于OneNet上传）
// 注意：温湿度数据直接从g_sensorData获取，这里只更新手势识别结果
void UartTxTask_UpdateSensorData(float temp, float humi, uint8_t gesture)
{
    // 手势识别结果单独存储
    g_last_gesture = gesture;

    // 温湿度数据已在sensor_task中更新到g_sensorData，这里可以选择性覆盖
    // 如果传入的参数是来自其他传感器，可以使用；否则直接使用g_sensorData的值
    if(temp != 0.0f || humi != 0.0f) {
        g_sensor_temperature = temp;
        g_sensor_humidity = humi;
    }
}

// 从g_sensorData同步数据到OneNet上传变量
static void SyncSensorDataToOneNet(void)
{
    // 同步DHT11温湿度数据（仅在有效时）
    if(g_sensorData.dht11Valid)
    {
        g_sensor_temperature = (float)g_sensorData.temperature;
        g_sensor_humidity = (float)g_sensorData.humidity;
    }

    // 同步光照强度数据
    g_sensor_light = g_sensorData.lightLux;

    // 同步距离数据
    g_sensor_distance = g_sensorData.distance;

    // 同步MQ2数据
    g_sensor_mq2 = g_sensorData.mq2PPM;

    // 同步心率数据（仅在有效时）
    if(g_sensorData.heartRateValid)
    {
        g_sensor_heartRate = g_sensorData.heartRate;
    }
    else
    {
        g_sensor_heartRate = 0;  // 无效时设为0
    }

    // 同步血氧数据（仅在有效时）
    if(g_sensorData.spo2Valid)
    {
        g_sensor_spo2 = g_sensorData.spo2;
    }
    else
    {
        g_sensor_spo2 = 0;  // 无效时设为0
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
    uint32_t last_bt_send = 0;  // 蓝牙数据发送计时
    uint32_t last_onenet_send = 0;  // OneNet数据发送计时
    uint32_t last_onenet_retry = 0;  // OneNet重连计时
    static uint8_t onenet_initialized = 0;  // OneNet初始化标志
    static uint8_t esp8266_init_failed = 0;  // ESP8266初始化失败标志

    // 延迟启动ESP8266初始化，给系统一些启动时间
    vTaskDelay(pdMS_TO_TICKS(2000));
    //ESP8266_Init();
    

    // 无限循环
    for(;;)
    {
        WorkMode_t mode = Task_GetMode();

        // 只在采集模式下处理图像
        if(mode == MODE_IDLE || mode == MODE_COLLECTING)
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

        // 每秒发送一次FPS（仅在非推理模式下，避免干扰调试）
        if(mode != MODE_INFERENCE)
        {
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
        }

        // 蓝牙：每2秒发送一次传感器数据
        uint32_t current_time = osKernelGetTickCount();
        if(current_time - last_bt_send >= 2000)
        {
            BT_SendSensorData();
            last_bt_send = current_time;
			
			ESP8266_SendMQTTMessage();
        }

        // 短延时
        vTaskDelayUntil(&pxPreviousWakeTime, pdMS_TO_TICKS(20));
    }
}
