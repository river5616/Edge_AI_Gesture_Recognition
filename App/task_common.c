#include "task_common.h"

// 注意：队列、信号量、互斥量的句柄在freertos.c中由CubeMX生成和定义
// 这里只需要定义全局变量

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
