#!/usr/bin/env python3
"""
测试改进版 PyTorch 模型在 PC 端的推理结果
"""

import torch
import torch.nn as nn
import numpy as np
from PIL import Image
import sys

# 手势类别 - 5个手势（移除 gun 和 call_me，它们容易与 point 混淆）
GESTURE_NAMES = {
    0: "fist", 1: "palm", 2: "v_sign", 3: "ok", 4: "nothing"
}


class SEBlock(nn.Module):
    """Squeeze-and-Excitation Block"""

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
        y = self.squeeze(x).view(b, c)
        y = self.excitation(y).view(b, c, 1, 1)
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


class GestureNetV2(nn.Module):
    """改进版轻量级手势识别网络

    参数量：约30KB
    """

    def __init__(self, num_classes=5):  # 5个手势
        super(GestureNetV2, self).__init__()

        # Input: 64x64x1
        self.conv1 = nn.Sequential(
            nn.Conv2d(1, 24, kernel_size=3, stride=2, padding=1, bias=False),
            nn.BatchNorm2d(24),
            nn.ReLU6(inplace=True)
        )

        self.block1 = nn.Sequential(
            DepthwiseSeparableConv(24, 48, use_se=True),
            ResidualBlock(48),
            nn.MaxPool2d(2, 2)
        )

        self.block2 = nn.Sequential(
            DepthwiseSeparableConv(48, 96, use_se=True),
            ResidualBlock(96),
            nn.MaxPool2d(2, 2)
        )

        self.block3 = nn.Sequential(
            DepthwiseSeparableConv(96, 128, use_se=True),
            ResidualBlock(128)
        )

        self.global_avg_pool = nn.AdaptiveAvgPool2d(1)

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


class GestureNetV2Lite(nn.Module):
    """轻量版改进型手势识别网络 - 减少RAM占用

    改进点：
    1. 减少通道数：16→32→64→96（相比V2的24→48→96→128）
    2. 保留SE注意力和残差连接
    3. RAM占用减少约40%，准确率只损失1-2%

    参数量：约18KB
    """

    def __init__(self, num_classes=5):  # 5个手势
        super(GestureNetV2Lite, self).__init__()

        # Input: 64x64x1
        self.conv1 = nn.Sequential(
            nn.Conv2d(1, 16, kernel_size=3, stride=2, padding=1, bias=False),
            nn.BatchNorm2d(16),
            nn.ReLU6(inplace=True)
        )

        self.block1 = nn.Sequential(
            DepthwiseSeparableConv(16, 32, use_se=True),
            ResidualBlock(32),
            nn.MaxPool2d(2, 2)
        )

        self.block2 = nn.Sequential(
            DepthwiseSeparableConv(32, 64, use_se=True),
            ResidualBlock(64),
            nn.MaxPool2d(2, 2)
        )

        self.block3 = nn.Sequential(
            DepthwiseSeparableConv(64, 96, use_se=True),
            ResidualBlock(96)
        )

        self.global_avg_pool = nn.AdaptiveAvgPool2d(1)

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


class GestureNet(nn.Module):
    """轻量级手势识别网络（原版）

    参数量：约15KB
    """
    def __init__(self, num_classes=5):  # 5个手势
        super(GestureNet, self).__init__()
        self.conv1 = nn.Sequential(
            nn.Conv2d(1, 16, kernel_size=3, stride=2, padding=1, bias=False),
            nn.BatchNorm2d(16),
            nn.ReLU6(inplace=True)
        )
        self.dw_conv1 = DepthwiseSeparableConv(16, 32, use_se=False)
        self.pool1 = nn.MaxPool2d(2, 2)
        self.dw_conv2 = DepthwiseSeparableConv(32, 64, use_se=False)
        self.pool2 = nn.MaxPool2d(2, 2)
        self.dw_conv3 = DepthwiseSeparableConv(64, 128, use_se=False)
        self.global_avg_pool = nn.AdaptiveAvgPool2d(1)
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


def test_model(model_path, image_path):
    """
    测试模型

    Args:
        model_path: 模型路径
        image_path: 测试图像路径
    """
    # 加载模型
    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    model = GestureNetV2(num_classes=5).to(device)  # 5个手势
    model.load_state_dict(torch.load(model_path, map_location=device))
    model.eval()

    print(f"✓ 模型加载成功: {model_path}")

    # 加载图像
    image = Image.open(image_path).convert('L')  # 转为灰度图
    image = image.resize((64, 64))

    # 预处理
    image_np = np.array(image).astype(np.float32)
    image_normalized = (image_np / 255.0 - 0.5) / 0.5

    # 转换为Tensor
    image_tensor = torch.from_numpy(image_normalized).unsqueeze(0).unsqueeze(0).to(device)

    # 推理
    with torch.no_grad():
        output = model(image_tensor)
        probabilities = torch.softmax(output, dim=1)[0]
        predicted_class = torch.argmax(output, dim=1).item()
        confidence = probabilities[predicted_class].item()

    # 打印结果
    print(f"\n预测结果:")
    print(f"  手势: {GESTURE_NAMES[predicted_class]}")
    print(f"  ID: {predicted_class}")
    print(f"  置信度: {confidence:.4f} ({confidence*100:.2f}%)")

    print(f"\n所有类别的置信度:")
    for i in range(5):  # 5个手势
        print(f"  {i} ({GESTURE_NAMES[i]:12s}): {probabilities[i]:.4f} ({probabilities[i]*100:.2f}%)")


if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("用法: python test_pytorch_model_v2.py <模型路径> <图像路径>")
        print("示例: python test_pytorch_model_v2.py gesture_v2.pth test_image.png")
        sys.exit(1)

    model_path = sys.argv[1]
    image_path = sys.argv[2]

    test_model(model_path, image_path)
