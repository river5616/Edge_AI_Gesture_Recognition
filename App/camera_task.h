#ifndef __CAMERA_TASK_H
#define __CAMERA_TASK_H

#include "task_common.h"

extern uint8_t cnt2;
extern uint8_t camera_init_flag;

// 摄像头任务入口函数
void AppTask_cam_capture(void *argument);

// 摄像头初始化
uint8_t CameraTask_Init(void);

// 获取摄像头状态
uint8_t CameraTask_IsReady(void);

#endif
