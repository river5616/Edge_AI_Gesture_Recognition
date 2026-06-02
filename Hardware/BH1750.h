#ifndef __BH1750_H
#define __BH1750_H

#include "main.h"
#include <stdint.h>

/* BH1750 GPIO定义 - 使用软件I2C */
#define BH1750_SCL_CLK_ENABLE()    __HAL_RCC_GPIOB_CLK_ENABLE()
#define BH1750_SCL_PORT            GPIOC
#define BH1750_SCL_PIN             GPIO_PIN_4
#define BH1750_SDA_CLK_ENABLE()    __HAL_RCC_GPIOF_CLK_ENABLE()
#define BH1750_SDA_PORT            GPIOF
#define BH1750_SDA_PIN             GPIO_PIN_5

/* I2C地址 */
#define BH1750_ADDR_WRITE           0x46      // 从机地址+写信号位
#define BH1750_ADDR_READ            0x47      // 从机地址+读信号位

/* BH1750命令 */
#define BH1750_POWER_DOWN           0x00      // 关闭模块
#define BH1750_POWER_ON             0x01      // 开模块等待测量指令
#define BH1750_RESET                0x07      // 重置数据寄存器值
#define BH1750_MODE_H1              0x10      // 高分辨率 单位1lx 测量时间120ms
#define BH1750_MODE_H2              0x11      // 高分辨率模式2 单位0.5lx 测量时间120ms
#define BH1750_MODE_L               0x13      // 低分辨率 单位4lx 测量时间16ms
#define BH1750_SIG_MODE_H           0x20      // 一次高分辨率 测量后模块转换 PowerDown模式
#define BH1750_SIG_MODE_H2          0x21      // 同上模式2
#define BH1750_SIG_MODE_L           0x23      // 低分辨率

/* 函数声明 */
void BH1750_Init(void);
void BH1750_SendCommand(uint8_t command);
uint16_t BH1750_ReadData(void);

/* 软件I2C函数 */
void BH1750_IIC_Init(void);
void BH1750_IIC_Start(void);
void BH1750_IIC_Stop(void);
void BH1750_IIC_SendByte(uint8_t data);
uint8_t BH1750_IIC_ReadByte(uint8_t ack);
uint8_t BH1750_IIC_WaitAck(void);
void BH1750_IIC_Ack(void);
void BH1750_IIC_NAck(void);

#endif /* __BH1750_H */
