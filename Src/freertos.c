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

extern UART_HandleTypeDef huart1;

// 全局句柄别名（指向CubeMX生成的Handle）
// 这些在task_common.h中声明为extern
osMessageQueueId_t imageQueue;
osMessageQueueId_t cmdQueue;
osMessageQueueId_t resultQueue;
osMutexId_t cameraMutex;
osMutexId_t uartMutex;
osSemaphoreId_t frameReadySem;
osSemaphoreId_t uartTxSem;

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
/* Definitions for Task_openmv_rx */
osThreadId_t Task_openmv_rxHandle;
const osThreadAttr_t Task_openmv_rx_attributes = {
  .name = "Task_openmv_rx",
  .stack_size = 2048 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal5,
};
/* Definitions for Task_uart_tx */
osThreadId_t Task_uart_txHandle;
const osThreadAttr_t Task_uart_tx_attributes = {
  .name = "Task_uart_tx",
  .stack_size = 2048 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal6,
};
/* Definitions for Task_uart_rx */
osThreadId_t Task_uart_rxHandle;
const osThreadAttr_t Task_uart_rx_attributes = {
  .name = "Task_uart_rx",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for Task_inference */
osThreadId_t Task_inferenceHandle;
const osThreadAttr_t Task_inference_attributes = {
  .name = "Task_inference",
  .stack_size = 4096 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for imageQueue */
osMessageQueueId_t imageQueueHandle;
const osMessageQueueAttr_t imageQueue_attributes = {
  .name = "imageQueue"
};
/* Definitions for cmdQueue */
osMessageQueueId_t cmdQueueHandle;
const osMessageQueueAttr_t cmdQueue_attributes = {
  .name = "cmdQueue"
};
/* Definitions for resultQueue */
osMessageQueueId_t resultQueueHandle;
const osMessageQueueAttr_t resultQueue_attributes = {
  .name = "resultQueue"
};
/* Definitions for cameraMutex */
osMutexId_t cameraMutexHandle;
const osMutexAttr_t cameraMutex_attributes = {
  .name = "cameraMutex"
};
/* Definitions for uartMutex */
osMutexId_t uartMutexHandle;
const osMutexAttr_t uartMutex_attributes = {
  .name = "uartMutex"
};
/* Definitions for frameReadySem */
osSemaphoreId_t frameReadySemHandle;
const osSemaphoreAttr_t frameReadySem_attributes = {
  .name = "frameReadySem"
};
/* Definitions for uartTxSem */
osSemaphoreId_t uartTxSemHandle;
const osSemaphoreAttr_t uartTxSem_attributes = {
  .name = "uartTxSem"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void AppTask_led_proc(void *argument);
void AppTask_key_read(void *argument);
void AppTask_key_proc(void *argument);
void AppTask_oled_disp(void *argument);
void AppTask_openmv_rx(void *argument);
void AppTask_uart_tx(void *argument);
void AppTask_uart_rx(void *argument);
void AppTask_inference(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* creation of cameraMutex */
  cameraMutexHandle = osMutexNew(&cameraMutex_attributes);

  /* creation of uartMutex */
  uartMutexHandle = osMutexNew(&uartMutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */

  // 初始化全局句柄别名
  cameraMutex = cameraMutexHandle;
  uartMutex = uartMutexHandle;

  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of frameReadySem */
  frameReadySemHandle = osSemaphoreNew(1, 1, &frameReadySem_attributes);

  /* creation of uartTxSem */
  uartTxSemHandle = osSemaphoreNew(1, 1, &uartTxSem_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */

  // 初始化全局句柄别名
  frameReadySem = frameReadySemHandle;
  uartTxSem = uartTxSemHandle;

  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of imageQueue */
  imageQueueHandle = osMessageQueueNew (2, 20, &imageQueue_attributes);

  /* creation of cmdQueue */
  cmdQueueHandle = osMessageQueueNew (8, 64, &cmdQueue_attributes);

  /* creation of resultQueue */
  resultQueueHandle = osMessageQueueNew (4, 12, &resultQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */

  // 初始化全局句柄别名
  imageQueue = imageQueueHandle;
  cmdQueue = cmdQueueHandle;
  resultQueue = resultQueueHandle;

  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of Task_led_proc */
  Task_led_procHandle = osThreadNew(AppTask_led_proc, NULL, &Task_led_proc_attributes);

  /* creation of Task_key_read */
  Task_key_readHandle = osThreadNew(AppTask_key_read, NULL, &Task_key_read_attributes);

  /* creation of Task_key_proc */
  Task_key_procHandle = osThreadNew(AppTask_key_proc, NULL, &Task_key_proc_attributes);

  /* creation of Task_oled_disp */
  Task_oled_dispHandle = osThreadNew(AppTask_oled_disp, NULL, &Task_oled_disp_attributes);

  /* creation of Task_openmv_rx */
  Task_openmv_rxHandle = osThreadNew(AppTask_openmv_rx, NULL, &Task_openmv_rx_attributes);

  /* creation of Task_uart_tx */
  Task_uart_txHandle = osThreadNew(AppTask_uart_tx, NULL, &Task_uart_tx_attributes);

  /* creation of Task_uart_rx */
  Task_uart_rxHandle = osThreadNew(AppTask_uart_rx, NULL, &Task_uart_rx_attributes);

  /* creation of Task_inference */
  Task_inferenceHandle = osThreadNew(AppTask_inference, NULL, &Task_inference_attributes);

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

/* USER CODE BEGIN Header_AppTask_key_proc */
/**
* @brief Function implementing the Task_key_proc thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_AppTask_key_proc */
void AppTask_key_proc(void *argument)
{
  /* USER CODE BEGIN AppTask_key_proc */
	TickType_t pxPrevioisWakeTime = xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {
	  key_proc();
    vTaskDelayUntil(&pxPrevioisWakeTime, pdMS_TO_TICKS(20));
  }
  /* USER CODE END AppTask_key_proc */
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
  //osThreadSuspend(NULL);
  /* Infinite loop */
  for(;;)
  {
    cnt1++;
    oled_disp();
    //OLED_ShowGestureResult();
    vTaskDelayUntil(&pxPrevioisWakeTime, pdMS_TO_TICKS(30));
  }
  /* USER CODE END AppTask_oled_disp */
}

/* USER CODE BEGIN Header_AppTask_openmv_rx */
/**
* @brief Function implementing the Task_openmv_rx thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_AppTask_openmv_rx */
__weak void AppTask_openmv_rx(void *argument)
{
  /* USER CODE BEGIN AppTask_openmv_rx */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END AppTask_openmv_rx */
}

/* USER CODE BEGIN Header_AppTask_uart_tx */
/**
* @brief Function implementing the Task_uart_tx thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_AppTask_uart_tx */
__weak void AppTask_uart_tx(void *argument)
{
  /* USER CODE BEGIN AppTask_uart_tx */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END AppTask_uart_tx */
}

/* USER CODE BEGIN Header_AppTask_uart_rx */
/**
* @brief Function implementing the Task_uart_rx thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_AppTask_uart_rx */
__weak void AppTask_uart_rx(void *argument)
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

/* USER CODE BEGIN Header_AppTask_inference */
/**
* @brief Function implementing the Task_inference thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_AppTask_inference */
__weak void AppTask_inference(void *argument)
{
  /* USER CODE BEGIN AppTask_inference */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END AppTask_inference */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */


/* USER CODE END Application */

