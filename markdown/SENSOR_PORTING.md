# 传感器移植说明文档

## 移植概述

本次移植将以下4个传感器从STM32F103标准库项目移植到STM32H7 HAL库项目（使用FreeRTOS）：

1. **BH1750 (GY30)** - I2C光照强度传感器
2. **HC-SR04** - 超声波测距传感器
3. **MQ2** - 烟雾/气体传感器（ADC）
4. **MAX30102** - I2C心率血氧传感器

## 移植的主要修正

### 1. BH1750传感器
**原代码问题：**
- 使用STM32F103标准库的位操作宏（PBout/PBin）
- 直接操作寄存器进行GPIO配置

**修正内容：**
- ✅ 改用HAL库GPIO API
- ✅ 添加DWT初始化用于微秒延时
- ✅ 保留软件I2C实现（兼容性更好）
- ✅ 引脚配置改为宏定义，便于修改

### 2. HC-SR04传感器
**原代码问题：**
- 混合使用了定时器捕获和简单轮询方式
- 有未使用的变量和代码结构
- 定时器中断相关代码未完善

**修正内容：**
- ✅ 统一使用轮询+DWT计时方式（更简单可靠）
- ✅ 改用HAL库GPIO API
- ✅ 移除未使用的定时器捕获代码
- ✅ 添加超时保护机制
- ✅ 改进距离计算公式

### 3. MQ2传感器
**原代码问题：**
- 使用extern声明但缺少实际的ADC函数实现
- 变量声明位置不符合C90标准
- 缺少完整的ADC配置代码

**修正内容：**
- ✅ 改用HAL库ADC API
- ✅ 添加完整的ADC配置代码
- ✅ 修正变量声明位置
- ✅ 使用FreeRTOS的osDelay替代原delay_ms
- ✅ 支持模拟和数字两种模式
- ✅ 调整为3.3V参考电压（原为5V）

**注意：** 需要在CubeMX中配置ADC1

### 4. MAX30102传感器
**原代码问题：**
- 依赖外部myiic模块，耦合度高
- 缺少独立的I2C实现

**修正内容：**
- ✅ 创建独立的软件I2C实现
- ✅ 改用HAL库GPIO API
- ✅ 添加DWT初始化用于微秒延时
- ✅ 保持原有的寄存器配置逻辑

## 文件结构

```
project7/
├── Hardware/
│   ├── BH1750.h/c          # BH1750光照传感器驱动
│   ├── HCSR04.h/c          # HC-SR04超声波驱动
│   ├── MQ2.h/c             # MQ2烟雾传感器驱动
│   ├── MAX30102.h/c        # MAX30102心率血氧驱动
│   ├── sensors.h/c         # 传感器统一初始化
│   └── (其他硬件驱动)
└── App/
    ├── sensor_task.h/c     # 传感器FreeRTOS任务示例
    └── (其他应用代码)
```

## 引脚配置

### BH1750 (软件I2C)
- SCL: PB13
- SDA: PB12

### HC-SR04
- Trig: PA5
- Echo: PA6

### MQ2
- AO (模拟输出): PA0 -> ADC1_IN0
- DO (数字输出): PA1

### MAX30102 (软件I2C)
- SCL: PB6
- SDA: PB7
- INT: PB4

**重要：** 请根据实际硬件连接修改引脚定义！

## 使用方法

### 方法1：使用FreeRTOS任务（推荐）
传感器数据会自动读取并显示在OLED上。

```c
#include "sensor_task.h"

void MX_FREERTOS_Init(void)
{
    // 创建传感器任务（自动读取并显示）
    CreateSensorTask();
}
```

OLED显示效果：
```
=Sensor Data=
Light:12345 Lux
Dist: 123 cm
MQ2:  1234
PPM:  25.5
R:123k IR:456k
```

### 方法2：统一初始化
```c
#include "sensors.h"

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    // 初始化所有传感器
    Sensors_Init();

    // 使用传感器
    uint16_t light = BH1750_ReadData();
    uint16_t distance = HCSR04_GetDistance();
    // ...
}
```

### 方法3：单独初始化
```c
#include "BH1750.h"
#include "HCSR04.h"
// ...

int main(void)
{
    HAL_Init();

    // 根据需要初始化特定传感器
    BH1750_Init();
    HCSR04_Init();

    // ...
}
```

## 配置要求

### CubeMX配置
1. **ADC1配置（仅MQ2使用）**
   - 使能ADC1
   - 配置PA0为ADC1_IN0（或根据实际修改）
   - 分辨率：12位
   - 转换模式：单次转换

2. **FreeRTOS配置**
   - 已启用
   - CMSIS-OS v2 API

3. **时钟配置**
   - 确保系统时钟正确配置
   - DWT用于微秒延时

## 传感器读取示例

### BH1750 光照传感器
```c
uint16_t lightValue = BH1750_ReadData();
float lux = lightValue / 1.2f;  // 转换为Lux
```

### HC-SR04 超声波
```c
uint16_t distance = HCSR04_GetDistance();
if (distance < 500) {  // 有效范围
    printf("Distance: %d cm\n", distance);
}
```

### MQ2 烟雾传感器
```c
// 方式1：读取ADC原始值
uint16_t adcValue = MQ2_GetData();

// 方式2：读取PPM浓度
float ppm = MQ2_GetDataPPM();
```

### MAX30102 心率血氧
```c
uint32_t redLED, irLED;
if (MAX30102_ReadFIFO(&redLED, &irLED) == 0) {
    // 成功读取
    // 需要使用算法计算心率和SpO2
}
```

## 注意事项

1. **延时函数**
   - BH1750、HC-SR04、MAX30102使用DWT进行微秒延时
   - MQ2在FreeRTOS环境下使用osDelay

2. **ADC配置**
   - MQ2需要配置ADC1
   - 默认使用PA0作为ADC输入
   - 参考电压根据硬件调整（默认3.3V）

3. **I2C通信**
   - BH1750和MAX30102使用软件I2C
   - 可以修改为硬件I2C以提高性能

4. **引脚冲突**
   - 检查引脚是否与其他外设冲突
   - 根据实际硬件修改引脚定义

5. **FreeRTOS任务**
   - 传感器任务栈大小：512*4字节
   - 任务优先级：Normal
   - 读取周期：1秒

## 常见问题

### Q1: 编译错误：hadc1 未定义
**A:** 需要在CubeMX中配置ADC1并重新生成代码。

### Q2: 传感器读取值异常
**A:** 检查引脚连接和电源供电，确认DWT已正确初始化。

### Q3: HC-SR04返回888或777
**A:** 超时错误，检查Echo引脚连接和模块供电。

### Q4: MQ2的PPM值不准确
**A:** 需要校准R0值，根据清洁空气下的RS/R0比值调整。

## 移植总结

所有传感器已成功从STM32F103标准库移植到STM32H7 HAL库，主要改进：

- ✅ 完全适配HAL库API
- ✅ 修正原代码中的错误和不规范之处
- ✅ 添加完善的错误处理和超时保护
- ✅ 提供FreeRTOS任务示例
- ✅ 统一的初始化接口
- ✅ **集成OLED实时显示功能**
- ✅ 详细的使用文档

**移植完成日期：** 2026-01-03

## 相关文档

- **SENSOR_PORTING.md** (本文档) - 传感器移植说明
- **OLED_DISPLAY_GUIDE.md** - OLED显示使用指南

查看 `OLED_DISPLAY_GUIDE.md` 了解如何自定义显示格式和使用显示功能。
