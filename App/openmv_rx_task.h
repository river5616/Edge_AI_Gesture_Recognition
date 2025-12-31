#ifndef __OPENMV_RX_TASK_H
#define __OPENMV_RX_TASK_H

#include "task_common.h"

// OpenMV图像参数
#define OPENMV_IMAGE_WIDTH   64
#define OPENMV_IMAGE_HEIGHT  64
#define OPENMV_IMAGE_SIZE    (OPENMV_IMAGE_WIDTH * OPENMV_IMAGE_HEIGHT)

// OpenMV数据包协议参数
#define OPENMV_HEADER_SIZE   2    // 帧头: 0xAA 0x55
#define OPENMV_LENGTH_SIZE   2    // 长度: 2字节
#define OPENMV_CHECKSUM_SIZE 1    // 校验: 1字节
#define OPENMV_FOOTER_SIZE   2    // 帧尾: 0x5A 0xA5
#define OPENMV_PACKET_SIZE   (OPENMV_HEADER_SIZE + OPENMV_LENGTH_SIZE + \
                              OPENMV_CHECKSUM_SIZE + OPENMV_IMAGE_SIZE + \
                              OPENMV_FOOTER_SIZE)  // 总计4103字节

// 帧头帧尾定义
#define OPENMV_HEADER_0      0xAA
#define OPENMV_HEADER_1      0x55
#define OPENMV_FOOTER_0      0x5A
#define OPENMV_FOOTER_1      0xA5

// OpenMV接收任务入口函数
void AppTask_openmv_rx(void *argument);

// 初始化
void OpenMVRxTask_Init(void);

// 解析接收缓冲区（暴露给uart_callbacks.c使用）
void parse_receive_buffer(const uint8_t* buffer, uint16_t length);

// 获取接收统计信息
typedef struct {
    uint32_t total_frames;      // 接收总帧数
    uint32_t error_frames;      // 错误帧数
    uint32_t checksum_errors;   // 校验错误数
    uint32_t timeout_errors;    // 超时错误数
    float fps;                  // 接收帧率
} OpenMVRxStats_t;

void OpenMVRxTask_GetStats(OpenMVRxStats_t* stats);

// UART句柄（需要在main.c中定义）
extern UART_HandleTypeDef huart3;
extern DMA_HandleTypeDef hdma_usart3_rx;

// 任务句柄（暴露给uart_callbacks.c使用）
extern osThreadId_t openmv_task_handle;

// DMA缓冲区（暴露给uart_callbacks.c使用）
extern uint8_t dma_buffer[];

#endif
