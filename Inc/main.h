/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define KEY_Pin GPIO_PIN_13
#define KEY_GPIO_Port GPIOC
#define VSYNC_Pin GPIO_PIN_3
#define VSYNC_GPIO_Port GPIOF
#define SCL_Pin GPIO_PIN_10
#define SCL_GPIO_Port GPIOF
#define KEY0_Pin GPIO_PIN_0
#define KEY0_GPIO_Port GPIOA
#define LED1_Pin GPIO_PIN_0
#define LED1_GPIO_Port GPIOB
#define HREF_Pin GPIO_PIN_12
#define HREF_GPIO_Port GPIOF
#define RST_Pin GPIO_PIN_13
#define RST_GPIO_Port GPIOF
#define NC_Pin GPIO_PIN_14
#define NC_GPIO_Port GPIOF
#define SDA_Pin GPIO_PIN_7
#define SDA_GPIO_Port GPIOE
#define D0_Pin GPIO_PIN_8
#define D0_GPIO_Port GPIOE
#define D1_Pin GPIO_PIN_9
#define D1_GPIO_Port GPIOE
#define D2_Pin GPIO_PIN_10
#define D2_GPIO_Port GPIOE
#define D3_Pin GPIO_PIN_11
#define D3_GPIO_Port GPIOE
#define D4_Pin GPIO_PIN_12
#define D4_GPIO_Port GPIOE
#define D5_Pin GPIO_PIN_13
#define D5_GPIO_Port GPIOE
#define D6_Pin GPIO_PIN_14
#define D6_GPIO_Port GPIOE
#define D7_Pin GPIO_PIN_15
#define D7_GPIO_Port GPIOE
#define LED3_Pin GPIO_PIN_14
#define LED3_GPIO_Port GPIOB
#define KEY1_Pin GPIO_PIN_11
#define KEY1_GPIO_Port GPIOD
#define OLED_SCL_Pin GPIO_PIN_12
#define OLED_SCL_GPIO_Port GPIOD
#define OLED_SDA_Pin GPIO_PIN_13
#define OLED_SDA_GPIO_Port GPIOD
#define PCLK_Pin GPIO_PIN_14
#define PCLK_GPIO_Port GPIOD
#define PWDN_Pin GPIO_PIN_15
#define PWDN_GPIO_Port GPIOD
#define LED2_Pin GPIO_PIN_1
#define LED2_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
