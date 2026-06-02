#ifndef __UART_TX_TASK_H
#define __UART_TX_TASK_H

#include "task_common.h"
#include "bluetooth.h"

// 串口发送任务入口函数
void AppTask_uart_tx(void *argument);

// 发送图像数据
void UartTxTask_SendImage(uint8_t* image_data, uint16_t width, uint16_t height, uint32_t data_size);

// 发送FPS信息
void UartTxTask_SendFPS(float fps);

// 发送进度信息
void UartTxTask_SendProgress(uint16_t current, uint16_t total);

// 发送推理结果
void UartTxTask_SendResult(uint8_t gesture_id, float confidence);

// OneNet数据上传相关
void UartTxTask_UpdateSensorData(float temp, float humi, uint8_t gesture);

#endif
