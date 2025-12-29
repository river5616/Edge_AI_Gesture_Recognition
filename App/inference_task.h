#ifndef __INFERENCE_TASK_H
#define __INFERENCE_TASK_H

#include "task_common.h"

// 推理任务入口函数
void AppTask_inference(void *argument);

// 初始化AI模型
uint8_t InferenceTask_Init(void);

// 推理一帧图像
uint8_t InferenceTask_Run(uint8_t* image_data, uint8_t* result, float* confidence);

#endif
