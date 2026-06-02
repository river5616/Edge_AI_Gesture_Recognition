#include "sensor_task.h"
#include "sensors.h"
#include "OLED.h"
#include "disp.h"
#include "algorithm.h"  // 添加算法头文件
#include "gesture_control.h"  // 添加手势控制头文件
#include "cmsis_os.h"
#include <stdio.h>

// 传感器初始化状态标志
static uint8_t sensor_init_ok = 0;

// MAX30102数据缓冲区（1024个样本，10.24秒数据，用于FFT）
static uint32_t aun_ir_buffer[1024];
static uint32_t aun_red_buffer[1024];
static uint16_t buffer_index = 0;

// 结果平滑滤波（移动平均）
#define SMOOTH_SIZE 3
static int32_t hr_history[SMOOTH_SIZE] = {0};
static int32_t spo2_history[SMOOTH_SIZE] = {0};
static uint8_t history_index = 0;
static uint8_t history_count = 0;

/**
 * @brief  传感器任务函数
 * @param  argument: 任务参数
 * @note   定期读取所有传感器数据并更新全局变量g_sensorData
 *         显示由oled_disp()统一处理（VIEW_SENSORS视图）
 */
void AppTask_sensor(void *argument)
{
    /* 等待系统稳定 - 给其他任务一些启动时间 */
    osDelay(1000);

    /* 尝试初始化传感器（可能失败，不阻塞系统） */
    Sensors_Init();
    sensor_init_ok = 1;  // 标记初始化完成（即使失败也继续）

    /* 初始化心率血氧为无效 */
    g_sensorData.heartRate = -999;
    g_sensorData.heartRateValid = 0;
    g_sensorData.spo2 = -999;
    g_sensorData.spo2Valid = 0;

    /* 采集计数器（用于控制其他传感器的采集频率） */
    uint8_t other_sensor_counter = 0;
    uint8_t dht11_counter = 0;  // DHT11计数器（2秒读取一次）

    /* 任务主循环 */
    for (;;)
    {
        /* MAX30102每10ms采集一次（100Hz，FFT需要） */
        if (MAX30102_ReadFIFO(&g_sensorData.redLED, &g_sensorData.irLED) == 0)
        {
            // 读取成功，保存到缓冲区
            aun_red_buffer[buffer_index] = g_sensorData.redLED;
            aun_ir_buffer[buffer_index] = g_sensorData.irLED;
            buffer_index++;

            // 收集满1024个样本后计算心率和血氧（10.24秒数据，用于FFT）
            if (buffer_index >= 1024)
            {
                int32_t temp_hr, temp_spo2;
                int8_t temp_hr_valid, temp_spo2_valid;

                // 执行FFT算法计算
                maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, 1024, aun_red_buffer,
                                                        &temp_spo2, &temp_spo2_valid,
                                                        &temp_hr, &temp_hr_valid);

                // 只对有效值进行平滑处理
                if (temp_hr_valid && temp_hr >= 40 && temp_hr <= 200) {
                    // 添加到历史记录
                    hr_history[history_index] = temp_hr;

                    // 计算移动平均
                    int32_t hr_sum = 0;
                    uint8_t valid_count = (history_count < SMOOTH_SIZE) ? history_count + 1 : SMOOTH_SIZE;
                    for (uint8_t i = 0; i < valid_count; i++) {
                        hr_sum += hr_history[i];
                    }
                    g_sensorData.heartRate = hr_sum / valid_count;
                    g_sensorData.heartRateValid = 1;
                } else {
                    g_sensorData.heartRate = temp_hr;
                    g_sensorData.heartRateValid = temp_hr_valid;
                }

                // SpO2平滑处理
                if (temp_spo2_valid && temp_spo2 >= 50 && temp_spo2 <= 100) {
                    // 添加到历史记录
                    spo2_history[history_index] = temp_spo2;

                    // 计算移动平均
                    int32_t spo2_sum = 0;
                    uint8_t valid_count = (history_count < SMOOTH_SIZE) ? history_count + 1 : SMOOTH_SIZE;
                    for (uint8_t i = 0; i < valid_count; i++) {
                        spo2_sum += spo2_history[i];
                    }
                    g_sensorData.spo2 = spo2_sum / valid_count;
                    g_sensorData.spo2Valid = 1;
                } else {
                    g_sensorData.spo2 = temp_spo2;
                    g_sensorData.spo2Valid = temp_spo2_valid;
                }

                // 更新历史索引
                history_index = (history_index + 1) % SMOOTH_SIZE;
                if (history_count < SMOOTH_SIZE) history_count++;

                // 使用滑动窗口：保留后512个样本，移到前面
                for (uint16_t i = 0; i < 512; i++) {
                    aun_ir_buffer[i] = aun_ir_buffer[512 + i];
                    aun_red_buffer[i] = aun_red_buffer[512 + i];
                }
                buffer_index = 512;  // 从512开始继续采集
            }
        }
        else
        {
            // 读取失败，清零数据
            g_sensorData.redLED = 0;
            g_sensorData.irLED = 0;
        }

        /* 其他传感器每100ms采集一次（计数器达到10时） */
        other_sensor_counter++;
        if (other_sensor_counter >= 10)
        {
            other_sensor_counter = 0;

            /* 读取BH1750光照强度 */
            g_sensorData.lightValue = BH1750_ReadData();
            if (g_sensorData.lightValue != 0xFFFF) {
                g_sensorData.lightLux = g_sensorData.lightValue / 1.2f;
            } else {
                g_sensorData.lightLux = 0;
            }

            /* 读取HC-SR04距离 */
            g_sensorData.distance = HCSR04_GetDistance();
            if (g_sensorData.distance > 500) {
                g_sensorData.distance = 999;
            }

            /* 读取MQ2烟雾传感器 */
            g_sensorData.mq2Value = MQ2_GetData();
            g_sensorData.mq2PPM = MQ2_GetDataPPM();

            /* DHT11每2秒读取一次（计数器达到20时，20*100ms=2000ms） */
            dht11_counter++;
            if (dht11_counter >= 20)
            {
                dht11_counter = 0;

                DHT11_Data_t dht11_data;
                if (DHT11_Read(&dht11_data) == 0)
                {
                    // 读取成功
                    g_sensorData.temperature = dht11_data.temperature_int;
                    g_sensorData.humidity = dht11_data.humidity_int;
                    g_sensorData.dht11Valid = 1;
                }
                else
                {
                    // 读取失败
                    g_sensorData.dht11Valid = 0;
                }
            }
        }

        /* 检查传感器报警状态 */
        GestureControl_CheckSensorAlarm();

        /* 延时10毫秒（MAX30102采样率100Hz） */
        osDelay(10);
    }
}



