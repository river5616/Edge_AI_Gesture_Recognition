#ifndef __LED_H
#define __LED_H

#include "headfile.h"

#define LED1_ON     HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET)
#define LED1_OFF    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET)
#define LED1_Toggle HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_0)
#define LED2_ON   HAL_GPIO_WritePin(GPIOE,GPIO_PIN_1,GPIO_PIN_SET)
#define LED2_OFF  HAL_GPIO_WritePin(GPIOE,GPIO_PIN_1,GPIO_PIN_RESET)
#define LED2_Toggle HAL_GPIO_TogglePin(GPIOE,GPIO_PIN_1)
#define LED3_ON   HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_SET)
#define LED3_OFF  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_RESET)
#define LED3_Toggle HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_14)




void led_proc(void);















#endif
