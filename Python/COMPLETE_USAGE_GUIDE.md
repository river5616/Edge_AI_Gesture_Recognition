# 手势识别系统 - 完整使用指南 (从零到部署)

## 📋 目录

1. [环境准备](#1-环境准备)
2. [数据采集](#2-数据采集)
3. [数据增强](#3-数据增强)
4. [模型训练](#4-模型训练)
5. [模型测试](#5-模型测试)
6. [模型转换](#6-模型转换)
7. [STM32部署](#7-stm32部署)
8. [完整示例](#8-完整示例)
9. [故障排除](#9-故障排除)

---

## 1. 环境准备

### 1.1 安装Python依赖

```bash
# 基础依赖
pip install torch torchvision  # PyTorch (如果有GPU，安装GPU版本)
pip install opencv-python       # 图像处理
pip install numpy              # 数值计算
pip install matplotlib         # 绘图
pip install seaborn            # 可视化
pip install scikit-learn       # 机器学习工具
pip install pillow             # 图像库
pip install pyserial           # 串口通信

# ONNX相关
pip install onnx               # ONNX模型
pip install onnxruntime        # ONNX推理引擎
```

### 1.2 检查GPU可用性

```bash
python -c "import torch; print(f'CUDA available: {torch.cuda.is_available()}'); print(f'Device: {torch.cuda.get_device_name(0) if torch.cuda.is_available() else \"CPU\"}')"
```

**预期输出（如果有GPU）**：
```
CUDA available: True
Device: NVIDIA GeForce RTX 3060
```

**预期输出（如果无GPU）**：
```
CUDA available: False
Device: CPU
```

### 1.3 验证文件结构

```bash
cd /home/river/project/project1/project6/Python
ls -la
```

**应该看到的文件**：
```
gesture_definitions.py          # 手势定义
data_collector.py              # 数据采集工具
collect_training_data.py       # 训练数据采集工具
augment_dataset.py             # 数据增强工具
train_model_v2.py              # 改进版训练脚本
test_pytorch_model_v2.py       # 测试脚本
convert_to_onnx_v2.py          # ONNX转换脚本
debug_inference.py             # 调试工具
```

---

## 2. 数据采集

### 2.1 了解手势类别

```bash
# 查看手势定义
python gesture_definitions.py
```

**输出**：
```
================================================================================
手势识别系统 - 手势定义（精简版 - 7个手势）
================================================================================

0. 拳头 (fist)
   描述: 五指完全握紧，拇指压在食指和中指上
   要点:
     - 握紧到看不见手指
     - 拳头可以有不同朝向
     - 不要半握

1. 手掌 (palm)
   描述: 五指完全展开并分开，手掌朝摄像头
   要点:
     - 五指尽量分开
     - 拇指要明显看到（最重要）
     - 不要四指并拢

2. 大拇指向上 (thumbs_up)
   ...

[总共7个手势]
```

### 2.2 启动STM32推理模式

在STM32上：
1. 确保已烧录固件
2. 将模式切换到 **INFERENCE** 模式
3. 连接串口（如 COM17）

### 2.3 采集数据

#### 方法A: 使用 collect_training_data.py（推荐）

```bash
# 启动采集工具
python collect_training_data.py --port COM17 --baudrate 921600
```

**交互式采集流程**：

```
✓ 已连接到 COM17
✓ 数据集目录已创建: dataset_real

请选择要采集的手势:
0: fist
1: palm
2: thumbs_up
3: ok
4: gun
5: call_me
6: nothing

请输入手势ID (0-6): 0                    # 选择手势 0 (fist)
请输入要采集的图像数量: 500               # 采集 500 张

开始采集手势: fist (ID: 0)
目标数量: 500 张

[采集中...]
已采集: 1/500 | 亮度: 125.3 | 存储: dataset_real/0_fist/img_0001.png
已采集: 2/500 | 亮度: 123.7 | 存储: dataset_real/0_fist/img_0002.png
已采集: 3/500 | 亮度: 128.1 | 存储: dataset_real/0_fist/img_0003.png
...
已采集: 500/500 | 亮度: 126.5 | 存储: dataset_real/0_fist/img_0500.png

✓ 采集完成！
  总数: 500 张
  平均亮度: 125.8
  保存目录: dataset_real/0_fist

继续采集下一个手势? (y/n):
```

#### 采集技巧

**手势 0-3 (fist, palm, thumbs_up, ok)**:
```bash
# 这些是已有手势，采集 300-500 张即可
python collect_training_data.py --port COM17

# 选择手势: 0
# 数量: 400
# 采集时注意：
# - 多角度（正面、左斜、右斜）
# - 多距离（近、中、远）
# - 多位置（左、中、右）
```

**手势 4-5 (gun, call_me)** - 新手势，重点采集：
```bash
# 新手势需要更多数据
python collect_training_data.py --port COM17

# 选择手势: 4 (gun)
# 数量: 800
# 采集时注意：
# - ⭐ 拇指向上 + 食指向前（L形）
# - ⭐ 多采集侧面角度（防止与 thumbs_up 混淆）
# - 各种手指伸直程度
# - 不同距离和位置
```

**手势 6 (nothing)** - 背景类：
```bash
# 采集空背景
python collect_training_data.py --port COM17

# 选择手势: 6 (nothing)
# 数量: 500
# 采集时注意：
# - 完全空的背景
# - 手完全移出画面
# - 不同光照条件下的背景
```

### 2.4 检查采集结果

```bash
# 查看数据集统计
ls -R dataset_real/

# 预期输出：
# dataset_real/0_fist:
# img_0001.png  img_0002.png  ...  img_0400.png
#
# dataset_real/1_palm:
# img_0001.png  img_0002.png  ...  img_0400.png
#
# ... (其他手势)
```

```bash
# 统计每个手势的数量
for dir in dataset_real/*/; do
    count=$(ls "$dir"*.png 2>/dev/null | wc -l)
    echo "$(basename "$dir"): $count 张"
done
```

**预期输出**：
```
0_fist: 400 张
1_palm: 400 张
2_thumbs_up: 400 张
3_ok: 400 张
4_gun: 800 张
5_call_me: 800 张
6_nothing: 500 张
总计: 3700 张
```

### 2.5 数据质量检查

```bash
# 随机查看几张图像
python -c "
import cv2
import random
import os
import glob

# 随机选择一个手势目录
gesture_dirs = glob.glob('dataset_real/*')
gesture_dir = random.choice(gesture_dirs)

# 随机选择一张图像
images = glob.glob(f'{gesture_dir}/*.png')
img_path = random.choice(images)

# 读取并显示
img = cv2.imread(img_path, cv2.IMREAD_GRAYSCALE)
print(f'图像: {img_path}')
print(f'形状: {img.shape}')
print(f'数据类型: {img.dtype}')
print(f'亮度范围: [{img.min()}, {img.max()}]')
print(f'平均亮度: {img.mean():.2f}')

# 保存到临时文件供查看
cv2.imwrite('sample_check.png', img)
print('已保存样例图像: sample_check.png')
"
```

**检查清单**：
- [ ] 图像尺寸为 64x64
- [ ] 灰度图（单通道）
- [ ] 平均亮度在 100-140 之间
- [ ] 手势清晰可见
- [ ] 没有明显噪声

---

## 3. 数据增强

### 3.1 为什么需要数据增强？

- **扩充数据量**: 3700张 → 37000张 (10倍)
- **提高泛化能力**: 模型见过更多变化
- **模拟真实场景**: 旋转、平移、亮度变化等

### 3.2 执行数据增强

```bash
# 基础增强 (10倍扩充)
python augment_dataset.py \
    --input dataset_real \
    --output dataset_augmented \
    --factor 10
```

**预期输出**：
```
数据增强工具
输入目录: dataset_real
输出目录: dataset_augmented
增强倍数: 10

处理手势: 0_fist
  找到 400 张原始图像
  生成 4000 张增强图像
  进度: [████████████████████] 100% (4000/4000)
  ✓ 完成

处理手势: 1_palm
  找到 400 张原始图像
  生成 4000 张增强图像
  进度: [████████████████████] 100% (4000/4000)
  ✓ 完成

处理手势: 2_thumbs_up
  找到 400 张原始图像
  生成 4000 张增强图像
  进度: [████████████████████] 100% (4000/4000)
  ✓ 完成

处理手势: 3_ok
  找到 400 张原始图像
  生成 4000 张增强图像
  进度: [████████████████████] 100% (4000/4000)
  ✓ 完成

处理手势: 4_gun
  找到 800 张原始图像
  生成 8000 张增强图像
  进度: [████████████████████] 100% (8000/8000)
  ✓ 完成

处理手势: 5_call_me
  找到 800 张原始图像
  生成 8000 张增强图像
  进度: [████████████████████] 100% (8000/8000)
  ✓ 完成

处理手势: 6_nothing
  找到 500 张原始图像
  生成 5000 张增强图像
  进度: [████████████████████] 100% (5000/5000)
  ✓ 完成

总计: 37000 张增强图像
耗时: 125.3 秒
✓ 数据增强完成！
```

### 3.3 验证增强结果

```bash
# 统计增强后的数据量
for dir in dataset_augmented/*/; do
    count=$(ls "$dir"*.png 2>/dev/null | wc -l)
    echo "$(basename "$dir"): $count 张"
done
```

**预期输出**：
```
0_fist: 4000 张
1_palm: 4000 张
2_thumbs_up: 4000 张
3_ok: 4000 张
4_gun: 8000 张
5_call_me: 8000 张
6_nothing: 5000 张
总计: 37000 张 ✓
```

---

## 4. 模型训练

### 4.1 选择训练版本

**GestureNet (原版)**:
- 参数量: ~15KB
- 推理速度: 80-100ms
- 预期准确率: 85-90%
- 适合: 对速度要求高的场景

**GestureNetV2 (改进版)** ⭐ 推荐:
- 参数量: ~30KB
- 推理速度: 120-150ms
- 预期准确率: 92-95%
- 适合: 对准确率要求高的场景

### 4.2 训练 GestureNetV2（GPU加速）

#### 第一次训练 - 使用 Focal Loss

```bash
# GPU训练（推荐）
python train_model_v2.py \
    --dataset dataset_augmented \
    --batch-size 64 \
    --epochs 150 \
    --lr 0.001 \
    --loss focal \
    --model-name gesture_v2_focal.pth
```

**如果没有GPU，使用CPU**：
```bash
# CPU训练（较慢）
python train_model_v2.py \
    --dataset dataset_augmented \
    --batch-size 32 \
    --epochs 150 \
    --lr 0.001 \
    --loss focal \
    --model-name gesture_v2_focal.pth
```

**训练过程输出**：

```
使用设备: cuda
加载数据集...
加载了 37000 张图像

各类别样本数量:
  0 (fist): 4000 张
  1 (palm): 4000 张
  2 (thumbs_up): 4000 张
  3 (ok): 4000 张
  4 (gun): 8000 张
  5 (call_me): 8000 张
  6 (nothing): 5000 张

训练集: 25900 张
验证集: 5550 张
测试集: 5550 张

创建模型...
模型参数量: 88,343
可训练参数: 88,343
模型大小估计: 34.51 KB (float32)
使用 Focal Loss

开始训练...
Epoch [1/150] Train Loss: 1.2345, Train Acc: 45.67% | Val Loss: 1.1234, Val Acc: 52.34% | LR: 0.001000
Epoch [2/150] Train Loss: 0.9876, Train Acc: 62.45% | Val Loss: 0.9123, Val Acc: 65.12% | LR: 0.000999
Epoch [3/150] Train Loss: 0.8234, Train Acc: 71.23% | Val Loss: 0.7891, Val Acc: 73.45% | LR: 0.000998
...
Epoch [45/150] Train Loss: 0.1234, Train Acc: 95.67% | Val Loss: 0.2345, Val Acc: 92.34% | LR: 0.000456
✓ 保存最佳模型 (验证准确率: 92.34%)
...
Epoch [82/150] Train Loss: 0.0876, Train Acc: 97.12% | Val Loss: 0.1987, Val Acc: 93.78% | LR: 0.000123
✓ 保存最佳模型 (验证准确率: 93.78%)
...
Epoch [120/150] Train Loss: 0.0654, Train Acc: 97.89% | Val Loss: 0.1876, Val Acc: 94.12% | LR: 0.000045
✓ 保存最佳模型 (验证准确率: 94.12%)
...

训练曲线已保存到: training_curves_v2.png

在测试集上评估最佳模型...
测试集准确率: 94.23%

混淆矩阵已保存到: confusion_matrix_v2.png

分类报告:
              precision    recall  f1-score   support

        fist       0.96      0.97      0.96       800
        palm       0.95      0.94      0.95       800
   thumbs_up       0.93      0.95      0.94       800
          ok       0.97      0.96      0.96       800
         gun       0.92      0.93      0.93      1600
     call_me       0.93      0.92      0.93      1600
     nothing       0.96      0.97      0.96       150

    accuracy                           0.94      5550
   macro avg       0.95      0.95      0.95      5550
weighted avg       0.94      0.94      0.94      5550

训练完成！最佳模型已保存到: gesture_v2_focal.pth
```

### 4.3 训练参数说明

| 参数 | 说明 | 推荐值 |
|------|------|--------|
| `--batch-size` | 批量大小 | GPU: 64, CPU: 32 |
| `--epochs` | 训练轮数 | 150-200 |
| `--lr` | 学习率 | 0.001 |
| `--loss` | 损失函数 | focal（不平衡数据）或 smooth（平衡数据） |

### 4.4 监控训练过程

**查看训练曲线**：
```bash
# 训练完成后会生成 training_curves_v2.png
# 使用图像查看器打开
```

**理想的训练曲线**：
- Train Loss 持续下降
- Val Loss 下降后趋于平稳
- Train Acc 达到 95-98%
- Val Acc 达到 92-95%
- Train Acc - Val Acc < 5% (无明显过拟合)

**查看混淆矩阵**：
```bash
# 查看 confusion_matrix_v2.png
```

**理想的混淆矩阵**：
- 对角线数值高 (>90%)
- 非对角线数值低 (<5%)
- 无明显混淆对

### 4.5 如果训练效果不理想

#### 准确率低 (<85%)

**可能原因**：
1. 数据量不足
2. 数据质量差
3. 训练轮数不够

**解决方案**：
```bash
# 增加训练轮数
python train_model_v2.py --epochs 200 ...

# 或者重新采集数据
python collect_training_data.py --port COM17
```

#### 过拟合 (Train Acc >> Val Acc)

**症状**：Train Acc: 98%, Val Acc: 85%

**解决方案**：
```bash
# 使用标签平滑
python train_model_v2.py --loss smooth ...

# 或增加数据增强
python augment_dataset.py --factor 15  # 增加到15倍
```

#### 特定手势混淆

**查看混淆矩阵**，如果 gun ↔ thumbs_up 混淆：

**解决方案**：
```bash
# 重新采集更多 gun 手势（不同角度）
python collect_training_data.py --port COM17
# 选择手势 4, 采集 1000 张

# 重新增强
python augment_dataset.py --input dataset_real --output dataset_augmented --factor 10

# 重新训练
python train_model_v2.py --dataset dataset_augmented --epochs 150 --loss focal
```

---

## 5. 模型测试

### 5.1 在PC端测试单张图像

```bash
# 测试一张图像
python test_pytorch_model_v2.py gesture_v2_focal.pth test_images/fist_sample.png
```

**预期输出**：
```
✓ 模型加载成功: gesture_v2_focal.pth

预测结果:
  手势: fist
  ID: 0
  置信度: 0.9823 (98.23%)

所有类别的置信度:
  0 (fist        ): 0.9823 (98.23%)
  1 (palm        ): 0.0045 (0.45%)
  2 (thumbs_up   ): 0.0023 (0.23%)
  3 (ok          ): 0.0012 (0.12%)
  4 (gun         ): 0.0067 (0.67%)
  5 (call_me     ): 0.0021 (0.21%)
  6 (nothing     ): 0.0009 (0.09%)
```

### 5.2 使用 debug_inference.py 测试实时推理

```bash
# 实时测试STM32推理 + PC端ONNX对比
python debug_inference.py \
    --port COM17 \
    --frames 100 \
    --onnx-model gesture_v2.onnx
```

**输出**：
```
调试目录: debug_inference
ONNX模型: gesture_v2.onnx
✓ ONNX模型加载成功

连接串口: COM17 (921600)...
✓ 串口连接成功

开始采集 100 帧...
✓ 帧 1: fist (0.95) | 均值:125.3 | 正常
✓ 帧 2: fist (0.97) | 均值:123.7 | 正常
✓ 帧 3: palm (0.92) | 均值:128.1 | 正常
...
✓ 帧 100: nothing (0.98) | 均值:126.5 | 正常

采集完成！保存到: debug_inference/

生成摘要报告...
✓ 摘要已保存: debug_inference/summary.txt
✓ 可视化已保存: debug_inference/summary.png
```

**查看结果**：
```bash
cat debug_inference/summary.txt
```

**预期 summary.txt 内容**：
```
================================================================================
推理调试报告
================================================================================
采集时间: 2025-01-02 14:30:00
采集帧数: 100
保存目录: debug_inference

================================================================================
图像分析统计:
================================================================================
亮度统计:
  平均值: 125.8
  标准差: 5.3
  最小值: 115.2
  最大值: 138.4

曝光情况:
  正常 (100-150): 98 帧 (98.0%)
  欠曝 (<100):     2 帧 (2.0%)
  过曝 (>150):     0 帧 (0.0%)

================================================================================
STM32 识别结果统计:
================================================================================
识别分布:
  0 (fist):      15 次 (15.0%)
  1 (palm):      14 次 (14.0%)
  2 (thumbs_up): 13 次 (13.0%)
  3 (ok):        14 次 (14.0%)
  4 (gun):       15 次 (15.0%)
  5 (call_me):   15 次 (15.0%)
  6 (nothing):   14 次 (14.0%)

平均置信度: 0.93

================================================================================
STM32 vs PC 对比统计:
================================================================================
对比总数: 100
一致: 94 次 (94.0%)
不一致: 6 次 (6.0%)

不一致的帧:
  帧 23: STM32=gun(0.85) vs PC=thumbs_up(0.88)
  帧 45: STM32=thumbs_up(0.82) vs PC=gun(0.86)
  ...

================================================================================
```

**分析**：
- **一致性 >90%**: 模型转换成功
- **一致性 <85%**: 可能需要检查量化或转换过程

---

## 6. 模型转换

### 6.1 转换为ONNX格式

```bash
python convert_to_onnx_v2.py \
    --input gesture_v2_focal.pth \
    --output gesture_v2.onnx
```

**预期输出**：
```
加载PyTorch模型: gesture_v2_focal.pth
✓ 模型加载成功
模型参数量: 88,343
模型大小: 34.51 KB

测试模型前向传播...
输入形状: torch.Size([1, 1, 64, 64])
输出形状: torch.Size([1, 7])
输出维度: 7 (应为7) ✓

导出ONNX模型: gesture_v2.onnx
✓ ONNX模型导出成功

验证ONNX模型...
✓ ONNX模型验证通过

输入:
  名称: input
  形状: [1, 1, 64, 64]

输出:
  名称: output
  形状: [1, 7]

使用ONNX Runtime测试...
✓ ONNX Runtime 推理成功
  输出形状: (1, 7)
  输出样例: [-2.345, 4.567, -1.234]...

PyTorch vs ONNX 最大误差: 0.000012
✓ 转换精度验证通过

✓ 转换完成！ONNX模型已保存到: gesture_v2.onnx

下一步:
1. 使用 X-CUBE-AI 导入 ONNX 模型
2. 验证输出维度为 7
3. 生成 C 代码并集成到 STM32 项目
```

### 6.2 验证ONNX模型

```bash
# 使用 ONNX Runtime 测试
python -c "
import onnxruntime as ort
import numpy as np

session = ort.InferenceSession('gesture_v2.onnx')

# 测试输入
input_data = np.random.randn(1, 1, 64, 64).astype(np.float32)
outputs = session.run(None, {'input': input_data})

print('输出形状:', outputs[0].shape)
print('输出维度:', outputs[0].shape[1], '(应为7)')
print('输出样例:', outputs[0][0])
"
```

**预期输出**：
```
输出形状: (1, 7)
输出维度: 7 (应为7)
输出样例: [-2.345  4.567 -1.234  0.876 -0.543  1.234  0.123]
```

---

## 7. STM32部署

### 7.1 使用 X-CUBE-AI 转换

#### 步骤1: 启动 X-CUBE-AI

1. 打开 STM32CubeMX
2. 在项目中选择 **Software Packs** → **X-CUBE-AI**
3. 点击 **Add Network**

#### 步骤2: 导入ONNX模型

```
1. Network Name: gesture_model
2. Model Type: ONNX
3. Model File: 选择 gesture_v2.onnx
4. 点击 "Analyze"
```

#### 步骤3: 验证模型信息

**应该看到**：
```
Model Analysis:
  Input:  [1, 1, 64, 64]  (4096 bytes)
  Output: [1, 7]          (28 bytes)

  Layers: 45
  Weights: 88,343 parameters
  Memory:
    - ROM: ~35 KB
    - RAM: ~8 KB

  Estimated Performance:
    - Cycles: ~12M
    - Time: ~120ms @ 100MHz
```

**关键检查**：
- ✅ Output dimension = 7 (不是9!)
- ✅ ROM < 50KB
- ✅ RAM < 20KB

#### 步骤4: 生成代码

```
1. 点击 "Generate Code"
2. 选择输出目录
3. 等待生成完成
```

**生成的文件**：
```
X-CUBE-AI/App/
  ├── gesture_model.c
  ├── gesture_model.h
  ├── gesture_model_data.c
  └── gesture_model_data.h
```

### 7.2 集成到STM32项目

#### 步骤1: 复制生成的文件

```bash
# 将生成的文件复制到项目
cp X-CUBE-AI/App/gesture_model* /path/to/stm32_project/X-CUBE-AI/App/
```

#### 步骤2: 验证 NUM_GESTURES

确认 `App/inference_task.h` 中：
```c
#define NUM_GESTURES 7  // ✓ 应该是7，不是9
```

#### 步骤3: 重新编译

```bash
# 在Keil MDK或STM32CubeIDE中
# 1. 清理项目
make clean

# 2. 重新编译
make

# 3. 检查编译输出
# 应该无错误，无警告
```

#### 步骤4: 烧录固件

```bash
# 使用ST-Link
make flash

# 或使用STM32CubeProgrammer
```

### 7.3 测试部署结果

#### 在STM32上测试

1. **OLED显示检查**：
   ```
   MODE: INFERENCE
   fps=25.3 c2=123
   Gest:G2 C:87%    ← 应显示 G0-G6
   ```

2. **置信度视图检查**（切换视图）：
   ```
   All Confidences
   Page 1/2
   0:Fis 45%
   1:Pal 12%
   2:Thm 87%
   3:OK  3%
   ```

3. **串口输出检查**：
   ```bash
   # 查看串口输出
   python -c "
   import serial
   ser = serial.Serial('COM17', 921600, timeout=1)
   for _ in range(10):
       line = ser.readline().decode('utf-8', errors='ignore')
       print(line.strip())
   "
   ```

   **预期输出**：
   ```
   Inference: ID=2, Conf=0.87
   Inference: ID=2, Conf=0.89
   Inference: ID=1, Conf=0.92
   ...
   ```

#### 端到端测试

```bash
# 运行完整测试
python debug_inference.py --port COM17 --frames 100 --onnx-model gesture_v2.onnx

# 检查 summary.txt
# - STM32 vs PC 一致性应 >90%
# - 识别结果应该多样化（不再全是 palm）
# - 平均置信度应 >0.85
```

---

## 8. 完整示例

### 8.1 从零开始的完整流程

```bash
# ============================================
# 第1步: 准备环境
# ============================================
cd /home/river/project/project1/project6/Python
pip install torch torchvision opencv-python numpy matplotlib seaborn scikit-learn pyserial onnx onnxruntime

# ============================================
# 第2步: 采集数据（约30分钟）
# ============================================
# 采集 fist (400张)
python collect_training_data.py --port COM17
# 输入: 0, 400

# 采集 palm (400张)
python collect_training_data.py --port COM17
# 输入: 1, 400

# 采集 thumbs_up (400张)
python collect_training_data.py --port COM17
# 输入: 2, 400

# 采集 ok (400张)
python collect_training_data.py --port COM17
# 输入: 3, 400

# 采集 gun (800张) - 重点！
python collect_training_data.py --port COM17
# 输入: 4, 800

# 采集 call_me (800张) - 重点！
python collect_training_data.py --port COM17
# 输入: 5, 800

# 采集 nothing (500张)
python collect_training_data.py --port COM17
# 输入: 6, 500

# ============================================
# 第3步: 数据增强（约2-3分钟）
# ============================================
python augment_dataset.py \
    --input dataset_real \
    --output dataset_augmented \
    --factor 10

# ============================================
# 第4步: 训练模型（约30-60分钟，GPU）
# ============================================
python train_model_v2.py \
    --dataset dataset_augmented \
    --batch-size 64 \
    --epochs 150 \
    --lr 0.001 \
    --loss focal \
    --model-name gesture_v2.pth

# ============================================
# 第5步: 测试模型
# ============================================
# 测试单张图像
python test_pytorch_model_v2.py gesture_v2.pth test_image.png

# 实时测试
python debug_inference.py --port COM17 --frames 100

# ============================================
# 第6步: 转换为ONNX
# ============================================
python convert_to_onnx_v2.py \
    --input gesture_v2.pth \
    --output gesture_v2.onnx

# ============================================
# 第7步: X-CUBE-AI转换（GUI操作）
# ============================================
# 1. 打开STM32CubeMX
# 2. X-CUBE-AI → Add Network
# 3. 导入 gesture_v2.onnx
# 4. Analyze → Generate Code
# 5. 复制生成的文件到项目

# ============================================
# 第8步: STM32编译和烧录
# ============================================
# 在Keil或STM32CubeIDE中：
# 1. Clean Project
# 2. Build
# 3. Flash

# ============================================
# 第9步: 最终测试
# ============================================
python debug_inference.py \
    --port COM17 \
    --frames 100 \
    --onnx-model gesture_v2.onnx

# 检查一致性 >90%
cat debug_inference/summary.txt
```

### 8.2 时间预估

| 步骤 | 时间（GPU） | 时间（CPU） |
|------|------------|------------|
| 数据采集 | 30 分钟 | 30 分钟 |
| 数据增强 | 2-3 分钟 | 2-3 分钟 |
| 模型训练 | 30-60 分钟 | 3-6 小时 |
| 模型测试 | 5 分钟 | 5 分钟 |
| ONNX转换 | 1 分钟 | 1 分钟 |
| X-CUBE-AI | 10 分钟 | 10 分钟 |
| STM32编译 | 5 分钟 | 5 分钟 |
| **总计** | **~1.5 小时** | **~4-7 小时** |

---

## 9. 故障排除

### 9.1 数据采集问题

#### 问题: 串口连接失败

```
✗ 连接失败: could not open port 'COM17'
```

**解决方案**：
```bash
# 检查串口是否被占用
python -c "
import serial.tools.list_ports
ports = serial.tools.list_ports.comports()
for port in ports:
    print(f'{port.device}: {port.description}')
"

# 尝试其他串口或关闭占用程序
```

#### 问题: 图像全黑（欠曝）

```
平均亮度: 22.48 (应为 120-140)
```

**解决方案**：
1. 检查 OpenMV 曝光设置
2. 使用 `get_camera_smart_exposure.py`
3. 增加环境光照

### 9.2 训练问题

#### 问题: GPU内存不足

```
RuntimeError: CUDA out of memory
```

**解决方案**：
```bash
# 减小批量大小
python train_model_v2.py --batch-size 32  # 从64改为32
```

#### 问题: 准确率很低 (<70%)

**可能原因**：
1. 数据质量差
2. 数据量不足
3. 曝光问题

**解决方案**：
```bash
# 1. 检查数据亮度
python debug_inference.py --port COM17 --frames 50
# 查看平均亮度

# 2. 重新采集数据
python collect_training_data.py --port COM17

# 3. 增加训练轮数
python train_model_v2.py --epochs 200
```

### 9.3 转换问题

#### 问题: ONNX输出维度错误

```
输出维度: 9 (应为7)
```

**解决方案**：
```bash
# 检查模型定义
python -c "
from test_pytorch_model_v2 import GestureNetV2
model = GestureNetV2(num_classes=7)  # 确认是7
print(model.fc[-1].out_features)  # 应输出7
"

# 重新训练模型
python train_model_v2.py --dataset dataset_augmented --epochs 150
```

#### 问题: X-CUBE-AI 报错

```
Error: Unsupported operator: XXX
```

**解决方案**：
- 确认 X-CUBE-AI 版本 ≥ 7.0
- 使用 opset_version=11 或更低
- 简化模型结构

### 9.4 部署问题

#### 问题: STM32识别全部错误

**解决方案**：
```bash
# 1. 检查模型输出维度
# 在 X-CUBE-AI 分析中确认 Output: [1, 7]

# 2. 检查 NUM_GESTURES
# App/inference_task.h 应该是 #define NUM_GESTURES 7

# 3. 重新编译烧录
make clean && make && make flash
```

#### 问题: STM32-PC一致性低 (<80%)

**解决方案**：
```bash
# 1. 检查量化设置
# X-CUBE-AI 中使用 float32，不要使用 int8

# 2. 重新转换ONNX
python convert_to_onnx_v2.py --input gesture_v2.pth --output gesture_v2.onnx

# 3. 重新生成代码
# 在 X-CUBE-AI 中重新 Generate Code
```

---

## 📊 性能指标参考

### 预期性能

| 指标 | GestureNet | GestureNetV2 |
|------|-----------|-------------|
| 训练准确率 | 90-93% | 95-98% |
| 验证准确率 | 85-90% | 92-95% |
| 测试准确率 | 85-90% | 92-95% |
| STM32-PC一致性 | 88-92% | 90-94% |
| 推理时间 (STM32) | 80-100ms | 120-150ms |
| 模型大小 | ~15KB | ~30KB |

### 数据量建议

| 场景 | 每类样本数 | 总样本数 | 预期准确率 |
|------|-----------|---------|-----------|
| 最小可用 | 300 | 2100 | 80-85% |
| 推荐 | 500-800 | 3500-5600 | 90-93% |
| 最佳 | 1000+ | 7000+ | 93-95% |

---

## ✅ 成功清单

训练部署成功的标志：

- [ ] 数据采集：每类 >500 张，亮度 100-140
- [ ] 数据增强：总量 >30000 张
- [ ] 训练结果：Val Acc >92%, Train-Val差距 <5%
- [ ] 混淆矩阵：对角线 >90%
- [ ] ONNX转换：输出维度=7，误差 <1e-5
- [ ] X-CUBE-AI：ROM <50KB, RAM <20KB
- [ ] STM32测试：识别结果正确，FPS >7
- [ ] 端到端测试：STM32-PC一致性 >90%

---

## 🎓 进阶优化

### 提高准确率

1. **采集更多数据** - 每类 1000+ 张
2. **针对性采集** - 重点采集混淆的手势
3. **使用 GestureNetV2** - 提升 3-5%
4. **调整训练参数** - 更多轮数、不同损失函数

### 提高速度

1. **使用 GestureNet** - 原版更快
2. **模型量化** - X-CUBE-AI int8 量化
3. **提高CPU频率** - STM32超频

### 减小模型

1. **通道数减半** - 降低通道数
2. **减少层数** - 简化网络结构
3. **权重量化** - 使用 int8 权重

---

祝训练顺利！如有问题，请参考故障排除部分。🚀
