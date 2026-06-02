#include "sensors.h"

/**
 * @brief  初始化所有传感器
 * @note   在FreeRTOS启动前或任务中调用
 */
void Sensors_Init(void)
{
    /* 初始化BH1750光照传感器 */
    BH1750_Init();

    /* 初始化HC-SR04超声波传感器 */
    HCSR04_Init();

    /* 初始化MQ2烟雾传感器 */
    MQ2_Init();

    /* 初始化MAX30102心率血氧传感器 */
    MAX30102_Init();

    /* 初始化DHT11温湿度传感器 */
    DHT11_Init();
}
