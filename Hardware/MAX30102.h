#ifndef __MAX30102_H
#define __MAX30102_H

#include "main.h"
#include <stdint.h>

/* MAX30102 I2C地址 */
#define MAX30102_I2C_WRITE_ADDR     0xAE
#define MAX30102_I2C_READ_ADDR      0xAF

/* MAX30102寄存器地址 */
#define MAX30102_REG_INTR_STATUS_1  0x00
#define MAX30102_REG_INTR_STATUS_2  0x01
#define MAX30102_REG_INTR_ENABLE_1  0x02
#define MAX30102_REG_INTR_ENABLE_2  0x03
#define MAX30102_REG_FIFO_WR_PTR    0x04
#define MAX30102_REG_OVF_COUNTER    0x05
#define MAX30102_REG_FIFO_RD_PTR    0x06
#define MAX30102_REG_FIFO_DATA      0x07
#define MAX30102_REG_FIFO_CONFIG    0x08
#define MAX30102_REG_MODE_CONFIG    0x09
#define MAX30102_REG_SPO2_CONFIG    0x0A
#define MAX30102_REG_LED1_PA        0x0C
#define MAX30102_REG_LED2_PA        0x0D
#define MAX30102_REG_PILOT_PA       0x10
#define MAX30102_REG_MULTI_LED_CTRL1 0x11
#define MAX30102_REG_MULTI_LED_CTRL2 0x12
#define MAX30102_REG_TEMP_INTR      0x1F
#define MAX30102_REG_TEMP_FRAC      0x20
#define MAX30102_REG_TEMP_CONFIG    0x21
#define MAX30102_REG_PROX_INT_THRESH 0x30
#define MAX30102_REG_REV_ID         0xFE
#define MAX30102_REG_PART_ID        0xFF

/* MAX30102 INT引脚定义 */
#define MAX30102_INT_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOB_CLK_ENABLE()
#define MAX30102_INT_PORT               GPIOB
#define MAX30102_INT_PIN                GPIO_PIN_3

/* 函数声明 */
void MAX30102_Init(void);
int8_t MAX30102_Reset(void);
int8_t MAX30102_WriteReg(uint8_t regAddr, uint8_t data);
int8_t MAX30102_ReadReg(uint8_t regAddr, uint8_t *pData);
int8_t MAX30102_ReadFIFO(uint32_t *pRedLED, uint32_t *pIRLED);

#endif /* __MAX30102_H */
