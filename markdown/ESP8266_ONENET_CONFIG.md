# ESP8266和OneNet云端数据上传配置说明

## 移植完成的内容

已成功移植以下模块到STM32H7项目：

1. **MQTT协议实现** (`Net/MQTT/`)
   - `Common.h` - 通用类型定义
   - `MqttKit.h/c` - MQTT协议实现

2. **ESP8266驱动** (`Net/ESP8266/`)
   - `esp8266.h/c` - ESP8266 WiFi模块驱动

3. **OneNet云平台接口** (`Net/OneNet/`)
   - `onenet.h/c` - OneNet云平台数据上传接口

4. **uart_tx_task集成**
   - 已在`App/uart_tx_task.c`中集成OneNet数据上传功能
   - 每5秒自动上传传感器数据到OneNet云平台

## 需要配置的内容

### 1. UART2硬件配置（在STM32CubeMX中）

**步骤：**
1. 打开 `project2.ioc` 文件
2. 配置UART2引脚：
   - Mode: Asynchronous
   - Baud Rate: 115200
   - Word Length: 8 Bits
   - Parity: None
   - Stop Bits: 1
3. 使能UART2中断：
   - NVIC Settings -> USART2 global interrupt -> Enabled
4. 生成代码

### 2. 添加UART2中断处理

**在 `Src/stm32h7xx_it.c` 中添加：**

```c
// 在文件开头的extern声明区域添加（约第62行）：
extern UART_HandleTypeDef huart2;

// 在文件末尾添加UART2中断处理函数（约第272行之后）：
/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */

  /* USER CODE END USART2_IRQn 0 */
  HAL_UART_IRQHandler(&huart2);
  /* USER CODE BEGIN USART2_IRQn 1 */

  /* USER CODE END USART2_IRQn 1 */
}
```

### 3. 配置UART2接收中断回调

**在 `Src/main.c` 或创建新文件 `App/uart_callbacks.c` 中添加：**

```c
#include "esp8266.h"

// UART接收完成回调
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    static uint8_t uart2_rx_data;

    if(huart->Instance == USART2)
    {
        // 调用ESP8266接收处理函数
        ESP8266_UART_IRQHandler(uart2_rx_data);

        // 重新启动接收
        HAL_UART_Receive_IT(&huart2, &uart2_rx_data, 1);
    }
}

// 在main函数中启动UART2接收中断：
// HAL_UART_Receive_IT(&huart2, &uart2_rx_data, 1);
```

### 4. OneNet平台配置

**修改 `Net/OneNet/onenet.c` 中的配置信息：**

```c
#define PROID		"你的产品ID"     // OneNet产品ID
#define AUTH_INFO	"你的鉴权信息"   // OneNet鉴权信息
#define DEVID		"你的设备名称"   // OneNet设备名称
#define ONENET_TOPIC	"$sys/产品ID/设备名称/thing/property/post"
```

### 5. WiFi配置

**修改 `Net/ESP8266/esp8266.c` 中的WiFi信息：**

```c
#define ESP8266_WIFI_INFO  "AT+CWJAP=\"你的WiFi名称\",\"你的WiFi密码\"\r\n"
```

### 6. 传感器数据更新

在你的传感器任务或推理任务中调用以下函数更新数据：

```c
#include "uart_tx_task.h"

// 更新传感器数据（温度、湿度、手势识别结果）
UartTxTask_UpdateSensorData(temperature, humidity, gesture_id);
```

## 编译配置

### 添加Include路径

在Keil MDK或其他IDE中添加以下Include路径：
- `Net/MQTT`
- `Net/ESP8266`
- `Net/OneNet`

### 添加源文件

将以下源文件添加到工程：
- `Net/MQTT/MqttKit.c`
- `Net/ESP8266/esp8266.c`
- `Net/OneNet/onenet.c`

## 功能说明

### 自动上传到云端
- 系统每5秒自动上传传感器数据到OneNet云平台
- 上传的数据包括：温度、湿度、手势识别结果
- 支持接收OneNet平台下发的命令

### ESP8266硬件连接
- UART2_TX (STM32) -> ESP8266 RX
- UART2_RX (STM32) -> ESP8266 TX
- PA0 (STM32) -> ESP8266 RST（复位引脚）
- VCC -> 3.3V
- GND -> GND

## 调试说明

### 查看ESP8266连接状态
通过串口监控UART2可以看到ESP8266的AT指令交互过程。

### OneNet数据上传格式
上传的JSON格式：
```json
{
  "id":"123",
  "version":"1.0",
  "params":{
    "temperature":{"value":25.5},
    "humidity":{"value":60.0},
    "gesture":{"value":1}
  }
}
```

## 注意事项

1. ESP8266需要单独供电（3.3V，电流需求较大）
2. 确保WiFi网络稳定
3. OneNet平台账号需要提前创建设备
4. UART2波特率固定为115200
5. 首次运行时ESP8266初始化需要约5-10秒

## 故障排除

### 无法连接WiFi
- 检查WiFi名称和密码是否正确
- 检查ESP8266供电是否正常
- 检查UART2连接是否正确

### 无法连接OneNet
- 检查产品ID、设备ID、鉴权信息是否正确
- 检查网络连接是否正常
- 查看OneNet控制台设备在线状态

### 数据未上传
- 检查传感器数据是否正常更新
- 检查OneNet连接是否成功
- 查看UART2串口日志

---

**生成时间:** 2026-01-04
**移植版本:** STM32H7
