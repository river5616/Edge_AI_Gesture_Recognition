/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ov2640.h"
#include "camera_app.h"
#include "uart_protocol.h"
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern UART_HandleTypeDef huart1;

// 串口接收缓冲区
static uint8_t uart_rx_buffer[256];
static uint16_t uart_rx_index = 0;
static uint8_t uart_rx_byte;
/* USER CODE END Variables */

/* Definitions for Task_camera */
osThreadId_t Task_cameraHandle;
const osThreadAttr_t Task_camera_attributes = {
  .name = "Task_camera",
  .stack_size = 2048 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Definitions for Task_uart_rx */
osThreadId_t Task_uart_rxHandle;
const osThreadAttr_t Task_uart_rx_attributes = {
  .name = "Task_uart_rx",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void AppTask_camera(void *argument);
void AppTask_uart_rx(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of Task_camera */
  Task_cameraHandle = osThreadNew(AppTask_camera, NULL, &Task_camera_attributes);

  /* creation of Task_uart_rx */
  Task_uart_rxHandle = osThreadNew(AppTask_uart_rx, NULL, &Task_uart_rx_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */

  // 启动串口中断接收
  HAL_UART_Receive_IT(&huart1, &uart_rx_byte, 1);
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_AppTask_camera */
/**
  * @brief  摄像头采集任务
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_AppTask_camera */
void AppTask_camera(void *argument)
{
  /* USER CODE BEGIN AppTask_camera */
  TickType_t pxPreviousWakeTime = xTaskGetTickCount();
  uint8_t* frame_data;
  uint32_t last_fps_update = 0;

  /* Infinite loop */
  for(;;)
  {
    // 获取当前工作模式
    Work_Mode_t mode = Protocol_GetMode();

    switch(mode)
    {
      case MODE_IDLE:
        // 空闲模式，降低频率
        vTaskDelayUntil(&pxPreviousWakeTime, pdMS_TO_TICKS(100));
        break;

      case MODE_COLLECTING:
        // 数据采集模式
        Camera_App_Start();

        // 捕获一帧
        if(Camera_App_CaptureFrame() == 0)
        {
          // 获取图像数据
          frame_data = Camera_App_GetFrame();

          if(frame_data != NULL)
          {
            // 发送图像到PC
            Protocol_SendImage(frame_data, IMAGE_WIDTH, IMAGE_HEIGHT);

            // 更新计数并发送进度
            Protocol_IncrementImageCount();
          }
        }

        // 更新FPS
        Camera_App_UpdateFPS();

        // 每秒发送一次FPS
        uint32_t current_time = HAL_GetTick();
        if(current_time - last_fps_update >= 1000)
        {
          float fps = Camera_App_GetFPS();
          Protocol_SendFPS(fps);
          last_fps_update = current_time;
        }

        vTaskDelayUntil(&pxPreviousWakeTime, pdMS_TO_TICKS(50));
        break;

      case MODE_INFERENCE:
        // 推理模式
        Camera_App_Start();

        // 捕获一帧
        if(Camera_App_CaptureFrame() == 0)
        {
          frame_data = Camera_App_GetFrame();

          if(frame_data != NULL)
          {
            // TODO: 调用AI推理
            // uint8_t predicted_gesture = AI_Inference(frame_data);
            // float confidence = AI_GetConfidence();
            // Protocol_SendResult(predicted_gesture, confidence);

            // 临时：发送图像用于调试
            Protocol_SendImage(frame_data, IMAGE_WIDTH, IMAGE_HEIGHT);
          }
        }

        // 更新FPS
        Camera_App_UpdateFPS();

        if(HAL_GetTick() - last_fps_update >= 1000)
        {
          float fps = Camera_App_GetFPS();
          Protocol_SendFPS(fps);
          last_fps_update = HAL_GetTick();
        }

        vTaskDelayUntil(&pxPreviousWakeTime, pdMS_TO_TICKS(50));
        break;
    }
  }
  /* USER CODE END AppTask_camera */
}

/* USER CODE BEGIN Header_AppTask_uart_rx */
/**
* @brief 串口接收任务
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_AppTask_uart_rx */
void AppTask_uart_rx(void *argument)
{
  /* USER CODE BEGIN AppTask_uart_rx */
  TickType_t pxPreviousWakeTime = xTaskGetTickCount();

  /* Infinite loop */
  for(;;)
  {
    // 串口接收在中断中处理，这里只做定期检查
    vTaskDelayUntil(&pxPreviousWakeTime, pdMS_TO_TICKS(10));
  }
  /* USER CODE END AppTask_uart_rx */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/**
  * @brief  UART接收完成回调
  * @param  huart: UART句柄
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart->Instance == USART1)
  {
    // 将接收到的字节添加到缓冲区
    if(uart_rx_index < sizeof(uart_rx_buffer) - 1)
    {
      uart_rx_buffer[uart_rx_index++] = uart_rx_byte;

      // 检查是否接收到完整命令（以\n结尾）
      if(uart_rx_byte == '\n')
      {
        uart_rx_buffer[uart_rx_index] = '\0';

        // 处理命令
        Protocol_ProcessCommand(uart_rx_buffer, uart_rx_index);

        // 重置缓冲区
        uart_rx_index = 0;
      }
    }
    else
    {
      // 缓冲区溢出，重置
      uart_rx_index = 0;
    }

    // 继续接收下一个字节
    HAL_UART_Receive_IT(&huart1, &uart_rx_byte, 1);
  }
}

/* USER CODE END Application */
