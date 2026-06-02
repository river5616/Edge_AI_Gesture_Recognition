#ifndef __DISP_H
#define __DISP_H

#include "headfile.h"

#define line1 0
#define line2 8
#define line3 16
#define line4 24
#define line5 32
#define line6 40
#define line7 48
#define line8 56

// 显示视图定义
#define VIEW_STATUS     0  // 状态信息视图
#define VIEW_IMAGE      1  // 图像显示视图
#define VIEW_UART_STATS 2  // UART接收统计视图
#define VIEW_SENSORS    3  // 传感器数据视图

// 传感器数据结构（用于disp.c显示）
typedef struct {
    uint16_t lightValue;      // 光照强度原始值
    float lightLux;           // 光照强度(Lux)
    uint16_t distance;        // 距离(cm)
    uint16_t mq2Value;        // MQ2 ADC值
    float mq2PPM;             // MQ2 PPM值
    uint32_t redLED;          // MAX30102红光LED
    uint32_t irLED;           // MAX30102红外LED
    int32_t heartRate;        // 心率(BPM)
    int8_t heartRateValid;    // 心率有效标志
    int32_t spo2;             // 血氧饱和度(%)
    int8_t spo2Valid;         // 血氧有效标志
    uint8_t temperature;      // DHT11温度(℃)
    uint8_t humidity;         // DHT11湿度(%)
    uint8_t dht11Valid;       // DHT11数据有效标志
} SensorData_t;

// 外部变量声明
extern u8 view;
extern SensorData_t g_sensorData;  // 全局传感器数据

void oled_disp(void);

/*应用函数*/
void OLED_ShowGestureResult(uint8_t GestureID, float Confidence);















#endif
