/**
 * @file bluetooth.h
 * @brief 蓝牙通信接口（UART4）
 */
#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H

#include "stm32h7xx_hal.h"
#include <stdint.h>

/* 蓝牙命令定义 */
typedef enum {
    BT_CMD_NONE = 0,
    BT_CMD_A = 'A',  // 命令A（可自定义功能）
    BT_CMD_B = 'B',  // 命令B
    BT_CMD_C = 'C',  // 命令C
    BT_CMD_D = 'D',  // 命令D
    BT_CMD_E = 'E',  // 命令E
    BT_CMD_F = 'F',  // 命令F
} BT_Command_t;

/* 命令回调函数类型 */
typedef void (*BT_CommandCallback_t)(BT_Command_t cmd);

/* 函数声明 */
void BT_Init(void);
void BT_SendSensorData(void);
void BT_RegisterCommandCallback(BT_CommandCallback_t callback);
void BT_ProcessCommand(uint8_t cmd);

/* UART4接收相关 */
extern uint8_t bt_rx_byte;
extern uint8_t bt_rx_buffer[64];
extern uint16_t bt_rx_index;

#endif /* __BLUETOOTH_H */
