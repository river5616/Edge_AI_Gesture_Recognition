#!/usr/bin/env python3
"""
测试 PyTorch 模型在 PC 端的推理结果
"""

import torch
import torch.nn as nn
import numpy as np
from PIL import Image
import sys

# 手势类别 - 7个手势精简版
GESTURE_NAMES = {
    0: "fist", 1: "palm", 2: "thumbs_up", 3: "ok",
    4: "gun", 5: "call_me", 6: "nothing"
}

# 定义深度可分离卷积（与训练时相同）
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

    def __init__(self, num_classes=7):
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

def preprocess_image(image_path):
    """预处理图像（与 STM32 端完全一致）"""
    # 读取图像并转为灰度
    img = Image.open(image_path).convert('L')

    # 确保尺寸为 64x64
    if img.size != (64, 64):
        img = img.resize((64, 64))

    # 转为 numpy 数组
    img_array = np.array(img, dtype=np.float32)

    # 归一化：(x / 255.0 - 0.5) / 0.5  映射到 [-1, 1]
    normalized = (img_array / 255.0 - 0.5) / 0.5

    # 调整维度为 (1, 1, 64, 64) - (batch, channel, height, width)
    input_tensor = torch.from_numpy(normalized).unsqueeze(0).unsqueeze(0).float()

    return input_tensor, img_array

def run_inference(model, input_tensor):
    """运行推理"""
    model.eval()
    with torch.no_grad():
        outputs = model(input_tensor)

        # 获取输出
        output = outputs[0].numpy()

        # 找到最大值的索引
        predicted_class = np.argmax(output)
        raw_score = output[predicted_class]

        return predicted_class, raw_score, output

def main():
    # 加载模型
    print("加载 PyTorch 模型...")
    try:
        model = GestureNet(num_classes=9)
        checkpoint = torch.load("best_model.pth", map_location=torch.device('cpu'), weights_only=False)
        model.load_state_dict(checkpoint)
        model.eval()
        print(f"✓ 模型已加载")
        print()
    except Exception as e:
        print(f"✗ 加载模型失败: {e}")
        return

    # 测试图像列表
    test_images = [
        ("训练集 - fist_0001", "dataset/0_fist/fist_0001.png", 0),
        ("训练集 - fist_0010", "dataset/0_fist/fist_0010.png", 0),
        ("训练集 - point_0001", "dataset/5_point/point_0001.png", 5),
        ("实际采集 - frame_0002", "debug_inference/images/frame_0002_point_conf3.1.png", 0),
    ]

    print("=" * 80)
    for name, image_path, expected_class in test_images:
        try:
            # 预处理
            input_tensor, raw_image = preprocess_image(image_path)

            # 推理
            predicted_class, raw_score, all_outputs = run_inference(model, input_tensor)

            # 显示结果
            correct = "✓" if predicted_class == expected_class else "✗"
            print(f"\n{correct} {name}")
            print(f"  路径: {image_path}")
            print(f"  期望: {expected_class} ({GESTURE_NAMES[expected_class]})")
            print(f"  预测: {predicted_class} ({GESTURE_NAMES[predicted_class]}) - 原始分数: {raw_score:.4f}")
            print(f"  图像均值: {np.mean(raw_image):.2f}")

            # 显示所有类别的输出
            print(f"  所有输出 (logits):")
            sorted_indices = np.argsort(all_outputs)[::-1]  # 从大到小排序
            for i in sorted_indices:
                marker = " ← MAX" if i == predicted_class else ""
                print(f"    {i} ({GESTURE_NAMES[i]:12s}): {all_outputs[i]:8.4f}{marker}")

            print("-" * 80)

        except Exception as e:
            print(f"✗ {name} - 错误: {e}")
            print("-" * 80)

    print("\n" + "=" * 80)
    print("测试完成")

if __name__ == "__main__":
    main()
