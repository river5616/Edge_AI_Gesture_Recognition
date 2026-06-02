# 传感器系统快速开始

## 概述

本项目已集成4个传感器，数据实时显示在OLED屏幕上：
- **BH1750** - 光照强度传感器
- **HC-SR04** - 超声波测距传感器
- **MQ2** - 烟雾/气体传感器
- **MAX30102** - 心率血氧传感器

## 快速开始（3步）

### 1️⃣ 检查引脚连接

确保传感器按以下引脚连接（或修改代码中的引脚定义）：

| 传感器 | 引脚 | STM32引脚 | 说明 |
|--------|------|-----------|------|
| BH1750 | SCL | PB13 | I2C时钟 |
| BH1750 | SDA | PB12 | I2C数据 |
| HC-SR04 | Trig | PA5 | 触发信号 |
| HC-SR04 | Echo | PA6 | 回声信号 |
| MQ2 | AO | PA0 | 模拟输出(ADC1_IN0) |
| MAX30102 | SCL | PB6 | I2C时钟 |
| MAX30102 | SDA | PB7 | I2C数据 |
| MAX30102 | INT | PB4 | 中断信号 |

### 2️⃣ 配置CubeMX（仅MQ2需要）

1. 打开CubeMX项目
2. 配置 **ADC1**：
   - 使能ADC1
   - 选择PA0为ADC1_IN0（或根据实际修改）
   - 分辨率：12位
   - 转换模式：单次转换
3. 重新生成代码

### 3️⃣ 添加传感器任务

在 `Core/Src/freertos.c` 中添加：

```c
/* USER CODE BEGIN Includes */
#include "sensor_task.h"
/* USER CODE END Includes */

void MX_FREERTOS_Init(void)
{
  /* USER CODE BEGIN RTOS_THREADS */

  // 创建传感器任务
  CreateSensorTask();

  /* USER CODE END RTOS_THREADS */
}
```

**完成！** 编译并下载程序，OLED将显示传感器数据。

## OLED显示效果

```
╔════════════════════════╗
║ =Sensor Data=          ║
║ Light:  156 Lux        ║
║ Dist:   23 cm          ║
║ MQ2:   234             ║
║ PPM:   12.3            ║
║ R: 45k IR: 67k         ║
╚════════════════════════╝
```

## 自定义配置

### 修改引脚

在对应的头文件中修改引脚定义：

**BH1750.h**
```c
#define BH1750_GPIO_PORT    GPIOB
#define BH1750_SCL_PIN      GPIO_PIN_13  // 改为你的引脚
#define BH1750_SDA_PIN      GPIO_PIN_12  // 改为你的引脚
```

**HCSR04.h**
```c
#define HCSR04_TRIG_PORT    GPIOA
#define HCSR04_TRIG_PIN     GPIO_PIN_5   // 改为你的引脚
#define HCSR04_ECHO_PORT    GPIOA
#define HCSR04_ECHO_PIN     GPIO_PIN_6   // 改为你的引脚
```

类似地修改 `MQ2.h` 和 `MAX30102.h`

### 修改刷新率

在 `App/sensor_task.c` 中修改延时：

```c
/* 延时500毫秒 - 改为你想要的刷新周期 */
osDelay(500);  // 改为250（更快）或1000（更慢）
```

### 禁用不用的传感器

如果某个传感器未连接，可以在 `App/sensor_task.c` 中注释掉：

```c
// 不用HC-SR04，注释掉这行
// sensorData.distance = HCSR04_GetDistance();
```

## 文件结构

```
project7/
├── Hardware/
│   ├── BH1750.h/c          ✅ BH1750驱动
│   ├── HCSR04.h/c          ✅ HC-SR04驱动
│   ├── MQ2.h/c             ✅ MQ2驱动
│   ├── MAX30102.h/c        ✅ MAX30102驱动
│   ├── sensors.h/c         ✅ 统一初始化
│   ├── OLED.h/c            ✅ OLED驱动
│   └── disp.h/c            ✅ 显示管理
├── App/
│   └── sensor_task.h/c     ✅ 传感器任务
├── SENSOR_PORTING.md       📖 移植详细说明
├── OLED_DISPLAY_GUIDE.md   📖 OLED显示指南
└── QUICK_START.md          📖 本文档
```

## 故障排除

### ❌ OLED无显示
1. 检查OLED供电（VCC/GND）
2. 检查OLED引脚连接（SCL/SDA）
3. 确认已调用 `OLED_Init()`

### ❌ 光照值一直为0
1. 检查BH1750供电
2. 检查I2C引脚连接（PB13/PB12）
3. 测试I2C通信是否正常

### ❌ 距离一直显示888或777
1. 检查HC-SR04供电（5V）
2. 确认Echo和Trig引脚连接正确
3. 检查前方是否有障碍物（2-400cm范围内）

### ❌ MQ2读取失败
1. 确认已在CubeMX中配置ADC1
2. 检查PA0引脚连接
3. MQ2需要预热2-3分钟

### ❌ MAX30102数据为0
1. 检查I2C引脚连接（PB6/PB7）
2. 确认传感器供电（3.3V）
3. 手指需要正确放置在传感器上

### ❌ 编译错误：hadc1未定义
需要在CubeMX中配置ADC1并重新生成代码。

## 进阶使用

### 添加数据记录

```c
// 在sensor_task.c中添加
void LogSensorData(SensorData_t *data)
{
    // 保存到SD卡或通过串口发送
    printf("Light:%d,Dist:%d,MQ2:%d\n",
           data->lightLux, data->distance, data->mq2Value);
}
```

### 添加报警功能

```c
// 烟雾浓度超标时报警
if (sensorData.mq2PPM > 50.0f) {
    HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_SET);
}
```

### 切换显示视图

使用按键切换不同的显示视图：

```c
extern uint8_t view;  // 在disp.c中定义

// 按键切换视图
if (KeyPressed()) {
    view = (view + 1) % 4;  // 切换到下一个视图
}
```

## 技术支持

- **移植详情**：查看 `SENSOR_PORTING.md`
- **显示自定义**：查看 `OLED_DISPLAY_GUIDE.md`
- **问题反馈**：检查代码注释和文档FAQ部分

## 版本信息

- **版本**: v1.0
- **日期**: 2026-01-03
- **平台**: STM32H7 + HAL库 + FreeRTOS
- **状态**: ✅ 测试完成，可直接使用
