
/**
  ******************************************************************************
  * @file    app_x-cube-ai.c
  * @author  X-CUBE-AI C code generator
  * @brief   AI program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

 /*
  * Description
  *   v1.0 - Minimum template to show how to use the Embedded Client API
  *          model. Only one input and one output is supported. All
  *          memory resources are allocated statically (AI_NETWORK_XX, defines
  *          are used).
  *          Re-target of the printf function is out-of-scope.
  *   v2.0 - add multiple IO and/or multiple heap support
  *
  *   For more information, see the embeded documentation:
  *
  *       [1] %X_CUBE_AI_DIR%/Documentation/index.html
  *
  *   X_CUBE_AI_DIR indicates the location where the X-CUBE-AI pack is installed
  *   typical : C:\Users\[user_name]\STM32Cube\Repository\STMicroelectronics\X-CUBE-AI\7.1.0
  */

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#if defined ( __ICCARM__ )
#elif defined ( __CC_ARM ) || ( __GNUC__ )
#endif

/* System headers */
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "app_x-cube-ai.h"
#include "main.h"
#include "ai_datatypes_defines.h"
#include "network.h"
#include "network_data.h"

/* USER CODE BEGIN includes */

#include "usart.h"
/* USER CODE END includes */

/* IO buffers ----------------------------------------------------------------*/

#if !defined(AI_NETWORK_INPUTS_IN_ACTIVATIONS)
AI_ALIGNED(4) ai_i8 data_in_1[AI_NETWORK_IN_1_SIZE_BYTES];
ai_i8* data_ins[AI_NETWORK_IN_NUM] = {
data_in_1
};
#else
ai_i8* data_ins[AI_NETWORK_IN_NUM] = {
NULL
};
#endif

#if !defined(AI_NETWORK_OUTPUTS_IN_ACTIVATIONS)
AI_ALIGNED(4) ai_i8 data_out_1[AI_NETWORK_OUT_1_SIZE_BYTES];
ai_i8* data_outs[AI_NETWORK_OUT_NUM] = {
data_out_1
};
#else
ai_i8* data_outs[AI_NETWORK_OUT_NUM] = {
NULL
};
#endif

/* Activations buffers -------------------------------------------------------*/

AI_ALIGNED(32)
static uint8_t pool0[AI_NETWORK_DATA_ACTIVATION_1_SIZE];

ai_handle data_activations0[] = {pool0};

/* AI objects ----------------------------------------------------------------*/

static ai_handle network = AI_HANDLE_NULL;

static ai_buffer* ai_input;
static ai_buffer* ai_output;

static void ai_log_err(const ai_error err, const char *fct)
{
  /* USER CODE BEGIN log */
//  if (fct)
//    printf("TEMPLATE - Error (%s) - type=0x%02x code=0x%02x\r\n", fct,
//        err.type, err.code);
//  else
//    printf("TEMPLATE - Error - type=0x%02x code=0x%02x\r\n", err.type, err.code);

  // ❌ 移除死循环，避免程序卡死
  // do {} while (1);

  // 通过串口输出错误信息
	char string[60];
	sprintf(string,"AI Error (%s) - type=0x%02x code=0x%02x\r\n", fct, err.type, err.code);
	HAL_UART_Transmit(&huart1, (uint8_t*)string, strlen(string), 1000);

  // LED 错误指示：LED3 快速闪烁 5 次
  for(int i = 0; i < 5; i++) {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
    HAL_Delay(100);
  }

	(void)fct;  // Suppress unused parameter warning
  (void)err;  // Suppress unused parameter warning
  /* USER CODE END log */
}

static int ai_boostrap(ai_handle *act_addr)
{
  ai_error err;

  /* Create and initialize an instance of the model */
  err = ai_network_create_and_init(&network, act_addr, NULL);
  if (err.type != AI_ERROR_NONE) {
    ai_log_err(err, "ai_network_create_and_init");
    return -1;
  }

  ai_input = ai_network_inputs_get(network, NULL);
  ai_output = ai_network_outputs_get(network, NULL);

#if defined(AI_NETWORK_INPUTS_IN_ACTIVATIONS)
  /*  In the case where "--allocate-inputs" option is used, memory buffer can be
   *  used from the activations buffer. This is not mandatory.
   */
  for (int idx=0; idx < AI_NETWORK_IN_NUM; idx++) {
	data_ins[idx] = ai_input[idx].data;
  }
#else
  for (int idx=0; idx < AI_NETWORK_IN_NUM; idx++) {
	  ai_input[idx].data = data_ins[idx];
  }
#endif

#if defined(AI_NETWORK_OUTPUTS_IN_ACTIVATIONS)
  /*  In the case where "--allocate-outputs" option is used, memory buffer can be
   *  used from the activations buffer. This is no mandatory.
   */
  for (int idx=0; idx < AI_NETWORK_OUT_NUM; idx++) {
	data_outs[idx] = ai_output[idx].data;
  }
#else
  for (int idx=0; idx < AI_NETWORK_OUT_NUM; idx++) {
	ai_output[idx].data = data_outs[idx];
  }
#endif

  return 0;
}

static int ai_run(void)
{
  ai_i32 batch;

  batch = ai_network_run(network, ai_input, ai_output);
  if (batch != 1) {
    ai_log_err(ai_network_get_error(network),
        "ai_network_run");
    return -1;
  }

  return 0;
}

/* USER CODE BEGIN 2 */

/**
 * @brief Preprocess image data: convert uint8 to float and normalize
 * @param src: source image data (uint8_t, 64x64 pixels)
 * @param dst: destination float buffer (AI input buffer)
 * @param size: number of pixels (should be 4096 for 64x64)
 *
 * @note Normalization formula: output = (input / 255.0 - 0.5) / 0.5
 *       This maps [0, 255] to [-1, 1], matching PyTorch training preprocessing
 */
static void preprocess_image(const uint8_t* src, float* dst, uint32_t size)
{
    for (uint32_t i = 0; i < size; i++) {
        // 与PyTorch训练时的Normalize(mean=[0.5], std=[0.5])保持一致
        // 先归一化到[0,1]，再应用 (x - 0.5) / 0.5 映射到[-1,1]
        float normalized = (float)src[i] / 255.0f;  // [0, 255] -> [0, 1]
        dst[i] = (normalized - 0.5f) / 0.5f;         // [0, 1] -> [-1, 1]
    }
}

/**
 * @brief Find the class with maximum probability
 * @param output_data: array of class probabilities (9 classes)
 * @param num_classes: number of classes (9)
 * @param max_idx: output - index of maximum class
 * @param max_val: output - maximum probability value
 */
static void postprocess_output(const float* output_data, uint8_t num_classes,
                               uint8_t* max_idx, float* max_val)
{
    *max_idx = 0;
    *max_val = output_data[0];

    for (uint8_t i = 1; i < num_classes; i++) {
        if (output_data[i] > *max_val) {
            *max_val = output_data[i];
            *max_idx = i;
        }
    }
}

/**
 * @brief Run gesture recognition inference
 * @param image_data: input image data (uint8_t, 64x64 grayscale)
 * @param gesture_id: output - recognized gesture ID (0-8)
 * @param confidence: output - confidence level (0.0-1.0)
 * @return 0 on success, negative on error
 */
int MX_X_CUBE_AI_Inference(const uint8_t* image_data, uint8_t* gesture_id, float* confidence)
{
    ai_i32 batch;

    // Check if network is initialized
    if (!network) {
        return -1;  // Network not initialized
    }

    // Check input parameters
    if (!image_data || !gesture_id || !confidence) {
        return -2;  // Invalid parameters
    }

    // Step 1: Preprocess image data (uint8 -> float, normalize)
    float* input_buffer = (float*)data_ins[0];
    preprocess_image(image_data, input_buffer, AI_NETWORK_IN_1_SIZE);

    // Step 2: Run inference
    batch = ai_network_run(network, ai_input, ai_output);
    if (batch != 1) {
        return -3;  // Inference failed
    }

    // Step 3: Get output and normalize
    float* output_buffer = (float*)data_outs[0];

    // 计算所有输出的和（用于归一化）
    float sum = 0.0f;
    for (uint8_t i = 0; i < AI_NETWORK_OUT_1_SIZE; i++) {
        sum += output_buffer[i];
    }

    // 避免除以0
    if (sum < 0.0001f) {
        sum = 1.0f;
    }

    // 归一化：将所有输出除以总和
    float normalized_output[AI_NETWORK_OUT_1_SIZE];
    for (uint8_t i = 0; i < AI_NETWORK_OUT_1_SIZE; i++) {
        normalized_output[i] = output_buffer[i] / sum;

        // 确保范围在 [0, 1]
        if (normalized_output[i] < 0.0f) {
            normalized_output[i] = 0.0f;
        } else if (normalized_output[i] > 1.0f) {
            normalized_output[i] = 1.0f;
        }
    }

    // Step 4: Postprocess output (使用归一化后的值)
    postprocess_output(normalized_output, AI_NETWORK_OUT_1_SIZE, gesture_id, confidence);

    return 0;  // Success
}

/**
 * @brief Run gesture recognition and get all class confidences
 * @param image_data: input image data (uint8_t, 64x64 grayscale)
 * @param gesture_id: output - recognized gesture ID (0-8)
 * @param confidence: output - confidence level (0.0-1.0) of the max class
 * @param all_confidences: output - array of 9 confidences (one per class)
 * @return 0 on success, negative on error
 */
int MX_X_CUBE_AI_Inference_Full(const uint8_t* image_data, uint8_t* gesture_id,
                                 float* confidence, float* all_confidences)
{
    ai_i32 batch;

    // Check if network is initialized
    if (!network) {
        return -1;  // Network not initialized
    }

    // Check input parameters
    if (!image_data || !gesture_id || !confidence || !all_confidences) {
        return -2;  // Invalid parameters
    }

    // Step 1: Preprocess image data (uint8 -> float, normalize)
    float* input_buffer = (float*)data_ins[0];
    preprocess_image(image_data, input_buffer, AI_NETWORK_IN_1_SIZE);

    // Step 2: Run inference
    batch = ai_network_run(network, ai_input, ai_output);
    if (batch != 1) {
        return -3;  // Inference failed
    }

    // Step 3: Get output buffer and normalize (manual softmax)
    float* output_buffer = (float*)data_outs[0];

    // 计算所有输出的和（用于归一化）
    float sum = 0.0f;
    for (uint8_t i = 0; i < AI_NETWORK_OUT_1_SIZE; i++) {
        sum += output_buffer[i];
    }

    // 避免除以0
    if (sum < 0.0001f) {
        sum = 1.0f;
    }

    // 归一化：将所有输出除以总和，确保和为1
    for (uint8_t i = 0; i < AI_NETWORK_OUT_1_SIZE; i++) {
        all_confidences[i] = output_buffer[i] / sum;

        // 额外的安全检查：确保范围在 [0, 1]
        if (all_confidences[i] < 0.0f) {
            all_confidences[i] = 0.0f;
        } else if (all_confidences[i] > 1.0f) {
            all_confidences[i] = 1.0f;
        }
    }

    // Step 4: Find max probability class (使用归一化后的值)
    postprocess_output(all_confidences, AI_NETWORK_OUT_1_SIZE, gesture_id, confidence);

    return 0;  // Success
}

int acquire_and_process_data(ai_i8* data[])
{
  /* fill the inputs of the c-model
  for (int idx=0; idx < AI_NETWORK_IN_NUM; idx++ )
  {
      data[idx] = ....
  }

  */
  return 0;
}

int post_process(ai_i8* data[])
{
  /* process the predictions
  for (int idx=0; idx < AI_NETWORK_OUT_NUM; idx++ )
  {
      data[idx] = ....
  }

  */
  return 0;
}
/* USER CODE END 2 */

/* Entry points --------------------------------------------------------------*/

void MX_X_CUBE_AI_Init(void)
{
    /* USER CODE BEGIN 5 */
  //printf("\r\nTEMPLATE - initialization\r\n");

  // LED 指示：AI 初始化开始（LED3 亮）
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);

  ai_boostrap(data_activations0);

  // LED 指示：AI 初始化完成（LED3 灭）
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
    /* USER CODE END 5 */
}

void MX_X_CUBE_AI_Process(void)
{
    /* USER CODE BEGIN 6 */
  int res = -1;

  //printf("TEMPLATE - run - main loop\r\n");

  if (network) {

    do {
      /* 1 - acquire and pre-process input data */
      res = acquire_and_process_data(data_ins);
      /* 2 - process the data - call inference engine */
      if (res == 0)
        res = ai_run();
      /* 3- post-process the predictions */
      if (res == 0)
        res = post_process(data_outs);
    } while (res==0);
  }

  if (res) {
    ai_error err = {AI_ERROR_INVALID_STATE, AI_ERROR_CODE_NETWORK};
    ai_log_err(err, "Process has FAILED");
  }
    /* USER CODE END 6 */
}
#ifdef __cplusplus
}
#endif
