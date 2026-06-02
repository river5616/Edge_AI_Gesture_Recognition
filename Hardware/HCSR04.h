#ifndef __HCSR04_H
#define __HCSR04_H

#include "main.h"
#include <stdint.h>

/* HC-SR04引脚定义 */
#define HCSR04_TRIG_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOA_CLK_ENABLE()
#define HCSR04_TRIG_PORT               GPIOA
#define HCSR04_TRIG_PIN                GPIO_PIN_2

#define HCSR04_ECHO_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOA_CLK_ENABLE()
#define HCSR04_ECHO_PORT               GPIOA
#define HCSR04_ECHO_PIN                GPIO_PIN_3

/* 函数声明 */
void HCSR04_Init(void);
uint16_t HCSR04_GetDistance(void);

#endif /* __HCSR04_H */
