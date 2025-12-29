#ifndef __UART_PROTOCOL_H
#define __UART_PROTOCOL_H

#include "main.h"
#include <stdint.h>

// 命令类型
typedef enum {
    CMD_NONE = 0,
    CMD_COLLECT,      // 采集数据
    CMD_STOP,         // 停止采集
    CMD_STATUS,       // 查询状态
    CMD_INFER,        // 推理模式
    CMD_STOP_INFER    // 停止推理
} Command_Type_t;

// 工作模式
typedef enum {
    MODE_IDLE = 0,
    MODE_COLLECTING,  // 数据采集模式
    MODE_INFERENCE    // 推理模式
} Work_Mode_t;

// 协议句柄
typedef struct {
    Command_Type_t cmd;
    Work_Mode_t mode;
    uint8_t gesture_id;      // 当前采集的手势ID
    uint16_t target_images;  // 目标图像数量
    uint16_t current_images; // 当前已采集数量
    uint8_t rx_buffer[256];  // 接收缓冲区
    uint16_t rx_index;
} Protocol_Handle_t;

// 外部UART句柄声明（需在main.c中定义）
extern UART_HandleTypeDef huart1;

// 函数声明
void Protocol_Init(void);
void Protocol_ProcessCommand(uint8_t* data, uint16_t len);
void Protocol_SendImage(uint8_t* image_data, uint16_t width, uint16_t height);
void Protocol_SendProgress(uint16_t current, uint16_t total);
void Protocol_SendResult(uint8_t gesture_id, float confidence);
void Protocol_SendFPS(float fps);
void Protocol_SendError(uint8_t error_code, const char* error_msg);
void Protocol_SendStatus(void);
Work_Mode_t Protocol_GetMode(void);
uint8_t Protocol_GetGestureID(void);
uint16_t Protocol_GetTargetImages(void);
void Protocol_IncrementImageCount(void);

// UART接收回调（需在main.c中实现）
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#endif
