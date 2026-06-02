# ESP8266和OneNet云端数据上传移植完成总结

## 移植概述

已成功将ESP8266 WiFi模块和OneNet云平台数据上传功能移植到STM32H7项目中。移植基于参考项目`新版OneNet（固定数据上传）`，并完全适配STM32H7平台和FreeRTOS。

## 移植的文件结构

```
project7/
├── Net/
│   ├── ESP8266/
│   │   ├── esp8266.h          # ESP8266驱动头文件
│   │   └── esp8266.c          # ESP8266驱动实现（STM32H7适配版）
│   ├── MQTT/
│   │   ├── Common.h           # 通用类型定义
│   │   ├── MqttKit.h          # MQTT协议头文件
│   │   └── MqttKit.c          # MQTT协议实现
│   └── OneNet/
│       ├── onenet.h           # OneNet接口头文件
│       └── onenet.c           # OneNet接口实现
├── App/
│   ├── uart_tx_task.h         # 已更新：添加OneNet数据更新接口
│   └── uart_tx_task.c         # 已更新：集成OneNet数据上传
└── ESP8266_ONENET_CONFIG.md  # 详细配置说明文档
```

## 主要功能

### 1. ESP8266 WiFi连接
- 自动连接配置的WiFi网络
- 支持AT指令交互
- 自动重连机制
- 硬件复位功能（PA0控制）

### 2. MQTT协议支持
- 完整的MQTT 3.1.1协议实现
- 支持QoS 0/1/2三种消息质量等级
- 支持Connect、Publish、Subscribe等操作
- 动态内存管理

### 3. OneNet云平台集成
- 自动连接OneNet MQTT服务器（mqtts.heclouds.com:1883）
- JSON格式数据上传
- 支持平台命令下发
- 每5秒自动上传传感器数据

### 4. uart_tx_task集成
- 在现有uart_tx_task中集成OneNet功能
- 不影响原有的图像传输、FPS显示等功能
- 与蓝牙数据发送并行工作
- 提供数据更新接口：`UartTxTask_UpdateSensorData()`

## 关键适配点

### STM32F1 -> STM32H7的移植变化

1. **GPIO初始化**
   - 从STM32F1的GPIO_InitTypeDef改为H7的HAL_GPIO_Init
   - 使能时钟方式不同：`__HAL_RCC_GPIOx_CLK_ENABLE()`

2. **延时函数**
   - 从`delay_ms()`改为`HAL_Delay()`
   - 在FreeRTOS中使用`vTaskDelay()`

3. **UART操作**
   - 统一使用HAL库的`HAL_UART_Transmit()`
   - 支持中断接收回调

4. **内存管理**
   - 使用标准C库的`malloc/free`
   - 与FreeRTOS兼容

## 数据流程

```
传感器数据采集
    ↓
UartTxTask_UpdateSensorData(temp, humi, gesture)
    ↓
全局变量存储 (g_sensor_temperature, g_sensor_humidity, g_last_gesture)
    ↓
uart_tx_task每5秒调用OneNet_SendData()
    ↓
OneNet_PrepareJSON() - 组装JSON数据
    ↓
MQTT_PacketPublish() - 封装MQTT数据包
    ↓
ESP8266_SendData() - 通过UART2发送到ESP8266
    ↓
ESP8266通过WiFi上传到OneNet云平台
```

## 配置要点

### 必须修改的配置
1. **WiFi信息**（`Net/ESP8266/esp8266.c`）
   ```c
   #define ESP8266_WIFI_INFO "AT+CWJAP=\"你的WiFi\",\"密码\"\r\n"
   ```

2. **OneNet设备信息**（`Net/OneNet/onenet.c`）
   ```c
   #define PROID "产品ID"
   #define AUTH_INFO "鉴权信息"
   #define DEVID "设备名称"
   ```

### 必须添加的硬件配置
1. 在CubeMX中配置UART2（115200, 8N1）
2. 使能UART2中断
3. 连接ESP8266硬件到UART2

## 使用示例

### 在推理任务中更新数据
```c
#include "uart_tx_task.h"

void InferenceTask(void)
{
    float temperature = 25.5;  // 从传感器读取
    float humidity = 60.0;     // 从传感器读取
    uint8_t gesture = 2;       // 手势识别结果

    // 更新数据，uart_tx_task会自动上传到OneNet
    UartTxTask_UpdateSensorData(temperature, humidity, gesture);
}
```

## 内存使用

- ESP8266接收缓冲区：128字节
- MQTT数据包缓冲区：动态分配（通常200-300字节）
- JSON数据缓冲区：256字节
- 总计约：~600字节RAM

## 时序说明

- **初始化阶段**（约5-10秒）
  - ESP8266复位：250ms
  - AT测试：循环直到成功
  - WiFi连接：自动重连直到成功
  - OneNet连接：自动重连直到成功

- **运行阶段**
  - OneNet数据上传：每5秒
  - 蓝牙数据发送：每2秒
  - FPS显示：每1秒

## 调试建议

1. **查看ESP8266通信**
   - 监控UART2可以看到所有AT指令
   - 观察ESP8266返回的响应

2. **OneNet平台调试**
   - 在OneNet控制台查看设备在线状态
   - 查看数据流可以看到上传的JSON数据

3. **常见问题**
   - WiFi连接失败：检查信号强度和密码
   - OneNet连接失败：检查设备信息配置
   - 数据未上传：检查`UartTxTask_UpdateSensorData()`是否被调用

## 后续优化建议

1. 添加错误处理和重连机制
2. 实现OTA远程升级功能
3. 支持平台命令控制设备
4. 添加数据缓存，网络断开时保存数据
5. 优化功耗，支持低功耗模式

## 参考文档

- `ESP8266_ONENET_CONFIG.md` - 详细配置说明
- OneNet官方文档：https://open.iot.10086.cn/doc/
- MQTT协议规范：http://mqtt.org/

---

**移植完成日期：** 2026-01-04
**移植版本：** v1.0
**基于参考项目：** 新版OneNet（固定数据上传）
