# STM32CubeMX FreeRTOS配置指南

## 1. 启用FreeRTOS

### 步骤1：打开CubeMX项目
1. 打开你的 `.ioc` 文件
2. 在左侧导航栏找到 **Middleware and Software Packs**

### 步骤2：启用FreeRTOS
1. 点击 **FREERTOS**
2. 勾选 **Interface**: `CMSIS_V2`（推荐）或 `CMSIS_V1`
3. 模式会自动设置

## 2. FreeRTOS基本配置

### Configuration标签页配置

#### 2.1 Kernel Settings（内核设置）
```
✓ USE_PREEMPTION: Enabled（启用抢占式调度）
✓ CPU_CLOCK_HZ: 根据你的系统时钟自动设置（如480000000 for H7）
✓ TICK_RATE_HZ: 1000（1ms一个时钟节拍）
✓ MAX_PRIORITIES: 56（优先级数量，默认即可）
✓ MINIMAL_STACK_SIZE: 128（最小堆栈大小，单位：字）
✓ MAX_TASK_NAME_LEN: 16
```

#### 2.2 Memory Management（内存管理）
```
Memory Management scheme: heap_4（推荐）

说明：
- heap_1: 最简单，不支持释放
- heap_2: 支持释放，但不合并碎片
- heap_3: 使用标准malloc/free
- heap_4: 支持释放和碎片合并（推荐）
- heap_5: heap_4的扩展版，支持不连续内存
```

**TOTAL_HEAP_SIZE（总堆大小）**：
```
推荐值：
- STM32F4: 15360 bytes (15KB)
- STM32H7: 32768 bytes (32KB) 或更大
- 根据实际任务数量和栈大小调整
```

#### 2.3 Hook Function Callback（钩子函数）
```
✓ USE_IDLE_HOOK: Disabled（通常不需要）
✓ USE_TICK_HOOK: Disabled（通常不需要）
✓ USE_MALLOC_FAILED_HOOK: Enabled（推荐，用于调试内存问题）
✓ USE_DAEMON_TASK_STARTUP_HOOK: Disabled
✓ CHECK_FOR_STACK_OVERFLOW: Option1或Option2（推荐启用）
```

#### 2.4 Software Timer（软件定时器）
```
✓ USE_TIMERS: Enabled（如果需要软件定时器）
✓ TIMER_TASK_PRIORITY: 2（定时器任务优先级）
✓ TIMER_QUEUE_LENGTH: 10
✓ TIMER_TASK_STACK_DEPTH: 256
```

## 3. 创建任务

### 方法1：在CubeMX中创建（推荐）

#### Tasks and Queues标签页

**添加任务：**
1. 点击 **Add** 按钮
2. 配置任务参数：

**示例：传感器任务**
```
Task Name: SensorTask
Priority: Normal (osPriorityNormal)
Stack Size (Words): 512
Entry Function: SensorTask
Code Generation Option: As weak（或As external）
```

**示例：OLED显示任务**
```
Task Name: DisplayTask
Priority: Normal (osPriorityNormal)
Stack Size (Words): 256
Entry Function: DisplayTask
```

**示例：推理任务**
```
Task Name: InferenceTask
Priority: High (osPriorityHigh)
Stack Size (Words): 2048（AI推理需要较大栈）
Entry Function: InferenceTask
```

### 方法2：手动创建任务（在代码中）

在 `freertos.c` 的 `MX_FREERTOS_Init()` 函数中：

```c
void MX_FREERTOS_Init(void)
{
  osThreadAttr_t task_attributes = {
    .name = "SensorTask",
    .priority = (osPriority_t) osPriorityNormal,
    .stack_size = 512 * 4  // 单位：字节
  };
  
  osThreadNew(SensorTask, NULL, &task_attributes);
}
```

## 4. 创建队列（Queue）

### 在CubeMX中创建

**Queues标签页：**
```
Queue Name: imageQueue
Queue Size: 2（队列长度）
Item Size: 16（每个元素大小，字节）
```

**代码使用：**
```c
extern osMessageQueueId_t imageQueueHandle;

// 发送数据
osMessageQueuePut(imageQueueHandle, &data, 0, 0);

// 接收数据
osMessageQueueGet(imageQueueHandle, &data, NULL, 100);
```

## 5. 创建互斥量（Mutex）

### 在CubeMX中创建

**Mutexes标签页：**
```
Mutex Name: displayMutex
```

**代码使用：**
```c
extern osMutexId_t displayMutexHandle;

// 获取互斥量
osMutexAcquire(displayMutexHandle, osWaitForever);

// 临界区代码
// ...

// 释放互斥量
osMutexRelease(displayMutexHandle);
```

## 6. 创建信号量（Semaphore）

### 在CubeMX中创建

**Semaphores标签页：**
```
Semaphore Name: frameSemaphore
Type: Binary（二进制）或Counting（计数）
Count: 1
```

**代码使用：**
```c
extern osSemaphoreId_t frameSemaphoreHandle;

// 等待信号量
osSemaphoreAcquire(frameSemaphoreHandle, osWaitForever);

// 释放信号量
osSemaphoreRelease(frameSemaphoreHandle);
```

## 7. 任务优先级建议

```
优先级           | 任务类型                    | 示例
----------------|----------------------------|------------------
osPriorityHigh  | 实时性要求高的任务           | 推理任务、数据采集
osPriorityAboveNormal | 重要但非实时任务        | 图像处理
osPriorityNormal | 普通任务                   | 传感器读取、显示
osPriorityBelowNormal | 低优先级任务            | 日志记录
osPriorityLow   | 后台任务                   | 统计、清理
```

## 8. 栈大小建议

```
任务类型              | 栈大小（Words）| 栈大小（字节）| 说明
--------------------|--------------|--------------|------------------
简单任务（LED闪烁）   | 128          | 512          | 最小栈
传感器读取           | 256-512      | 1-2KB        | 中等栈
OLED显示            | 256-512      | 1-2KB        | 中等栈
图像处理             | 1024-2048    | 4-8KB        | 大栈
AI推理              | 2048-4096    | 8-16KB       | 超大栈
```

**注意：STM32H7需要更大的栈，建议在上述基础上×1.5-2**

## 9. 常见问题和解决方法

### 问题1：任务栈溢出
**现象**：程序运行一段时间后崩溃或进入HardFault

**解决**：
1. 启用栈溢出检查：`CHECK_FOR_STACK_OVERFLOW = Option2`
2. 增加任务栈大小
3. 在任务中使用 `uxTaskGetStackHighWaterMark()` 检查剩余栈

### 问题2：堆内存不足
**现象**：创建任务/队列失败，或malloc返回NULL

**解决**：
1. 增加 `TOTAL_HEAP_SIZE`
2. 检查是否有内存泄漏
3. 启用 `USE_MALLOC_FAILED_HOOK` 捕获错误

### 问题3：任务调度异常
**现象**：某些任务不执行或执行顺序异常

**解决**：
1. 检查任务优先级设置
2. 确保任务循环中有延时（osDelay）
3. 检查是否有任务一直占用CPU

## 10. 代码生成后的修改

### freertos.c中的初始化

```c
void MX_FREERTOS_Init(void)
{
  /* USER CODE BEGIN Init */
  
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* 添加互斥量初始化 */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* 添加信号量初始化 */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* 软件定时器初始化 */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* 队列初始化 */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* 在这里创建任务 */
  
  /* USER CODE BEGIN RTOS_THREADS */
  /* 添加额外的任务创建 */
  CreateSensorTask();  // 示例：调用你的任务创建函数
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* 添加事件标志组等 */
  /* USER CODE END RTOS_EVENTS */
}
```

## 11. 本项目的具体配置

### 当前任务配置

```
任务名称          | 优先级              | 栈大小    | 说明
----------------|--------------------|---------|--------------------------
DefaultTask     | osPriorityNormal   | 128     | 默认任务（可删除）
SensorTask      | osPriorityNormal   | 512     | 传感器数据读取
DisplayTask     | osPriorityNormal   | 256     | OLED显示刷新
InferenceTask   | osPriorityHigh     | 2048    | AI手势识别推理
OpenMVRxTask    | osPriorityAboveNormal | 1024 | 图像接收
```

### 队列配置

```
imageQueue:
- 队列长度: 2
- 元素大小: sizeof(ImageMsg_t)
- 用途: 传递图像数据给推理任务
```

### 显示视图切换

```c
// 在main.c或其他地方切换显示页面
extern u8 view;

// 切换到传感器视图
view = VIEW_SENSORS;  // 显示传感器数据

// 切换到状态视图
view = VIEW_STATUS;   // 显示手势识别结果

// 切换到图像视图
view = VIEW_IMAGE;    // 显示接收的图像信息

// 切换到UART统计视图
view = VIEW_UART_STATS; // 显示UART统计
```

## 12. 调试技巧

### 使用SEGGER SystemView
1. 在CubeMX中启用：Middleware → FREERTOS → Config parameters → Include parameters
2. 启用 `INCLUDE_xTaskGetIdleTaskHandle` 等调试相关选项
3. 添加SystemView库到项目

### 使用printf调试
```c
// 在任务中打印调试信息（需要重定向printf到UART）
printf("Task running, stack left: %d\r\n", 
       uxTaskGetStackHighWaterMark(NULL));
```

---

**配置完成后，记得点击 "Generate Code" 生成代码！**
