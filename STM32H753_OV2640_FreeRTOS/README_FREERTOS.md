# STM32H753 OV2640手势识别 - FreeRTOS版本

## 项目概述

本项目是基于FreeRTOS的STM32H753 OV2640摄像头手势识别系统，采用多任务架构实现图像采集、数据传输、AI推理等功能。

---

## ✨ 特性

- ✅ **FreeRTOS多任务架构** - 任务职责清晰，易于维护
- ✅ **队列通信** - 任务间安全的数据传递
- ✅ **信号量同步** - 高效的任务同步机制
- ✅ **互斥量保护** - 共享资源安全访问
- ✅ **96x96灰度图像** - 适合嵌入式AI推理
- ✅ **串口通信协议** - 完整的命令和数据传输协议
- ✅ **Python工具支持** - 数据采集和模型训练
- ✅ **X-CUBE-AI就绪** - 支持AI模型部署

---

## 📁 项目结构

```
STM32H753_OV2640_FreeRTOS/
├── task_common.h/c          # 任务间共享定义和全局变量
├── camera_task.h/c          # 摄像头采集任务
├── uart_rx_task.h/c         # 串口接收任务
├── uart_tx_task.h/c         # 串口发送任务
├── inference_task.h/c       # AI推理任务
├── ov2640.h/c               # OV2640驱动（需复制）
├── sccb.h/c                 # SCCB通信（需复制）
├── ov2640cfg.h/c            # 寄存器配置（需复制）
├── CUBEMX_CONFIG_GUIDE.md   # STM32CubeMX配置详细指南
├── FREERTOS_INTEGRATION.md  # freertos.c修改示例
└── README_FREERTOS.md       # 本文档
```

---

## 🏗️ 系统架构

### 任务结构

```
┌─────────────────────┐
│ Task_cam_capture    │  优先级: Normal (24)
│ (2048字 = 8KB)      │  功能: 图像采集
└──────────┬──────────┘
           │ imageQueue (2)
           ▼
┌─────────────────────┐     ┌─────────────────────┐
│ Task_uart_tx        │     │ Task_uart_rx        │
│ (2048字 = 8KB)      │◄────│ (1024字 = 4KB)      │
│ 优先级: Normal      │     │ 优先级: AboveNormal │
│ 功能: 数据发送      │     │ 功能: 命令接收      │
└──────────┬──────────┘     └──────────┬──────────┘
           │                           │
           │ frameReadySem            │ cmdQueue (8)
           │                           │
           ▼                           ▼
┌─────────────────────┐     ┌─────────────────────┐
│ Task_inference      │     │ [全局状态变量]      │
│ (4096字 = 16KB)     │     │ - g_work_mode       │
│ 优先级: Normal      │     │ - g_gesture_id      │
│ 功能: AI推理        │     │ - g_target_images   │
└─────────────────────┘     └─────────────────────┘
```

### 同步机制

**队列（Queues）：**
- `imageQueue (2)` - 传递图像消息（ImageMsg_t）
- `cmdQueue (8)` - 传递命令消息
- `resultQueue (4)` - 传递推理结果

**信号量（Semaphores）：**
- `frameReadySem` - 通知帧数据就绪
- `uartTxSem` - 串口发送完成通知

**互斥量（Mutexes）：**
- `cameraMutex` - 保护摄像头访问
- `uartMutex` - 保护串口发送

---

## 🚀 快速开始

### 步骤1：准备驱动代码

从非FreeRTOS版本复制驱动文件到本目录：

```bash
cp ../STM32H753_OV2640/ov2640.h ./
cp ../STM32H753_OV2640/ov2640.c ./
cp ../STM32H753_OV2640/sccb.h ./
cp ../STM32H753_OV2640/sccb.c ./
cp ../STM32H753_OV2640/ov2640cfg.h ./
cp ../STM32H753_OV2640/ov2640cfg.c ./
```

### 步骤2：STM32CubeMX配置

详细步骤请参考 **[CUBEMX_CONFIG_GUIDE.md](CUBEMX_CONFIG_GUIDE.md)**

**要点总结：**
1. 配置时钟到480MHz
2. 配置所有GPIO引脚
3. 配置USART1（921600波特率）
4. 启用FreeRTOS（CMSIS_V2）
5. 添加4个任务
6. 添加3个队列
7. 添加2个信号量
8. 添加2个互斥量
9. 设置Heap大小为40KB
10. 生成代码

### 步骤3：修改freertos.c

详细步骤请参考 **[FREERTOS_INTEGRATION.md](FREERTOS_INTEGRATION.md)**

**要点总结：**
1. 在 `USER CODE BEGIN Includes` 添加头文件
2. 在 `USER CODE BEGIN Variables` 添加句柄映射
3. 在 `USER CODE BEGIN Init` 初始化全局变量

### 步骤4：添加文件到项目

**Keil MDK-ARM：**
1. 右键项目 → Add New Group → 命名为 `Tasks`
2. 右键 `Tasks` → Add Existing Files
3. 选择所有 `.c` 文件：
   - `task_common.c`
   - `camera_task.c`
   - `uart_rx_task.c`
   - `uart_tx_task.c`
   - `inference_task.c`
   - `ov2640.c`
   - `sccb.c`
   - `ov2640cfg.c`

4. 添加头文件路径：
   - 项目选项 → C/C++ → Include Paths
   - 添加：`.\Tasks` 或你放置头文件的路径

**STM32CubeIDE：**
1. 右键项目 → New → Source Folder → 命名为 `Tasks`
2. 将所有 `.c` 和 `.h` 文件拖入 `Tasks` 文件夹
3. 右键项目 → Properties → C/C++ Build → Settings
4. Tool Settings → MCU GCC Compiler → Include paths
5. 添加 `"${workspace_loc:/${ProjName}/Tasks}"`

### 步骤5：编译和烧录

```bash
# Keil
Build → Rebuild all target files

# GCC/Makefile
make clean
make

# 烧录
ST-LINK Utility 或 OpenOCD
```

### 步骤6：测试

使用Python工具测试：

```bash
cd ../Python

# 预览模式
python data_collector.py --port COM3 --mode preview

# 采集模式
python data_collector.py --port COM3 --mode collect --gesture 0 --num 100
```

---

## 📊 资源占用

### 内存占用（预估）

**任务堆栈（静态分配）：**
```
Task_cam_capture:  2048字 × 4 = 8KB
Task_uart_rx:      1024字 × 4 = 4KB
Task_uart_tx:      2048字 × 4 = 8KB
Task_inference:    4096字 × 4 = 16KB
FreeRTOS内部任务:           ~2KB
总计:                       ~38KB
```

**Heap占用（动态分配）：**
```
队列:                        ~2KB
信号量/互斥量:               ~0.5KB
FreeRTOS内部:                ~2KB
总计:                        ~4.5KB
```

**静态数据（全局/静态变量）：**
```
图像缓冲区: 96×96×2 =       18KB (DTCM)
驱动变量:                    ~2KB
任务变量:                    ~1KB
总计:                        ~21KB
```

**总RAM: ~64KB**（STM32H753有128KB DTCM + 512KB SRAM，完全够用）

### Flash占用（预估）

```
任务代码:                    ~12KB
驱动代码:                    ~15KB
FreeRTOS内核:                ~8KB
HAL库:                       ~50KB
启动代码:                    ~2KB
AI模型:                      ~100KB (量化后)
总计:                        ~187KB
```

**总Flash: ~200KB**（STM32H753有2MB，完全够用）

---

## 🔄 工作流程

### 数据采集模式流程

```
1. PC发送: $COLLECT,0,100\r\n
   ↓
2. Task_uart_rx接收命令
   ↓
3. 设置 g_work_mode = MODE_COLLECTING
   ↓
4. Task_cam_capture开始采集
   ↓
5. 每采集一帧:
   - 放入imageQueue
   - 释放frameReadySem
   ↓
6. Task_uart_tx等到信号
   ↓
7. 从imageQueue获取图像
   ↓
8. 通过串口发送图像
   ↓
9. g_current_images++
   ↓
10. 发送进度: $PROGRESS,x,100\r\n
   ↓
11. 达到目标 → $COMPLETE\r\n
```

### 推理模式流程

```
1. PC发送: $INFER\r\n
   ↓
2. 设置 g_work_mode = MODE_INFERENCE
   ↓
3. Task_cam_capture采集帧
   ↓
4. Task_inference等到信号
   ↓
5. 从imageQueue获取图像
   ↓
6. 运行AI推理
   ↓
7. 发送结果: $RESULT,3,0.95\r\n
   ↓
8. 循环继续
```

---

## 🔧 配置和调优

### 调整任务优先级

如果发现任务响应不及时，可以调整优先级（在CubeMX中）：

```c
Task_uart_rx:    osPriorityAboveNormal  // 命令接收优先
Task_cam_capture: osPriorityNormal       // 图像采集
Task_uart_tx:    osPriorityNormal       // 数据发送
Task_inference:  osPriorityNormal       // AI推理
```

### 调整队列长度

如果队列经常满，增加长度（在CubeMX中）：

```c
imageQueue:  从2增加到4
cmdQueue:    从8增加到16
```

### 调整Heap大小

如果遇到内存不足，增加Heap（在CubeMX中）：

```c
configTOTAL_HEAP_SIZE: 从40960增加到51200 (50KB)
```

### 优化图像采集速度

```c
// 在camera_task.c中调整延时
vTaskDelayUntil(&pxPreviousWakeTime, pdMS_TO_TICKS(33)); // 30 FPS
```

---

## 🐛 调试技巧

### 1. 检查任务堆栈使用情况

```c
// 在任务中添加
UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
printf("Stack free: %d words\r\n", uxHighWaterMark);
```

### 2. 监控Heap使用

```c
size_t freeHeap = xPortGetFreeHeapSize();
printf("Heap free: %d bytes\r\n", freeHeap);
```

### 3. 检查队列状态

```c
UBaseType_t queueSpaces = uxQueueSpacesAvailable(imageQueueHandle);
UBaseType_t queueWaiting = uxQueueMessagesWaiting(imageQueueHandle);
printf("Queue: %d free, %d waiting\r\n", queueSpaces, queueWaiting);
```

### 4. 使用configASSERT

在 `FreeRTOSConfig.h` 中启用断言：

```c
#define configASSERT(x) if((x) == 0) { taskDISABLE_INTERRUPTS(); for(;;); }
```

### 5. FreeRTOS追踪

使用ST的SystemView或Percepio Tracealyzer进行高级调试。

---

## ❓ 常见问题

### Q1: Hard Fault错误

**原因：**
- 堆栈溢出
- 访问NULL指针
- 中断中使用了不安全的API

**解决：**
1. 增加任务堆栈大小
2. 检查句柄是否为NULL再使用
3. 使用 `FromISR` 版本的API

### Q2: 任务无法运行

**检查：**
1. 任务是否正确创建（句柄不为NULL）
2. 优先级是否合理
3. 是否有死循环阻塞了高优先级任务

### Q3: 队列操作失败

**检查：**
1. 队列是否创建成功
2. 队列项大小是否匹配
3. 是否队列已满/已空
4. 超时设置是否合理

### Q4: 图像数据错误

**检查：**
1. 摄像头是否正确初始化
2. GPIO配置是否正确
3. 时序是否正确（PCLK/VSYNC/HREF）
4. 互斥量是否正确使用

### Q5: 串口数据丢失

**检查：**
1. 波特率是否匹配（921600）
2. DMA是否正确配置
3. 缓冲区是否足够大
4. 互斥量是否保护了串口发送

---

## 📈 性能指标

### 目标性能

| 指标 | 目标值 | 实际值 |
|------|--------|--------|
| 图像采集FPS | 15-20 | 待测试 |
| 串口传输时间 | <50ms | 待测试 |
| AI推理时间 | <50ms | 待测试 |
| 命令响应时间 | <100ms | 待测试 |
| CPU占用率 | <80% | 待测试 |

### 测试方法

```c
// 在任务中测量时间
uint32_t start_time = osKernelGetTickCount();
// ... 执行操作 ...
uint32_t elapsed = osKernelGetTickCount() - start_time;
printf("Operation took %lu ms\r\n", elapsed);
```

---

## 🔜 后续优化

1. **使用DMA传输图像数据**
   - 减少CPU占用
   - 提升传输速度

2. **使用DCMI硬件接口**
   - 硬件采集图像
   - FPS可达30+

3. **添加软件定时器**
   - 定时发送FPS
   - 定时检查状态

4. **实现事件组**
   - 更灵活的任务同步

5. **添加任务通知**
   - 比信号量更轻量

---

## 📚 参考文档

- [CUBEMX_CONFIG_GUIDE.md](CUBEMX_CONFIG_GUIDE.md) - STM32CubeMX详细配置
- [FREERTOS_INTEGRATION.md](FREERTOS_INTEGRATION.md) - freertos.c修改指南
- [FreeRTOS官方文档](https://www.freertos.org/Documentation/RTOS_book.html)
- [STM32H7参考手册](https://www.st.com/resource/en/reference_manual/rm0433-stm32h742-stm32h743753-and-stm32h750-value-line-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)

---

## 📞 支持

如有问题，请检查：
1. STM32CubeMX配置是否正确
2. 所有文件是否添加到项目
3. 引脚连接是否正确
4. 波特率是否匹配

祝你开发顺利！🎉
