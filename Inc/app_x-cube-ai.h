
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_AI_H
#define __APP_AI_H
#ifdef __cplusplus
extern "C" {
#endif
/**
  ******************************************************************************
  * @file    app_x-cube-ai.h
  * @author  X-CUBE-AI C code generator
  * @brief   AI entry function definitions
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
/* Includes ------------------------------------------------------------------*/
#include "ai_platform.h"

void MX_X_CUBE_AI_Init(void);
void MX_X_CUBE_AI_Process(void);

/* Gesture recognition interface */
/**
 * @brief Run gesture recognition inference
 * @param image_data: input image (uint8_t, 64x64 grayscale)
 * @param gesture_id: output recognized gesture ID (0-4, 5个手势: fist/palm/v_sign/ok/nothing)
 * @param confidence: output confidence level (0.0-1.0)
 * @return 0 on success, negative on error
 */
int MX_X_CUBE_AI_Inference(const uint8_t* image_data, uint8_t* gesture_id, float* confidence);

/**
 * @brief Run gesture recognition and get all class confidences
 * @param image_data: input image (uint8_t, 64x64 grayscale)
 * @param gesture_id: output recognized gesture ID (0-4, 5个手势: fist/palm/v_sign/ok/nothing)
 * @param confidence: output confidence level (0.0-1.0) of the max class
 * @param all_confidences: output array of 5 confidences (one per class)
 * @return 0 on success, negative on error
 */
int MX_X_CUBE_AI_Inference_Full(const uint8_t* image_data, uint8_t* gesture_id,
                                 float* confidence, float* all_confidences);

/* USER CODE BEGIN includes */
/* USER CODE END includes */
#ifdef __cplusplus
}
#endif
#endif /*__STMicroelectronics_X-CUBE-AI_10_2_0_H */
