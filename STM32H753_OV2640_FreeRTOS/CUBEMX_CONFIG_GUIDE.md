# STM32CubeMX详细配置步骤 - FreeRTOS手势识别项目

## 📋 配置概览

本文档详细说明如何在STM32CubeMX中配置FreeRTOS任务、队列、信号量和互斥量。

---

## 🔧 步骤1：打开项目

1. 打开STM32CubeMX
2. 选择 **STM32H753** 芯片（或你的具体型号）
3. 或打开已有的 `.ioc` 文件

---

## ⚙️ 步骤2：配置时钟

### 2.1 RCC配置
- **Pinout & Configuration** → **System Core** → **RCC**
- High Speed Clock (HSE): **Crystal/Ceramic Resonator**
- Low Speed Clock (LSE): Disable（可选）

### 2.2 时钟树配置
进入 **Clock Configuration** 标签：

```
HSE: 25 MHz
PLLM: 5
PLLN: 192
PLLP: 2
→ SYSCLK: 480 MHz
→ AHB: 240 MHz
→ APB1/2: 120 MHz
```

---

## 🔌 步骤3：配置GPIO

### 3.1 摄像头引脚（按照你的连接）

| 功能 | 引脚 | 配置 | 说明 |
|------|------|------|------|
| **SCCB** |  |  |  |
| SCL | PD7 | GPIO_Output | SCCB时钟 |
| SDA | PD6 | GPIO_Output | SCCB数据（代码中动态切换） |
| **数据线** |  |  |  |
| D0 | PD5 | GPIO_Input, Pull-up | 数据位0 |
| D1 | PB1 | GPIO_Input, Pull-up | 数据位1 |
| D2 | PD4 | GPIO_Input, Pull-up | 数据位2 |
| D3 | PC2 | GPIO_Input, Pull-up | 数据位3 |
| D4 | PD3 | GPIO_Input, Pull-up | 数据位4 |
| D5 | PF10 | GPIO_Input, Pull-up | 数据位5 |
| D6 | PE2 | GPIO_Input, Pull-up | 数据位6 |
| D7 | PB2 | GPIO_Input, Pull-up | 数据位7 |
| **控制信号** |  |  |  |
| PCLK | PE4 | GPIO_Input | 像素时钟 |
| VSYNC | PA3 | GPIO_Input | 垂直同步 |
| HREF | PC0 | GPIO_Input | 水平参考 |
| RST | PC3 | GPIO_Output, High | 复位 |
| PWDN | PE5 | GPIO_Output, Low | 掉电控制 |

### 3.2 配置方法
在 **Pinout view** 中：
1. 点击对应引脚
2. 选择 `GPIO_Output` 或 `GPIO_Input`
3. 右键 → **Pin Configuration** 设置上拉/下拉和速度

---

## 📡 步骤4：配置USART

### 4.1 USART1配置
**Connectivity** → **USART1**

| 参数 | 值 |
|------|-----|
| Mode | Asynchronous |
| Baud Rate | 921600 |
| Word Length | 8 Bits |
| Parity | None |
| Stop Bits | 1 |
| Data Direction | Receive and Transmit |
| Over Sampling | 16 Samples |

### 4.2 DMA配置（可选，提升性能）
在 **DMA Settings** 标签中：
- Add: `USART1_TX`
  - Stream: DMA1 Stream 0
  - Direction: Memory To Periph
  - Priority: Medium
  - Mode: Normal
  - Increment Address: Memory

---

## 🔲 步骤5：配置I2C（OLED，可选）

**Connectivity** → **I2C1**

| 参数 | 值 |
|------|-----|
| I2C Speed Mode | Fast Mode |
| I2C Clock Speed | 400 KHz |

---

## 🔄 步骤6：配置FreeRTOS

### 6.1 启用FreeRTOS
**Middleware** → **FREERTOS**
- Interface: **CMSIS_V2**

### 6.2 基本配置
**Config parameters** 标签：

| 参数 | 推荐值 | 说明 |
|------|--------|------|
| configUSE_PREEMPTION | Enabled | 抢占式调度 |
| configCPU_CLOCK_HZ | 480000000 | CPU频率 |
| configTICK_RATE_HZ | 1000 | 1ms时基 |
| configMAX_PRIORITIES | 56 | 最大优先级 |
| **configTOTAL_HEAP_SIZE** | **40960** | ⭐Heap大小40KB |
| configMINIMAL_STACK_SIZE | 128 | 最小堆栈 |
| configMAX_TASK_NAME_LEN | 32 | 任务名长度 |
| configUSE_MUTEXES | Enabled | 启用互斥量 |
| configUSE_COUNTING_SEMAPHORES | Enabled | 启用信号量 |

---

## 📝 步骤7：添加任务

**Tasks and Queues** 标签 → **Tasks** → **Add**

### 任务配置表

| 任务名 | 优先级 | 堆栈大小(字) | 入口函数 | 代码生成 | 说明 |
|--------|--------|--------------|----------|----------|------|
| **Task_cam_capture** | osPriorityNormal (24) | 2048 | AppTask_cam_capture | Dynamic | 图像采集 |
| **Task_uart_rx** | osPriorityAboveNormal (32) | 1024 | AppTask_uart_rx | Dynamic | 串口接收 |
| **Task_uart_tx** | osPriorityNormal (24) | 2048 | AppTask_uart_tx | Dynamic | 串口发送 |
| **Task_inference** | osPriorityNormal (24) | 4096 | AppTask_inference | Dynamic | AI推理 |

### 详细添加步骤（以Task_cam_capture为例）
1. 点击 **Add** 按钮
2. 填写信息：
   - **Task Name**: `Task_cam_capture`
   - **Priority**: `osPriorityNormal` (下拉选择)
   - **Stack Size**: `2048` (字，实际为2048*4=8KB)
   - **Entry Function**: `AppTask_cam_capture`
   - **Code Generation Option**: `As weak` 或 `Default`
   - **Allocation**: `Dynamic`
   - **Buffer Name**: NULL
   - **Control Block Name**: NULL
3. 点击 **OK**
4. 重复以上步骤添加其他3个任务

---

## 📬 步骤8：添加队列

**Tasks and Queues** 标签 → **Queues** → **Add**

### 队列配置表

| 队列名 | 队列长度 | 项大小(字节) | 说明 |
|--------|----------|--------------|------|
| **imageQueue** | 2 | 20 | 图像消息(ImageMsg_t) |
| **cmdQueue** | 8 | 64 | 命令字符串 |
| **resultQueue** | 4 | 12 | 推理结果(InferenceResult_t) |

### 详细添加步骤（以imageQueue为例）
1. 点击 **Add** 按钮
2. 填写信息：
   - **Queue Name**: `imageQueue`
   - **Queue Size**: `2`
   - **Item Size**: `20` (sizeof(ImageMsg_t)=20字节)
   - **Allocation**: `Dynamic`
3. 点击 **OK**
4. 重复以上步骤添加其他队列

**注意**：项大小计算
```c
sizeof(ImageMsg_t) = sizeof(uint8_t*) + sizeof(uint16_t)*2 + sizeof(uint32_t)*2
                   = 4 + 2 + 2 + 4 + 4 = 16字节 (32位系统)
                   = 8 + 2 + 2 + 4 + 4 = 20字节 (64位系统)
// 建议设置为20字节以兼容
```

---

## 🚦 步骤9：添加信号量

**Tasks and Queues** 标签 → **Binary Semaphores** → **Add**

### 信号量配置表

| 信号量名 | 类型 | 说明 |
|----------|------|------|
| **frameReadySem** | Binary Semaphore | 帧就绪通知 |
| **uartTxSem** | Binary Semaphore | 串口发送完成 |

### 详细添加步骤
1. 点击 **Add** 按钮
2. 填写信息：
   - **Semaphore Name**: `frameReadySem`
   - **Allocation**: `Dynamic`
3. 点击 **OK**
4. 重复添加 `uartTxSem`

---

## 🔐 步骤10：添加互斥量

**Tasks and Queues** 标签 → **Mutexes** → **Add**

### 互斥量配置表

| 互斥量名 | 说明 |
|----------|------|
| **cameraMutex** | 保护摄像头访问 |
| **uartMutex** | 保护串口发送 |

### 详细添加步骤
1. 点击 **Add** 按钮
2. 填写信息：
   - **Mutex Name**: `cameraMutex`
   - **Allocation**: `Dynamic`
3. 点击 **OK**
4. 重复添加 `uartMutex`

---

## 🎯 步骤11：生成代码

1. 点击 **Project** → **Settings**
2. 选择 **Toolchain/IDE**: `MDK-ARM V5` 或 `STM32CubeIDE`
3. 设置项目名称和路径
4. 点击 **OK**
5. 点击 **Generate Code** 按钮
6. 等待代码生成完成

---

## 📂 步骤12：查看生成的代码

生成后，你会在 `Src/freertos.c` 中看到：

```c
// 任务句柄
osThreadId_t Task_cam_captureHandle;
osThreadId_t Task_uart_rxHandle;
osThreadId_t Task_uart_txHandle;
osThreadId_t Task_inferenceHandle;

// 队列句柄
osMessageQueueId_t imageQueueHandle;
osMessageQueueId_t cmdQueueHandle;
osMessageQueueId_t resultQueueHandle;

// 信号量句柄
osSemaphoreId_t frameReadySemHandle;
osSemaphoreId_t uartTxSemHandle;

// 互斥量句柄
osMutexId_t cameraMutexHandle;
osMutexId_t uartMutexHandle;

// 任务函数声明
void AppTask_cam_capture(void *argument);
void AppTask_uart_rx(void *argument);
void AppTask_uart_tx(void *argument);
void AppTask_inference(void *argument);
```

---

## ⚠️ 重要注意事项

### 1. 句柄名称对应
CubeMX生成的句柄名称会自动加上 `Handle` 后缀。

**你的代码中使用时需要对应：**
```c
// 你的task_common.h中声明为: imageQueue
// 但实际使用CubeMX生成的: imageQueueHandle

// 解决方案：在task_common.c中添加宏定义
#define imageQueue    imageQueueHandle
#define cmdQueue      cmdQueueHandle
#define resultQueue   resultQueueHandle
#define frameReadySem frameReadySemHandle
#define uartTxSem     uartTxSemHandle
#define cameraMutex   cameraMutexHandle
#define uartMutex     uartMutexHandle
```

或者直接在 `task_common.h` 中声明为 `xxxHandle`。

### 2. USER CODE 区域
所有自定义代码必须放在 `/* USER CODE BEGIN xxx */` 和 `/* USER CODE END xxx */` 之间，否则重新生成代码时会被覆盖！

### 3. Heap大小调整
如果编译时提示内存不足，需要增加 `configTOTAL_HEAP_SIZE`：
- 在CubeMX中：**FREERTOS** → **Config parameters** → `configTOTAL_HEAP_SIZE`
- 或直接修改 `Inc/FreeRTOSConfig.h` 中的定义

### 4. DTCM内存
图像缓冲区建议放在DTCM（Data Tightly Coupled Memory）：
```c
static uint8_t image_buffer[2][IMAGE_SIZE] __attribute__((section(".dtcmram")));
```

需要在链接脚本中定义 `.dtcmram` 段。

---

## ✅ 配置完成检查清单

完成配置后，确认以下内容：

- [ ] 系统时钟配置为480MHz
- [ ] 所有GPIO引脚配置正确
- [ ] USART1波特率921600
- [ ] FreeRTOS Heap大小 ≥ 40KB
- [ ] 4个任务已添加
- [ ] 3个队列已添加
- [ ] 2个信号量已添加
- [ ] 2个互斥量已添加
- [ ] 代码生成成功
- [ ] 编译无错误

---

## 🔧 常见问题

### Q1: 编译时提示堆栈溢出
**A:** 增加对应任务的堆栈大小，在CubeMX的任务配置中修改 `Stack Size`。

### Q2: osMessageQueuePut总是失败
**A:** 检查队列是否已满，或增加队列长度。

### Q3: 任务无法运行
**A:** 检查任务优先级是否冲突，确保空闲任务有机会运行（优先级0）。

### Q4: Hard Fault错误
**A:**
1. 检查是否有堆栈溢出
2. 检查是否在中断中使用了不安全的FreeRTOS API
3. 使用 `configASSERT` 调试

---

## 📖 下一步

配置完成后：
1. 将任务实现代码添加到项目中
2. 在 `Src/freertos.c` 的 `USER CODE` 区域添加必要的初始化代码
3. 编译并烧录
4. 使用Python工具测试

完整的代码示例请参考 `freertos_integration.md`。
