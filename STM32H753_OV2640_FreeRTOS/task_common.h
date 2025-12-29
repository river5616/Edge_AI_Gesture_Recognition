#ifndef __TASK_COMMON_H
#define __TASK_COMMON_H

#include "main.h"
#include "cmsis_os.h"
#include <stdint.h>

// 图像参数
#define IMAGE_WIDTH   96
#define IMAGE_HEIGHT  96
#define IMAGE_SIZE    (IMAGE_WIDTH * IMAGE_HEIGHT)

// 工作模式
typedef enum {
    MODE_IDLE = 0,
    MODE_COLLECTING,  // 数据采集模式
    MODE_INFERENCE    // 推理模式
} WorkMode_t;

// 图像消息结构（队列传递）
typedef struct {
    uint8_t* data;        // 图像数据指针（不复制数据）
    uint16_t width;       // 图像宽度
    uint16_t height;      // 图像高度
    uint32_t timestamp;   // 时间戳
    uint32_t frame_id;    // 帧ID
} ImageMsg_t;

// 命令消息结构
typedef struct {
    uint8_t cmd_type;     // 命令类型
    uint8_t param1;       // 参数1（如手势ID）
    uint16_t param2;      // 参数2（如图像数量）
    char cmd_str[56];     // 命令字符串
} CommandMsg_t;

// 推理结果结构
typedef struct {
    uint8_t gesture_id;   // 手势ID
    float confidence;     // 置信度
    uint32_t timestamp;   // 时间戳
} InferenceResult_t;

// 全局队列句柄声明（extern）
extern osMessageQueueId_t imageQueue;
extern osMessageQueueId_t cmdQueue;
extern osMessageQueueId_t resultQueue;

// 全局信号量句柄声明
extern osSemaphoreId_t frameReadySem;
extern osSemaphoreId_t uartTxSem;

// 全局互斥量句柄声明
extern osMutexId_t cameraMutex;
extern osMutexId_t uartMutex;

// 全局变量声明
extern WorkMode_t g_work_mode;
extern uint8_t g_gesture_id;
extern uint16_t g_target_images;
extern uint16_t g_current_images;
extern float g_current_fps;

// 函数声明
void Task_SetMode(WorkMode_t mode);
WorkMode_t Task_GetMode(void);
void Task_UpdateFPS(float fps);
float Task_GetFPS(void);

#endif
