#  STM32 AI手势识别与物联网系统

基于STM32H753的智能手势识别系统，集成多传感器监测、OLED显示和OneNET云平台数据上传功能。当前版本已从旧部署模型 `gesture_v1_5.onnx` 更新到 **`gesture_v2_strongaug_int8.onnx`**，STM32端使用 X-CUBE-AI 生成的 `network_1` 模型进行本地推理。

## 📋 目录

- [项目概述](#项目概述)
- [核心功能](#核心功能)
- [硬件需求](#硬件需求)
- [软件架构](#软件架构)
- [目录结构](#目录结构)
- [快速开始](#快速开始)
- [使用指南](#使用指南)
- [常见问题](#常见问题)
- [开发文档](#开发文档)

---

## 📌 项目概述

本项目是一个基于**深度学习**的智能家居控制与健康守护系统，实现了：
- ✅ **实时手势识别** (5种输出：拳头、手掌、点赞、OK、无手势) - OV5640摄像头+OpenMV预处理
- ✅ **多传感器环境监测** (温湿度、光照、距离、烟雾、心率血氧) - 异常自动报警
- ✅ **手势控制智能设备** (LED可调光、蜂鸣器可调音) - 无接触交互
- ✅ **OneNET云平台数据上传** (MQTT协议) - 远程监控
- ✅ **蓝牙近场控制** (手机APP) - 低延迟通信
- ✅ **OLED实时数据显示** (传感器数据、手势结果、系统状态)
- ✅ **上位机工具链** (数据集采集、模型训练、ONNX导出、X-CUBE-AI部署)

### 技术亮点
- 🧠 **轻量级CNN模型** (GestureNet V2，SE注意力/残差/空间池化可选)
- 🚀 **X-CUBE-AI部署** (`gesture_v2_strongaug_int8.onnx` → `network_1`，权重约76KB，激活区约150KB)
- ⚡ **FreeRTOS多任务架构** (7个并发任务)
- 🔗 **OneNET物联网平台** (MQTT协议，5秒/次上传)
- 📊 **实时推理** (~30 FPS @ 64x64灰度图)
- 📷 **OpenMV图像传输** (OV5640→OpenMV→UART3→STM32)
- 💻 **上位机工具链** (数据集采集→训练→部署全流程)
- 📡 **多模通信** (UART1上位机 + UART3摄像头 + UART4蓝牙 + UART5 WiFi)
- 🚨 **智能报警** (本地LED + 蓝牙推送 + 云平台记录)

---

## 🎯 核心功能

### 系统概述

本系统是一个**智能家居环境下的智能控制与健康守护系统**，融合了AI边缘计算、多传感器融合、物联网通信等技术，实现了以下核心能力：

- 🤖 **AI视觉交互**: 基于摄像头与边缘AI的手势识别，实现无接触智能控制
- 🌡️ **环境安防**: 多传感器实时监测温湿度、光照、烟雾、距离，异常自动报警
- ❤️ **健康守护**: 心率血氧实时采集，支持健康数据追踪
- 📡 **多模通信**: WiFi云平台远程监控 + 蓝牙近场低延迟控制
- 🎛️ **智能控制**: 手势/语音/APP多模态控制智能家居设备
- 🚨 **异常报警**: 本地灯光报警 + 蓝牙消息推送 + 云平台通知

### 1. AI视觉模块 - 手势识别

**硬件配置**:
- **摄像头**: OV5640 (通过OpenMV Cam模块)
- **连接方式**: OpenMV → UART3 (PA11/PA12, 1500000波特率)
- **图像传输**: OpenMV预处理后发送64x64灰度图像到STM32

**AI推理**:
- **算法**: GestureNet V2 / V2 Spatial（深度可分离卷积 + SE注意力 + 残差连接）
- **性能**: 64x64灰度图本地推理，具体FPS受串口图像传输、OLED刷新和调试输出影响
- **部署**: `gesture_v2_strongaug_int8.onnx` 经 X-CUBE-AI 生成 `network_1`
- **量化参数**: 输入scale=0.007874016、zero_point=0；输出scale=0.147128820、zero_point=0
- **内存占用**: 权重约 76KB，激活区约 150KB（见 `Inc/network_1_data_params.h`）

**神经网络架构** (当前部署模型基于 `train_model_v2.py` 的标准 `GestureNetV2`):
```
GestureNet V2 - 改进型轻量级手势识别网络
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
输入层:  [1, 1, 64, 64]  (64x64 灰度图)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Conv1:   Conv2d(1→24, k=3, s=2, p=1)  →  [1, 24, 32, 32]
         + BatchNorm2d + ReLU6
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Block1:  DepthwiseSeparable(24→48) + SE + Residual
         + MaxPool2d(2x2)               →  [1, 48, 16, 16]
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Block2:  DepthwiseSeparable(48→96) + SE + Residual
         + MaxPool2d(2x2)               →  [1, 96, 8, 8]
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Block3:  DepthwiseSeparable(96→128) + SE + Residual
                                           [1, 128, 8, 8]
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
GlobalAvgPool: AdaptiveAvgPool2d(1)      →  [1, 128, 1, 1]
         Flatten                         →  [1, 128]
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
FC层:    Dropout(0.5)
         Linear(128→64) + ReLU
         Dropout(0.3)
         Linear(64→5)                    →  [1, 5]
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
输出层:  [fist, palm, thumbs_up, ok, nothing]
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

说明：当前部署的 `gesture_v2_strongaug_int8.onnx` 是标准 `GestureNetV2`，不是 `--spatial-head` 版本。仓库中仍保留旧部署模型 `gesture_v1_5.onnx`、V2 Lite、Spatial Head、7类候选模型和消融实验文件；
当前STM32固件以 `network_1` 的 5 类 INT8 输出为准。
```

**旧版基础网络结构**（`train_model.py`仍保留，用于对比和历史复现）:
```
Conv1:   Conv2d(1→16, k=3, s=2, p=1)  →  [1, 16, 32, 32]
DW-Conv1: DepthwiseSeparable(16→32)   →  [1, 32, 32, 32]
         - Depthwise Conv(k=3, groups=16)
         - Pointwise Conv(1x1)
         - BatchNorm + ReLU6
         + MaxPool2d(2x2)               →  [1, 32, 16, 16]
DW-Conv2: DepthwiseSeparable(32→64)   →  [1, 64, 16, 16]
         + MaxPool2d(2x2)               →  [1, 64, 8, 8]
DW-Conv3: DepthwiseSeparable(64→128)  →  [1, 128, 8, 8]
GlobalAvgPool: AdaptiveAvgPool2d(1)   →  [1, 128, 1, 1]
         Flatten                       →  [1, 128]
FC层:    Dropout(0.5)
         Linear(128→64) + ReLU
         Dropout(0.3)
         Linear(64→5)                  →  [1, 5]
输出层:  [fist, palm, thumbs_up, ok, nothing]
```

**训练配置**:
- 优化器: AdamW (lr=0.001, weight_decay=0.01)
- 损失函数: Focal Loss / CrossEntropyLoss / Label Smoothing（三选一，当前常用Focal Loss）
- 学习率调度: CosineAnnealingLR
- 早停: patience=20
- 批量大小: 32
- 数据增强:
  - 旋转 (±40°)
  - 仿射变换 (平移±15%, 缩放0.85-1.15, 剪切±10°)
  - 亮度/对比度调整 (普通增强±30%，强增强可到±50%)
  - Gamma、低对比度、暗光噪声增强（`--strong-aug`）
  - 随机擦除 (概率30%-40%, 面积2%-15%)

**支持手势** (当前固件5类输出):
| 手势 | 功能 | 说明 |
|------|------|------|
| 拳头 (fist) | 关闭设备 | LED/蜂鸣器占空比 0% |
| 手掌 (palm) | 高档运行 | LED/蜂鸣器占空比 30% |
| 点赞 (thumbs_up) | 低档运行 | LED/蜂鸣器占空比 10% |
| OK手势 (ok) | 切换目标 | 在LED和蜂鸣器间切换 |
| 无手势 (nothing) | 背景/空画面 | 不触发控制动作 |

> 注：`Python/gesture_definitions.py` 和部分历史文档中仍保留7类手势方案（含 `gun`、`call_me`），用于数据集实验和模型对比；当前STM32固件中的 `NUM_GESTURES` 为5，实际部署未使用 `gun` 和 `call_me`。

**当前部署模型相对旧版的主要改动**:

| 项目 | 旧版部署 | 当前部署 |
|------|----------|----------|
| ONNX模型 | `gesture_v1_5.onnx` | `gesture_v2_strongaug_int8.onnx` |
| STM32生成模型名 | `network` / 旧模型文件 | `network_1` |
| 网络结构 | 早期轻量GestureNet | GestureNetV2标准版 |
| 通道数 | 16→32→64→128 | 24→48→96→128 |
| 注意力机制 | 无 | SE Block通道注意力 |
| 残差连接 | 无 | 每个主卷积阶段加入ResidualBlock |
| 分类头 | GlobalAvgPool + FC | GlobalAvgPool + FC |
| 训练增强 | 普通亮度/对比度/仿射增强 | `--strong-aug`：更强亮度/对比度、Gamma、低对比度、噪声增强 |
| 损失函数 | CrossEntropy为主 | Focal Loss为主 |
| 优化器/调度 | Adam / ReduceLROnPlateau | AdamW / CosineAnnealingLR |
| 部署精度 | float ONNX导入X-CUBE-AI | 静态INT8量化ONNX导入X-CUBE-AI |

`gesture_v2_strongaug_int8.onnx` 对应的训练日志为 `Python/strongaug_train.log`：

- 模型结构：标准 `GestureNetV2`，不是 `--spatial-head` 版本
- 参数量：73,797（PyTorch训练日志），X-CUBE-AI报告中为73,229 items
- 训练配置：150 epochs上限、Focal Loss、AdamW、CosineAnnealingLR、强光照增强
- 训练划分：随机分层划分，测试集1183张
- 训练结果：最佳验证准确率99.83%，最终测试集准确率99.41%
- 早停：第108轮触发，20轮验证准确率未提升

X-CUBE-AI生成报告 `network_1_generate_report.txt` 中的部署参数：

```text
model_name: gesture_v2_strongaug_int8
input : int8(1x64x64x1), QLinear(scale=0.007874016, zero_point=0)
output: int8(1x5),       QLinear(scale=0.147128820, zero_point=0)
macc  : 10,995,965
weights: 76,460 B
activations: 150,292 B
```

难例测试集 `dataset_hard_test` 上，当前INT8模型的主要错误集中在：

- `thumbs_up` 被识别为 `fist` 或 `ok`
- `nothing` 被识别为 `fist` 或 `palm`
- 少量 `fist` 被识别为 `thumbs_up`

对应混淆矩阵见 `Python/gesture_v2_strongaug_int8_dataset_hard_test_confusion_matrix.csv`，错样本明细见 `Python/gesture_v2_strongaug_int8_dataset_hard_test_wrong_samples.csv`。

**上位机工具链支持**:
1. 🎓 **数据集采集**: 通过UART1发送指令，STM32从OpenMV获取图像并返回给PC
   - 自动标注、批量保存
   - 支持断点续传（从最大编号继续）
   - 实时显示采集进度和图像质量
2. 🔧 **数据增强**: 离线/在线数据增强，扩充数据集5-10倍
   - 旋转、平移、缩放、剪切
   - 亮度、对比度调节
   - 随机擦除模拟遮挡
3. 🧠 **模型训练**: PyTorch完整训练流程
   - 自动数据集划分（训练/验证/测试 7:1.5:1.5）
   - 学习率衰减、早停机制
   - 混淆矩阵、训练曲线可视化
4. 📦 **模型转换**: PyTorch → ONNX → X-CUBE-AI
   - 量化优化、模型压缩
   - ONNX Runtime验证
5. 🧪 **实时测试**: PC端与STM32端对比推理
   - 逐帧对比准确率
   - 性能分析（推理时间、FPS）
6. 🚀 **边缘推理**: STM32本地实时推理，无需云端依赖

### 2. 多传感器环境监测

**传感器配置**:
| 传感器 | 监测参数 | 报警阈值 | 报警方式 | 采样周期 |
|--------|---------|---------|---------|----------|
| DHT11 | 温度/湿度 | 可配置 | RGB LED + 蓝牙推送 | 1Hz |
| GY-30 (BH1750) | 光照强度 | >1000 Lux | 红色LED + 推送 | 1Hz |
| HC-SR04 | 超声波距离 | <10cm (进入) / >15cm (解除) | 绿色LED + 推送 | 1Hz |
| MQ-2 | 烟雾浓度 | >10 PPM | 蓝色LED + 推送 | 1Hz |

**智能报警**:
- ⚡ **本地响应**: RGB三色LED独立指示不同异常类型
- 📱 **蓝牙推送**: 实时推送报警消息到手机APP
- ☁️ **云平台记录**: 异常事件上传OneNET云平台存档
- 🔇 **去抖动**: 滞后阈值 + 连续3次检测确认，避免误报

### 3. 健康数据采集

**MAX30102心率血氧传感器**:
- **监测指标**: 心率 (BPM) + 血氧饱和度 (SpO2)
- **接口**: I2C通信
- **算法**: 基于PPG光电容积脉搏波描记法
- **用途**:
  - 实时健康状态监测
  - 数据上传云平台形成健康档案
  - 异常指标本地提示

### 4. 通信模块 - 多模连接

#### 4.1 上位机调试 (UART1)
- **连接**: PC ↔ STM32 UART1 (PA9/PA10)
- **波特率**: 921600
- **功能**:
  - 发送控制指令到STM32
  - 接收摄像头图像数据用于数据集采集
  - 实时查看推理结果和传感器数据
  - 系统调试和日志输出

#### 4.2 蓝牙近场通信 (UART4)
- **连接**: 蓝牙模块 ↔ STM32 UART4 (PA0/PA1)
- **协议**: SPP (Serial Port Profile)
- **功能**:
  - 📱 **手机APP控制**: 切换控制目标、调节占空比
  - 🚨 **报警推送**: 环境/健康异常实时推送
  - 📊 **数据查询**: 查看当前传感器状态
  - ⚡ **低延迟**: 本地通信，响应<100ms

**蓝牙命令**:
| 命令 | 功能 | 响应 |
|------|------|------|
| `A` | 切换控制目标 (LED ↔ 蜂鸣器) | `Target: LED/BUZZ` |
| `B` | 设置占空比 0% | `Duty: 0%` |
| `C` | 设置占空比 10% | `Duty: 10%` |
| `D` | 设置占空比 30% | `Duty: 30%` |

#### 4.3 WiFi云平台通信 (UART5)
- **模块**: ESP8266 WiFi模块
- **连接**: ESP8266 ↔ STM32 UART5 (PC12/PD2)
- **平台**: 中国移动OneNET物联网平台
- **协议**: MQTT
- **上传周期**: 5秒/次（可配置）

**数据流**:
| 数据流ID | 内容 | 格式 |
|---------|------|------|
| `gesture` | 手势识别结果 | `{id: 0-4, conf: 0.0-1.0}` |
| `temp_humi` | 温湿度 | `{temp: °C, humi: %}` |
| `light` | 光照强度 | `{lux: float}` |
| `distance` | 距离 | `{cm: int}` |
| `mq2` | 烟雾浓度 | `{ppm: float}` |
| `hr_spo2` | 心率血氧 | `{hr: bpm, spo2: %}` |
| `alarm` | 报警状态 | `{type: string, level: int}` |

### 5. 用户交互

#### 5.1 OLED显示屏
- **尺寸**: 0.96寸 128x64
- **接口**: SPI
- **显示内容**:
  - 实时传感器数据（温湿度、光照、距离、烟雾）
  - 当前手势识别结果 + 置信度
  - 控制目标和占空比状态
  - WiFi连接状态
  - 报警信息提示

#### 5.2 RGB指示灯
- **红灯** (PD10): 光照过强报警
- **绿灯** (PG7): 距离过近报警
- **蓝灯** (PG4): 烟雾检测报警

#### 5.3 PWM控制输出
- **LED3** (PB14): 可调光LED，0/10/30% 三档
- **蜂鸣器** (PG6): 可调音量，0/10/30% 三档

### 6. 智能控制场景

#### 场景1: 手势控制照明
```
用户做出"手掌"手势 → AI识别 → LED3亮度30%
用户做出"点赞"手势 → AI识别 → LED3亮度10%
用户做出"拳头"手势 → AI识别 → LED3关闭
```

#### 场景2: 环境异常报警
```
烟雾浓度 >10 PPM → 蓝灯亮起 → 蓝牙推送"烟雾报警" → 上传云平台
光照 >1000 Lux → 红灯亮起 → 蓝牙推送"光照异常" → 上传云平台
距离 <10cm → 绿灯亮起 → 蓝牙推送"有人靠近" → 上传云平台
```

#### 场景3: 远程监控
```
手机APP → 查询OneNET云平台 → 获取实时传感器数据
手机APP → 蓝牙连接STM32 → 发送"A"命令 → 切换控制目标
```

### 7. 系统特色

✨ **边缘AI**: 本地推理无需联网，响应速度快，隐私安全
✨ **多模通信**: WiFi远程 + 蓝牙近场，适应不同场景
✨ **异常预警**: 多级传感器融合，误报率低
✨ **健康守护**: 心率血氧实时监测，关爱家人健康
✨ **开发友好**: 完整数据采集→训练→部署工具链
✨ **可扩展**: 模块化设计，易于添加新功能

---

## 🛠 硬件需求

### 核心板
- **MCU**: STM32H753ZIT6 (480MHz, 2MB Flash, 1MB RAM)
- **开发板**: ST官方 NUCLEO-H753ZI 开发板

### 外设模块
| 模块 | 接口 | 引脚 | 说明 |
|------|------|------|------|
| 上位机 | UART1 | PA9/PA10 | 数据集采集、调试、日志输出 |
| OpenMV Cam (OV5640) | UART3 | PA11/PA12 | 发送64x64灰度图像数据 |
| 蓝牙模块 | UART4 | PA0/PA1 | 近场控制、报警推送 |
| ESP8266 WiFi | UART5 | PC12/PD2 | OneNET云平台数据上传 |
| DHT11 | GPIO | PG9 | 温湿度传感器 |
| GY-30 (BH1750) | I2C | - | 光照传感器 |
| HC-SR04 | GPIO | - | 超声波测距 |
| MQ-2 | ADC | - | 烟雾传感器 |
| MAX30102 | I2C | - | 心率血氧 |
| OLED | SPI | - | 0.96寸显示屏 |
| RGB LED | GPIO | PD10/PG7/PG4 | 红绿蓝三色灯 |
| LED3 | GPIO | PB14 | PWM可调光 |
| 蜂鸣器 | GPIO | PG6 | PWM可调音量 |

---

## 🏗 软件架构

### FreeRTOS任务架构

```
┌─────────────────────────────────────────┐
│         FreeRTOS Scheduler              │
└─────────────────────────────────────────┘
           │
           ├─→ CameraTask (优先级3)
           │   └─ OpenMV图像接收 → imageQueue
           │
           ├─→ InferenceTask (优先级3)
           │   └─ AI推理 → 手势识别
           │
           ├─→ SensorTask (优先级2)
           │   └─ 多传感器数据采集
           │
           ├─→ UartTxTask (优先级2)
           │   └─ 串口数据发送
           │
           ├─→ UartRxTask (优先级2)
           │   └─ 蓝牙命令接收
           │
           ├─→ OneNETTask (优先级1)
           │   └─ 云平台数据上传
           │
           └─→ DisplayTask (优先级1)
               └─ OLED显示更新
```

### 数据流

```
OpenMV (OV5640) → UART3 → CameraTask → imageQueue
                                           ↓
                                     InferenceTask
                                     (X-CUBE-AI)
                                           ↓
                               ┌───────────┴───────────┐
                               ↓                       ↓
                       GestureControl          UartTxTask
                       (LED/蜂鸣器)            (UART1上位机)
                               ↓
                            OLED显示

传感器数据 → SensorTask → OLED显示
                      ↓
              ┌───────┴────────┐
              ↓                ↓
         蓝牙推送         OneNET云平台
        (UART4)            (UART5)
```

### AI推理流程

```
1. 图像预处理
   uint8[64x64] → float32归一化 → int8量化输入
   归一化: normalized = (x/255 - 0.5) / 0.5  →  [-1, 1]
   量化: q = round(normalized / 0.007874016)，并裁剪到[-128, 127]

2. 神经网络推理
   int8 Input[1,64,64,1] → X-CUBE-AI network_1 → int8 Output[1,5]

3. 后处理
   反量化: logit = output_int8 * 0.147128820
   Softmax(logits) → Probabilities[0-1]
   argmax() → Gesture ID

4. 手势滤波
   滑动窗口(size=5) → 稳定手势输出
```

---

## 🧠 神经网络训练详解 (当前基于 train_model_v2.py)

### 模型架构设计思路

当前推荐训练脚本为 `Python/train_model_v2.py`。相比早期 `train_model.py`，V2版本不只是换了模型名称，而是同时调整了网络结构、训练划分方式、损失函数、增强策略和评估输出。旧版 `train_model.py` 仍保留在仓库中，主要用于复现早期GestureNet结果和对比实验。

GestureNet V2 采用**深度可分离卷积 + SE注意力 + 残差连接**架构，灵感来源于 MobileNet，同时针对手势形状差异和MCU部署做了轻量化处理：

**为什么选择深度可分离卷积？**
- **参数量减少**: 标准卷积参数量 = `K×K×Cin×Cout`，深度可分离卷积 = `K×K×Cin + Cin×Cout`
- **计算量降低**: 减少 8-9 倍计算量，适合资源受限的 MCU
- **精度保持**: 在轻量化的同时保持较高识别准确率

**V2网络各层设计原理**:

1. **Conv1 (1→24, stride=2)**:
   - 快速降维到 32x32，减少后续计算量
   - 使用 ReLU6 激活函数（对量化友好）

2. **Block1 (24→48)**:
   - 深度可分离卷积提取局部特征
   - SEBlock学习通道权重，增强关键手部轮廓特征
   - ResidualBlock缓解深层轻量网络的退化问题
   - MaxPool 进一步降维到 16x16

3. **Block2 (48→96)**:
   - 提取中层特征（边缘、纹理）
   - 降维到 8x8

4. **Block3 (96→128)**:
   - 提取高层语义特征
   - 保持 8x8 分辨率

5. **分类头（两种）**:
   - `GestureNetV2`: Global Average Pooling，参数更少
   - `GestureNetV2Spatial`: 2x2空间池化头，保留粗空间位置信息，更适合区分拇指突起、OK圆圈等位置敏感特征

6. **FC层**:
   - Dropout 防止过拟合
   - 最终输出 5 个类别的 logits

**可选模型结构**:

| 参数 | 模型 | 说明 |
|---|---|---|
| 默认 | `GestureNetV2` | 标准V2模型，24→48→96→128通道 |
| `--lite` | `GestureNetV2Lite` | 16→32→64→96通道，降低RAM占用 |
| `--spatial-head` | `GestureNetV2Spatial` | 2x2空间池化分类头，当前实验中常用于增强空间判别能力 |

### 训练流程 (train_model_v2.py 详解)

**1. 数据集加载**
```python
# 自动扫描数据集目录
dataset_real/ 或 dataset_augmented/
├── 0_fist/    (1000张)
├── 1_palm/    (1000张)
├── 2_thumbs_up/  (1000张)
├── 3_ok/      (1000张)
└── 4_nothing/ (1000张)

# 自动划分比例
训练集: 70% (3500张)
验证集: 15% (750张)
测试集: 15% (750张)
```

V2新增了 **session-based划分**，可以按连续编号区间划分训练/验证/测试集，避免同一连续采集片段中的近重复帧同时出现在训练集和测试集，评估结果更接近真实泛化能力。

```bash
--split-mode session
--session-size 100
--split-manifest-prefix split_session_shuffle
```

启用后会生成：

```text
split_session_shuffle_train.csv
split_session_shuffle_val.csv
split_session_shuffle_test.csv
```

**2. 数据增强策略**
```python
# 几何变换
RandomRotation(40°)           # 适应不同手势角度
RandomAffine(translate=15%, scale=0.85-1.15)  # 适应距离变化
RandomAffine(shear=10°)       # 适应视角变化

# 颜色增强
ColorJitter(brightness=0.3, contrast=0.3)  # 适应光照变化

# 强光照增强 (--strong-aug)
ColorJitter(brightness=0.5, contrast=0.5)
RandomGamma(0.6-1.6)
RandomLowContrast(0.3-0.7)
RandomGaussianNoise(3.0-15.0)

# 随机擦除
RandomErasing(p=0.3-0.4, scale=0.02-0.15)  # 模拟遮挡
```

**3. 训练策略**
```python
# 优化器
AdamW(lr=0.001, weight_decay=0.01)

# 学习率调度
CosineAnnealingLR(T_max=NUM_EPOCHS, eta_min=1e-6)

# 损失函数（三选一）
CrossEntropyLoss()
FocalLoss(gamma=2)              # 默认，强化难样本
LabelSmoothingLoss(smoothing=0.1)

# 早停机制
EarlyStopping(patience=20)  # 20个epoch无改善则停止
```

**4. 训练输出**
```
gesture_v2*.pth             # PyTorch 模型权重
gesture_v2*.onnx            # ONNX 导出模型
gesture_v2*_int8.onnx       # INT8量化模型
training_curves_v2.png      # 训练/验证 损失和准确率曲线
confusion_matrix_v2.png     # 混淆矩阵 (评估各类别表现)
*_wrong_samples.csv         # 错误样本分析
*_confusion_matrix.csv      # 数据集评估矩阵
分类报告 (终端输出)          # Precision/Recall/F1-Score
```

**5. 性能指标**
```
输入尺寸: [1, 1, 64, 64]
输出类别: [fist, palm, thumbs_up, ok, nothing]
模型大小: V2 ONNX约300KB量级，INT8后约190KB-220KB（随结构变化）
STM32模型: X-CUBE-AI生成network_1，权重约76KB，激活区约150KB
```

### 使用方法

**当前部署模型对应训练命令（V2 + 强增强）**:
```bash
python train_model_v2.py \
  --dataset dataset_real \
  --epochs 150 \
  --batch-size 32 \
  --lr 0.001 \
  --loss focal \
  --strong-aug \
  --model-name gesture_v2_strongaug.pth
```

说明：`--spatial-head`、`--split-mode session`、`--split-manifest-prefix` 是后来用于消融实验和更严格评估的能力，仓库中保留了 `gesture_v2_session_smooth_spatial_shuffle.*`、`gesture_v2_ablation*.onnx` 等实验产物；当前烧录到STM32的部署链路以 `gesture_v2_strongaug_int8.onnx` 为准。

**快速测试 (使用10%数据)**:
```bash
python train_model_v2.py \
  --dataset dataset_real \
  --subset 0.1 \
  --epochs 10
```

**使用预增强数据 (跳过在线增强)**:
```bash
python train_model_v2.py \
  --dataset dataset_augmented \
  --no-augment \
  --num-workers 4
```

**导出ONNX**:
```bash
python convert_to_onnx_v2.py \
  --input gesture_v2_strongaug.pth \
  --output gesture_v2_strongaug.onnx \
  --model-type v2
```

**静态INT8量化**:
```bash
python quantize_onnx_int8.py \
  --model gesture_v2_strongaug.onnx \
  --output gesture_v2_strongaug_int8.onnx \
  --dataset dataset_real \
  --calib-images 300
```

**评估ONNX/INT8模型**:
```bash
python eval_onnx_dataset.py \
  --model gesture_v2_strongaug_int8.onnx \
  --dataset dataset_hard_test
```

**核对STM32量化等价性**:
```bash
python verify_stm32_equivalence.py
```

### 模型优化技巧

1. **数据集质量**:
   - 每个手势至少 500 张图像
   - 多角度、多光照、多距离采集
   - 使用 `augment_dataset.py` 扩充到 5-10 倍

2. **超参数调优**:
   - 学习率: 0.0005 - 0.002 之间尝试
   - 批量大小: 根据 GPU 内存调整 (16/32/64)
   - Dropout: 0.3 - 0.5 之间调整

3. **训练技巧**:
   - 使用 GPU 加速 (CUDA)
   - 监控训练曲线，防止过拟合
   - 查看混淆矩阵，针对性增强易混淆类别数据

4. **部署优化**:
   - ONNX 导出后使用 X-CUBE-AI 量化
   - 验证 STM32 推理结果与 PC 端一致性
   - 使用 `realtime_inference_compare.py` 对比测试

---

## 📁 目录结构

```
project7/
├── App/                      # 应用层代码
│   ├── inference_task.c/h    # AI推理任务
│   ├── sensor_task.c/h       # 传感器采集任务
│   ├── uart_tx_task.c/h      # 串口发送任务
│   ├── uart_rx_task.c/h      # 串口接收任务
│   ├── gesture_control.c/h   # 手势控制逻辑
│   ├── task_common.c/h       # 任务公共定义
│   ├── uart_callbacks.c/h    # UART中断回调
│   ├── esp8266.c/h           # ESP8266 WiFi驱动
│   └── openmv_rx_task.c/h    # OpenMV数据接收解析
│
├── Hardware/                 # 硬件驱动层
│   ├── DHT11.c/h             # 温湿度传感器
│   ├── BH1750.c/h            # 光照传感器
│   ├── HCSR04.c/h            # 超声波测距
│   ├── MQ2.c/h               # 烟雾传感器
│   ├── MAX30102.c/h          # 心率血氧传感器
│   ├── algorithm.c/h         # MAX30102算法
│   ├── OLED.c/h              # OLED显示驱动
│   ├── disp.c/h              # 显示UI逻辑
│   ├── bluetooth.c/h         # 蓝牙通信
│   ├── sensors.c/h           # 传感器统一接口
│   ├── led.c/h               # LED控制
│   └── key.c/h               # 按键检测
│
├── Src/                      # STM32 HAL层源码
│   ├── main.c                # 主函数
│   ├── freertos.c            # FreeRTOS配置
│   ├── app_x-cube-ai.c       # X-CUBE-AI推理接口
│   ├── stm32h7xx_it.c        # 中断处理
│   └── ...                   # 其他HAL文件
│
├── Inc/                      # STM32 HAL层头文件
│   ├── main.h
│   ├── FreeRTOSConfig.h      # FreeRTOS配置
│   ├── app_x-cube-ai.h
│   ├── network_1.h           # 当前AI模型接口(network_1)
│   ├── network_1_data.h      # 当前AI模型权重声明
│   ├── network_1_data_params.h # 当前AI模型权重/激活区参数
│   ├── network.h             # 旧版AI模型接口(历史保留)
│   ├── network_data.h        # 旧版AI模型权重(历史保留)
│   └── ...
│
├── Python/                   # 训练与工具脚本
│   ├── 📊 数据采集工具
│   │   ├── collect_training_data.py  # UART1采集图像数据集
│   │   ├── collect_webcam_data.py    # PC摄像头采集数据集
│   │   └── data_collector.py         # 数据采集基础类
│   │
│   ├── 🔧 数据增强工具
│   │   └── augment_dataset.py        # 离线数据增强(×5-10倍)
│   │
│   ├── 🧠 模型训练工具
│   │   ├── train_model.py            # 旧版GestureNet训练脚本
│   │   └── train_model_v2.py         # 当前主训练脚本(V2/V2Lite/Spatial)
│   │
│   ├── 📦 模型转换工具
│   │   ├── convert_to_onnx_v2.py     # PyTorch→ONNX转换
│   │   └── quantize_onnx_int8.py     # ONNX静态INT8量化
│   │
│   ├── 🧪 模型测试工具
│   │   ├── test_pytorch_model.py     # PyTorch模型测试
│   │   ├── test_pytorch_model_v2.py  # v2版本
│   │   ├── test_onnx_model.py        # ONNX模型验证
│   │   ├── eval_onnx_dataset.py      # ONNX数据集评估/错样本导出
│   │   ├── realtime_inference.py     # PC端实时推理
│   │   ├── realtime_inference_webcam.py   # PC摄像头实时推理
│   │   ├── realtime_inference_compare.py  # PC vs STM32对比
│   │   └── verify_stm32_equivalence.py    # STM32量化预处理/后处理等价性核对
│   │
│   ├── 🔧 调试工具
│   │   ├── debug_inference.py        # 推理过程调试
│   │   ├── inspect_wrong_samples.py  # 错误样本检查
│   │   ├── analyze_wrong_features.py # 错误样本特征统计
│   │   └── test_serial.py            # 串口通信测试
│   │
│   └── 📚 配置与定义
│       ├── gesture_definitions.py    # 手势类别定义
│       └── requirements.txt          # Python依赖包
│
├── Drivers/                  # STM32驱动库
│   ├── CMSIS/                # ARM CMSIS库
│   └── STM32H7xx_HAL_Driver/ # HAL库
│
├── Middlewares/              # 中间件
│   ├── FreeRTOS/             # FreeRTOS源码
│   └── ST/AI/                # X-CUBE-AI运行时
│
├── MDK-ARM/                  # Keil MDK工程
├── .ai/                      # AI模型缓存
├── network_1_generate_report.txt # 当前network_1模型生成报告
├── project2.ioc              # STM32CubeMX配置
│
└── 文档/
    └── README.md             # 本文件
```

---

## 📂 核心文件说明

### 🔹 应用层 (App/)

| 文件 | 说明 | 关键函数 | 详细功能 |
|------|------|---------|----------|
| **inference_task.c** | AI手势推理任务 | `AppTask_inference()` | 从图像队列获取数据，调用X-CUBE-AI推理接口，获取手势识别结果和置信度，更新OLED显示，触发手势控制动作，发送结果到UART |
| **sensor_task.c** | 多传感器数据采集 | `AppTask_sensor()` | 采集DHT11(温湿度)、BH1750(光照)、HC-SR04(距离)、MQ2(烟雾)、MAX30102(心率血氧)，使用FFT算法处理心率血氧数据，移动平均滤波，更新全局传感器数据结构 |
| **gesture_control.c** | 手势控制与报警 | `GestureControl_HandleGesture()` | 手势滤波(5帧移动窗口)，手势映射到LED/蜂鸣器控制，软件PWM(1kHz, TIM2中断)，传感器报警检测(光照/距离/烟雾)，RGB LED报警指示，蓝牙报警推送 |
| **uart_tx_task.c** | 串口发送任务 | `UartTxTask_SendImage()` | 发送64x64图像数据到上位机(UART1)用于数据集采集，发送推理结果和FPS，发送进度信息，OneNET数据上传准备，蓝牙数据发送 |
| **uart_rx_task.c** | 串口接收任务 | `AppTask_uart_rx()` | 接收上位机命令(UART1, 921600bps)，解析采集/推理/状态查询命令，接收蓝牙控制命令(UART4)，中断驱动接收，命令路由分发 |
| **openmv_rx_task.c** | OpenMV数据接收 | `AppTask_openmv_rx()` | UART3 DMA循环接收OpenMV图像数据，空闲中断触发解析，帧头/帧尾校验，校验和验证，双缓冲机制，FPS统计，超时重连机制 |
| **esp8266.c** | WiFi模块驱动 | `ESP8266_SendMQTTMessage()` | ESP8266初始化(AT指令配置)，连接WiFi热点，配置MQTT客户端(OneNET平台)，发布传感器数据到云平台，MQTT主题订阅 |
| **task_common.c** | 任务公共资源 | `Task_SetMode()`, `Task_GetFPS()` | 工作模式管理(IDLE/COLLECTING/INFERENCE)，FPS全局变量，图像采集进度计数器，模式切换函数 |
| **uart_callbacks.c** | UART回调统一管理 | `HAL_UART_RxCpltCallback()` | 统一处理所有UART中断回调(UART1/3/4/5/6)，避免符号重复定义，分发到对应任务处理函数，错误处理和自动恢复 |

### 🔹 驱动层 (Hardware/)

| 文件 | 说明 | 接口 |
|------|------|------|
| **DHT11.c** | 温湿度传感器 | `DHT11_Read()` |
| **BH1750.c** | 光照传感器 | `BH1750_ReadLightIntensity()` |
| **HCSR04.c** | 超声波测距 | `HCSR04_GetDistance()` |
| **MQ2.c** | 烟雾传感器 | `MQ2_ReadPPM()` |
| **MAX30102.c** | 心率血氧 | `MAX30102_ReadFIFO()` |
| **OLED.c** | OLED驱动 | `OLED_ShowString()`, `OLED_DrawBitmap()` |
| **disp.c** | 显示UI | `OLED_ShowSensorData()`, `OLED_ShowGestureResult()` |
| **bluetooth.c** | 蓝牙协议 | `BT_ParseCommand()` |
| **sensors.c** | 传感器统一接口 | `Sensors_Init()`, `Sensors_ReadAll()` |

### 🔹 AI推理 (Src/app_x-cube-ai.c)

| 函数 | 说明 |
|------|------|
| `MX_X_CUBE_AI_Init()` | 初始化AI模型 |
| `MX_X_CUBE_AI_Inference()` | 单次推理（返回最大概率类别） |
| `MX_X_CUBE_AI_Inference_Full()` | 完整推理（返回所有类别概率） |
| `preprocess_image()` | 图像预处理 (uint8→归一化→int8量化) |
| `postprocess_output()` | int8输出反量化 + Softmax + argmax |

### 🔹 INT8量化与STM32端处理

当前部署模型 `gesture_v2_strongaug_int8.onnx` 是经过静态INT8量化后的ONNX模型，再通过 X-CUBE-AI 生成 `network_1`。量化不是只“压缩文件大小”，还影响 STM32 端输入/输出的数据格式，因此 README 中的部署流程必须保证训练预处理、ONNX量化和 C 端推理一致。

**1. 量化生成**

```bash
cd Python
python quantize_onnx_int8.py \
  --model gesture_v2_strongaug.onnx \
  --output gesture_v2_strongaug_int8.onnx \
  --dataset dataset_real \
  --calib-images 300
```

脚本使用 `onnxruntime.quantization.quantize_static` 做静态量化：

- 校准数据来自 5 类目录：`0_fist`、`1_palm`、`2_thumbs_up`、`3_ok`、`4_nothing`
- 输入预处理保持训练一致：灰度图resize到64x64，再归一化到 `[-1, 1]`
- 量化格式使用 QDQ，激活和权重均使用 `QInt8`
- 量化后会对比 float ONNX 和 INT8 ONNX 的准确率、Top-1一致率和输出误差

**2. X-CUBE-AI导入结果**

`network_1_generate_report.txt` 中确认当前模型参数：

```text
model file : gesture_v2_strongaug_int8.onnx
model_name : gesture_v2_strongaug_int8
input      : int8(1x64x64x1), QLinear(0.007874016, 0, int8)
output     : int8(1x5),       QLinear(0.147128820, 0, int8)
macc       : 10,995,965
weights    : 76,460 B
activations: 150,292 B
```

**3. STM32端输入量化**

`Src/app_x-cube-ai.c` 中的 `preprocess_image()` 不再只做 float 归一化，而是复刻训练预处理后量化为 int8：

```c
normalized = image / 255.0f;
x = (normalized - 0.5f) / 0.5f;      // [-1, 1]
q = round(x / 0.007874016f);         // zero_point = 0
q = clamp(q, -128, 127);
```

这样 `uint8[64x64]` 图像会进入 `int8` 输入buffer，与 X-CUBE-AI 报告中的 `input int8(1x64x64x1)` 对齐。

**4. STM32端输出反量化与Softmax**

X-CUBE-AI输出为 `int8(1x5)`，C端先反量化为logits，再做softmax：

```c
logit = output_int8 * 0.147128820f;  // zero_point = 0
prob = softmax(logits);
gesture_id = argmax(prob);
```

完整置信度数组通过 `MX_X_CUBE_AI_Inference_Full()` 传给 `InferenceTask_GetAllConfidences()`，用于OLED置信度显示和串口调试。

**5. 等价性验证**

修改量化参数、模型文件或C端预处理后，应运行：

```bash
cd Python
python verify_stm32_equivalence.py
```

该脚本用于核对 PC 端 ONNX 推理和 STM32 C 端“归一化→int8量化→反量化→softmax”链路是否数学一致。若结果偏差明显，优先检查 `AI_INPUT_QUANT_SCALE`、`AI_OUTPUT_QUANT_SCALE`、输入layout和模型类别数是否匹配。

### 🔹 Python工具

| 脚本 | 功能 | 用法 |
|------|------|------|
| **train_model_v2.py** | 当前主训练脚本，训练GestureNetV2 | `python train_model_v2.py --dataset dataset_real --epochs 150 --loss focal --strong-aug` |
| **train_model.py** | 旧版GestureNet训练脚本，保留用于对比 | `python train_model.py --dataset dataset --epochs 100` |
| **collect_training_data.py** | 从STM32采集训练数据 | `python collect_training_data.py --port COM3` |
| **collect_webcam_data.py** | 使用PC摄像头采集训练数据 | `python collect_webcam_data.py --dataset dataset_webcam` |
| **augment_dataset.py** | 离线数据增强 | `python augment_dataset.py --input dataset --output dataset_aug --factor 5` |
| **convert_to_onnx_v2.py** | PyTorch→ONNX转换 | `python convert_to_onnx_v2.py --input gesture_v2_strongaug.pth --output gesture_v2_strongaug.onnx --model-type v2` |
| **quantize_onnx_int8.py** | ONNX静态INT8量化 | `python quantize_onnx_int8.py --model gesture_v2_strongaug.onnx --output gesture_v2_strongaug_int8.onnx --dataset dataset_real` |
| **eval_onnx_dataset.py** | ONNX/INT8模型数据集评估 | `python eval_onnx_dataset.py --model gesture_v2_strongaug_int8.onnx --dataset dataset_hard_test` |
| **realtime_inference.py** | PC端实时测试 | `python realtime_inference.py --port COM3` |
| **test_onnx_model.py** | ONNX模型验证 | `python test_onnx_model.py --model gesture_v2_strongaug.onnx` |
| **debug_inference.py** | STM32与PC端ONNX推理对比调试 | `python debug_inference.py --port COM3 --onnx-model gesture_v2_strongaug_int8.onnx` |
| **verify_stm32_equivalence.py** | 核对STM32量化预处理/后处理等价性 | `python verify_stm32_equivalence.py` |

---

---

## 🚀 快速开始

### 1️⃣ 环境准备

#### STM32开发环境
- **IDE**: Keil MDK-ARM v5.38+
- **工具**: STM32CubeMX v6.0+, STM32CubeProgrammer
- **SDK**: X-CUBE-AI v7.1.0+

#### Python环境
```bash
cd Python
pip install -r requirements.txt
```

`requirements.txt` 内容：
```
torch>=1.10.0
torchvision>=0.11.0
opencv-python>=4.5.0
numpy>=1.21.0
matplotlib>=3.4.0
seaborn>=0.11.0
scikit-learn>=0.24.0
pillow>=8.3.0
pyserial>=3.5
onnx>=1.10.0
onnxruntime>=1.9.0
tqdm>=4.62.0
```

### 2️⃣ 硬件连接

参考 `QUICK_START.md` 进行硬件接线。

### 3️⃣ 编译烧录

```bash
# 1. 使用STM32CubeMX生成代码（如需修改配置）
# 打开 project2.ioc

# 2. 使用Keil编译
# 打开 MDK-ARM/project2.uvprojx
# Build → Rebuild All

# 3. 烧录固件
# 使用ST-Link或J-Link烧录 .hex/.bin 文件
```

### 4️⃣ 串口调试

```bash
# 连接串口（波特率921600）
# Windows: 使用串口调试助手
# Linux/Mac:
screen /dev/ttyUSB0 921600
# 或
minicom -D /dev/ttyUSB0 -b 921600
```

**预期输出**:
```
System Init...
FreeRTOS Start
Camera Task Start
Inference Task Start
Sensor Task Start
...
$RESULT,2,0.98  # 手势ID=2(thumbs_up), 置信度=98%
$FPS,28.5       # 推理帧率
```

---

## 📖 使用指南

### Python上位机工具详解

#### 🎯 工具1: 数据集采集 (collect_training_data.py)

**功能**: 通过UART1从STM32采集摄像头图像，自动构建训练数据集

**使用方法**:
```bash
cd Python
python collect_training_data.py --port COM3 --dataset-dir dataset_real
```

**主要参数**:
| 参数 | 说明 | 默认值 |
|------|------|--------|
| `--port` | 串口号 | 必需 |
| `--baudrate` | 波特率 | 921600 |
| `--dataset-dir` | 数据集保存目录 | dataset_real |

**交互流程**:
1. 选择手势ID (0-4: fist/palm/thumbs_up/ok/nothing)
2. 输入采集数量 (建议200-500张/手势)
3. 设置准备倒计时 (默认2秒)
4. 自动采集并保存到 `dataset_real/{id}_{name}/` 目录

**核心功能**:
- ✅ **自动标注**: 根据手势ID自动创建分类目录
- ✅ **断点续传**: 从最大图像编号+1继续，避免覆盖
- ✅ **实时显示**: 采集进度、图像亮度、保存状态
- ✅ **质量检测**: 显示图像均值，检测过曝/过暗
- ✅ **手动中断**: Ctrl+C提前结束，已采集数据保留

**采集技巧**:
```
左手拿OpenMV摄像头，右手做手势
- 多角度：正面、侧面(左/右)、斜视、旋转(顺时针/逆时针)
- 多距离：远(50cm)、中(30cm)、近(15cm)
- 多位置：左、中、右、上、下、对角线移动
- 多光照：明亮(窗边)、昏暗(背光)、逆光、侧光
- 多背景：纯色墙、复杂背景、室内、室外
```

**输出示例**:
```
dataset_real/
├── 0_fist/
│   ├── fist_0001.png
│   ├── fist_0002.png
│   └── ...
├── 1_palm/
│   ├── palm_0001.png
│   └── ...
└── ...
```

---

#### 🔧 工具2: 数据增强 (augment_dataset.py)

**功能**: 对原始数据集进行离线增强，扩充到5-10倍

**使用方法**:
```bash
python augment_dataset.py --input dataset_real --output dataset_aug --factor 5
```

**主要参数**:
| 参数 | 说明 | 默认值 |
|------|------|--------|
| `--input` | 输入数据集目录 | dataset |
| `--output` | 输出增强数据集目录 | dataset_augmented |
| `--factor` | 扩充倍数 (每张原图生成N张变体) | 5 |

**增强策略**:
1. **几何变换**:
   - 旋转: -40° ~ +40°
   - 平移: ±15% 水平/垂直
   - 缩放: 0.85 ~ 1.15倍
   - 剪切: ±10°

2. **颜色增强**:
   - 亮度: ±30%
   - 对比度: ±30%

3. **随机擦除**:
   - 概率: 30%
   - 面积: 2% ~ 15%
   - 模拟手指遮挡、部分手势

**效果对比**:
```
原始数据集: 200张/手势 × 4手势 = 800张
增强后(×5): 1000张/手势 × 4手势 = 4000张
训练时间: 约10分钟 (GPU) / 1小时 (CPU)
精度提升: 85% → 95%+
```

**注意事项**:
- ⚠️ 增强后数据集占用空间约为原始的5-10倍
- ⚠️ 建议保留原始数据集用于对比
- ✅ 可与训练时的在线增强配合使用

---

#### 🧠 工具3: 模型训练 (train_model_v2.py)

**功能**: 当前主训练流程，训练GestureNetV2系列模型，生成PyTorch权重，并配合 `convert_to_onnx_v2.py`、`quantize_onnx_int8.py` 完成ONNX和INT8部署模型。

**使用方法**:
```bash
python train_model_v2.py --dataset dataset_real --epochs 150 --batch-size 32 --loss focal --strong-aug --model-name gesture_v2_strongaug.pth
```

**主要参数**:
| 参数 | 说明 | 默认值 |
|------|------|--------|
| `--dataset` | 数据集目录 | dataset |
| `--batch-size` | 批量大小 | 32 |
| `--epochs` | 训练轮数 | 150 |
| `--lr` | 学习率 | 0.001 |
| `--loss` | 损失函数: ce/focal/smooth | focal |
| `--model-name` | 模型保存名称 | gesture_v2.pth |
| `--no-augment` | 跳过训练时增强 (数据已预增强) | False |
| `--num-workers` | 数据加载线程数 | 0 (Windows), 4-8 (Linux) |
| `--subset` | 使用数据子集比例 (快速测试) | 1.0 (全部) |
| `--lite` | 使用GestureNetV2Lite轻量版 | False |
| `--spatial-head` | 使用2x2空间池化分类头 | False |
| `--split-mode` | random或session划分 | random |
| `--session-size` | session划分时每组连续帧数量 | 100 |
| `--split-manifest-prefix` | 保存train/val/test划分CSV | None |
| `--strong-aug` | 启用强光照/低对比度/噪声增强 | False |

**训练流程**:
1. **数据集划分** (自动):
   - 训练集: 70%
   - 验证集: 15%
   - 测试集: 15%
   - 可选session-based划分，避免连续帧泄漏到不同集合

2. **训练策略**:
   - 优化器: AdamW
   - 损失函数: Focal Loss / CrossEntropyLoss / Label Smoothing
   - 学习率调度: CosineAnnealingLR
   - 早停: patience=20 (验证集准确率不提升)

3. **输出内容**:
   - `gesture_v2_strongaug.pth` - 当前部署链路对应PyTorch权重
   - `training_curves_v2.png` - 训练/验证曲线
   - `confusion_matrix_v2.png` - 混淆矩阵
   - 可选 `split_session_shuffle_*.csv` - session划分清单
   - 终端输出: 分类报告 (precision/recall/f1-score)

**快速测试模式**:
```bash
# 使用10%数据快速测试流程 (1-2分钟)
python train_model_v2.py --dataset dataset_real --subset 0.1 --epochs 10
```

**GPU加速**:
```bash
# 自动检测GPU，若有CUDA则自动使用
python train_model_v2.py --dataset dataset_real --epochs 150
# 输出: Using device: cuda
```

---

#### 📦 工具4: ONNX转换 (convert_to_onnx_v2.py)

**功能**: 将PyTorch模型转换为ONNX格式，用于X-CUBE-AI部署

**使用方法**:
```bash
python convert_to_onnx_v2.py --input gesture_v2_strongaug.pth --output gesture_v2_strongaug.onnx --model-type v2
```

**转换步骤**:
1. 加载PyTorch模型
2. 设置为评估模式
3. 导出ONNX (opset_version=11)
4. 验证ONNX模型可读取
5. 可选: ONNX Runtime验证推理

**注意事项**:
- ✅ 确保输入尺寸: [1, 1, 64, 64]
- ✅ 确保输出尺寸: [1, 5]
- ✅ 导出时关闭Dropout、BatchNorm等训练层
- ✅ X-CUBE-AI支持ONNX opset 9-12

**INT8量化**:
```bash
python quantize_onnx_int8.py --model gesture_v2_strongaug.onnx --output gesture_v2_strongaug_int8.onnx --dataset dataset_real --calib-images 300
```

量化后使用X-CUBE-AI导入 `gesture_v2_strongaug_int8.onnx`，生成 `network_1.*` 相关文件。

---

#### 🧪 工具5: ONNX模型测试 (test_onnx_model.py)

**功能**: 验证ONNX模型推理结果，检查转换正确性

**使用方法**:
```bash
python test_onnx_model.py --model gesture_v2_strongaug.onnx --image test_image.png

# 数据集评估（推荐用于当前INT8模型）
python eval_onnx_dataset.py --model gesture_v2_strongaug_int8.onnx --dataset dataset_hard_test
```

**测试内容**:
- ✅ 模型加载验证
- ✅ 推理速度测试
- ✅ 输出格式检查
- ✅ 与PyTorch模型对比 (可选)

---

#### 📊 工具6: 实时推理对比 (realtime_inference.py / realtime_inference_compare.py)

**功能**: PC端实时接收STM32图像，对比PC推理与STM32推理结果

**使用方法**:
```bash
# 单独PC端推理
python realtime_inference.py --port COM3 --model gesture_v2_strongaug.pth

# PC vs STM32对比
python realtime_inference_compare.py --port COM3 --model gesture_v2_strongaug.pth
```

**显示内容**:
```
Frame #123
  STM32: thumbs_up (0.98)  <-- STM32推理结果
  PC:    thumbs_up (0.97)  <-- PC端推理结果
  Match: ✓  (一致)
  FPS: 28.5
  Latency: 35ms
```

**用途**:
- 🔍 验证STM32部署正确性
- 📈 性能分析 (FPS、延迟)
- 🐛 调试推理差异

---

#### 🔬 工具7: PyTorch模型测试 (test_pytorch_model.py)

**功能**: 测试单张图像的PyTorch模型推理，查看所有类别输出

**使用方法**:
```bash
python test_pytorch_model.py
```

**输出示例**:
```
✓ 测试图像: dataset/1_palm/palm_0001.png
  预测: palm (1) - 原始分数: 3.8542
  置信度: 97.48%

  所有输出 (logits):
    0 (fist    ):  -2.5012
    1 (palm    ):   3.8542  ← MAX
    2 (thumbs_up): -1.2034
    3 (ok      ):  -0.5123
```

---

#### 🔧 工具8: 调试工具 (debug_inference.py, verify_output_issue.py)

**debug_inference.py**: 保存推理过程的中间数据
- 输入图像
- 预处理后的数据
- 模型各层输出
- 最终预测结果

**verify_output_issue.py**: 验证Softmax后处理问题
- 模拟logits输出
- 对比错误方法vs正确方法
- 用于排查旧版模型输出后处理错误

---

### 🔄 完整开发流程图

#### 从数据采集到STM32部署的完整流程

```
┌─────────────────────────────────────────────────────────────────┐
│                    第一阶段：数据采集                              │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ↓
    ┌─────────────────────────────────────────────────┐
    │  1. 硬件准备                                      │
    │  - OpenMV Cam + OV5640 摄像头                    │
    │  - STM32H753开发板                               │
    │  - USB转TTL连接PC                                │
    └─────────────────────────────────────────────────┘
                              │
                              ↓
    ┌─────────────────────────────────────────────────┐
    │  2. 运行上位机采集工具                            │
    │  python collect_training_data.py --port COM3    │
    │  - 选择手势ID (0-4)                              │
    │  - 设置采集数量 (200-500张/手势)                  │
    │  - 多角度、多光照、多距离采集                      │
    └─────────────────────────────────────────────────┘
                              │
                              ↓
    ┌─────────────────────────────────────────────────┐
    │  3. 数据集输出                                    │
    │  dataset_real/                                  │
    │  ├── 0_fist/      (200-500张)                   │
    │  ├── 1_palm/      (200-500张)                   │
    │  ├── 2_thumbs_up/ (200-500张)                   │
    │  ├── 4_nothing/   (200-500张)                   │
    │  └── 3_ok/        (200-500张)                   │
    │  总计: 800-2000张原始图像                         │
    └─────────────────────────────────────────────────┘
                              │
                              ↓
┌─────────────────────────────────────────────────────────────────┐
│                    第二阶段：数据增强                              │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ↓
    ┌─────────────────────────────────────────────────┐
    │  4. 离线数据增强                                  │
    │  python augment_dataset.py \                    │
    │    --input dataset_real \                       │
    │    --output dataset_aug \                       │
    │    --factor 5                                   │
    │                                                 │
    │  增强策略:                                       │
    │  - 几何变换 (旋转、平移、缩放、剪切)                │
    │  - 颜色调整 (亮度、对比度)                        │
    │  - 随机擦除 (模拟遮挡)                           │
    └─────────────────────────────────────────────────┘
                              │
                              ↓
    ┌─────────────────────────────────────────────────┐
    │  5. 增强后数据集                                  │
    │  dataset_aug/                                   │
    │  ├── 0_fist/      (1000-2500张)                 │
    │  ├── 1_palm/      (1000-2500张)                 │
    │  ├── 2_thumbs_up/ (1000-2500张)                 │
    │  ├── 4_nothing/   (1000-2500张)                 │
    │  └── 3_ok/        (1000-2500张)                 │
    │  总计: 4000-10000张增强图像                       │
    └─────────────────────────────────────────────────┘
                              │
                              ↓
┌─────────────────────────────────────────────────────────────────┐
│                    第三阶段：模型训练                              │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ↓
    ┌─────────────────────────────────────────────────┐
    │  6. 训练GestureNet模型                           │
    │  python train_model.py \                        │
    │    --dataset dataset_aug \                      │
    │    --epochs 100 \                               │
    │    --batch-size 32                              │
    │                                                 │
    │  自动划分:                                       │
    │  - 训练集: 70%                                   │
    │  - 验证集: 15%                                   │
    │  - 测试集: 15%                                   │
    └─────────────────────────────────────────────────┘
                              │
                              ↓
    ┌─────────────────────────────────────────────────┐
    │  7. 训练输出                                      │
    │  - gesture_v2_strongaug.pth       (PyTorch模型) │
    │  - gesture_v2_strongaug.onnx      (ONNX模型)    │
    │  - gesture_v2_strongaug_int8.onnx (INT8模型)   │
    │  - training_curves.png   (训练曲线)              │
    │  - confusion_matrix.png  (混淆矩阵)              │
    │                                                 │
    │  性能指标:                                       │
    │  - 测试集准确率: >95%                             │
    │  - INT8模型: gesture_v2_strongaug_int8.onnx     │
    └─────────────────────────────────────────────────┘
                              │
                              ↓
┌─────────────────────────────────────────────────────────────────┐
│                    第四阶段：模型验证                              │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ↓
    ┌─────────────────────────────────────────────────┐
    │  8. PC端验证                                      │
    │  - test_pytorch_model.py   (PyTorch推理测试)     │
    │  - test_onnx_model.py      (ONNX推理测试)        │
    │  - realtime_inference.py   (实时推理测试)         │
    │                                                 │
    │  验证通过: 准确率、推理速度符合要求                  │
    └─────────────────────────────────────────────────┘
                              │
                              ↓
┌─────────────────────────────────────────────────────────────────┐
│                  第五阶段：STM32部署                               │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ↓
    ┌─────────────────────────────────────────────────┐
    │  9. X-CUBE-AI转换                                │
    │  - 打开STM32CubeMX项目                           │
    │  - 导入 gesture_v2_strongaug_int8.onnx          │
    │  - 分析模型 (Analyze)                            │
    │  - 验证模型 (Validate on target)                 │
    │  - 生成C代码 (Generate Code)                     │
    │                                                 │
    │  生成文件:                                       │
    │  - Inc/network_1.h              (AI模型接口)    │
    │  - Inc/network_1_data.h         (模型权重声明)  │
    │  - Src/network_1_data*.c        (模型权重数据)  │
    │  - Src/network.c         (AI运行时)              │
    └─────────────────────────────────────────────────┘
                              │
                              ↓
    ┌─────────────────────────────────────────────────┐
    │  10. 编译烧录                                     │
    │  - Keil MDK编译固件                              │
    │  - ST-Link烧录到STM32H753                       │
    │  - 串口调试验证                                   │
    └─────────────────────────────────────────────────┘
                              │
                              ↓
    ┌─────────────────────────────────────────────────┐
    │  11. 实时测试                                     │
    │  python realtime_inference_compare.py \         │
    │    --port COM3 --model gesture_v2_strongaug.pth │
    │                                                 │
    │  对比PC推理 vs STM32推理:                         │
    │  - 准确率一致性验证                               │
    │  - FPS性能测试                                   │
    │  - 延迟测试                                      │
    └─────────────────────────────────────────────────┘
                              │
                              ↓
┌─────────────────────────────────────────────────────────────────┐
│                    部署完成 ✓                                    │
│  STM32实时手势识别系统运行中                                      │
│  - 推理速度: ~30 FPS                                            │
│  - 准确率: >95%                                                 │
│  - 延迟: <35ms                                                  │
└─────────────────────────────────────────────────────────────────┘
```

---

### 📡 数据采集详细流程

#### UART通信与图像传输流程

```
        PC端                    STM32H753                 OpenMV Cam
   (collect_training_          (CameraTask +            (OV5640摄像头)
    data.py)                    UartTxTask)
        │                           │                         │
        │                           │                         │
        │  1. 发送采集命令          │                         │
        ├──────────────────────────>│                         │
        │   "CMD:GET_IMAGE\r\n"     │                         │
        │   (UART1, 921600bps)      │                         │
        │                           │                         │
        │                           │  2. 请求图像              │
        │                           ├────────────────────────>│
        │                           │   (UART3, 1500000bps)   │
        │                           │                         │
        │                           │  3. 发送64x64灰度图       │
        │                           │<────────────────────────┤
        │                           │   包头: $IMG,4096       │
        │                           │   数据: [4096 bytes]     │
        │                           │   包尾: \r\n            │
        │                           │                         │
        │                           │  4. 缓存到imageQueue     │
        │                           │     (FreeRTOS队列)       │
        │                           │                         │
        │  5. 返回图像数据           │                         │
        │<──────────────────────────┤                         │
        │   包头: $IMG,4096         │                         │
        │   数据: [4096 bytes]      │                         │
        │   包尾: \r\n              │                         │
        │   (UART1, 921600bps)      │                         │
        │                           │                         │
        │  6. 解析并保存             │                         │
        │  - 校验数据长度            │                         │
        │  - 转换为64x64数组         │                         │
        │  - 保存为PNG文件           │                         │
        │  - 显示图像                │                         │
        ↓                           ↓                         ↓
   fist_0001.png              等待下次请求                  待命状态
   fist_0002.png
   ...
```

**采集速度**:
- 单张图像传输时间: ~50ms (UART1瓶颈)
- 采集速度: ~20张/秒
- 采集500张耗时: ~25秒

**注意事项**:
1. UART1波特率921600限制了传输速度
2. imageQueue深度=5，避免数据积压
3. 超时机制: 2秒无响应自动重试
4. 自动跳过重复编号，从最大编号+1开始

---

### 🧠 模型训练详细流程

```
┌─────────────────────────────────────────────────────────────────┐
│  开始训练                                                         │
│  python train_model_v2.py --dataset dataset_real --epochs 150  │
│      --loss focal --strong-aug --model-name gesture_v2_strongaug.pth │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ↓
    ┌─────────────────────────────────────────────────┐
    │  阶段1: 数据集加载与划分                          │
    ├─────────────────────────────────────────────────┤
    │  1. 扫描数据集目录                                │
    │     dataset_real/                               │
    │     ├── 0_fist/    (1000张)                     │
    │     ├── 1_palm/    (1000张)                     │
    │     ├── 2_thumbs_up/ (1000张)                  │
    │     ├── 3_ok/      (1000张)                     │
    │     └── 4_nothing/ (1000张)                     │
    │                                                 │
    │  2. 自动划分数据集                                │
    │     训练集: 70%                                  │
    │     验证集: 15%                                  │
    │     测试集: 15%                                  │
    │                                                 │
    │  3. 创建DataLoader                               │
    │     batch_size = 32                             │
    │     num_workers = 4 (Linux) / 0 (Windows)       │
    │     shuffle = True (训练集)                      │
    └─────────────────────────────────────────────────┘
                              │
                              ↓
    ┌─────────────────────────────────────────────────┐
    │  阶段2: 模型初始化                                │
    ├─────────────────────────────────────────────────┤
    │  1. 创建GestureNetV2模型                         │
    │     - 输入: [1, 1, 64, 64]                      │
    │     - 输出: [1, 5] (5类手势)                     │
    │     - 参数量: 73,797                             │
    │                                                 │
    │  2. 定义损失函数和优化器                          │
    │     loss_fn = FocalLoss()                       │
    │     optimizer = AdamW(lr=0.001)                 │
    │     scheduler = CosineAnnealingLR()             │
    │                                                 │
    │  3. 移动到GPU (如果可用)                          │
    │     device = cuda / cpu                         │
    │     model.to(device)                            │
    └─────────────────────────────────────────────────┘
                              │
                              ↓
    ┌─────────────────────────────────────────────────┐
    │  阶段3: 训练循环                                  │
    ├─────────────────────────────────────────────────┤
    │  For epoch in range(150):                       │
    │                                                 │
    │    ┌─────────────────────────────────────┐     │
    │    │  3.1 训练阶段                        │     │
    │    │  model.train()                      │     │
    │    │                                     │     │
    │    │  For batch in train_loader:         │     │
    │    │    - 前向传播: output = model(x)    │     │
    │    │    - 计算损失: loss = loss_fn(...)  │     │
    │    │    - 反向传播: loss.backward()      │     │
    │    │    - 更新权重: optimizer.step()     │     │
    │    │                                     │     │
    │    │  训练集准确率: 88 batch平均          │     │
    │    └─────────────────────────────────────┘     │
    │                   │                             │
    │                   ↓                             │
    │    ┌─────────────────────────────────────┐     │
    │    │  3.2 验证阶段                        │     │
    │    │  model.eval()                       │     │
    │    │                                     │     │
    │    │  For batch in val_loader:           │     │
    │    │    - 前向传播 (no gradient)         │     │
    │    │    - 计算准确率                      │     │
    │    │                                     │     │
    │    │  验证集准确率: 95.2%                 │     │
    │    └─────────────────────────────────────┘     │
    │                   │                             │
    │                   ↓                             │
    │    ┌─────────────────────────────────────┐     │
    │    │  3.3 学习率调整                      │     │
    │    │  scheduler.step(val_acc)            │     │
    │    │  - 验证集准确率不提升5次 → lr × 0.1  │     │
    │    └─────────────────────────────────────┘     │
    │                   │                             │
    │                   ↓                             │
    │    ┌─────────────────────────────────────┐     │
    │    │  3.4 早停检查                        │     │
    │    │  if val_acc > best_acc:             │     │
    │    │    保存最佳模型                       │     │
    │    │    patience_counter = 0             │     │
    │    │  else:                              │     │
    │    │    patience_counter += 1            │     │
    │    │                                     │     │
    │    │  if patience_counter >= 20:         │     │
    │    │    提前终止训练                       │     │
    │    └─────────────────────────────────────┘     │
    │                                                 │
    └─────────────────────────────────────────────────┘
                              │
                              ↓
    ┌─────────────────────────────────────────────────┐
    │  阶段4: 测试与评估                                │
    ├─────────────────────────────────────────────────┤
    │  1. 加载最佳模型权重                              │
    │     model.load_state_dict(best_weights)         │
    │                                                 │
    │  2. 测试集评估                                   │
    │     test_acc = 99.41%                           │
    │                                                 │
    │  3. 生成混淆矩阵                                  │
    │            预测                                  │
    │         0  1  2  3  4                           │
    │     0 [98  0  2  0  0]  fist                    │
    │  真 1 [ 0 99  0  0  1]  palm                    │
    │  实 2 [ 9  0 82  9  0]  thumbs_up               │
    │     3 [ 0  1  0 97  2]  ok                      │
    │     4 [ 8  3  0  0 89]  nothing                 │
    │                                                 │
    │  4. 生成分类报告                                  │
    │     Precision / Recall / F1-Score               │
    │     - hard test主要混淆：thumbs_up↔fist/ok      │
    │     - nothing偶尔被识别为fist/palm              │
    └─────────────────────────────────────────────────┘
                              │
                              ↓
    ┌─────────────────────────────────────────────────┐
    │  阶段5: 模型导出                                  │
    ├─────────────────────────────────────────────────┤
    │  1. 保存PyTorch模型                              │
    │     torch.save(model, 'gesture_v2_strongaug.pth') │
    │                                                 │
    │  2. 导出ONNX模型                                 │
    │     torch.onnx.export(...)                      │
    │     - 输入: dummy_input [1,1,64,64]             │
    │     - 输出: gesture_v2_strongaug.onnx           │
    │     - 量化: gesture_v2_strongaug_int8.onnx      │
    │     - opset_version: 11                         │
    │                                                 │
    │  3. 保存训练曲线图                                │
    │     training_curves.png                         │
    │     - 训练/验证损失曲线                           │
    │     - 训练/验证准确率曲线                         │
    │                                                 │
    │  4. 保存混淆矩阵图                                │
    │     confusion_matrix.png                        │
    └─────────────────────────────────────────────────┘
                              │
                              ↓
┌─────────────────────────────────────────────────────────────────┐
│  训练完成 ✓                                                      │
│  - 最佳验证准确率: 95.2% (epoch 68)                              │
│  - 测试集准确率: 96.3%                                           │
│  - 训练总时间: 约10分钟 (GPU) / 1小时 (CPU)                       │
└─────────────────────────────────────────────────────────────────┘
```

---

### ⚡ 实时手势识别流程

#### STM32端完整数据流

```
┌─────────────────────────────────────────────────────────────────┐
│  硬件输入                                                         │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ↓
    ┌─────────────────────────────────────────────────┐
    │  OpenMV Cam + OV5640                            │
    │  - 捕获帧: 30 FPS                                │
    │  - 分辨率: 64x64 灰度                            │
    │  - 输出: UART3 (1500000 bps)                    │
    └─────────────────────────────────────────────────┘
                              │
                              ↓
    ┌─────────────────────────────────────────────────┐
    │  CameraTask (FreeRTOS, 优先级3)                 │
    ├─────────────────────────────────────────────────┤
    │  1. UART3 DMA接收                                │
    │     - 接收缓冲区: 4096字节                        │
    │     - 超时: 100ms                                │
    │                                                 │
    │  2. 协议解析                                     │
    │     - 校验包头: "$IMG,4096"                      │
    │     - 提取图像数据                                │
    │     - 校验包尾: "\r\n"                           │
    │                                                 │
    │  3. 队列发送                                     │
    │     xQueueSend(imageQueue, img, timeout)        │
    │     - 队列深度: 5                                │
    │     - 阻塞超时: 50ms                             │
    └─────────────────────────────────────────────────┘
                              │
                              ↓
    ┌─────────────────────────────────────────────────┐
    │  InferenceTask (FreeRTOS, 优先级3)              │
    ├─────────────────────────────────────────────────┤
    │  4. 获取图像                                     │
    │     xQueueReceive(imageQueue, img, portMAX)     │
    │                                                 │
    │  5. 预处理                                       │
    │     uint8[4096] → float[4096]                   │
    │     归一化: (x/255 - 0.5) / 0.5 → [-1, 1]       │
    │                                                 │
    │  6. AI推理                                       │
    │     ┌───────────────────────────────┐           │
    │     │  X-CUBE-AI Engine            │           │
    │     │  - 输入: float[1,1,64,64]    │           │
    │     │  - GestureNet CNN            │           │
    │     │  - 输出: logits[1,4]         │           │
    │     │  - 推理时间: ~33ms            │           │
    │     └───────────────────────────────┘           │
    │                                                 │
    │  7. 后处理                                       │
    │     Softmax: logits → probabilities [0-1]       │
    │     - 找最大值max_val                            │
    │     - 计算exp(x - max_val)                      │
    │     - 归一化: softmax = exp / sum(exp)          │
    │     - argmax: 最大概率类别                       │
    │                                                 │
    │  8. 手势滤波                                     │
    │     滑动窗口 (size=5)                            │
    │     - 缓存最近5个手势                             │
    │     - 统计出现最多的手势                          │
    │     - 超过阈值(3次)才输出                         │
    │                                                 │
    │  输出: gesture_id, confidence                   │
    └─────────────────────────────────────────────────┘
                              │
                              ↓
    ┌─────────────────────────────────────────────────┐
    │  GestureControl_HandleGesture()                 │
    ├─────────────────────────────────────────────────┤
    │  9. 手势映射                                     │
    │     switch(gesture_id):                         │
    │       case GESTURE_FIST (0):                    │
    │         → 占空比 0%                              │
    │       case GESTURE_PALM (1):                    │
    │         → 占空比 30%                             │
    │       case GESTURE_V (2):                       │
    │         → 占空比 10%                             │
    │       case GESTURE_OK (3):                      │
    │         → 切换控制目标 (LED ↔ BUZZ)              │
    │                                                 │
    │  10. PWM控制                                     │
    │      TIM2软件PWM (1kHz, 100步)                  │
    │      - LED3  (PB14): 独立占空比                  │
    │      - BUZZ  (PG6):  独立占空比                  │
    └─────────────────────────────────────────────────┘
                              │
                              ↓
    ┌─────────────────────────────────────────────────┐
    │  物理输出                                        │
    ├─────────────────────────────────────────────────┤
    │  - LED3亮度变化                                  │
    │  - 蜂鸣器音量变化                                │
    └─────────────────────────────────────────────────┘
                              │
                              ↓
    ┌─────────────────────────────────────────────────┐
    │  DisplayTask (FreeRTOS, 优先级1)                │
    ├─────────────────────────────────────────────────┤
    │  11. OLED显示更新 (10Hz)                         │
    │      ┌───────────────────────────┐              │
    │      │ 手势: thumbs_up            │              │
    │      │ 置信度: 97.8%             │              │
    │      │ 目标: LED3                │              │
    │      │ 占空比: 10%               │              │
    │      │ FPS: 28.5                 │              │
    │      └───────────────────────────┘              │
    └─────────────────────────────────────────────────┘
                              │
                              ↓
    ┌─────────────────────────────────────────────────┐
    │  UartTxTask (FreeRTOS, 优先级2)                 │
    ├─────────────────────────────────────────────────┤
    │  12. 串口输出 (UART1, 921600bps)                 │
    │      $RESULT,2,0.978                            │
    │      $FPS,28.5                                  │
    │      $TARGET,LED                                │
    │      $DUTY,10                                   │
    └─────────────────────────────────────────────────┘
```

**性能指标**:
- 端到端延迟: 图像捕获 → PWM输出 < 50ms
- 推理速度: ~30 FPS
- 手势识别准确率: >95%
- CPU占用率: ~60% @ 480MHz

---

### 🚨 传感器报警处理流程

#### 多传感器融合报警机制

```
┌─────────────────────────────────────────────────────────────────┐
│  SensorTask (FreeRTOS, 优先级2, 周期1Hz)                         │
└─────────────────────────────────────────────────────────────────┘
                              │
          ┌───────────────────┼───────────────────┐
          │                   │                   │
          ↓                   ↓                   ↓
    ┌─────────┐         ┌─────────┐         ┌─────────┐
    │ DHT11   │         │ GY-30   │         │ HC-SR04 │
    │ 温湿度  │         │ 光照    │         │ 距离    │
    └─────────┘         └─────────┘         └─────────┘
          │                   │                   │
          ↓                   ↓                   ↓
    ┌─────────┐         ┌─────────┐         ┌─────────┐
    │ MQ-2    │         │MAX30102 │         │  ...    │
    │ 烟雾    │         │心率血氧 │         │         │
    └─────────┘         └─────────┘         └─────────┘
          │                   │                   │
          └───────────────────┴───────────────────┘
                              │
                              ↓
    ┌─────────────────────────────────────────────────┐
    │  GestureControl_CheckSensorAlarm()              │
    └─────────────────────────────────────────────────┘
                              │
          ┌───────────────────┼───────────────────┐
          │                   │                   │
          ↓                   ↓                   ↓
┌─────────────────┐ ┌─────────────────┐ ┌─────────────────┐
│  光照报警检测    │ │  距离报警检测    │ │  烟雾报警检测    │
│  阈值: >1000Lux │ │  阈值: <10cm    │ │  阈值: >10PPM   │
└─────────────────┘ └─────────────────┘ └─────────────────┘
          │                   │                   │
          │                   │  ┌──────────────┐ │
          │                   └─>│ 滞后阈值检查  │ │
          │                      │ 进入: <10cm  │ │
          │                      │ 退出: >15cm  │ │
          │                      └──────────────┘ │
          │                              │        │
          │                              ↓        │
          │                      ┌──────────────┐ │
          │                      │ 去抖动计数器  │ │
          │                      │ 连续3次确认  │ │
          │                      └──────────────┘ │
          │                              │        │
          └──────────────────┬───────────┴────────┘
                             │
                             ↓
    ┌─────────────────────────────────────────────────┐
    │  报警状态变化检测                                 │
    ├─────────────────────────────────────────────────┤
    │  if (alarm_now != last_alarm_state):            │
    │    触发报警动作                                  │
    └─────────────────────────────────────────────────┘
                             │
          ┌──────────────────┼──────────────────┐
          │                  │                  │
          ↓                  ↓                  ↓
    ┌──────────┐       ┌──────────┐       ┌──────────┐
    │ RGB LED  │       │ 蓝牙推送  │       │ OneNET   │
    │ 本地指示  │       │ 手机APP  │       │ 云平台   │
    └──────────┘       └──────────┘       └──────────┘
          │                  │                  │
          ↓                  ↓                  ↓
    ┌──────────┐       ┌──────────┐       ┌──────────┐
    │ 红灯亮起  │       │ "ALARM:  │       │ 数据流   │
    │ (光照)   │       │  Light   │       │ 'alarm'  │
    │          │       │  too     │       │ 上传     │
    │ 绿灯亮起  │       │  high!"  │       │          │
    │ (距离)   │       │          │       │          │
    │          │       │ "ALARM:  │       │          │
    │ 蓝灯亮起  │       │  Dist    │       │          │
    │ (烟雾)   │       │  close!" │       │          │
    └──────────┘       └──────────┘       └──────────┘
```

**报警去抖动示例 (超声波距离)**:

```
实际距离波动:
时间(s): 0    1    2    3    4    5    6    7    8    9
距离(cm): 8    9   11    9    8    9    8   18   17   19
                ↑        ↑
            误触发1   误触发2

去抖动后:
状态:   IDLE IDLE IDLE IDLE ALARM ALARM ALARM CLEAR CLEAR CLEAR
                          ↑                    ↑
                    连续3次<10cm         连续3次>15cm
                    真正进入报警          真正解除报警

计数器变化:
进入计数: 0  1  0  1  2  3  0  0  0  0
          └─┘ └──────┘
          清零    达到阈值
退出计数: 0  0  0  0  0  0  0  1  2  3
                                 └────┘
                                 达到阈值
```

**滞后阈值作用**:
- 避免在临界点反复触发/解除
- 进入阈值: 10cm (更敏感)
- 退出阈值: 15cm (更保守)
- 形成5cm的滞后区间

---

### 数据集采集与模型训练

#### 方式1: 使用上位机采集（推荐）

1. **连接硬件**
```bash
PC ← USB转TTL → STM32 UART1 (PA9/PA10)
波特率: 921600
```

2. **运行上位机采集工具**
```bash
cd Python
python collect_training_data.py --port COM3 --dataset-dir dataset_real
# 按提示输入手势ID (0-4)，每个手势采集200-500张
# STM32会自动从OpenMV获取图像并通过UART1发送到PC
```

3. **采集要点**:
- 左手拿OpenMV摄像头，右手做手势
- 多角度：正面、侧面、斜视、旋转
- 多距离：远(50cm)、中(30cm)、近(15cm)
- 多位置：左、中、右、上、下移动
- 多光照：明亮、昏暗、逆光

#### 方式2: 直接在OpenMV上采集

```python
# OpenMV脚本：保存图像到SD卡
# 然后复制到PC进行训练
```

#### 模型训练与部署

1. **数据增强**
```bash
python augment_dataset.py --input dataset_real --output dataset_aug --factor 5
# 将数据扩充5倍（旋转、平移、亮度变化等）
```

2. **训练模型**
```bash
python train_model_v2.py --dataset dataset_real --epochs 150 --batch-size 32 --loss focal --strong-aug --model-name gesture_v2_strongaug.pth
# 训练完成后生成: gesture_v2_strongaug.pth、training_curves_v2.png、confusion_matrix_v2.png
```

3. **导出和量化模型**
```bash
python convert_to_onnx_v2.py --input gesture_v2_strongaug.pth --output gesture_v2_strongaug.onnx --model-type v2
python quantize_onnx_int8.py --model gesture_v2_strongaug.onnx --output gesture_v2_strongaug_int8.onnx --dataset dataset_real --calib-images 300
```

4. **测试模型**
```bash
# PC端测试PyTorch模型
python test_pytorch_model_v2.py

# PC端测试ONNX模型
python test_onnx_model.py --model gesture_v2_strongaug.onnx

# 数据集评估INT8模型
python eval_onnx_dataset.py --model gesture_v2_strongaug_int8.onnx --dataset dataset_hard_test

# 实时推理对比（需连接STM32 UART1）
python realtime_inference_compare.py --port COM3 --model gesture_v2_strongaug.pth
```

5. **部署到STM32**
- 打开STM32CubeMX项目
- 使用X-CUBE-AI插件导入`gesture_v2_strongaug_int8.onnx`
- 分析模型 → 验证 → 生成代码，当前生成模型名为 `network_1`
- 确认生成文件包含 `Inc/network_1.h`、`Src/network_1.c`、`Src/network_1_data.c` 等
- 重新编译烧录固件

### 蓝牙控制

通过蓝牙发送以下命令：

| 命令 | 功能 | 说明 |
|------|------|------|
| `A` | 切换控制目标 | LED ↔ 蜂鸣器 |
| `B` | 占空比0% | 关闭 |
| `C` | 占空比10% | 低速 |
| `D` | 占空比30% | 中速 |

### 传感器报警

- **光照过强** (>1000 Lux): 红色LED亮起
- **距离过近** (<10cm): 绿色LED亮起
- **烟雾检测** (>10 PPM): 蓝色LED亮起

报警采用**滞后阈值**和**去抖动计数器**防止误报。

### OneNET云平台

1. 配置WiFi参数 (`Net/onenet/onenet_config.h`)
```c
#define ONENET_DEVICE_ID    "your_device_id"
#define ONENET_API_KEY      "your_api_key"
#define WIFI_SSID           "your_wifi_ssid"
#define WIFI_PASSWORD       "your_wifi_password"
```

2. 数据自动上传周期：5秒

3. 在OneNET平台查看数据流图表

---

## 🐛 常见问题

### Q1: STM32端推理结果和PC端ONNX结果不一致
**A**: 当前部署的是INT8模型，优先检查量化链路是否一致：
1. 确认STM32导入的是 `gesture_v2_strongaug_int8.onnx`，生成模型名为 `network_1`
2. 确认 `AI_INPUT_QUANT_SCALE = 0.007874016f`
3. 确认 `AI_OUTPUT_QUANT_SCALE = 0.147128820f`
4. 确认输入图像为64x64灰度图，归一化公式为 `(x/255 - 0.5) / 0.5`
5. 运行 `Python/verify_stm32_equivalence.py` 核对PC端和STM32端量化/反量化等价性

### Q2: 摄像头无图像数据
**A**: 检查：
1. OpenMV Cam是否正常运行（烧录手势识别脚本）
2. OV5640摄像头是否正确连接到OpenMV
3. UART3连接是否正确 (PA11/PA12)
4. 波特率设置 (921600)
5. OpenMV脚本是否正确发送图像数据格式

### Q3: 传感器数据读取失败
**A**: 参考 `SENSOR_TASK_TROUBLESHOOTING.md`：
1. 检查I2C上拉电阻（BH1750, MAX30102）
2. DHT11信号线需要上拉
3. MQ2预热时间 >24小时

### Q4: FreeRTOS任务卡死
**A**:
1. 增大堆栈大小 (`FreeRTOSConfig.h`)
2. 检查中断优先级配置
3. 使用Keil调试器查看任务状态

### Q5: ESP8266无法连接WiFi
**A**: 参考 `ESP8266_INIT_FIX.md`：
1. AT固件版本 ≥ 2.0
2. 复位后等待2秒再发送AT命令
3. 检查波特率（默认115200）

### Q6: 训练精度低
**A**:
1. 增加数据集大小（建议500+/手势）
2. 使用数据增强
3. 确保采集环境多样性（光照、角度、距离）
4. 参考 `Python/MODEL_IMPROVEMENT_GUIDE.md`

### Q7: UART通信异常
**A**: 检查端口配置：
- UART1 (PA9/PA10): 上位机，921600波特率
- UART3 (PA11/PA12): OpenMV，921600波特率
- UART4 (PA0/PA1): 蓝牙模块
- UART5 (PC12/PD2): ESP8266 WiFi
- 确保TX/RX交叉连接（STM32的TX → 模块的RX）

---

## 📚 开发文档

### 核心文档
- [**QUICK_START.md**](QUICK_START.md) - 快速开始指南
- [**COMPLETE_USAGE_GUIDE.md**](Python/COMPLETE_USAGE_GUIDE.md) - 完整使用指南
- [**GESTURE_VISUAL_GUIDE.md**](Python/GESTURE_VISUAL_GUIDE.md) - 手势定义图解

### 专题文档
- [**FREERTOS_CUBEMX_CONFIG_GUIDE.md**](FREERTOS_CUBEMX_CONFIG_GUIDE.md) - FreeRTOS配置
- [**ESP8266_INIT_FIX.md**](ESP8266_INIT_FIX.md) - WiFi模块初始化
- [**SENSOR_TASK_TROUBLESHOOTING.md**](SENSOR_TASK_TROUBLESHOOTING.md) - 传感器故障排除

### AI模型文档
- [**MODEL_IMPROVEMENT_GUIDE.md**](Python/MODEL_IMPROVEMENT_GUIDE.md) - 模型优化指南
- [**GESTURE_NET_V2_GUIDE.md**](Python/GESTURE_NET_V2_GUIDE.md) - GestureNet v2架构
- [**ONNX_COMPARISON_GUIDE.md**](Python/ONNX_COMPARISON_GUIDE.md) - ONNX模型对比

---

## 🎨 系统架构图

### 通信架构总览

```
                     STM32H753ZI 核心控制器
                            |
        +-------------------+-------------------+
        |                   |                   |
    UART1 (上位机)      UART3 (OpenMV)     UART4 (蓝牙)     UART5 (WiFi)
        |                   |                   |                |
    PA9/PA10           PA11/PA12           PA0/PA1          PC12/PD2
        |                   |                   |                |
        ↓                   ↓                   ↓                ↓
   ┌─────────┐        ┌──────────┐       ┌──────────┐     ┌──────────┐
   │  PC端   │        │ OpenMV   │       │  蓝牙    │     │ ESP8266  │
   │ (调试/  │        │  Cam     │       │  模块    │     │  WiFi    │
   │ 数据集) │        │ (OV5640) │       │ (SPP)    │     │ (MQTT)   │
   └─────────┘        └──────────┘       └──────────┘     └──────────┘
        ↑                   ↑                   ↑                ↑
        |                   |                   |                |
   图像数据流          64x64图像          控制命令           云平台
   921600 bps         921600 bps         低延迟           5秒/次
        |                   |                   |                |
        └───────────────────┴───────────────────┴────────────────┘
                               |
                      FreeRTOS任务调度
                               |
        ┌──────────────────────┼──────────────────────┐
        |                      |                      |
    CameraTask          InferenceTask          SensorTask
    (图像接收)          (AI推理)               (环境监测)
        |                      |                      |
        └──────────────────────┼──────────────────────┘
                               |
                        GestureControl
                     (手势→设备控制)
                               |
        ┌──────────────────────┼──────────────────────┐
        |                      |                      |
    LED3/蜂鸣器            RGB报警灯              OLED显示
    (PWM控制)              (PD10/PG7/PG4)         (SPI)
```

### 整体架构
```
┌──────────────────────────────────────────────────────────┐
│                    用户交互层                              │
│  ┌────────┐  ┌─────────┐  ┌────────┐  ┌──────────┐     │
│  │ 手势   │  │ 蓝牙    │  │ OLED   │  │ OneNET   │     │
│  │ 控制   │  │ 命令    │  │ 显示   │  │ 云平台   │     │
│  └────────┘  └─────────┘  └────────┘  └──────────┘     │
└──────────────────────────────────────────────────────────┘
                          ↕
┌──────────────────────────────────────────────────────────┐
│                  应用任务层 (FreeRTOS)                     │
│  ┌──────────┐ ┌───────────┐ ┌──────────┐ ┌──────────┐  │
│  │ Camera   │→│ Inference │→│ Control  │→│ Display  │  │
│  │ Task     │ │ Task      │ │ Task     │ │ Task     │  │
│  └──────────┘ └───────────┘ └──────────┘ └──────────┘  │
│  ┌──────────┐ ┌───────────┐ ┌──────────┐ ┌──────────┐  │
│  │ Sensor   │→│ UartTx    │ │ UartRx   │ │ OneNET   │  │
│  │ Task     │ │ Task      │ │ Task     │ │ Task     │  │
│  └──────────┘ └───────────┘ └──────────┘ └──────────┘  │
└──────────────────────────────────────────────────────────┘
                          ↕
┌──────────────────────────────────────────────────────────┐
│                   驱动层 (Hardware)                       │
│  ┌──────┐ ┌────────┐ ┌──────┐ ┌──────┐                  │
│  │OpenMV│ │SENSORS │ │ OLED │ │ WiFi │                  │
│  └──────┘ └────────┘ └──────┘ └──────┘                  │
└──────────────────────────────────────────────────────────┘
                          ↕
┌──────────────────────────────────────────────────────────┐
│              HAL层 (STM32 Cube HAL)                       │
│    UART | I2C | SPI | GPIO | ADC | TIM | DMA             │
└──────────────────────────────────────────────────────────┘
```

### 数据流转
```
外设输入 → 驱动层 → 任务层 → 应用逻辑 → 输出动作
  ↓          ↓        ↓        ↓           ↓
OpenMV → UART3  → Camera → AI推理 → 手势控制 → LED/蜂鸣器
传感器 → DHT11  → Sensor → 数据处理 → 报警检测 → RGB灯 + 蓝牙推送(UART4)
上位机 → UART1  → UartRx → 命令解析 → 图像发送 → 数据集采集
蓝牙   → UART4  → UartRx → 命令解析 → 设备控制 → 状态更新
WiFi   → UART5  → ESP8266→ MQTT     → 云平台   → 数据存储
```

---

## 🔧 开发工具链

### STM32开发
- **Keil MDK-ARM** v5.38+ (编译、调试)
- **STM32CubeMX** v6.0+ (外设配置)
- **STM32CubeProgrammer** (固件烧录)
- **X-CUBE-AI** v7.1.0+ (AI模型转换)

### Python开发
- **PyTorch** v1.10+ (模型训练)
- **ONNX** v1.10+ (模型转换)
- **OpenCV** v4.5+ (图像处理)

### 调试工具
- **ST-Link** / **J-Link** (硬件调试器)
- **串口助手** (数据监控)
- **Logic Analyzer** (时序分析)

---

## 📊 性能指标

| 指标 | 数值 | 备注 |
|------|------|------|
| AI推理速度 | 受串口图像传输/OLED刷新影响 | 64x64灰度图，本地INT8推理 |
| 模型计算量 | 10,995,965 MACC | X-CUBE-AI生成报告 |
| PyTorch参数量 | 73,797 | `strongaug_train.log` |
| X-CUBE-AI参数量 | 73,229 items | `network_1_generate_report.txt` |
| 模型权重 | 76,460 B | `network_1` INT8权重 |
| 激活区 | 150,292 B | 输入/输出buffer分配在activations中 |
| ONNX模型 | `gesture_v2_strongaug_int8.onnx` | 当前部署模型 |
| 训练测试集准确率 | 99.41% | `strongaug_train.log` |
| 难例测试主要问题 | thumbs_up↔fist/ok，nothing↔fist/palm | `dataset_hard_test`评估 |
| 传感器采样率 | 1 Hz | Sensor Task周期 |
| OLED刷新率 | 10 Hz | Display Task周期 |
| OneNET上传周期 | 5s | 可配置 |

---

## 🤝 贡献指南

欢迎提交Issue和Pull Request！

### 开发分支
- `main` - 稳定版本
- `dev` - 开发版本
- `feature/*` - 新功能分支

### 代码规范
- C代码：遵循Linux内核风格
- Python代码：遵循PEP 8
- 注释：中英文皆可，关键逻辑必须注释

---

## 📜 许可证

本项目基于 **MIT License** 开源。

部分组件遵循各自许可证：
- STM32 HAL库：STMicroelectronics License
- FreeRTOS：MIT License
- X-CUBE-AI：STMicroelectronics License

---

## 📧 联系方式

- **项目维护者**: [您的名字]
- **Email**: your.email@example.com
- **GitHub**: https://github.com/yourusername/project7

---

## 🙏 致谢

- STMicroelectronics - STM32开发板、HAL库及X-CUBE-AI工具链
- FreeRTOS社区
- PyTorch社区
- OpenMV开源项目

---

## 📝 更新日志

### v1.2.0 (2026-01-12)
- ✅ 修复旧版模型输出后处理问题（Softmax）
- ✅ 添加超声波传感器去抖动
- ✅ 优化数据采集工具（从最大编号继续）
- ✅ 完善README文档

### v1.1.0 (2026-01-08)
- ✅ 集成OpenMV图像传输
- ✅ 添加OneNET云平台支持
- ✅ 优化FreeRTOS任务调度

### v1.0.0 (2026-01-03)
- 🎉 首个正式版本发布
- ✅ 手势识别基本功能
- ✅ 多传感器监测
- ✅ OLED显示

---

**🌟 如果这个项目对你有帮助，请给个Star！**
