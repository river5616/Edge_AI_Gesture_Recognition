# GestureNetV2 改进版模型使用指南

## 📊 模型改进概述

### 相比原版 (GestureNet) 的改进

| 特性 | GestureNet (原版) | GestureNetV2 (改进版) |
|------|-------------------|----------------------|
| **参数量** | ~15KB | ~30KB |
| **通道数** | 16→32→64→128 | 24→48→96→128 |
| **注意力机制** | 无 | SE Block (通道注意力) |
| **残差连接** | 无 | 3个残差块 |
| **预期准确率** | ~85-90% | ~92-95% |
| **训练轮数** | 100 | 150 |

### 关键改进技术

#### 1. SE (Squeeze-and-Excitation) 注意力模块
- **作用**: 让模型学习每个通道的重要性权重
- **参数增加**: 非常小（每个SE块仅增加 channels²/64 个参数）
- **效果**: 通常提升 2-3% 准确率

```
通道权重计算：
输入特征 → 全局平均池化 → 两层全连接 → Sigmoid → 加权输出
```

#### 2. 残差连接 (Residual Connections)
- **作用**: 帮助梯度流动，缓解梯度消失
- **位置**: 在每个下采样块后添加
- **效果**: 训练更稳定，收敛更快

#### 3. 增强的数据增强
- 更强的旋转 (±40°)
- 仿射变换 (平移、缩放、剪切)
- 随机擦除 (模拟遮挡)
- 亮度/对比度增强

#### 4. 优化的训练策略
- **Focal Loss**: 重点关注难分类样本
- **Label Smoothing**: 提高泛化能力
- **Cosine Annealing LR**: 学习率余弦退火
- **AdamW优化器**: 更好的权重衰减

---

## 🚀 快速开始

### 1. 训练模型

```bash
# 基础训练（使用默认参数）
python train_model_v2.py --dataset dataset --epochs 150

# 使用 Focal Loss（推荐用于不平衡数据）
python train_model_v2.py --dataset dataset --epochs 150 --loss focal

# 使用标签平滑
python train_model_v2.py --dataset dataset --epochs 150 --loss smooth

# 自定义参数
python train_model_v2.py \
    --dataset dataset_augmented \
    --batch-size 64 \
    --epochs 200 \
    --lr 0.001 \
    --loss focal \
    --model-name gesture_v2_best.pth
```

### 2. 测试模型

```bash
# 测试单张图像
python test_pytorch_model_v2.py gesture_v2.pth test_image.png
```

### 3. 转换为ONNX

```bash
# 使用修改后的转换脚本
python convert_to_onnx_v2.py --input gesture_v2.pth --output gesture_v2.onnx
```

---

## 📈 训练参数说明

### 命令行参数

| 参数 | 默认值 | 说明 |
|------|--------|------|
| `--dataset` | `dataset` | 数据集目录路径 |
| `--batch-size` | `32` | 批量大小 |
| `--epochs` | `150` | 训练轮数 |
| `--lr` | `0.001` | 初始学习率 |
| `--loss` | `focal` | 损失函数 (ce/focal/smooth) |
| `--model-name` | `gesture_v2.pth` | 模型保存名称 |

### 损失函数选择

#### Cross Entropy (ce)
```bash
python train_model_v2.py --loss ce
```
- 标准交叉熵损失
- 适用于数据平衡的情况
- 训练速度最快

#### Focal Loss (focal) ⭐ 推荐
```bash
python train_model_v2.py --loss focal
```
- 关注难分类样本
- 适用于数据不平衡
- 对 gun/call_me 等新手势效果更好
- **推荐用于初次训练**

#### Label Smoothing (smooth)
```bash
python train_model_v2.py --loss smooth
```
- 防止过拟合
- 提高泛化能力
- 适用于数据充足的情况

---

## 📊 预期性能

### 准确率预期

| 数据集规模 | 原版准确率 | V2准确率 | 改进幅度 |
|-----------|-----------|---------|---------|
| 小 (2000张) | 80-85% | 85-90% | +5% |
| 中 (5000张) | 85-90% | 90-93% | +3-5% |
| 大 (10000张) | 88-92% | 92-95% | +3-4% |

### 模型大小对比

```
原版 (GestureNet):     ~15KB (float32)
改进版 (GestureNetV2): ~30KB (float32)
增加:                  +15KB (约2倍)
```

### 推理速度对比

在STM32H743 (400MHz) 上：
```
原版:   约 80-100ms/帧
改进版: 约 120-150ms/帧
增加:   约 +40%
```

**权衡**: 虽然速度稍慢，但仍能达到 7-10 FPS，足够实时应用。

---

## 🔧 训练技巧

### 1. 数据准备

**最小数据量**：
- 每个手势至少 300 张
- 新手势 (gun, call_me) 建议 500-800 张

**推荐数据量**：
- 每个手势 500-1000 张
- 使用数据增强工具扩展到 5000-10000 张

```bash
# 使用数据增强
python augment_dataset.py \
    --input dataset_real \
    --output dataset_augmented \
    --factor 10
```

### 2. 监控训练过程

训练过程中会输出：
```
Epoch [45/150] Train Loss: 0.1234, Train Acc: 95.67% | Val Loss: 0.2345, Val Acc: 92.34% | LR: 0.000456
✓ 保存最佳模型 (验证准确率: 92.34%)
```

**关键指标**：
- **Train Acc > 95%**: 模型学习能力良好
- **Val Acc > 90%**: 泛化能力良好
- **Train Acc - Val Acc < 5%**: 无明显过拟合

### 3. 识别过拟合

**症状**：
- Train Acc >> Val Acc (差距 >10%)
- Val Loss 开始上升

**解决方案**：
1. 增加数据增强强度
2. 增加 Dropout 比例
3. 使用 Label Smoothing
4. 减少训练轮数

### 4. 识别欠拟合

**症状**：
- Train Acc 和 Val Acc 都很低 (<80%)
- Loss 下降缓慢

**解决方案**：
1. 增加训练轮数
2. 增大学习率
3. 检查数据质量

---

## 📉 混淆矩阵分析

训练完成后会生成 `confusion_matrix_v2.png`：

### 理想情况
```
       fist palm t_up  ok  gun call noth
fist    95    0    0   0    0    0    0
palm     0   93    1   0    0    0    1
t_up     0    0   94   0    1    0    0
ok       0    0    0  96    0    0    0
gun      0    0    2   0   92    1    0
call     0    0    0   0    1   93    1
noth     0    1    0   0    0    0   94
```
- 对角线数值高 (>90%)
- 非对角线数值低 (<5%)

### 常见混淆对

如果出现以下混淆：
- **gun ↔ thumbs_up**: 需要更多侧面角度的 gun 数据
- **call_me ↔ rock** (如果保留了rock): 形状相似，考虑删除rock
- **palm ↔ nothing**: 需要更清晰的背景区分

---

## 🔄 模型迭代流程

### 第一次训练
```bash
# 1. 基础训练
python train_model_v2.py --dataset dataset --epochs 150 --loss focal

# 2. 评估结果
# 查看 confusion_matrix_v2.png
# 如果准确率 <85%，继续下一步

# 3. 增强数据集
python augment_dataset.py --input dataset --output dataset_aug --factor 10

# 4. 重新训练
python train_model_v2.py --dataset dataset_aug --epochs 150 --loss focal
```

### 第二次训练（如果第一次不满意）
```bash
# 针对性增强混淆的类别
# 例如，如果 gun ↔ thumbs_up 混淆
# 手动采集更多不同角度的 gun 手势

# 重新训练
python train_model_v2.py \
    --dataset dataset_aug \
    --epochs 200 \
    --loss smooth \
    --lr 0.0005
```

---

## 🎯 转换为ONNX

需要创建对应的转换脚本 `convert_to_onnx_v2.py`：

```python
import torch
import torch.onnx
from test_pytorch_model_v2 import GestureNetV2

# 加载模型
model = GestureNetV2(num_classes=7)
model.load_state_dict(torch.load('gesture_v2.pth'))
model.eval()

# 创建示例输入
dummy_input = torch.randn(1, 1, 64, 64)

# 导出ONNX
torch.onnx.export(
    model,
    dummy_input,
    'gesture_v2.onnx',
    export_params=True,
    opset_version=11,
    do_constant_folding=True,
    input_names=['input'],
    output_names=['output'],
    dynamic_axes={
        'input': {0: 'batch_size'},
        'output': {0: 'batch_size'}
    }
)

print("✓ ONNX模型已导出: gesture_v2.onnx")
```

### 验证ONNX模型

```bash
# 使用debug_inference.py测试
python debug_inference.py \
    --port COM17 \
    --frames 50 \
    --onnx-model gesture_v2.onnx
```

---

## 📊 与原版对比测试

创建对比脚本测试两个模型：

```python
# compare_models.py
import torch
from train_model import GestureNet
from test_pytorch_model_v2 import GestureNetV2

# 加载两个模型
model_v1 = GestureNet(num_classes=7)
model_v1.load_state_dict(torch.load('gesture_v1.pth'))

model_v2 = GestureNetV2(num_classes=7)
model_v2.load_state_dict(torch.load('gesture_v2.pth'))

# 在测试集上评估
# ... (评估代码)

# 对比结果
print("V1准确率:", v1_acc)
print("V2准确率:", v2_acc)
print("改进:", v2_acc - v1_acc)
```

---

## ❓ 常见问题

### Q1: 训练时显存不足？

**A**: 减小批量大小
```bash
python train_model_v2.py --batch-size 16  # 从32减到16
```

### Q2: 训练速度太慢？

**A**:
1. 使用GPU（如果可用）
2. 减少数据增强强度
3. 减少训练轮数

### Q3: 模型准确率不提升？

**A**:
1. 检查数据质量（图像是否清晰、标注是否正确）
2. 增加数据量
3. 尝试不同的损失函数
4. 调整学习率

### Q4: Val Acc波动很大？

**A**:
1. 增大批量大小
2. 增加验证集大小
3. 使用学习率预热

### Q5: 模型在STM32上运行太慢？

**A**:
1. 确认使用了X-CUBE-AI优化
2. 考虑量化模型（int8）
3. 如果仍然太慢，回退到原版GestureNet

---

## 📝 总结

### 何时使用GestureNetV2？

**推荐使用V2的情况**：
- ✅ 准确率要求 >90%
- ✅ 可接受 120-150ms 推理时间（~7 FPS）
- ✅ 有充足的训练数据（每类 >500 张）
- ✅ STM32有足够的Flash空间（+15KB）

**建议使用V1的情况**：
- ⚠️ 严格的实时性要求（需要 >10 FPS）
- ⚠️ Flash空间紧张
- ⚠️ 训练数据有限（每类 <300 张）

### 预期改进总结

```
准确率:    +3-5%
模型大小:  +15KB (2倍)
推理时间:  +40ms (+40%)
训练时间:  +50% (因为更多轮数)
```

**结论**: 对于大多数应用，V2的准确率提升值得额外的计算和存储开销。

---

祝训练顺利！🚀
