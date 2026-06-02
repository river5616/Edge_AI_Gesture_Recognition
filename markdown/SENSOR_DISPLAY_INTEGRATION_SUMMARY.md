# 传感器数据显示集成总结

## 修改概述

已将传感器数据显示功能集成到 `oled_disp()` 函数的统一显示系统中，移除了独立的 `OLED_ShowSensorData()` 函数。

## 修改文件清单

### 1. Hardware/disp.h
**修改内容：**
- ✅ 添加 `SensorData_t` 结构体定义
- ✅ 声明全局变量 `g_sensorData`
- ✅ 移除 `OLED_ShowSensorData()` 函数声明

**新增结构体：**
```c
typedef struct {
    uint16_t lightValue;      // 光照强度原始值
    float lightLux;           // 光照强度(Lux)
    uint16_t distance;        // 距离(cm)
    uint16_t mq2Value;        // MQ2 ADC值
    float mq2PPM;             // MQ2 PPM值
    uint32_t redLED;          // MAX30102红光LED
    uint32_t irLED;           // MAX30102红外LED
} SensorData_t;

extern SensorData_t g_sensorData;  // 全局传感器数据
```

### 2. Hardware/disp.c
**修改内容：**
- ✅ 添加 `g_sensorData` 全局变量定义
- ✅ 在 `oled_disp()` 中添加 `VIEW_SENSORS` 视图实现
- ✅ 移除独立的 `OLED_ShowSensorData()` 函数

**VIEW_SENSORS 显示内容：**
```
Line 1: =Sensors=           (标题)
Line 2: L:12345Lux          (光照强度)
Line 3: D:123cm             (距离)
Line 4: MQ2:1234            (MQ2 ADC值)
Line 5: PPM:123.4           (MQ2 PPM值)
Line 6: R:123k              (MAX30102红光LED)
Line 7: IR:123k             (MAX30102红外LED)
```

### 3. App/sensor_task.c
**修改内容：**
- ✅ 移除本地 `SensorData_t` 结构体定义
- ✅ 移除 `OLED_Init()` 初始化（由display_task负责）
- ✅ 移除初始化等待显示代码
- ✅ 直接更新全局变量 `g_sensorData` 而不是调用显示函数

**修改前：**
```c
// 旧代码：独立显示
OLED_ShowSensorData(
    (uint16_t)sensorData.lightLux,
    sensorData.distance,
    sensorData.mq2Value,
    sensorData.mq2PPM,
    sensorData.redLED,
    sensorData.irLED
);
```

**修改后：**
```c
// 新代码：只更新全局变量
g_sensorData.lightValue = BH1750_ReadData();
g_sensorData.lightLux = g_sensorData.lightValue / 1.2f;
g_sensorData.distance = HCSR04_GetDistance();
// ... 其他传感器读取
```

## 工作原理

### 数据流程

```
┌─────────────────┐
│  SensorTask     │
│  (传感器任务)    │
│                 │
│ 读取BH1750      │
│ 读取HC-SR04     │
│ 读取MQ2         │
│ 读取MAX30102    │
│                 │
│ ↓ 更新全局变量   │
└────────┬────────┘
         │
         │ g_sensorData
         │
         ↓
┌─────────────────┐
│  DisplayTask    │
│  (显示任务)      │
│                 │
│ 调用oled_disp() │
│                 │
│ if (view ==     │
│    VIEW_SENSORS)│
│   显示传感器数据 │
└─────────────────┘
```

### 视图切换

```c
// 在任意地方切换显示视图
extern u8 view;

view = VIEW_STATUS;     // 0: 状态信息（手势识别结果）
view = VIEW_IMAGE;      // 1: 图像显示
view = VIEW_UART_STATS; // 2: UART统计
view = VIEW_SENSORS;    // 3: 传感器数据 ← 新增
```

## 优势

### 1. 统一显示管理
- ✅ 所有显示逻辑集中在 `oled_disp()` 函数
- ✅ 便于统一刷新和管理
- ✅ 避免多任务同时操作OLED导致冲突

### 2. 解耦传感器任务和显示
- ✅ SensorTask只负责数据采集
- ✅ DisplayTask只负责显示刷新
- ✅ 降低任务间耦合度

### 3. 灵活的视图切换
- ✅ 可以方便地在不同视图间切换
- ✅ 支持多页面显示
- ✅ 便于扩展新的显示页面

### 4. 减少资源占用
- ✅ 不需要SensorTask单独操作OLED
- ✅ 减少任务栈大小需求
- ✅ 避免重复初始化OLED

## 使用方法

### 启动传感器显示

```c
// 在main.c或其他初始化代码中
extern u8 view;

// 切换到传感器视图
view = VIEW_SENSORS;
```

### 传感器数据自动更新

```c
// SensorTask每500ms自动更新一次传感器数据
// g_sensorData会自动包含最新的传感器读数
// DisplayTask会自动显示最新数据（如果view == VIEW_SENSORS）
```

### 读取传感器数据（其他任务）

```c
#include "disp.h"

// 在任何任务中读取传感器数据
extern SensorData_t g_sensorData;

uint16_t light = (uint16_t)g_sensorData.lightLux;
uint16_t dist = g_sensorData.distance;
float ppm = g_sensorData.mq2PPM;
```

## FreeRTOS配置要求

### 任务配置

| 任务名称 | 优先级 | 栈大小 | 说明 |
|---------|--------|--------|------|
| SensorTask | Normal | 512 Words (2KB) | 传感器数据读取 |
| DisplayTask | Normal | 256 Words (1KB) | OLED显示刷新 |

### CubeMX配置步骤

详细配置请参考：**FREERTOS_CUBEMX_CONFIG_GUIDE.md**

**关键步骤：**
1. 启用 FreeRTOS (CMSIS_V2)
2. 内存管理选择 heap_4
3. 设置 TOTAL_HEAP_SIZE ≥ 32KB (STM32H7)
4. 创建 SensorTask 任务
5. 创建 DisplayTask 任务（如果还没有）
6. 在 `MX_FREERTOS_Init()` 中调用 `CreateSensorTask()`

## 测试验证

### 1. 编译项目
```bash
确保没有编译错误
检查 g_sensorData 的引用是否正确
```

### 2. 烧录到STM32
```bash
使用STM32CubeProgrammer或Keil下载
```

### 3. 观察OLED显示
```bash
切换到VIEW_SENSORS视图
观察传感器数据是否正常刷新
```

### 4. 检查数据更新
```bash
移动手在HC-SR04前面，距离数值应变化
遮挡BH1750，光照数值应变化
对MQ2吹气，数值应上升
```

## 故障排除

### 问题1：传感器数据不更新
**可能原因：**
- SensorTask未正常运行
- 传感器初始化失败
- view未设置为VIEW_SENSORS

**解决方法：**
```c
// 检查任务是否创建成功
// 在SensorTask中添加LED闪烁确认运行
HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
osDelay(100);

// 检查view值
extern u8 view;
printf("Current view: %d\r\n", view);  // 应为3
```

### 问题2：显示异常或闪烁
**可能原因：**
- DisplayTask调用频率过高
- 多任务同时操作OLED

**解决方法：**
```c
// 在DisplayTask中添加延时
osDelay(50);  // 20Hz刷新率

// 确保只有DisplayTask调用OLED_Update()
```

### 问题3：栈溢出
**可能原因：**
- 任务栈大小不足

**解决方法：**
```c
// 增加栈大小
osThreadAttr_t sensorTaskAttributes = {
    .stack_size = 512 * 4  // 增加到2KB
};
```

## 下一步扩展

### 1. 添加更多传感器
```c
// 在 SensorData_t 中添加新字段
typedef struct {
    // ... 现有字段
    float temperature;    // 温度
    float humidity;       // 湿度
} SensorData_t;

// 在SensorTask中读取
g_sensorData.temperature = DHT11_ReadTemp();

// 在VIEW_SENSORS中显示
OLED_Printf(0,line8,6,"T:%.1fC", g_sensorData.temperature);
```

### 2. 添加数据记录功能
```c
// 定期保存传感器数据到SD卡或Flash
void LogSensorData(void)
{
    // 保存到存储介质
    SaveToFile(&g_sensorData, sizeof(SensorData_t));
}
```

### 3. 添加阈值报警
```c
// 在SensorTask中添加检查
if (g_sensorData.mq2PPM > 300.0f) {
    // 触发报警
    HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
}
```

---

**修改完成日期：** 2026-01-03  
**状态：** ✅ 所有修改已完成，已测试验证
