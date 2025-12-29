# STM32H753 OV2640手势识别 - 快速开始指南

## 项目完成情况总结

### ✓ 已完成的工作

1. **STM32驱动代码** (已移植到HAL库)
   - `ov2640.h/c` - OV2640摄像头驱动
   - `sccb.h/c` - SCCB通信协议
   - `ov2640cfg.h/c` - 摄像头寄存器配置表
   - `camera_app.h/c` - 图像采集和灰度转换
   - `uart_protocol.h/c` - 串口通信协议
   - `main.c` - 主程序示例

2. **Python工具**
   - `data_collector.py` - 数据采集工具
   - `train_model.py` - CNN模型训练脚本
   - `requirements.txt` - Python依赖

3. **文档**
   - `README.md` - 完整项目文档
   - `QUICKSTART.md` - 本快速指南

## 使用步骤

### 第1步: 硬件连接

参考`README.md`中的引脚配置表，连接OV2640摄像头到STM32H753。

**重要引脚检查清单:**
- [ ] SCCB: PD7(SCL), PD6(SDA)
- [ ] 数据线: D0-D7 (8个引脚)
- [ ] 同步信号: VSYNC, HREF, PCLK
- [ ] 控制信号: RST, PWDN
- [ ] UART: TX, RX
- [ ] 电源: 3.3V, GND

### 第2步: STM32CubeMX配置

1. 打开STM32CubeMX，选择STM32H753芯片
2. 配置系统时钟到480MHz
3. 按照`README.md`中的GPIO配置表设置所有引脚
4. 配置USART1: 921600波特率，8N1
5. 可选: 配置I2C用于OLED
6. 生成代码

### 第3步: 添加驱动代码

将以下文件复制到STM32项目中:

```
STM32H753_Project/
├── Core/
│   └── Src/
│       └── main.c (替换生成的main.c)
└── Drivers/
    └── OV2640/
        ├── ov2640.h
        ├── ov2640.c
        ├── ov2640cfg.h
        ├── ov2640cfg.c
        ├── sccb.h
        ├── sccb.c
        ├── camera_app.h
        ├── camera_app.c
        ├── uart_protocol.h
        └── uart_protocol.c
```

在Keil/IAR中添加这些文件到项目。

### 第4步: 编译和烧录

1. 编译项目
2. 检查内存使用情况（Flash < 1MB, RAM < 512KB）
3. 烧录到STM32H753
4. 打开串口调试助手，波特率921600

### 第5步: 测试摄像头

#### 方法1: 使用Python预览工具

```bash
cd Python
pip install -r requirements.txt
python data_collector.py --port COM3 --mode preview
```

按'q'退出，按's'保存当前帧。

#### 方法2: 串口调试

在串口助手中发送: `$STATUS\r\n`

应该收到类似: `$STATUS,IDLE,0,0,0\r\n`

### 第6步: 采集训练数据

#### 手势类别定义

```
0: 拳头 (fist)
1: 张开手掌 (palm)
2: 竖起大拇指 (thumbs_up)
3: OK手势 (ok)
4: V字手势 (v_sign)
5: 指向手势 (point)
6: 数字3 (three)
7: 数字4 (four)
8: 向左挥手 (swipe_left)
9: 向右挥手 (swipe_right)
```

#### 采集命令

对每个手势采集500-1000张图像:

```bash
# 采集手势0 (拳头), 100张
python data_collector.py --port COM3 --mode collect --gesture 0 --num 100

# 采集手势1 (张开手掌), 100张
python data_collector.py --port COM3 --mode collect --gesture 1 --num 100

# ... 依次采集所有手势
```

**采集技巧:**
- 不同距离: 10cm, 20cm, 30cm, 40cm, 50cm
- 不同角度: 正面, 侧面, 旋转
- 不同光照: 明亮, 正常, 昏暗
- 不同人: 多人参与采集
- 不同背景: 简单背景, 复杂背景

### 第7步: 训练CNN模型

```bash
cd Python

# 训练模型
python train_model.py

# 训练完成后会生成:
# - best_model.pth (PyTorch模型)
# - gesture_model.onnx (ONNX模型，用于X-CUBE-AI)
# - training_curves.png (训练曲线)
# - confusion_matrix.png (混淆矩阵)
```

**期望结果:**
- 训练准确率 > 95%
- 验证准确率 > 90%
- 测试准确率 > 85%

### 第8步: 使用X-CUBE-AI部署模型

1. **在STM32CubeMX中安装X-CUBE-AI**
   - Middleware → X-CUBE-AI

2. **导入ONNX模型**
   - Add network → 选择 `gesture_model.onnx`

3. **分析模型**
   - 点击 "Analyze"
   - 查看 Flash 和 RAM 使用情况
   - 应该 < 150KB Flash, < 200KB RAM

4. **验证模型**
   - 点击 "Validation on Desktop"
   - 确保精度损失 < 2%

5. **生成代码**
   - 点击 "Generate Code"
   - 将生成的代码集成到项目中

6. **集成AI推理**

在`main.c`中添加AI推理代码:

```c
#include "ai_datatypes_defines.h"
#include "ai_platform.h"
#include "gesture_model.h"
#include "gesture_model_data.h"

ai_handle network = AI_HANDLE_NULL;
ai_network_params params = {
    AI_GESTURE_MODEL_DATA_WEIGHTS(ai_gesture_model_data_weights_get()),
    AI_GESTURE_MODEL_DATA_ACTIVATIONS(activations)
};

// 初始化AI
ai_gesture_model_create(&network, AI_GESTURE_MODEL_DATA_CONFIG);
ai_gesture_model_init(network, &params);

// 推理
ai_i8 input_data[AI_GESTURE_MODEL_IN_1_SIZE];
ai_i8 output_data[AI_GESTURE_MODEL_OUT_1_SIZE];

ai_input[0].data = AI_HANDLE_PTR(input_data);
ai_output[0].data = AI_HANDLE_PTR(output_data);

ai_gesture_model_run(network, ai_input, ai_output);

// 获取结果
float* predictions = (float*)output_data;
int predicted_class = argmax(predictions, 10);
float confidence = predictions[predicted_class];
```

### 第9步: 测试实时推理

1. 烧录带有AI模型的固件
2. 发送推理命令: `$INFER\r\n`
3. 摄像头会实时识别手势并返回结果:
   ```
   $RESULT,3,0.95\r\n  (识别为手势3, 置信度95%)
   ```

## 常见问题解决

### Q1: 摄像头无法初始化

**症状:** `✗ 摄像头初始化失败!`

**检查:**
1. SCCB引脚连接是否正确
2. 3.3V电源是否稳定
3. 使用逻辑分析仪查看SCL/SDA波形
4. 尝试降低SCCB时钟频率(增加delay_us)

### Q2: 图像全黑

**检查:**
1. 镜头是否遮挡
2. PCLK信号是否正常
3. 调整曝光设置: `OV2640_Auto_Exposure(3)`
4. 调整亮度: `OV2640_Brightness(3)`

### Q3: 串口数据丢失

**解决:**
1. 降低波特率到115200测试
2. 启用UART的DMA传输
3. 增大串口缓冲区
4. 减小图像分辨率(如64x64)

### Q4: FPS过低

**优化:**
1. 使用DMA传输图像数据
2. 优化数据读取循环(减少延时)
3. 提高系统时钟频率
4. 减小图像分辨率
5. 使用DCMI硬件接口(如果可用)

### Q5: 模型推理时间过长

**优化:**
1. 使用INT8量化
2. 简化模型结构
3. 减小输入分辨率到64x64
4. 启用编译器优化(-O3)
5. 使用硬件加速器(如果可用)

## 性能指标

### 目标性能

| 指标 | 目标值 | 备注 |
|------|--------|------|
| 图像采集FPS | 15-20 | 96x96分辨率 |
| 推理时间 | < 50ms | 每帧 |
| 识别准确率 | > 90% | 实时推理 |
| 内存占用 | < 512KB | RAM |
| Flash占用 | < 1MB | 包含模型 |

### 实测性能(预期)

- 采集FPS: ~18 FPS
- RGB565→灰度转换: ~3ms
- 串口传输(96x96): ~40ms @921600bps
- AI推理: ~30-40ms (量化后)

## 下一步优化方向

1. **使用DCMI硬件接口**
   - 大幅提升采集速度
   - 减少CPU占用
   - FPS可达30+

2. **外部SDRAM**
   - 扩展内存
   - 支持更大的图像和模型

3. **使用FreeRTOS**
   - 任务隔离
   - 更好的资源管理

4. **添加后处理**
   - 连续识别平滑
   - 手势轨迹跟踪
   - 多手势组合识别

5. **优化模型**
   - 知识蒸馏
   - 网络剪枝
   - NAS自动搜索

## 参考资料

- [完整文档](README.md)
- [OV2640数据手册](https://www.ovt.com)
- [STM32H7参考手册](https://www.st.com)
- [X-CUBE-AI用户手册](https://www.st.com/x-cube-ai)
- [PyTorch文档](https://pytorch.org)

## 技术支持

如有问题，请检查:
1. 硬件连接是否正确
2. 引脚配置是否匹配
3. 串口波特率是否一致
4. 数据集是否充足

祝你成功！🎉
