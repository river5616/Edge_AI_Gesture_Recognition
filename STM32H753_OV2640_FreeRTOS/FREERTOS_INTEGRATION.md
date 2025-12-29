# freertos.c 修改示例

本文档展示如何修改CubeMX生成的 `Src/freertos.c` 文件以集成手势识别功能。

## ⚠️ 重要提示

**所有修改必须在 `USER CODE` 区域进行，否则重新生成代码时会被覆盖！**

---

## 📝 完整的freertos.c示例

```c
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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

// 添加任务头文件
#include "task_common.h"
#include "camera_task.h"
#include "uart_rx_task.h"
#include "uart_tx_task.h"
#include "inference_task.h"

// 添加驱动头文件
#include "ov2640.h"

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

// 将CubeMX生成的句柄映射到task_common.h中声明的名称
#define imageQueue    imageQueueHandle
#define cmdQueue      cmdQueueHandle
#define resultQueue   resultQueueHandle
#define frameReadySem frameReadySemHandle
#define uartTxSem     uartTxSemHandle
#define cameraMutex   cameraMutexHandle
#define uartMutex     uartMutexHandle

/* USER CODE END Variables */

/* Definitions for Task_cam_capture */
osThreadId_t Task_cam_captureHandle;
const osThreadAttr_t Task_cam_capture_attributes = {
  .name = "Task_cam_capture",
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

/* Definitions for Task_uart_tx */
osThreadId_t Task_uart_txHandle;
const osThreadAttr_t Task_uart_tx_attributes = {
  .name = "Task_uart_tx",
  .stack_size = 2048 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Definitions for Task_inference */
osThreadId_t Task_inferenceHandle;
const osThreadAttr_t Task_inference_attributes = {
  .name = "Task_inference",
  .stack_size = 4096 * 4,
  .priority = (osPriority_t) osPriorityNormal,
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

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void AppTask_cam_capture(void *argument);
void AppTask_uart_rx(void *argument);
void AppTask_uart_tx(void *argument);
void AppTask_inference(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  // 初始化全局变量
  g_work_mode = MODE_IDLE;
  g_gesture_id = 0;
  g_target_images = 0;
  g_current_images = 0;
  g_current_fps = 0.0f;

  /* USER CODE END Init */

  /* Create the mutex(es) */
  /* creation of cameraMutex */
  cameraMutexHandle = osMutexNew(&cameraMutex_attributes);

  /* creation of uartMutex */
  uartMutexHandle = osMutexNew(&uartMutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of frameReadySem */
  frameReadySemHandle = osSemaphoreNew(1, 0, &frameReadySem_attributes);

  /* creation of uartTxSem */
  uartTxSemHandle = osSemaphoreNew(1, 1, &uartTxSem_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of imageQueue */
  imageQueueHandle = osMessageQueueNew(2, 20, &imageQueue_attributes);

  /* creation of cmdQueue */
  cmdQueueHandle = osMessageQueueNew(8, 64, &cmdQueue_attributes);

  /* creation of resultQueue */
  resultQueueHandle = osMessageQueueNew(4, 12, &resultQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of Task_cam_capture */
  Task_cam_captureHandle = osThreadNew(AppTask_cam_capture, NULL, &Task_cam_capture_attributes);

  /* creation of Task_uart_rx */
  Task_uart_rxHandle = osThreadNew(AppTask_uart_rx, NULL, &Task_uart_rx_attributes);

  /* creation of Task_uart_tx */
  Task_uart_txHandle = osThreadNew(AppTask_uart_tx, NULL, &Task_uart_tx_attributes);

  /* creation of Task_inference */
  Task_inferenceHandle = osThreadNew(AppTask_inference, NULL, &Task_inference_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_AppTask_cam_capture */
/**
  * @brief  摄像头采集任务（实现在camera_task.c）
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_AppTask_cam_capture */
__weak void AppTask_cam_capture(void *argument)
{
  /* USER CODE BEGIN AppTask_cam_capture */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END AppTask_cam_capture */
}

/* USER CODE BEGIN Header_AppTask_uart_rx */
/**
  * @brief  串口接收任务（实现在uart_rx_task.c）
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_AppTask_uart_rx */
__weak void AppTask_uart_rx(void *argument)
{
  /* USER CODE BEGIN AppTask_uart_rx */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END AppTask_uart_rx */
}

/* USER CODE BEGIN Header_AppTask_uart_tx */
/**
  * @brief  串口发送任务（实现在uart_tx_task.c）
  * @param  argument: Not used
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

/* USER CODE BEGIN Header_AppTask_inference */
/**
  * @brief  AI推理任务（实现在inference_task.c）
  * @param  argument: Not used
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
```

---

## 🔑 关键修改点说明

### 1. 添加头文件（USER CODE BEGIN Includes）
```c
#include "task_common.h"
#include "camera_task.h"
#include "uart_rx_task.h"
#include "uart_tx_task.h"
#include "inference_task.h"
#include "ov2640.h"
```

### 2. 句柄名称映射（USER CODE BEGIN Variables）
```c
#define imageQueue    imageQueueHandle
#define cmdQueue      cmdQueueHandle
// ... 其他映射
```

这样做的原因：
- CubeMX生成的句柄名称有 `Handle` 后缀
- 你的代码中使用的是不带 `Handle` 的名称
- 通过宏定义映射，避免修改所有代码

### 3. 初始化全局变量（USER CODE BEGIN Init）
```c
g_work_mode = MODE_IDLE;
g_gesture_id = 0;
g_target_images = 0;
g_current_images = 0;
g_current_fps = 0.0f;
```

### 4. 弱定义的任务函数
CubeMX生成的任务函数带有 `__weak` 属性：
```c
__weak void AppTask_cam_capture(void *argument)
{
    // ...
}
```

你在 `camera_task.c` 中实现的同名函数会覆盖这个弱定义。

---

## 📂 项目文件结构

```
YourProject/
├── Core/
│   ├── Inc/
│   │   ├── main.h
│   │   └── FreeRTOSConfig.h
│   └── Src/
│       ├── main.c
│       └── freertos.c  ← 本文档修改的文件
├── Drivers/
│   ├── OV2640/
│   │   ├── ov2640.h/c
│   │   ├── sccb.h/c
│   │   └── ov2640cfg.h/c
│   └── Tasks/
│       ├── task_common.h/c
│       ├── camera_task.h/c
│       ├── uart_rx_task.h/c
│       ├── uart_tx_task.h/c
│       └── inference_task.h/c
└── MDK-ARM/  (或 STM32CubeIDE/)
    └── project.uvprojx
```

---

## ✅ 验证步骤

1. 修改完 `freertos.c` 后，确保所有 `USER CODE` 区域的修改正确
2. 编译项目：
   ```bash
   # 如果使用Keil
   uvprojx -b project.uvprojx

   # 如果使用GCC
   make
   ```
3. 检查编译输出，确保没有错误
4. 烧录到板子
5. 打开串口助手（921600波特率）
6. 发送 `$STATUS\r\n` 测试

---

## 🐛 常见问题

### Q1: 编译时提示 "undefined reference to AppTask_xxx"
**A:** 确保任务实现文件（如camera_task.c）已添加到项目中并编译。

### Q2: 运行时Hard Fault
**A:**
1. 检查堆栈大小是否足够
2. 检查句柄是否正确初始化（不为NULL）
3. 使用调试器查看fault寄存器

### Q3: 队列操作总是失败
**A:**
1. 确认队列已创建：`if(imageQueueHandle != NULL)`
2. 检查队列项大小是否匹配
3. 检查队列是否已满

### Q4: 重新生成代码后修改丢失
**A:** 确保所有修改都在 `USER CODE BEGIN/END` 区域内！

---

## 📖 下一步

1. 将所有任务代码文件添加到项目
2. 将OV2640驱动文件添加到项目
3. 配置链接脚本（如果使用DTCM）
4. 编译、调试、测试

完整的项目集成指南请参考 `README_FREERTOS.md`。
