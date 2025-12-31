#ifndef __UART_RX_TASK_H
#define __UART_RX_TASK_H

#include "task_common.h"

// 串口接收任务入口函数
void AppTask_uart_rx(void *argument);

// 初始化
void UartRxTask_Init(void);

// 解析命令（暴露给uart_callbacks.c使用）
void parse_command(uint8_t* cmd_str, uint16_t len);

// UART句柄（需要在main.c中定义）
extern UART_HandleTypeDef huart1;

// 接收缓冲区变量（暴露给uart_callbacks.c使用）
extern uint8_t rx_buffer[256];
extern uint16_t rx_index;
extern uint8_t rx_byte;

#endif
