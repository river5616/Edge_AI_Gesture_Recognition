#ifndef __UART_CALLBACKS_H
#define __UART_CALLBACKS_H

#include "main.h"

// UART回调函数（在uart_callbacks.c中统一实现）
// 这些函数由HAL库自动调用，不需要用户手动调用

// UART接收完成回调
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

// UART接收半完成回调
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart);

// UART错误回调
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart);

#endif
