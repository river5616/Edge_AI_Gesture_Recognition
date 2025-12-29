#include "inference_task.h"
#include "uart_tx_task.h"
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

// 初始化AI模型
uint8_t InferenceTask_Init(void)
{
    /* 如果已集成X-CUBE-AI，取消注释以下代码：

    ai_error err;

    // 创建网络
    err = ai_gesture_model_create(&network, AI_GESTURE_MODEL_DATA_CONFIG);
    if(err.type != AI_ERROR_NONE) {
        return 1;
    }

    // 初始化网络
    const ai_network_params params = {
        AI_GESTURE_MODEL_DATA_WEIGHTS(ai_gesture_model_data_weights_get()),
        AI_GESTURE_MODEL_DATA_ACTIVATIONS(activations)
    };

    if(!ai_gesture_model_init(network, &params)) {
        return 2;
    }

    // 获取输入输出缓冲区信息
    ai_gesture_model_get_info(network, &network_info);
    ai_input[0] = network_info.inputs[0];
    ai_output[0] = network_info.outputs[0];

    return 0;  // 成功
    */

    // 暂时返回成功（未集成AI时）
    return 0;
}

// 运行推理
uint8_t InferenceTask_Run(uint8_t* image_data, uint8_t* result, float* confidence)
{
    /* 如果已集成X-CUBE-AI，取消注释以下代码：

    // 准备输入数据
    ai_input[0].data = AI_HANDLE_PTR(image_data);

    // 准备输出缓冲区
    float output_data[9];
    ai_output[0].data = AI_HANDLE_PTR(output_data);

    // 运行推理
    ai_i32 batch = ai_gesture_model_run(network, &ai_input[0], &ai_output[0]);
    if(batch != 1) {
        return 1;  // 推理失败
    }

    // 找到最大概率的类别
    uint8_t max_idx = 0;
    float max_val = output_data[0];

    for(uint8_t i = 1; i < 9; i++) {
        if(output_data[i] > max_val) {
            max_val = output_data[i];
            max_idx = i;
        }
    }

    *result = max_idx;
    *confidence = max_val;

    return 0;  // 成功
    */

    // 暂时返回模拟结果（未集成AI时）
    *result = 0;
    *confidence = 0.95f;
    return 0;
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

                    // 运行推理
                    if(InferenceTask_Run(img_msg.data, &gesture_id, &confidence) == 0)
                    {
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
