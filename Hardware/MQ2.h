#ifndef __MQ2_H
#define __MQ2_H

#include "main.h"
#include "adc.h"
#include <stdint.h>
#include <math.h>

/* MQ2配置 */
#define MQ2_READ_TIMES      10      // ADC循环读取次数

/* 模式选择: 1=模拟AO(ADC), 0=数字DO */
#define MQ2_MODE            1

/* MQ2引脚定义 */
#if MQ2_MODE
/* 模拟输出AO接口（ADC通道） */
#define MQ2_AO_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOF_CLK_ENABLE()
#define MQ2_AO_PORT                 GPIOF
#define MQ2_AO_PIN                  GPIO_PIN_4
#define MQ2_ADC_CHANNEL             ADC_CHANNEL_9
#else
/* 数字输出DO接口 */
#define MQ2_DO_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOA_CLK_ENABLE()
#define MQ2_DO_PORT                 GPIOA
#define MQ2_DO_PIN                  GPIO_PIN_1
#endif

/* 函数声明 */
void MQ2_Init(void);
uint16_t MQ2_GetData(void);
float MQ2_GetDataPPM(void);

#endif /* __MQ2_H */
