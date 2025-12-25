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

#include "headfile.h"

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

/* USER CODE END Variables */
/* Definitions for Task_led_proc */
osThreadId_t Task_led_procHandle;
const osThreadAttr_t Task_led_proc_attributes = {
  .name = "Task_led_proc",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Task_key_read */
osThreadId_t Task_key_readHandle;
const osThreadAttr_t Task_key_read_attributes = {
  .name = "Task_key_read",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityRealtime4,
};
/* Definitions for Task_key_proc */
osThreadId_t Task_key_procHandle;
const osThreadAttr_t Task_key_proc_attributes = {
  .name = "Task_key_proc",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityHigh1,
};
/* Definitions for Task_oled_disp */
osThreadId_t Task_oled_dispHandle;
const osThreadAttr_t Task_oled_disp_attributes = {
  .name = "Task_oled_disp",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal1,
};
/* Definitions for Task_adc_proc */
osThreadId_t Task_adc_procHandle;
const osThreadAttr_t Task_adc_proc_attributes = {
  .name = "Task_adc_proc",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Task_uart_tx_proc */
osThreadId_t Task_uart_tx_procHandle;
const osThreadAttr_t Task_uart_tx_proc_attributes = {
  .name = "Task_uart_tx_proc",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityHigh3,
};
/* Definitions for Task_camera */
osThreadId_t Task_cameraHandle;
const osThreadAttr_t Task_camera_attributes = {
  .name = "Task_camera",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void AppTask_led_proc(void *argument);
void AppTask_key_read(void *argument);
void StartTask03(void *argument);
void AppTask_oled_disp(void *argument);
void AppTask_adc_proc(void *argument);
void AppTask_uart_tx_proc(void *argument);
void AppTask_camera(void *argument);

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
  /* creation of Task_led_proc */
  Task_led_procHandle = osThreadNew(AppTask_led_proc, NULL, &Task_led_proc_attributes);

  /* creation of Task_key_read */
  Task_key_readHandle = osThreadNew(AppTask_key_read, NULL, &Task_key_read_attributes);

  /* creation of Task_key_proc */
  Task_key_procHandle = osThreadNew(StartTask03, NULL, &Task_key_proc_attributes);

  /* creation of Task_oled_disp */
  Task_oled_dispHandle = osThreadNew(AppTask_oled_disp, NULL, &Task_oled_disp_attributes);

  /* creation of Task_adc_proc */
  Task_adc_procHandle = osThreadNew(AppTask_adc_proc, NULL, &Task_adc_proc_attributes);

  /* creation of Task_uart_tx_proc */
  Task_uart_tx_procHandle = osThreadNew(AppTask_uart_tx_proc, NULL, &Task_uart_tx_proc_attributes);

  /* creation of Task_camera */
  Task_cameraHandle = osThreadNew(AppTask_camera, NULL, &Task_camera_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_AppTask_led_proc */
/**
  * @brief  Function implementing the Task_led_proc thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_AppTask_led_proc */
void AppTask_led_proc(void *argument)
{
  /* USER CODE BEGIN AppTask_led_proc */
  TickType_t pxPrevioisWakeTime = xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {
    LED1_Toggle;
    vTaskDelayUntil(&pxPrevioisWakeTime, pdMS_TO_TICKS(1000));
  }
  /* USER CODE END AppTask_led_proc */
}

/* USER CODE BEGIN Header_AppTask_key_read */
/**
* @brief Function implementing the Task_key_read thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_AppTask_key_read */
void AppTask_key_read(void *argument)
{
  /* USER CODE BEGIN AppTask_key_read */
  TickType_t pxPrevioisWakeTime = xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {
    key_task();
    vTaskDelayUntil(&pxPrevioisWakeTime, pdMS_TO_TICKS(10));
  }
  /* USER CODE END AppTask_key_read */
}

/* USER CODE BEGIN Header_StartTask03 */
/**
* @brief Function implementing the Task_key_proc thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask03 */
void StartTask03(void *argument)
{
  /* USER CODE BEGIN StartTask03 */
  TickType_t pxPrevioisWakeTime = xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {
    key_proc();
    vTaskDelayUntil(&pxPrevioisWakeTime, pdMS_TO_TICKS(20));
  }
  /* USER CODE END StartTask03 */
}

/* USER CODE BEGIN Header_AppTask_oled_disp */
/**
* @brief Function implementing the Task_oled_disp thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_AppTask_oled_disp */
void AppTask_oled_disp(void *argument)
{
  /* USER CODE BEGIN AppTask_oled_disp */
  TickType_t pxPrevioisWakeTime = xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {
    cnt++;
    oled_disp();
    vTaskDelayUntil(&pxPrevioisWakeTime, pdMS_TO_TICKS(50));
  }
  /* USER CODE END AppTask_oled_disp */
}

/* USER CODE BEGIN Header_AppTask_adc_proc */
/**
* @brief Function implementing the Task_adc_proc thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_AppTask_adc_proc */
void AppTask_adc_proc(void *argument)
{
  /* USER CODE BEGIN AppTask_adc_proc */
  TickType_t pxPrevioisWakeTime = xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {
    vTaskDelayUntil(&pxPrevioisWakeTime, pdMS_TO_TICKS(1000));
  }
  /* USER CODE END AppTask_adc_proc */
}

/* USER CODE BEGIN Header_AppTask_uart_tx_proc */
/**
* @brief Function implementing the Task_uart_tx_proc thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_AppTask_uart_tx_proc */
void AppTask_uart_tx_proc(void *argument)
{
  /* USER CODE BEGIN AppTask_uart_tx_proc */
  TickType_t pxPrevioisWakeTime = xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {
    vTaskDelayUntil(&pxPrevioisWakeTime, pdMS_TO_TICKS(20));
  }
  /* USER CODE END AppTask_uart_tx_proc */
}

/* USER CODE BEGIN Header_AppTask_camera */
/**
* @brief Function implementing the Task_camera thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_AppTask_camera */
void AppTask_camera(void *argument)
{
  /* USER CODE BEGIN AppTask_camera */
  TickType_t pxPrevioisWakeTime = xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {
    vTaskDelayUntil(&pxPrevioisWakeTime, pdMS_TO_TICKS(50));
  }
  /* USER CODE END AppTask_camera */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

