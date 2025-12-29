#include "task_common.h"

// 全局队列句柄定义
osMessageQueueId_t imageQueue = NULL;
osMessageQueueId_t cmdQueue = NULL;
osMessageQueueId_t resultQueue = NULL;

// 全局信号量句柄定义
osSemaphoreId_t frameReadySem = NULL;
osSemaphoreId_t uartTxSem = NULL;

// 全局互斥量句柄定义
osMutexId_t cameraMutex = NULL;
osMutexId_t uartMutex = NULL;

// 全局变量定义
WorkMode_t g_work_mode = MODE_IDLE;
uint8_t g_gesture_id = 0;
uint16_t g_target_images = 0;
uint16_t g_current_images = 0;
float g_current_fps = 0.0f;

// 设置工作模式
void Task_SetMode(WorkMode_t mode)
{
    g_work_mode = mode;
}

// 获取工作模式
WorkMode_t Task_GetMode(void)
{
    return g_work_mode;
}

// 更新FPS
void Task_UpdateFPS(float fps)
{
    g_current_fps = fps;
}

// 获取FPS
float Task_GetFPS(void)
{
    return g_current_fps;
}
