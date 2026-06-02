/**
 * @file DHT11.h
 * @brief DHT11温湿度传感器驱动（优化安全版本）
 */
#ifndef __DHT11_H
#define __DHT11_H

#include "stm32h7xx_hal.h"
#include <stdint.h>

/* DHT11引脚定义 */
#define DHT11_PORT              GPIOB
#define DHT11_PIN               GPIO_PIN_13
#define DHT11_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()

/* DHT11数据结构 */
typedef struct {
    uint8_t humidity_int;       // 湿度整数部分
    uint8_t humidity_dec;       // 湿度小数部分
    uint8_t temperature_int;    // 温度整数部分
    uint8_t temperature_dec;    // 温度小数部分
    uint8_t valid;              // 数据有效标志
} DHT11_Data_t;

/* 函数声明 */
void DHT11_Init(void);
uint8_t DHT11_Read(DHT11_Data_t *data);

#endif /* __DHT11_H */
