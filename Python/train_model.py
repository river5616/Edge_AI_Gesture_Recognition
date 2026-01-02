#!/usr/bin/env python3
"""
STM32 OV2640手势识别 - CNN模型训练脚本
使用PyTorch训练轻量级CNN模型用于手势识别
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

# 手势类别
GESTURE_NAMES = {
    0: "fist", 1: "palm", 2: "thumbs_up", 3: "ok", 4: "v_sign",
    5: "point", 6: "three", 7: "four", 8: "nothing"
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


class DepthwiseSeparableConv(nn.Module):
    """深度可分离卷积"""

    def __init__(self, in_channels, out_channels, kernel_size=3, stride=1, padding=1):
        super(DepthwiseSeparableConv, self).__init__()

        self.depthwise = nn.Conv2d(in_channels, in_channels, kernel_size=kernel_size,
                                   stride=stride, padding=padding, groups=in_channels, bias=False)
        self.pointwise = nn.Conv2d(in_channels, out_channels, kernel_size=1, bias=False)
        self.bn = nn.BatchNorm2d(out_channels)
        self.relu = nn.ReLU6(inplace=True)

    def forward(self, x):
        x = self.depthwise(x)
        x = self.pointwise(x)
        x = self.bn(x)
        x = self.relu(x)
        return x


class GestureNet(nn.Module):
    """轻量级手势识别网络"""

    def __init__(self, num_classes=9):
        super(GestureNet, self).__init__()

        # Input: 64x64x1
        self.conv1 = nn.Sequential(
            nn.Conv2d(1, 16, kernel_size=3, stride=2, padding=1, bias=False),  # 32x32x16
            nn.BatchNorm2d(16),
            nn.ReLU6(inplace=True)
        )

        self.dw_conv1 = DepthwiseSeparableConv(16, 32)  # 32x32x32
        self.pool1 = nn.MaxPool2d(2, 2)                  # 16x16x32

        self.dw_conv2 = DepthwiseSeparableConv(32, 64)  # 16x16x64
        self.pool2 = nn.MaxPool2d(2, 2)                  # 8x8x64

        self.dw_conv3 = DepthwiseSeparableConv(64, 128) # 8x8x128

        self.global_avg_pool = nn.AdaptiveAvgPool2d(1)  # 1x1x128

        self.fc = nn.Sequential(
            nn.Dropout(0.5),
            nn.Linear(128, 64),
            nn.ReLU(inplace=True),
            nn.Dropout(0.3),
            nn.Linear(64, num_classes)
        )

    def forward(self, x):
        x = self.conv1(x)
        x = self.dw_conv1(x)
        x = self.pool1(x)
        x = self.dw_conv2(x)
        x = self.pool2(x)
        x = self.dw_conv3(x)
        x = self.global_avg_pool(x)
        x = x.view(x.size(0), -1)
        x = self.fc(x)
        return x


def load_dataset(dataset_dir):
    """
    加载数据集

    Args:
        dataset_dir: 数据集目录

    Returns:
        image_paths: 图像路径列表
        labels: 标签列表
    """
    image_paths = []
    labels = []

    for gesture_id in range(9):
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

    return image_paths, labels


def get_data_transforms(augment=True):
    """
    获取数据变换

    Args:
        augment: 是否进行数据增强

    Returns:
        train_transform, val_transform
    """
    if augment:
        train_transform = transforms.Compose([
            transforms.Resize((64, 64)),
            transforms.RandomRotation(30),
            transforms.RandomAffine(0, translate=(0.1, 0.1)),
            transforms.ColorJitter(brightness=0.2),
            transforms.ToTensor(),
            transforms.Normalize(mean=[0.5], std=[0.5])
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

    for images, labels in dataloader:
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

    val_loss = running_loss / len(dataloader)
    val_acc = 100. * correct / total

    return val_loss, val_acc


def plot_confusion_matrix(model, dataloader, device, save_path="confusion_matrix.png"):
    """绘制混淆矩阵"""
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
    plt.figure(figsize=(9, 8))
    sns.heatmap(cm, annot=True, fmt='d', cmap='Blues',
                xticklabels=[GESTURE_NAMES[i] for i in range(9)],
                yticklabels=[GESTURE_NAMES[i] for i in range(9)])
    plt.ylabel('True Label')
    plt.xlabel('Predicted Label')
    plt.title('Confusion Matrix')
    plt.tight_layout()
    plt.savefig(save_path)
    plt.close()

    print(f"混淆矩阵已保存到: {save_path}")

    # 打印分类报告
    print("\n分类报告:")
    print(classification_report(all_labels, all_preds,
                                target_names=[GESTURE_NAMES[i] for i in range(9)]))


def main():
    # 超参数
    DATASET_DIR = "dataset"
    BATCH_SIZE = 32
    LEARNING_RATE = 0.001
    NUM_EPOCHS = 100
    PATIENCE = 15  # 早停耐心值

    # 设备
    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    print(f"使用设备: {device}")

    # 加载数据集
    print("加载数据集...")
    image_paths, labels = load_dataset(DATASET_DIR)

    if len(image_paths) == 0:
        print("错误: 数据集为空！")
        return

    # 划分数据集
    train_paths, temp_paths, train_labels, temp_labels = train_test_split(
        image_paths, labels, test_size=0.3, random_state=42, stratify=labels
    )

    val_paths, test_paths, val_labels, test_labels = train_test_split(
        temp_paths, temp_labels, test_size=0.5, random_state=42, stratify=temp_labels
    )

    print(f"训练集: {len(train_paths)}, 验证集: {len(val_paths)}, 测试集: {len(test_paths)}")

    # 数据变换
    train_transform, val_transform = get_data_transforms(augment=True)

    # 创建数据集
    train_dataset = GestureDataset(train_paths, train_labels, train_transform)
    val_dataset = GestureDataset(val_paths, val_labels, val_transform)
    test_dataset = GestureDataset(test_paths, test_labels, val_transform)

    # 创建数据加载器
    train_loader = DataLoader(train_dataset, batch_size=BATCH_SIZE, shuffle=True, num_workers=0)
    val_loader = DataLoader(val_dataset, batch_size=BATCH_SIZE, shuffle=False, num_workers=0)
    test_loader = DataLoader(test_dataset, batch_size=BATCH_SIZE, shuffle=False, num_workers=0)

    # 创建模型
    print("创建模型...")
    model = GestureNet(num_classes=9).to(device)

    # 打印模型信息
    total_params = sum(p.numel() for p in model.parameters())
    print(f"模型参数量: {total_params:,}")

    # 损失函数和优化器
    criterion = nn.CrossEntropyLoss()
    optimizer = optim.Adam(model.parameters(), lr=LEARNING_RATE)
    scheduler = optim.lr_scheduler.ReduceLROnPlateau(optimizer, mode='min', factor=0.5, patience=5, verbose=True)

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
        scheduler.step(val_loss)

        print(f"Epoch [{epoch+1}/{NUM_EPOCHS}] "
              f"Train Loss: {train_loss:.4f}, Train Acc: {train_acc:.2f}% | "
              f"Val Loss: {val_loss:.4f}, Val Acc: {val_acc:.2f}%")

        # 保存最佳模型
        if val_acc > best_val_acc:
            best_val_acc = val_acc
            torch.save(model.state_dict(), "best_model.pth")
            print(f"✓ 保存最佳模型 (Val Acc: {val_acc:.2f}%)")
            patience_counter = 0
        else:
            patience_counter += 1

        # 早停
        if patience_counter >= PATIENCE:
            print(f"\n早停触发! (Patience: {PATIENCE})")
            break

    # 加载最佳模型
    model.load_state_dict(torch.load("best_model.pth"))

    # 测试
    print("\n测试最佳模型...")
    test_loss, test_acc = validate(model, test_loader, criterion, device)
    print(f"Test Loss: {test_loss:.4f}, Test Acc: {test_acc:.2f}%")

    # 绘制训练曲线
    plt.figure(figsize=(12, 4))

    plt.subplot(1, 2, 1)
    plt.plot(train_losses, label='Train Loss')
    plt.plot(val_losses, label='Val Loss')
    plt.xlabel('Epoch')
    plt.ylabel('Loss')
    plt.legend()
    plt.title('Training and Validation Loss')

    plt.subplot(1, 2, 2)
    plt.plot(train_accs, label='Train Acc')
    plt.plot(val_accs, label='Val Acc')
    plt.xlabel('Epoch')
    plt.ylabel('Accuracy (%)')
    plt.legend()
    plt.title('Training and Validation Accuracy')

    plt.tight_layout()
    plt.savefig("training_curves.png")
    print("训练曲线已保存到: training_curves.png")

    # 绘制混淆矩阵
    plot_confusion_matrix(model, test_loader, device)

    # 导出为ONNX (用于X-CUBE-AI)
    print("\n导出ONNX模型...")
    dummy_input = torch.randn(1, 1, 64, 64).to(device)
    torch.onnx.export(model, dummy_input, "gesture_model.onnx",
                      export_params=True,
                      opset_version=11,
                      do_constant_folding=True,
                      input_names=['input'],
                      output_names=['output'],
                      dynamic_axes={'input': {0: 'batch_size'},
                                    'output': {0: 'batch_size'}})
    print("✓ ONNX模型已保存到: gesture_model.onnx")

    print("\n训练完成!")


if __name__ == "__main__":
    main()
