# STM32H753 OV2640手势识别项目

## 项目概述
本项目实现基于STM32H753的OV2640摄像头手势识别系统，包括图像采集、灰度处理、串口传输、训练数据采集和CNN模型部署。

## 硬件连接

### OV2640摄像头引脚配置
| 功能 | STM32H753引脚 | 说明 |
|------|--------------|------|
| SCL | PD7 | SCCB时钟线 |
| SDA | PD6 | SCCB数据线 |
| D0 | PD5 | 数据位0 |
| D1 | PB1 | 数据位1 |
| D2 | PD4 | 数据位2 |
| D3 | PC2 | 数据位3 |
| D4 | PD3 | 数据位4 |
| D5 | PF10 | 数据位5 |
| D6 | PE2 | 数据位6 |
| D7 | PB2 | 数据位7 |
| PCLK | PE4 | 像素时钟 |
| VSYNC | PA3 | 垂直同步 |
| HREF | PC0 | 水平参考 |
| RST | PC3 | 复位(初始高) |
| PWDN | PE5 | 掉电控制(初始低) |

### UART配置
- 波特率: 921600 (用于高速图像传输)
- 数据位: 8
- 停止位: 1
- 校验位: None

### OLED配置
- 接口: I2C或SPI
- 用途: 显示实时FPS

## STM32CubeMX配置步骤

### 1. 系统时钟配置
- 外部晶振: 25MHz
- SYSCLK: 480MHz (最大频率)
- AHB: 240MHz
- APB: 120MHz

### 2. GPIO配置

#### 输出引脚
```
PD7 (SCL): GPIO_Output, High Speed
PD6 (SDA): GPIO_Output, High Speed (代码中动态切换)
PC3 (RST): GPIO_Output, Initial High
PE5 (PWDN): GPIO_Output, Initial Low
```

#### 输入引脚
```
PD5, PB1, PD4, PC2, PD3, PF10, PE2, PB2: GPIO_Input, Pull-Up
PE4 (PCLK): GPIO_Input
PA3 (VSYNC): GPIO_Input
PC0 (HREF): GPIO_Input
```

### 3. UART配置
```
USART1:
- Mode: Asynchronous
- Baud Rate: 921600
- Word Length: 8 Bits
- Stop Bits: 1
- Parity: None
- Hardware Flow Control: Disable
- DMA Settings:
  - TX: DMA1 Stream 0, Normal Mode, Memory Increment
```

### 4. I2C配置 (用于OLED)
```
I2C1:
- Mode: I2C
- Speed: 400KHz (Fast Mode)
```

### 5. Timer配置 (用于FPS计算)
```
TIM2:
- Clock Source: Internal Clock
- Prescaler: 计算得出1ms时基
- Counter Period: 0xFFFFFFFF (32位)
- Auto-reload Preload: Enable
```

## 项目结构

```
STM32H753_OV2640/
├── Core/
│   ├── Inc/
│   │   └── main.h
│   └── Src/
│       └── main.c
├── Drivers/
│   ├── OV2640/
│   │   ├── ov2640.h
│   │   ├── ov2640.c
│   │   ├── ov2640cfg.h
│   │   ├── ov2640cfg.c
│   │   ├── sccb.h
│   │   └── sccb.c
│   └── OLED/
│       ├── oled.h
│       └── oled.c
├── Application/
│   ├── camera_app.h
│   ├── camera_app.c
│   ├── uart_protocol.h
│   └── uart_protocol.c
├── Python/
│   ├── data_collector.py      # 数据采集脚本
│   ├── train_model.py          # 模型训练脚本
│   └── requirements.txt
└── README.md
```

## 串口通信协议

### 命令格式
所有命令以`$`开始，以`\r\n`结束

### 电脑→STM32命令

#### 1. 开始采集手势数据
```
$COLLECT,<gesture_id>,<num_images>\r\n
```
- `gesture_id`: 手势类别ID (0-9)
- `num_images`: 要采集的图像数量

示例: `$COLLECT,0,100\r\n` (采集100张手势0的图像)

#### 2. 停止采集
```
$STOP\r\n
```

#### 3. 查询状态
```
$STATUS\r\n
```

#### 4. 开始推理模式
```
$INFER\r\n
```

#### 5. 停止推理
```
$STOP_INFER\r\n
```

### STM32→电脑响应

#### 1. 图像数据包
```
$IMG,<width>,<height>,<data_length>\r\n
<binary_gray_image_data>
$END\r\n
```

#### 2. 采集进度
```
$PROGRESS,<current>,<total>\r\n
```

#### 3. 推理结果
```
$RESULT,<gesture_id>,<confidence>\r\n
```

#### 4. FPS信息
```
$FPS,<fps_value>\r\n
```

#### 5. 错误信息
```
$ERROR,<error_code>,<error_msg>\r\n
```

## 图像处理流程

### 1. 图像采集
- 分辨率: 96x96 (手势识别合适的分辨率)
- 格式: RGB565 → 灰度转换
- 帧率: ~15-20 FPS

### 2. 灰度转换公式
```c
Gray = (R * 299 + G * 587 + B * 114) / 1000
```

对于RGB565格式:
```c
R = (RGB565 >> 11) & 0x1F;  // 5位
G = (RGB565 >> 5) & 0x3F;   // 6位
B = RGB565 & 0x1F;          // 5位

// 转换到8位
R8 = (R * 255) / 31;
G8 = (G * 255) / 63;
B8 = (B * 255) / 31;

Gray = (R8 * 299 + G8 * 587 + B8 * 114) / 1000;
```

### 3. 数据传输优化
- 使用DMA传输减少CPU占用
- 乒乓缓冲区防止数据覆盖
- 压缩传输(可选)

## 摄像头验证方法(无LCD屏)

### 方法1: 串口图像显示
使用Python脚本实时显示采集的图像:
```python
python data_collector.py --mode preview
```

### 方法2: OLED显示简化图
在OLED上显示缩略图 (如16x16分辨率)

### 方法3: 串口调试输出
输出图像的统计信息:
- 平均亮度
- 最大/最小值
- 图像校验和

## CNN模型架构

### 推荐架构: MobileNet-V2 简化版

#### 模型规格
- 输入: 96x96x1 (灰度图)
- 输出: 10个手势类别
- 参数量: ~50K (适合STM32H753)
- 推理时间: <50ms

#### 网络结构
```python
Model: GestureNet
Input: (96, 96, 1)
├── Conv2D(16, 3x3, stride=2) + ReLU6      # 48x48x16
├── DepthwiseSeparableConv(32, 3x3)         # 48x48x32
├── MaxPool2D(2x2)                          # 24x24x32
├── DepthwiseSeparableConv(64, 3x3)         # 24x24x64
├── MaxPool2D(2x2)                          # 12x12x64
├── DepthwiseSeparableConv(128, 3x3)        # 12x12x128
├── GlobalAveragePooling2D                  # 128
├── Dense(64) + ReLU                        # 64
├── Dropout(0.5)
└── Dense(10) + Softmax                     # 10 classes
```

#### 量化选项
- 使用8位整数量化(INT8)
- X-CUBE-AI自动优化
- 精度损失 <2%

## 数据采集方案

### 1. 手势类别定义
```
0: 拳头
1: 张开手掌
2: 竖起大拇指
3: OK手势
4: V字手势
5: 指向手势
6: 数字3
7: 数字4
8: 向左挥手
9: 向右挥手
```

### 2. 采集要求
- 每个手势: 500-1000张图像
- 不同光照条件
- 不同手势角度
- 不同距离(10-50cm)
- 不同人的手势

### 3. 数据增强
```python
augmentation = {
    'rotation': 15,          # ±15度旋转
    'width_shift': 0.1,      # 水平平移
    'height_shift': 0.1,     # 垂直平移
    'brightness': [0.8, 1.2],# 亮度调整
    'zoom': [0.9, 1.1],      # 缩放
    'horizontal_flip': True  # 水平翻转
}
```

## 模型训练流程

### 1. 环境准备
```bash
pip install torch torchvision numpy opencv-python matplotlib scikit-learn
```

### 2. 训练步骤
```python
# 数据划分
train: 70%
validation: 15%
test: 15%

# 训练参数
batch_size = 32
learning_rate = 0.001
optimizer = Adam
epochs = 100
early_stopping_patience = 15

# 学习率调度
lr_scheduler = ReduceLROnPlateau(patience=5, factor=0.5)
```

### 3. 评估指标
- 准确率 (Accuracy)
- 混淆矩阵 (Confusion Matrix)
- F1-Score
- 推理时间

## X-CUBE-AI部署步骤

### 1. 安装X-CUBE-AI
- 在STM32CubeMX中安装X-CUBE-AI包
- 版本: 8.0或更高

### 2. 模型转换
```python
# 导出PyTorch模型为ONNX
torch.onnx.export(model,
                  dummy_input,
                  "gesture_model.onnx",
                  export_params=True,
                  opset_version=11)
```

### 3. CubeMX配置
1. 在Additional Software中选择X-CUBE-AI
2. Add network from file: 选择gesture_model.onnx
3. Analyze: 查看模型信息和内存占用
4. Validation on Desktop: 验证模型精度
5. Generate Code

### 4. 代码集成
```c
// 初始化AI模型
ai_network_create(&network, AI_NETWORK_DATA_CONFIG);
ai_network_init(network, &params);

// 推理
ai_network_run(network, input_buffer, output_buffer);

// 获取结果
float* predictions = (float*)output_buffer[0].data;
int predicted_class = argmax(predictions, 10);
float confidence = predictions[predicted_class];
```

### 5. 内存要求
- Flash: ~100KB (模型权重)
- RAM: ~150KB (激活值+工作缓冲区)
- 确保STM32H753有足够空间

## 性能优化建议

### 1. 摄像头优化
- 降低分辨率到96x96
- 使用DMA传输
- 仅在需要时采集图像

### 2. 串口优化
- 提高波特率到921600
- 使用DMA发送
- 批量传输

### 3. 模型优化
- 使用INT8量化
- 剪枝不重要的连接
- 知识蒸馏

### 4. 系统优化
- 使用FreeRTOS进行任务管理
- 分离采集和推理线程
- 使用D-Cache加速内存访问

## 调试技巧

### 1. 摄像头调试
```c
// 读取并打印制造商ID
uint16_t mid = (SCCB_RD_Reg(0x1C) << 8) | SCCB_RD_Reg(0x1D);
printf("MID: 0x%04X (should be 0x7FA2)\r\n", mid);

// 读取产品ID
uint16_t pid = (SCCB_RD_Reg(0x0A) << 8) | SCCB_RD_Reg(0x0B);
printf("PID: 0x%04X (should be 0x2642)\r\n", pid);
```

### 2. 图像验证
```c
// 打印图像统计
uint32_t sum = 0, min = 255, max = 0;
for(int i = 0; i < width * height; i++) {
    sum += image[i];
    if(image[i] < min) min = image[i];
    if(image[i] > max) max = image[i];
}
printf("Avg: %d, Min: %d, Max: %d\r\n",
       sum / (width * height), min, max);
```

### 3. FPS计算
```c
static uint32_t frame_count = 0;
static uint32_t last_time = 0;

frame_count++;
uint32_t current_time = HAL_GetTick();
if(current_time - last_time >= 1000) {
    float fps = frame_count * 1000.0f / (current_time - last_time);
    printf("FPS: %.2f\r\n", fps);
    frame_count = 0;
    last_time = current_time;
}
```

## 常见问题

### Q1: 摄像头无法初始化
- 检查SCCB引脚连接
- 测量SCL/SDA波形
- 确认电源供电(3.3V)
- 检查上拉电阻

### Q2: 图像全黑或全白
- 检查PCLK信号
- 确认VSYNC/HREF时序
- 调整曝光参数
- 检查镜头是否遮挡

### Q3: 串口数据传输慢
- 提高波特率
- 使用DMA传输
- 减小图像分辨率
- 启用硬件流控制

### Q4: 模型推理时间过长
- 减小模型复杂度
- 使用INT8量化
- 降低输入分辨率
- 优化编译选项(-O3)

### Q5: 内存不足
- 减小缓冲区大小
- 使用单缓冲
- 降低分辨率
- 使用外部SDRAM

## 下一步计划

1. 完成基础驱动开发
2. 实现数据采集功能
3. 采集训练数据集
4. 训练CNN模型
5. 使用X-CUBE-AI转换模型
6. 集成到STM32
7. 性能优化和调试

## 参考资料

- [OV2640数据手册](https://www.ovt.com/sensors/OV2640)
- [STM32H753参考手册](https://www.st.com/resource/en/reference_manual/rm0433-stm32h742-stm32h743753-and-stm32h750-value-line-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- [X-CUBE-AI用户手册](https://www.st.com/en/embedded-software/x-cube-ai.html)
- [PyTorch官方文档](https://pytorch.org/docs/stable/index.html)

## 许可证

本项目仅用于学习和研究目的。
