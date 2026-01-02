#ifndef __INFERENCE_TASK_H
#define __INFERENCE_TASK_H

#include "task_common.h"

// 手势类别数量 - 5个手势（移除 gun 和 call_me）
#define NUM_GESTURES 5

// 推理统计结构
typedef struct {
    uint32_t total_count;    // 总推理次数
    uint32_t success_count;  // 成功次数
    uint32_t fail_count;     // 失败次数
} InferenceStats_t;

// 推理任务入口函数
void AppTask_inference(void *argument);

// 初始化AI模型
uint8_t InferenceTask_Init(void);

// 推理一帧图像
uint8_t InferenceTask_Run(uint8_t* image_data, uint8_t* result, float* confidence);

// 获取推理统计信息
void InferenceTask_GetStats(InferenceStats_t* stats);

// 获取所有手势的置信度（用于OLED显示）
void InferenceTask_GetAllConfidences(float* confidences);

#endif
