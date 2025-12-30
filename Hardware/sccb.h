#ifndef __SCCB_H
#define __SCCB_H

#include "main.h"


// SCCB引脚定义(根据你的配置)
#define SCCB_SCL_PIN           GPIO_PIN_10
#define SCCB_SCL_PORT          GPIOF

#define SCCB_SDA_PIN           GPIO_PIN_7
#define SCCB_SDA_PORT          GPIOE

// SCCB控制宏
#define SCCB_SCL_HIGH()        HAL_GPIO_WritePin(SCCB_SCL_PORT, SCCB_SCL_PIN, GPIO_PIN_SET)
#define SCCB_SCL_LOW()         HAL_GPIO_WritePin(SCCB_SCL_PORT, SCCB_SCL_PIN, GPIO_PIN_RESET)
#define SCCB_SDA_HIGH()        HAL_GPIO_WritePin(SCCB_SDA_PORT, SCCB_SDA_PIN, GPIO_PIN_SET)
#define SCCB_SDA_LOW()         HAL_GPIO_WritePin(SCCB_SDA_PORT, SCCB_SDA_PIN, GPIO_PIN_RESET)
#define SCCB_SDA_READ()        HAL_GPIO_ReadPin(SCCB_SDA_PORT, SCCB_SDA_PIN)

// OV2640的SCCB设备地址
#define SCCB_ID                0x60

// SDA方向切换
#define SCCB_SDA_OUT()         SCCB_SDA_Mode_Out()
#define SCCB_SDA_IN()          SCCB_SDA_Mode_In()

// 函数声明
void SCCB_Init(void);
void SCCB_SDA_Mode_Out(void);
void SCCB_SDA_Mode_In(void);
void SCCB_Start(void);
void SCCB_Stop(void);
void SCCB_No_Ack(void);
uint8_t SCCB_WR_Byte(uint8_t dat);
uint8_t SCCB_RD_Byte(void);
uint8_t SCCB_WR_Reg(uint8_t reg, uint8_t data);
uint8_t SCCB_RD_Reg(uint8_t reg);

#endif
