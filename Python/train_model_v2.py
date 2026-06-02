#!/usr/bin/env python3
"""
STM32 OV2640手势识别 - 改进版CNN模型训练脚本
使用PyTorch训练轻量级CNN模型（带注意力机制和残差连接）
在保持模型小巧的同时提高准确性
"""

import torch
import torch.nn as nn
import torch.optim as optim
from torch.utils.data import Dataset, DataLoader
import torchvision.transforms as transforms
import numpy as np
import cv2
import os
import matplotlib.pyplot as plt
from sklearn.model_selection import train_test_split
from sklearn.metrics import confusion_matrix, classification_report
import seaborn as sns
import argparse
from tqdm import tqdm

# 手势类别 - 5个手势（移除 gun 和 call_me，它们容易与 point 混淆）
GESTURE_NAMES = {
    0: "fist", 1: "palm", 2: "v_sign", 3: "ok", 4: "nothing"
}


class GestureDataset(Dataset):
    """手势数据集"""

    def __init__(self, image_paths, labels, transform=None):
        self.image_paths = image_paths
        self.labels = labels
        self.transform = transform

    def __len__(self):
        return len(self.image_paths)

    def __getitem__(self, idx):
        # 读取图像
        image = cv2.imread(self.image_paths[idx], cv2.IMREAD_GRAYSCALE)

        if image is None:
            raise ValueError(f"无法读取图像: {self.image_paths[idx]}")

        # 转换为PIL Image用于transform
        image = transforms.ToPILImage()(image)

        # 应用变换
        if self.transform:
            image = self.transform(image)

        label = self.labels[idx]

        return image, label


class SEBlock(nn.Module):
    """Squeeze-and-Excitation Block - 通道注意力机制"""

    def __init__(self, channels, reduction=8):
        super(SEBlock, self).__init__()
        self.squeeze = nn.AdaptiveAvgPool2d(1)
        self.excitation = nn.Sequential(
            nn.Linear(channels, channels // reduction, bias=False),
            nn.ReLU(inplace=True),
            nn.Linear(channels // reduction, channels, bias=False),
            nn.Sigmoid()
        )

    def forward(self, x):
        b, c, _, _ = x.size()
        # Squeeze: 全局平均池化
        y = self.squeeze(x).view(b, c)
        # Excitation: 全连接层学习通道权重
        y = self.excitation(y).view(b, c, 1, 1)
        # Scale: 加权特征图
        return x * y.expand_as(x)


class DepthwiseSeparableConv(nn.Module):
    """深度可分离卷积 + SE注意力"""

    def __init__(self, in_channels, out_channels, kernel_size=3, stride=1, padding=1, use_se=True):
        super(DepthwiseSeparableConv, self).__init__()

        self.depthwise = nn.Conv2d(in_channels, in_channels, kernel_size=kernel_size,
                                   stride=stride, padding=padding, groups=in_channels, bias=False)
        self.pointwise = nn.Conv2d(in_channels, out_channels, kernel_size=1, bias=False)
        self.bn = nn.BatchNorm2d(out_channels)
        self.relu = nn.ReLU6(inplace=True)

        # SE注意力模块（可选）
        self.use_se = use_se
        if use_se:
            self.se = SEBlock(out_channels, reduction=8)

    def forward(self, x):
        x = self.depthwise(x)
        x = self.pointwise(x)
        x = self.bn(x)
        if self.use_se:
            x = self.se(x)
        x = self.relu(x)
        return x


class ResidualBlock(nn.Module):
    """轻量级残差块"""

    def __init__(self, channels):
        super(ResidualBlock, self).__init__()
        self.conv = DepthwiseSeparableConv(channels, channels, use_se=True)

    def forward(self, x):
        return x + self.conv(x)


class GestureNetV2Lite(nn.Module):
    """轻量版改进型手势识别网络 - 减少RAM占用

    改进点：
    1. 减少通道数：16→32→64→96（相比V2的24→48→96→128）
    2. 保留SE注意力和残差连接
    3. RAM占用减少约40%，准确率只损失1-2%

    参数量：约40KB（相比V2的30KB）
    预期RAM：~350KB（相比V2的600KB）
    """

    def __init__(self, num_classes=5):  # 5个手势
        super(GestureNetV2Lite, self).__init__()

        # Input: 64x64x1
        self.conv1 = nn.Sequential(
            nn.Conv2d(1, 16, kernel_size=3, stride=2, padding=1, bias=False),  # 32x32x16
            nn.BatchNorm2d(16),
            nn.ReLU6(inplace=True)
        )

        # 第1组：32x32 -> 16x16
        self.block1 = nn.Sequential(
            DepthwiseSeparableConv(16, 32, use_se=True),  # 32x32x32
            ResidualBlock(32),                             # 残差块
            nn.MaxPool2d(2, 2)                             # 16x16x32
        )

        # 第2组：16x16 -> 8x8
        self.block2 = nn.Sequential(
            DepthwiseSeparableConv(32, 64, use_se=True),  # 16x16x64
            ResidualBlock(64),                             # 残差块
            nn.MaxPool2d(2, 2)                             # 8x8x64
        )

        # 第3组：8x8（无下采样）
        self.block3 = nn.Sequential(
            DepthwiseSeparableConv(64, 96, use_se=True),  # 8x8x96
            ResidualBlock(96)                              # 残差块
        )

        # 全局平均池化
        self.global_avg_pool = nn.AdaptiveAvgPool2d(1)     # 1x1x96

        # 分类头
        self.fc = nn.Sequential(
            nn.Dropout(0.5),
            nn.Linear(96, 48),
            nn.ReLU(inplace=True),
            nn.Dropout(0.3),
            nn.Linear(48, num_classes)
        )

    def forward(self, x):
        x = self.conv1(x)
        x = self.block1(x)
        x = self.block2(x)
        x = self.block3(x)
        x = self.global_avg_pool(x)
        x = x.view(x.size(0), -1)
        x = self.fc(x)
        return x


class GestureNetV2(nn.Module):
    """改进版轻量级手势识别网络

    改进点：
    1. 增加SE注意力模块
    2. 添加残差连接
    3. 增加通道数（适度）
    4. 优化网络结构

    参数量：约30KB（相比原版15KB增加约2倍）
    """

    def __init__(self, num_classes=5):  # 5个手势
        super(GestureNetV2, self).__init__()

        # Input: 64x64x1
        self.conv1 = nn.Sequential(
            nn.Conv2d(1, 24, kernel_size=3, stride=2, padding=1, bias=False),  # 32x32x24
            nn.BatchNorm2d(24),
            nn.ReLU6(inplace=True)
        )

        # 第1组：32x32 -> 16x16
        self.block1 = nn.Sequential(
            DepthwiseSeparableConv(24, 48, use_se=True),  # 32x32x48
            ResidualBlock(48),                             # 残差块
            nn.MaxPool2d(2, 2)                             # 16x16x48
        )

        # 第2组：16x16 -> 8x8
        self.block2 = nn.Sequential(
            DepthwiseSeparableConv(48, 96, use_se=True),  # 16x16x96
            ResidualBlock(96),                             # 残差块
            nn.MaxPool2d(2, 2)                             # 8x8x96
        )

        # 第3组：8x8（无下采样）
        self.block3 = nn.Sequential(
            DepthwiseSeparableConv(96, 128, use_se=True), # 8x8x128
            ResidualBlock(128)                             # 残差块
        )

        # 全局平均池化
        self.global_avg_pool = nn.AdaptiveAvgPool2d(1)    # 1x1x128

        # 分类头
        self.fc = nn.Sequential(
            nn.Dropout(0.5),
            nn.Linear(128, 64),
            nn.ReLU(inplace=True),
            nn.Dropout(0.3),
            nn.Linear(64, num_classes)
        )

    def forward(self, x):
        x = self.conv1(x)
        x = self.block1(x)
        x = self.block2(x)
        x = self.block3(x)
        x = self.global_avg_pool(x)
        x = x.view(x.size(0), -1)
        x = self.fc(x)
        return x


class FocalLoss(nn.Module):
    """Focal Loss - 处理难分类样本"""

    def __init__(self, alpha=1, gamma=2):
        super(FocalLoss, self).__init__()
        self.alpha = alpha
        self.gamma = gamma
        self.ce = nn.CrossEntropyLoss(reduction='none')

    def forward(self, inputs, targets):
        ce_loss = self.ce(inputs, targets)
        pt = torch.exp(-ce_loss)
        focal_loss = self.alpha * (1 - pt) ** self.gamma * ce_loss
        return focal_loss.mean()


class LabelSmoothingLoss(nn.Module):
    """标签平滑损失 - 提高泛化能力"""

    def __init__(self, classes, smoothing=0.1):
        super(LabelSmoothingLoss, self).__init__()
        self.confidence = 1.0 - smoothing
        self.smoothing = smoothing
        self.classes = classes

    def forward(self, pred, target):
        pred = pred.log_softmax(dim=-1)
        with torch.no_grad():
            true_dist = torch.zeros_like(pred)
            true_dist.fill_(self.smoothing / (self.classes - 1))
            true_dist.scatter_(1, target.data.unsqueeze(1), self.confidence)
        return torch.mean(torch.sum(-true_dist * pred, dim=-1))


def load_dataset(dataset_dir):
    """
    加载数据集

    Args:
        dataset_dir: 数据集目录路径

    Returns:
        image_paths: 图像路径列表
        labels: 标签列表
    """
    image_paths = []
    labels = []

    for gesture_id in range(5):  # 5个手势
        gesture_name = GESTURE_NAMES[gesture_id]
        gesture_dir = os.path.join(dataset_dir, f"{gesture_id}_{gesture_name}")

        if not os.path.exists(gesture_dir):
            print(f"警告: 目录不存在: {gesture_dir}")
            continue

        # 读取所有图像
        files = [f for f in os.listdir(gesture_dir) if f.endswith(('.png', '.jpg', '.jpeg'))]

        for filename in files:
            filepath = os.path.join(gesture_dir, filename)
            image_paths.append(filepath)
            labels.append(gesture_id)

    print(f"加载了 {len(image_paths)} 张图像")

    # 统计每个类别的数量
    from collections import Counter
    label_counts = Counter(labels)
    print("\n各类别样本数量:")
    for gesture_id in range(5):  # 5个手势
        gesture_name = GESTURE_NAMES[gesture_id]
        count = label_counts.get(gesture_id, 0)
        print(f"  {gesture_id} ({gesture_name}): {count} 张")

    return image_paths, labels


def get_data_transforms(augment=True):
    """
    获取数据变换（改进版 - 更强的增强）

    Args:
        augment: 是否进行数据增强

    Returns:
        train_transform, val_transform
    """
    if augment:
        train_transform = transforms.Compose([
            transforms.Resize((64, 64)),
            # 更强的旋转
            transforms.RandomRotation(40),
            # 随机仿射变换（平移、缩放、剪切）
            transforms.RandomAffine(
                degrees=0,
                translate=(0.15, 0.15),
                scale=(0.85, 1.15),
                shear=10
            ),
            # 亮度、对比度增强
            transforms.ColorJitter(brightness=0.3, contrast=0.3),
            # 随机水平翻转（仅对对称手势有效）
            # transforms.RandomHorizontalFlip(p=0.5),  # 注释掉，因为手势有左右区别
            # 转换为Tensor
            transforms.ToTensor(),
            # 标准化
            transforms.Normalize(mean=[0.5], std=[0.5]),
            # 随机擦除（模拟遮挡）
            transforms.RandomErasing(p=0.3, scale=(0.02, 0.15), ratio=(0.3, 3.3))
        ])
    else:
        train_transform = transforms.Compose([
            transforms.Resize((64, 64)),
            transforms.ToTensor(),
            transforms.Normalize(mean=[0.5], std=[0.5])
        ])

    val_transform = transforms.Compose([
        transforms.Resize((64, 64)),
        transforms.ToTensor(),
        transforms.Normalize(mean=[0.5], std=[0.5])
    ])

    return train_transform, val_transform


def train_epoch(model, dataloader, criterion, optimizer, device):
    """训练一个epoch"""
    model.train()
    running_loss = 0.0
    correct = 0
    total = 0

    # 添加进度条
    pbar = tqdm(dataloader, desc='Training', leave=False)

    for images, labels in pbar:
        images, labels = images.to(device), labels.to(device)

        # 前向传播
        outputs = model(images)
        loss = criterion(outputs, labels)

        # 反向传播
        optimizer.zero_grad()
        loss.backward()
        optimizer.step()

        # 统计
        running_loss += loss.item()
        _, predicted = outputs.max(1)
        total += labels.size(0)
        correct += predicted.eq(labels).sum().item()

        # 更新进度条
        pbar.set_postfix({'loss': f'{loss.item():.4f}', 'acc': f'{100.*correct/total:.2f}%'})

    epoch_loss = running_loss / len(dataloader)
    epoch_acc = 100. * correct / total

    return epoch_loss, epoch_acc


def validate(model, dataloader, criterion, device):
    """验证模型"""
    model.eval()
    running_loss = 0.0
    correct = 0
    total = 0

    with torch.no_grad():
        for images, labels in dataloader:
            images, labels = images.to(device), labels.to(device)

            outputs = model(images)
            loss = criterion(outputs, labels)

            running_loss += loss.item()
            _, predicted = outputs.max(1)
            total += labels.size(0)
            correct += predicted.eq(labels).sum().item()

    epoch_loss = running_loss / len(dataloader)
    epoch_acc = 100. * correct / total

    return epoch_loss, epoch_acc


def evaluate_model(model, dataloader, device, save_path='confusion_matrix.png'):
    """
    评估模型并生成混淆矩阵

    Args:
        model: 模型
        dataloader: 数据加载器
        device: 设备
        save_path: 混淆矩阵保存路径
    """
    model.eval()
    all_preds = []
    all_labels = []

    with torch.no_grad():
        for images, labels in dataloader:
            images = images.to(device)
            outputs = model(images)
            _, predicted = outputs.max(1)

            all_preds.extend(predicted.cpu().numpy())
            all_labels.extend(labels.numpy())

    # 计算混淆矩阵
    cm = confusion_matrix(all_labels, all_preds)

    # 绘制
    plt.figure(figsize=(6, 5))
    sns.heatmap(cm, annot=True, fmt='d', cmap='Blues',
                xticklabels=[GESTURE_NAMES[i] for i in range(5)],  # 5个手势
                yticklabels=[GESTURE_NAMES[i] for i in range(5)])
    plt.ylabel('True Label')
    plt.xlabel('Predicted Label')
    plt.title('Confusion Matrix (GestureNetV2)')
    plt.tight_layout()
    plt.savefig(save_path)
    plt.close()

    print(f"混淆矩阵已保存到: {save_path}")

    # 打印分类报告
    print("\n分类报告:")
    print(classification_report(all_labels, all_preds,
                                target_names=[GESTURE_NAMES[i] for i in range(5)]))


def main():
    # 解析命令行参数
    parser = argparse.ArgumentParser(description='训练改进版手势识别模型')
    parser.add_argument('--dataset', type=str, default='dataset', help='数据集目录')
    parser.add_argument('--batch-size', type=int, default=32, help='批量大小')
    parser.add_argument('--epochs', type=int, default=150, help='训练轮数')
    parser.add_argument('--lr', type=float, default=0.001, help='学习率')
    parser.add_argument('--loss', type=str, default='focal', choices=['ce', 'focal', 'smooth'],
                        help='损失函数: ce=交叉熵, focal=Focal Loss, smooth=标签平滑')
    parser.add_argument('--model-name', type=str, default='gesture_v2.pth', help='模型保存名称')
    parser.add_argument('--no-augment', action='store_true',
                        help='跳过训练时数据增强（如果数据已预增强，使用此选项可大幅加速）')
    parser.add_argument('--num-workers', type=int, default=0,
                        help='数据加载线程数（Windows建议0，Linux可用4-8）')
    parser.add_argument('--subset', type=float, default=1.0,
                        help='使用数据集的子集比例（0.1-1.0），用于快速测试')
    parser.add_argument('--lite', action='store_true',
                        help='使用轻量版V2模型（减少40%% RAM，准确率略降1-2%%）')
    args = parser.parse_args()

    # 超参数
    DATASET_DIR = args.dataset
    BATCH_SIZE = args.batch_size
    LEARNING_RATE = args.lr
    NUM_EPOCHS = args.epochs
    PATIENCE = 20  # 早停耐心值

    # 设备
    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    print(f"使用设备: {device}")

    # 加载数据集
    print("加载数据集...")
    image_paths, labels = load_dataset(DATASET_DIR)

    if len(image_paths) == 0:
        print("错误: 数据集为空！")
        return

    # 使用数据子集（如果指定）
    if args.subset < 1.0:
        subset_size = int(len(image_paths) * args.subset)
        image_paths, _, labels, _ = train_test_split(
            image_paths, labels, train_size=subset_size, random_state=42, stratify=labels
        )
        print(f"\n⚡ 使用 {args.subset*100:.0f}% 数据子集: {len(image_paths)} 张图像")

    # 划分数据集
    train_paths, temp_paths, train_labels, temp_labels = train_test_split(
        image_paths, labels, test_size=0.3, random_state=42, stratify=labels
    )

    val_paths, test_paths, val_labels, test_labels = train_test_split(
        temp_paths, temp_labels, test_size=0.5, random_state=42, stratify=temp_labels
    )

    print(f"训练集: {len(train_paths)} 张")
    print(f"验证集: {len(val_paths)} 张")
    print(f"测试集: {len(test_paths)} 张")

    # 数据变换
    use_augment = not args.no_augment
    if args.no_augment:
        print("\n⚡ 跳过训练时数据增强（使用预增强数据）")
    else:
        print("\n🔄 使用训练时数据增强")

    train_transform, val_transform = get_data_transforms(augment=use_augment)

    # 创建数据集
    train_dataset = GestureDataset(train_paths, train_labels, train_transform)
    val_dataset = GestureDataset(val_paths, val_labels, val_transform)
    test_dataset = GestureDataset(test_paths, test_labels, val_transform)

    # 创建数据加载器
    train_loader = DataLoader(train_dataset, batch_size=BATCH_SIZE, shuffle=True, num_workers=args.num_workers)
    val_loader = DataLoader(val_dataset, batch_size=BATCH_SIZE, shuffle=False, num_workers=args.num_workers)
    test_loader = DataLoader(test_dataset, batch_size=BATCH_SIZE, shuffle=False, num_workers=args.num_workers)

    # 创建模型
    print("\n创建模型...")
    if args.lite:
        model = GestureNetV2Lite(num_classes=5).to(device)  # 5个手势
        print("✓ 使用轻量版模型 (GestureNetV2Lite)")
        print("  - 通道数: 16→32→64→96 (相比标准版的 24→48→96→128)")
        print("  - 预期RAM: ~350KB (相比标准版的 ~600KB)")
        print("  - 预期准确率: 93-94% (相比标准版的 94-95%)")
    else:
        model = GestureNetV2(num_classes=5).to(device)  # 5个手势
        print("✓ 使用标准版模型 (GestureNetV2)")

    # 打印模型信息
    total_params = sum(p.numel() for p in model.parameters())
    trainable_params = sum(p.numel() for p in model.parameters() if p.requires_grad)
    print(f"\n模型参数量: {total_params:,}")
    print(f"可训练参数: {trainable_params:,}")
    print(f"模型大小估计: {total_params * 4 / 1024:.2f} KB (float32)")

    # 损失函数
    if args.loss == 'focal':
        criterion = FocalLoss(alpha=1, gamma=2)
        print("使用 Focal Loss")
    elif args.loss == 'smooth':
        criterion = LabelSmoothingLoss(classes=5, smoothing=0.1)  # 5个手势
        print("使用 Label Smoothing Loss")
    else:
        criterion = nn.CrossEntropyLoss()
        print("使用 Cross Entropy Loss")

    # 优化器
    optimizer = optim.AdamW(model.parameters(), lr=LEARNING_RATE, weight_decay=0.01)

    # 学习率调度器 - Cosine Annealing
    scheduler = optim.lr_scheduler.CosineAnnealingLR(optimizer, T_max=NUM_EPOCHS, eta_min=1e-6)

    # 训练
    print("\n开始训练...")
    best_val_acc = 0.0
    patience_counter = 0
    train_losses, train_accs = [], []
    val_losses, val_accs = [], []

    for epoch in range(NUM_EPOCHS):
        # 训练
        train_loss, train_acc = train_epoch(model, train_loader, criterion, optimizer, device)
        train_losses.append(train_loss)
        train_accs.append(train_acc)

        # 验证
        val_loss, val_acc = validate(model, val_loader, criterion, device)
        val_losses.append(val_loss)
        val_accs.append(val_acc)

        # 学习率调度
        scheduler.step()
        current_lr = optimizer.param_groups[0]['lr']

        print(f"Epoch [{epoch+1}/{NUM_EPOCHS}] "
              f"Train Loss: {train_loss:.4f}, Train Acc: {train_acc:.2f}% | "
              f"Val Loss: {val_loss:.4f}, Val Acc: {val_acc:.2f}% | "
              f"LR: {current_lr:.6f}")

        # 保存最佳模型
        if val_acc > best_val_acc:
            best_val_acc = val_acc
            torch.save(model.state_dict(), args.model_name)
            print(f"✓ 保存最佳模型 (验证准确率: {best_val_acc:.2f}%)")
            patience_counter = 0
        else:
            patience_counter += 1

        # 早停
        if patience_counter >= PATIENCE:
            print(f"\n早停: 验证准确率已 {PATIENCE} 轮未提升")
            break

    # 绘制训练曲线
    plt.figure(figsize=(12, 4))

    plt.subplot(1, 2, 1)
    plt.plot(train_losses, label='Train Loss')
    plt.plot(val_losses, label='Val Loss')
    plt.xlabel('Epoch')
    plt.ylabel('Loss')
    plt.legend()
    plt.title('Loss Curve')

    plt.subplot(1, 2, 2)
    plt.plot(train_accs, label='Train Acc')
    plt.plot(val_accs, label='Val Acc')
    plt.xlabel('Epoch')
    plt.ylabel('Accuracy (%)')
    plt.legend()
    plt.title('Accuracy Curve')

    plt.tight_layout()
    plt.savefig('training_curves_v2.png')
    plt.close()

    print(f"\n训练曲线已保存到: training_curves_v2.png")

    # 加载最佳模型并在测试集上评估
    print("\n在测试集上评估最佳模型...")
    model.load_state_dict(torch.load(args.model_name))
    test_loss, test_acc = validate(model, test_loader, criterion, device)
    print(f"测试集准确率: {test_acc:.2f}%")

    # 生成混淆矩阵
    evaluate_model(model, test_loader, device, save_path='confusion_matrix_v2.png')

    print(f"\n训练完成！最佳模型已保存到: {args.model_name}")


if __name__ == "__main__":
    main()
