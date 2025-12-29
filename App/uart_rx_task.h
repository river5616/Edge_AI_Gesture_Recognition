#ifndef __UART_RX_TASK_H
#define __UART_RX_TASK_H

#include "task_common.h"

// 串口接收任务入口函数
void AppTask_uart_rx(void *argument);

// 初始化
void UartRxTask_Init(void);

// UART句柄（需要在main.c中定义）
extern UART_HandleTypeDef huart1;

#endif
