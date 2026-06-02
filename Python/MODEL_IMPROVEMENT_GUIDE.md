# 手势识别模型改进完整方案

## 📋 当前问题诊断

### 1. 性能指标
- **STM32 vs PC 一致性**: 72% ✅（转换成功）
- **主要混淆对**:
  - palm ↔ four: 7次混淆 (50%)
  - point ↔ v_sign: 6次混淆 (43%)
- **置信度分析**:
  - 高：palm (6-9), four (6-8)
  - 低：point (1.9-3.4), v_sign (1.8-3.4)

### 2. 根本原因
**训练数据与实际部署环境不匹配**（Distribution Shift）
- 训练集：理想条件下采集
- 实际部署：不同角度、距离、光照
- 相似手势特征空间重叠

---

## 🎯 改进方案（三个阶段）

## 阶段一：重新采集数据 ⭐ **最重要**

### 方案 A：使用实际硬件采集（强烈推荐）

#### 1. 数据采集计划

**工具**: `collect_training_data.py`（已创建）

**使用方法**:
```bash
# 1. 切换 STM32 到采集模式（MODE_COLLECTING）
# 2. 运行采集工具
python collect_training_data.py --port COM17

# 3. 按提示输入手势ID和数量
```

**采集目标**:

| 手势 | 最少 | 推荐 | 最佳 | 优先级 |
|------|------|------|------|--------|
| **palm (1)** | 300 | 600 | 1000 | ⚠️ 高（易混淆） |
| **four (7)** | 300 | 600 | 1000 | ⚠️ 高（易混淆） |
| **v_sign (4)** | 300 | 600 | 1000 | ⚠️ 高（易混淆） |
| **point (5)** | 300 | 600 | 1000 | ⚠️ 高（易混淆） |
| fist (0) | 200 | 400 | 800 | 中 |
| thumbs_up (2) | 200 | 400 | 800 | 中 |
| ok (3) | 200 | 400 | 800 | 中 |
| three (6) | 200 | 400 | 800 | 中 |
| nothing (8) | 200 | 400 | 800 | 中 |
| **总计** | **2000** | **4400** | **8200** | - |

#### 2. 采集多样性要求（重要！）

**对于易混淆手势（palm/four, point/v_sign）**，必须包含：

##### 角度变化（至少5个角度）:
- 正面（0°）：手掌垂直于摄像头
- 左斜（-30°）：手掌向左倾斜
- 右斜（+30°）：手掌向右倾斜
- 上仰（+20°）：手掌向上倾斜
- 下俯（-20°）：手掌向下倾斜

##### 距离变化（至少3个距离）:
- 近距离：手部占据画面 70%
- 中距离：手部占据画面 50%（推荐）
- 远距离：手部占据画面 30%

##### 位置变化（至少9个位置）:
```
左上  中上  右上
左中  中中  右中
左下  中下  右下
```

##### 手指细节变化:
- **palm vs four 区分重点**：
  - palm: 五指完全展开、拇指分开
  - four: 四指并拢、拇指收起或贴近手掌
  - 采集 palm 时确保拇指明显分开
  - 采集 four 时确保拇指收起

- **point vs v_sign 区分重点**：
  - point: 食指伸直、其他手指弯曲
  - v_sign: 食指+中指伸直、分开呈V字
  - 采集 point 时确保只有一根手指
  - 采集 v_sign 时确保两根手指明显分开

##### 背景变化:
- 移动手部到不同背景（墙壁、桌面、窗户等）

#### 3. 采集流程示例

```bash
# 示例：采集 palm 手势 500 张
python collect_training_data.py --port COM17

# 输入:
# 手势ID: 1
# 数量: 500

# 采集过程（约5-10分钟）：
# 分组采集，确保多样性
# - 第1组 (100张): 正面角度，三个距离
# - 第2组 (100张): 左斜角度，三个距离
# - 第3组 (100张): 右斜角度，三个距离
# - 第4组 (100张): 上仰角度，三个距离
# - 第5组 (100张): 下俯角度，三个距离

# 移动到不同位置和背景，继续采集
```

---

### 方案 B：数据增强（辅助方案）

如果无法采集足够真实数据，使用数据增强扩充数据集：

创建增强工具 `augment_dataset.py`:

```python
#!/usr/bin/env python3
"""
数据增强工具 - 对现有数据集进行增强
"""

import cv2
import numpy as np
import os
from pathlib import Path

def augment_image(image):
    """
    对单张图像进行多种增强

    返回: 增强后的图像列表
    """
    augmented = []

    # 1. 原图
    augmented.append(('original', image))

    # 2. 轻微旋转 (-15° 到 +15°)
    for angle in [-15, -10, -5, 5, 10, 15]:
        h, w = image.shape
        M = cv2.getRotationMatrix2D((w/2, h/2), angle, 1.0)
        rotated = cv2.warpAffine(image, M, (w, h), borderValue=int(np.mean(image)))
        augmented.append((f'rot{angle:+d}', rotated))

    # 3. 平移 (-8 到 +8 像素)
    for dx, dy in [(-8, -8), (-8, 0), (-8, 8), (0, -8), (0, 8), (8, -8), (8, 0), (8, 8)]:
        h, w = image.shape
        M = np.float32([[1, 0, dx], [0, 1, dy]])
        shifted = cv2.warpAffine(image, M, (w, h), borderValue=int(np.mean(image)))
        augmented.append((f'shift{dx:+d}{dy:+d}', shifted))

    # 4. 缩放 (0.85 到 1.15)
    for scale in [0.85, 0.90, 0.95, 1.05, 1.10, 1.15]:
        h, w = image.shape
        new_h, new_w = int(h * scale), int(w * scale)
        scaled = cv2.resize(image, (new_w, new_h))

        # 裁剪或填充到原始尺寸
        if scale > 1.0:
            # 裁剪中心
            start_y = (new_h - h) // 2
            start_x = (new_w - w) // 2
            result = scaled[start_y:start_y+h, start_x:start_x+w]
        else:
            # 填充边缘
            result = np.full((h, w), int(np.mean(image)), dtype=np.uint8)
            start_y = (h - new_h) // 2
            start_x = (w - new_w) // 2
            result[start_y:start_y+new_h, start_x:start_x+new_w] = scaled

        augmented.append((f'scale{int(scale*100)}', result))

    # 5. 亮度调整 (-30 到 +30)
    for delta in [-30, -20, -10, 10, 20, 30]:
        adjusted = cv2.add(image, delta)
        augmented.append((f'bright{delta:+d}', adjusted))

    # 6. 对比度调整 (0.7 到 1.3)
    for alpha in [0.7, 0.8, 0.9, 1.1, 1.2, 1.3]:
        adjusted = cv2.convertScaleAbs(image, alpha=alpha, beta=0)
        augmented.append((f'contrast{int(alpha*100)}', adjusted))

    # 7. 高斯噪声 (sigma=5, 10)
    for sigma in [5, 10]:
        noise = np.random.normal(0, sigma, image.shape)
        noisy = np.clip(image + noise, 0, 255).astype(np.uint8)
        augmented.append((f'noise{sigma}', noisy))

    # 8. 高斯模糊 (kernel=3, 5)
    for ksize in [3, 5]:
        blurred = cv2.GaussianBlur(image, (ksize, ksize), 0)
        augmented.append((f'blur{ksize}', blurred))

    # 9. 水平翻转（小心使用，某些手势不对称）
    flipped = cv2.flip(image, 1)
    augmented.append(('flip', flipped))

    return augmented

def augment_dataset(input_dir, output_dir, augment_factor=10):
    """
    增强整个数据集

    Args:
        input_dir: 输入数据集目录
        output_dir: 输出增强后的数据集目录
        augment_factor: 每张图像生成多少个增强版本
    """
    input_path = Path(input_dir)
    output_path = Path(output_dir)
    output_path.mkdir(parents=True, exist_ok=True)

    # 遍历每个手势类别
    for gesture_dir in input_path.iterdir():
        if not gesture_dir.is_dir():
            continue

        gesture_name = gesture_dir.name
        output_gesture_dir = output_path / gesture_name
        output_gesture_dir.mkdir(exist_ok=True)

        print(f"\n处理手势: {gesture_name}")

        # 获取所有图像
        image_files = list(gesture_dir.glob("*.png")) + list(gesture_dir.glob("*.jpg"))
        total = len(image_files)

        for idx, img_file in enumerate(image_files):
            # 读取图像
            image = cv2.imread(str(img_file), cv2.IMREAD_GRAYSCALE)

            if image is None:
                print(f"  ✗ 无法读取: {img_file.name}")
                continue

            # 确保尺寸为 64x64
            if image.shape != (64, 64):
                image = cv2.resize(image, (64, 64))

            # 生成增强图像
            augmented_images = augment_image(image)

            # 只保存前 augment_factor 个增强版本
            for aug_idx, (aug_name, aug_img) in enumerate(augmented_images[:augment_factor]):
                output_filename = f"{img_file.stem}_aug{aug_idx:02d}_{aug_name}.png"
                output_path_full = output_gesture_dir / output_filename
                cv2.imwrite(str(output_path_full), aug_img)

            if (idx + 1) % 10 == 0:
                print(f"  进度: {idx+1}/{total} ({(idx+1)/total*100:.1f}%)")

        final_count = len(list(output_gesture_dir.glob("*.png")))
        print(f"  ✓ 完成: 原始 {total} 张 → 增强后 {final_count} 张 (x{final_count/total:.1f})")

if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description="数据增强工具")
    parser.add_argument("--input", type=str, default="dataset", help="输入数据集目录")
    parser.add_argument("--output", type=str, default="dataset_augmented", help="输出目录")
    parser.add_argument("--factor", type=int, default=10, help="每张图像生成多少个增强版本")

    args = parser.parse_args()

    print("="*60)
    print("数据增强工具")
    print("="*60)
    print(f"输入: {args.input}")
    print(f"输出: {args.output}")
    print(f"增强倍数: {args.factor}")
    print("="*60)

    augment_dataset(args.input, args.output, args.factor)

    print("\n" + "="*60)
    print("✓ 数据增强完成！")
    print("="*60)
```

**使用方法**:
```bash
# 对现有数据集进行10倍增强
python augment_dataset.py --input dataset --output dataset_augmented --factor 10
```

---

## 阶段二：改进模型架构 🔧

### 1. 增大模型容量（牺牲速度换精度）

当前模型参数量很小，可以增加：

#### 方案 A：加深网络（推荐）

修改 `train_model.py` 中的 `GestureNet`:

```python
class GestureNetV2(nn.Module):
    """增强版手势识别网络 - 更深更宽"""

    def __init__(self, num_classes=9):
        super(GestureNetV2, self).__init__()

        # Input: 64x64x1
        self.conv1 = nn.Sequential(
            nn.Conv2d(1, 24, kernel_size=3, stride=2, padding=1, bias=False),  # 32x32x24 (增加通道)
            nn.BatchNorm2d(24),
            nn.ReLU6(inplace=True)
        )

        # 增加一层深度可分离卷积
        self.dw_conv1 = DepthwiseSeparableConv(24, 48)  # 32x32x48
        self.dw_conv2 = DepthwiseSeparableConv(48, 64)  # 32x32x64
        self.pool1 = nn.MaxPool2d(2, 2)                  # 16x16x64

        self.dw_conv3 = DepthwiseSeparableConv(64, 128) # 16x16x128
        self.dw_conv4 = DepthwiseSeparableConv(128, 128) # 16x16x128 (新增)
        self.pool2 = nn.MaxPool2d(2, 2)                  # 8x8x128

        self.dw_conv5 = DepthwiseSeparableConv(128, 256) # 8x8x256
        self.dw_conv6 = DepthwiseSeparableConv(256, 256) # 8x8x256 (新增)

        self.global_avg_pool = nn.AdaptiveAvgPool2d(1)  # 1x1x256

        # 更大的全连接层
        self.fc = nn.Sequential(
            nn.Dropout(0.5),
            nn.Linear(256, 128),  # 增加隐藏层大小
            nn.ReLU(inplace=True),
            nn.Dropout(0.3),
            nn.Linear(128, 64),   # 新增一层
            nn.ReLU(inplace=True),
            nn.Dropout(0.2),
            nn.Linear(64, num_classes)
        )

    def forward(self, x):
        x = self.conv1(x)
        x = self.dw_conv1(x)
        x = self.dw_conv2(x)
        x = self.pool1(x)
        x = self.dw_conv3(x)
        x = self.dw_conv4(x)
        x = self.pool2(x)
        x = self.dw_conv5(x)
        x = self.dw_conv6(x)
        x = self.global_avg_pool(x)
        x = x.view(x.size(0), -1)
        x = self.fc(x)
        return x
```

**预计效果**:
- 参数量: ~15K → ~80K (约5倍)
- 推理时间: ~20ms → ~50ms (STM32H743, 400MHz)
- 精度提升: 预计 +10-15%

#### 方案 B：使用注意力机制

```python
class ChannelAttention(nn.Module):
    """通道注意力模块"""
    def __init__(self, channels, reduction=8):
        super(ChannelAttention, self).__init__()
        self.avg_pool = nn.AdaptiveAvgPool2d(1)
        self.fc = nn.Sequential(
            nn.Linear(channels, channels // reduction),
            nn.ReLU(inplace=True),
            nn.Linear(channels // reduction, channels),
            nn.Sigmoid()
        )

    def forward(self, x):
        b, c, _, _ = x.size()
        y = self.avg_pool(x).view(b, c)
        y = self.fc(y).view(b, c, 1, 1)
        return x * y

# 在 DepthwiseSeparableConv 后添加注意力
class DepthwiseSeparableConvWithAttention(nn.Module):
    def __init__(self, in_channels, out_channels, kernel_size=3, stride=1, padding=1):
        super().__init__()
        self.depthwise = nn.Conv2d(in_channels, in_channels, kernel_size, stride, padding, groups=in_channels, bias=False)
        self.pointwise = nn.Conv2d(in_channels, out_channels, 1, bias=False)
        self.bn = nn.BatchNorm2d(out_channels)
        self.relu = nn.ReLU6(inplace=True)
        self.attention = ChannelAttention(out_channels)  # 新增注意力

    def forward(self, x):
        x = self.depthwise(x)
        x = self.pointwise(x)
        x = self.bn(x)
        x = self.relu(x)
        x = self.attention(x)  # 应用注意力
        return x
```

---

### 2. 改进损失函数

针对混淆的手势对，使用 **Focal Loss** 或 **类别加权**：

```python
import torch.nn.functional as F

class FocalLoss(nn.Module):
    """
    Focal Loss - 让模型更关注难分类的样本
    """
    def __init__(self, alpha=0.25, gamma=2.0):
        super(FocalLoss, self).__init__()
        self.alpha = alpha
        self.gamma = gamma

    def forward(self, inputs, targets):
        ce_loss = F.cross_entropy(inputs, targets, reduction='none')
        pt = torch.exp(-ce_loss)
        focal_loss = self.alpha * (1 - pt) ** self.gamma * ce_loss
        return focal_loss.mean()

# 或者使用类别加权
class_weights = torch.tensor([
    1.0,  # 0: fist
    2.0,  # 1: palm      ← 增加权重（易混淆）
    1.0,  # 2: thumbs_up
    1.2,  # 3: ok
    2.0,  # 4: v_sign    ← 增加权重（易混淆）
    2.0,  # 5: point     ← 增加权重（易混淆）
    1.2,  # 6: three
    2.0,  # 7: four      ← 增加权重（易混淆）
    1.0   # 8: nothing
])

criterion = nn.CrossEntropyLoss(weight=class_weights.to(device))
```

---

### 3. 训练策略改进

#### A. 使用更长的训练时间

```python
# 增加 epoch 数
num_epochs = 200  # 原来可能是 50-100

# 使用学习率衰减
scheduler = torch.optim.lr_scheduler.CosineAnnealingLR(
    optimizer,
    T_max=num_epochs,
    eta_min=1e-6
)

# 或使用 ReduceLROnPlateau
scheduler = torch.optim.lr_scheduler.ReduceLROnPlateau(
    optimizer,
    mode='max',
    factor=0.5,
    patience=10,
    verbose=True
)
```

#### B. 使用集成学习（Ensemble）

训练多个模型，推理时取平均或投票：

```python
# 训练3个不同初始化的模型
models = []
for i in range(3):
    model = GestureNetV2(num_classes=9)
    # 训练...
    models.append(model)

# 推理时集成
def ensemble_predict(models, input_image):
    outputs = []
    for model in models:
        model.eval()
        with torch.no_grad():
            output = model(input_image)
            outputs.append(output)

    # 平均logits
    avg_output = torch.mean(torch.stack(outputs), dim=0)
    return avg_output
```

---

## 阶段三：训练与评估流程

### 1. 完整训练流程

```bash
# 步骤1：采集真实数据
python collect_training_data.py --port COM17
# 建议采集：每个手势 500-1000 张

# 步骤2：（可选）数据增强
python augment_dataset.py --input dataset_real --output dataset_real_aug --factor 5

# 步骤3：合并新旧数据集
# 将 dataset_real_aug 和原 dataset 合并

# 步骤4：训练改进模型
python train_model_v2.py \
    --dataset dataset_combined \
    --model-type v2 \
    --epochs 200 \
    --batch-size 32 \
    --use-focal-loss \
    --augment-online

# 步骤5：评估模型
python evaluate_model.py --model best_model_v2.pth --test-dir dataset_real/test

# 步骤6：转换为 ONNX
python export_to_onnx.py --model best_model_v2.pth --output gesture_model_v2.onnx

# 步骤7：使用 X-CUBE-AI 转换

# 步骤8：部署到 STM32 并测试
python debug_inference.py --port COM17 --frames 200 --onnx-model gesture_model_v2.onnx
```

### 2. 评估指标

创建详细评估工具：

```python
from sklearn.metrics import classification_report, confusion_matrix
import seaborn as sns
import matplotlib.pyplot as plt

def evaluate_model(model, test_loader, class_names):
    model.eval()

    all_preds = []
    all_labels = []

    with torch.no_grad():
        for images, labels in test_loader:
            outputs = model(images)
            _, preds = torch.max(outputs, 1)

            all_preds.extend(preds.cpu().numpy())
            all_labels.extend(labels.cpu().numpy())

    # 分类报告
    print("=" * 60)
    print("分类报告")
    print("=" * 60)
    print(classification_report(all_labels, all_preds, target_names=class_names))

    # 混淆矩阵
    cm = confusion_matrix(all_labels, all_preds)

    plt.figure(figsize=(12, 10))
    sns.heatmap(cm, annot=True, fmt='d', cmap='Blues',
                xticklabels=class_names, yticklabels=class_names)
    plt.title('混淆矩阵')
    plt.ylabel('真实标签')
    plt.xlabel('预测标签')
    plt.savefig('confusion_matrix_v2.png', dpi=150, bbox_inches='tight')
    print("\n✓ 混淆矩阵已保存: confusion_matrix_v2.png")

    # 重点关注易混淆对
    print("\n" + "=" * 60)
    print("易混淆手势对分析")
    print("=" * 60)

    confusion_pairs = [
        (1, 7, "palm vs four"),
        (5, 4, "point vs v_sign")
    ]

    for i, j, name in confusion_pairs:
        correct_i_as_i = cm[i, i]
        wrong_i_as_j = cm[i, j]
        correct_j_as_j = cm[j, j]
        wrong_j_as_i = cm[j, i]

        total_i = correct_i_as_i + wrong_i_as_j
        total_j = correct_j_as_j + wrong_j_as_i

        print(f"\n{name}:")
        print(f"  {class_names[i]}: 正确 {correct_i_as_i}/{total_i} ({correct_i_as_i/total_i*100:.1f}%), "
              f"误判为 {class_names[j]}: {wrong_i_as_j} ({wrong_i_as_j/total_i*100:.1f}%)")
        print(f"  {class_names[j]}: 正确 {correct_j_as_j}/{total_j} ({correct_j_as_j/total_j*100:.1f}%), "
              f"误判为 {class_names[i]}: {wrong_j_as_i} ({wrong_j_as_i/total_j*100:.1f}%)")

    return cm
```

---

## 📅 实施时间表

### 第1周：数据采集
- **Day 1-2**: 采集易混淆手势（palm, four, v_sign, point）各500张
- **Day 3-4**: 采集其他手势各300张
- **Day 5**: 数据清洗和质量检查

### 第2周：模型训练
- **Day 1**: 设置训练脚本，使用 GestureNetV2
- **Day 2-4**: 训练多个模型（不同超参数）
- **Day 5**: 模型评估和选择

### 第3周：优化和部署
- **Day 1**: 转换为 ONNX 和 X-CUBE-AI
- **Day 2-3**: 部署到 STM32 并测试
- **Day 4-5**: 调整和优化

---

## 🎯 预期改进效果

| 项目 | 当前 | 目标 | 实现方法 |
|------|------|------|----------|
| **palm/four 区分** | 50% 混淆 | <10% | 采集更多角度数据 + 增大模型 |
| **point/v_sign 区分** | 43% 混淆 | <10% | 采集更多角度数据 + 增大模型 |
| **整体准确率** | ~70% | >90% | 真实数据 + 改进模型 + 更长训练 |
| **STM32-PC 一致性** | 72% | >95% | 使用更鲁棒的模型架构 |
| **推理时间** | ~20ms | ~50ms | 可接受（50Hz → 20Hz） |
| **模型大小** | ~15KB | ~80KB | 可接受（STM32H7 有充足空间） |

---

## 🔍 故障排除

### 问题1：采集数据后准确率仍然很低

**可能原因**:
- 数据多样性不足
- 手势定义不清晰（palm vs four 拇指位置）

**解决方案**:
1. 检查采集的数据是否包含足够的角度变化
2. 重新定义手势标准，拍摄参考图
3. 增加采集数量到 1000+ 每个手势

### 问题2：模型在训练集上很好，但实际部署很差

**可能原因**:
- 过拟合
- 训练集和测试集分布不一致

**解决方案**:
1. 增加 Dropout (0.3 → 0.5)
2. 使用更多数据增强
3. 确保训练集包含真实部署环境的图像

### 问题3：STM32 推理时间过长

**可能原因**:
- 模型太大

**解决方案**:
1. 减少最后一层卷积的通道数 (256 → 128)
2. 使用模型剪枝（Pruning）
3. 降低输入分辨率 (64x64 → 48x48)

---

## ✅ 检查清单

开始之前，确认：

- [ ] STM32 可以正常采集图像（MODE_COLLECTING）
- [ ] 串口通信稳定（921600 波特率）
- [ ] Python 环境已安装所有依赖
- [ ] 有足够的存储空间（至少 5GB 用于数据集）
- [ ] 准备好参考图片（标准手势定义）

采集数据时：

- [ ] 每个手势至少 300 张（推荐 500+）
- [ ] 包含至少 5 个不同角度
- [ ] 包含至少 3 个不同距离
- [ ] palm 和 four 的拇指位置明显区分
- [ ] point 和 v_sign 的手指数量明显区分

训练模型时：

- [ ] 使用真实采集的数据
- [ ] 训练至少 100 个 epoch
- [ ] 监控验证集准确率，避免过拟合
- [ ] 保存最佳模型（验证集准确率最高）

部署测试时：

- [ ] 使用 debug_inference.py 进行 ONNX 对比
- [ ] STM32-PC 一致性 > 80%
- [ ] 在所有 9 个手势上测试
- [ ] 记录混淆矩阵，分析剩余问题

---

## 📞 需要帮助？

如果在实施过程中遇到问题：

1. **数据采集问题**: 检查 collect_training_data.py 的输出日志
2. **训练问题**: 查看训练曲线，检查是否过拟合/欠拟合
3. **部署问题**: 使用 debug_inference.py 对比 STM32 和 PC 结果
4. **性能问题**: 使用 STM32 的性能分析工具测量各层耗时

祝改进顺利！🚀
