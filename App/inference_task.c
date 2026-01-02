#include "inference_task.h"
#include "uart_tx_task.h"
#include "app_x-cube-ai.h"
#include "disp.h"
#include <string.h>

// AI模型相关（如果已集成X-CUBE-AI，取消注释）
/*
#include "ai_datatypes_defines.h"
#include "ai_platform.h"
#include "gesture_model.h"
#include "gesture_model_data.h"

static ai_handle network = AI_HANDLE_NULL;
static ai_buffer ai_input[AI_GESTURE_MODEL_IN_NUM];
static ai_buffer ai_output[AI_GESTURE_MODEL_OUT_NUM];
static uint8_t activations[AI_GESTURE_MODEL_DATA_ACTIVATIONS_SIZE];
*/

// 推理统计变量
static uint32_t s_inference_count = 0;      // 推理次数
static uint32_t s_inference_success = 0;    // 成功次数
static uint32_t s_inference_fail = 0;       // 失败次数

// 初始化AI模型
uint8_t InferenceTask_Init(void)
{
    // AI 模型已在 main.c 的 MX_X_CUBE_AI_Init() 中初始化完成
    // 这里不需要额外操作
    return 0;  // 成功
}

// 运行推理
uint8_t InferenceTask_Run(uint8_t* image_data, uint8_t* result, float* confidence)
{
    // 调用 X-CUBE-AI 推理接口
    int ret = MX_X_CUBE_AI_Inference(image_data, result, confidence);

    if (ret == 0) {
        return 0;  // 成功
    } else {
        // 推理失败，返回默认值
        *result = 0;
        *confidence = 0.0f;
        return 1;  // 失败
    }
}

// 获取推理统计信息
void InferenceTask_GetStats(InferenceStats_t* stats)
{
    if(stats != NULL)
    {
        stats->total_count = s_inference_count;
        stats->success_count = s_inference_success;
        stats->fail_count = s_inference_fail;
    }
}

/**
 * @brief  AI推理任务
 * @param  argument: 未使用
 * @retval None
 */
void AppTask_inference(void *argument)
{
    TickType_t pxPreviousWakeTime = xTaskGetTickCount();

    // 初始化AI模型
    if(InferenceTask_Init() != 0)
    {
        // 初始化失败，挂起任务
        vTaskSuspend(NULL);
    }

    // 无限循环
    for(;;)
    {
        WorkMode_t mode = Task_GetMode();

        if(mode == MODE_INFERENCE)
        {
            // 等待图像数据
            if(osSemaphoreAcquire(frameReadySem, 1000) == osOK)
            {
                ImageMsg_t img_msg;

                // 从队列获取图像
                if(osMessageQueueGet(imageQueue, &img_msg, NULL, 0) == osOK)
                {
                    uint8_t gesture_id;
                    float confidence;

                    s_inference_count++;

                    // 运行推理
                    uint8_t ret = InferenceTask_Run(img_msg.data, &gesture_id, &confidence);

                    if(ret == 0)
                    {
                        s_inference_success++;

                        // 在 OLED 上显示识别结果
                        OLED_ShowGestureResult(gesture_id, confidence);

                        // 发送推理结果
                        UartTxTask_SendResult(gesture_id, confidence);

                        // 可选：将结果放入结果队列供OLED显示
                        InferenceResult_t result = {
                            .gesture_id = gesture_id,
                            .confidence = confidence,
                            .timestamp = osKernelGetTickCount()
                        };
                        osMessageQueuePut(resultQueue, &result, 0, 0);
                    }
                    else
                    {
                        s_inference_fail++;

                        // 推理失败，也显示一个错误状态
                        OLED_ShowGestureResult(0xFF, 0.0f);  // 0xFF 表示失败
                    }

                    // 短暂延时，让其他任务有机会运行
                    osDelay(10);  // 10ms延时，确保OLED等任务能及时更新
                }
            }
        }
        else
        {
            // 非推理模式，休眠
            vTaskDelayUntil(&pxPreviousWakeTime, pdMS_TO_TICKS(100));
        }
    }
}
