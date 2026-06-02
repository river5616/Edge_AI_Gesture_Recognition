#!/usr/bin/env python3
"""
实时推理工具 - 从 STM32 接收图像并在 PC 端实时识别显示
结合串口接收、PyTorch 推理和实时可视化
"""

import serial
import numpy as np
import cv2
import torch
import torch.nn as nn
import torchvision.transforms as transforms
import argparse
import time
from datetime import datetime
import os
from collections import deque

# 手势类别 - 7个手势
GESTURE_NAMES = {
    0: "fist", 1: "palm", 2: "thumbs_up", 3: "ok", 4: "nothing"
}

GESTURE_COLORS = {
    0: (255, 0, 0),      # 蓝色 - fist
    1: (0, 255, 0),      # 绿色 - palm
    2: (0, 0, 255),      # 红色 - thumbs_up
    3: (255, 255, 0),    # 青色 - ok
    4: (128, 128, 128)   # 灰色 - nothing
}

# ==================== 模型定义 ====================
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


class GestureNetV2(nn.Module):
    """改进版轻量级手势识别网络

    改进点：
    1. 增加SE注意力模块
    2. 添加残差连接
    3. 增加通道数（适度）
    4. 优化网络结构

    参数量：约30KB（相比原版15KB增加约2倍）
    """

    def __init__(self, num_classes=5):
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


class GestureNetV2Spatial(GestureNetV2):
    """GestureNetV2 with 2x2 spatial pooling head."""

    def __init__(self, num_classes=5):
        super().__init__(num_classes=num_classes)
        self.global_avg_pool = nn.AvgPool2d(kernel_size=4, stride=4)
        self.fc = nn.Sequential(
            nn.Dropout(0.5),
            nn.Linear(128 * 2 * 2, 64),
            nn.ReLU(inplace=True),
            nn.Dropout(0.3),
            nn.Linear(64, num_classes)
        )


class GestureNetV2Lite(nn.Module):
    """轻量版改进型手势识别网络 - 减少RAM占用

    改进点：
    1. 减少通道数：16→32→64→96（相比V2的24→48→96→128）
    2. 保留SE注意力和残差连接
    3. RAM占用减少约40%，准确率只损失1-2%

    参数量：约40KB（相比V2的30KB）
    预期RAM：~350KB（相比V2的600KB）
    """

    def __init__(self, num_classes=5):
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


class GestureNet(nn.Module):
    """轻量级手势识别网络（原版）"""
    def __init__(self, num_classes=5):
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


# ==================== 实时推理类 ====================
class RealtimeInference:
    def __init__(self, port, baudrate, model_path, save_dir="realtime_frames"):
        self.port = port
        self.baudrate = baudrate
        self.model_path = model_path
        self.save_dir = save_dir
        self.ser = None
        self.model = None
        self.device = None
        self.transform = None

        # 创建保存目录
        os.makedirs(save_dir, exist_ok=True)

        # 统计信息
        self.frame_count = 0
        self.fps_queue = deque(maxlen=30)  # 最近30帧用于计算FPS
        self.last_time = time.time()

        # 暂停标志
        self.paused = False

    def load_model(self):
        """加载 PyTorch 模型"""
        print(f"\n加载模型: {self.model_path}")

        # 检测设备
        self.device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
        print(f"使用设备: {self.device}")

        # 加载模型
        checkpoint = torch.load(self.model_path, map_location=self.device, weights_only=False)

        # 检查 checkpoint 类型
        print(f"Checkpoint 类型: {type(checkpoint)}")
        if isinstance(checkpoint, dict):
            print(f"Checkpoint 键: {list(checkpoint.keys())[:10]}")  # 显示前10个键

        # 尝试不同的模型架构
        errors = []
        for model_class in [GestureNetV2, GestureNetV2Spatial, GestureNetV2Lite, GestureNet]:
            try:
                print(f"\n尝试加载为 {model_class.__name__}...")
                model = model_class(num_classes=5)

                # 检查是否需要从嵌套字典中提取 state_dict
                if isinstance(checkpoint, dict) and 'state_dict' in checkpoint:
                    state_dict = checkpoint['state_dict']
                else:
                    state_dict = checkpoint

                model.load_state_dict(state_dict)
                self.model = model.to(self.device)
                self.model.eval()
                print(f"✓ 模型加载成功: {model_class.__name__}")
                break
            except Exception as e:
                error_msg = f"{model_class.__name__}: {str(e)}"
                errors.append(error_msg)
                print(f"  ✗ 失败: {str(e)[:100]}")
                continue

        if self.model is None:
            print("\n所有模型架构都加载失败:")
            for error in errors:
                print(f"  - {error}")
            raise ValueError("无法加载模型，请检查模型文件和架构")

        # 数据预处理（与训练时一致）
        self.transform = transforms.Compose([
            transforms.ToPILImage(),
            transforms.Resize((64, 64)),
            transforms.ToTensor(),
            transforms.Normalize(mean=[0.5], std=[0.5])
        ])

        print("✓ 模型加载完成\n")

    def connect(self):
        """连接串口"""
        try:
            self.ser = serial.Serial(self.port, self.baudrate, timeout=1)
            print(f"✓ 已连接到 {self.port}")
            time.sleep(2)
            return True
        except Exception as e:
            print(f"✗ 连接失败: {e}")
            return False

    def disconnect(self):
        """断开串口"""
        if self.ser and self.ser.is_open:
            self.ser.close()
            print("\n✓ 串口已断开")

    def read_line(self):
        """读取一行"""
        if self.ser and self.ser.is_open:
            return self.ser.readline().decode('utf-8', errors='ignore').strip()
        return ""

    def receive_image(self):
        """接收图像数据"""
        try:
            # 读取图像头部
            line = self.read_line()

            if not line.startswith("$IMG"):
                return None

            # 解析参数
            parts = line.split(',')
            if len(parts) != 4:
                return None

            width = int(parts[1])
            height = int(parts[2])
            data_length = int(parts[3])

            # 读取图像数据
            image_data = self.ser.read(data_length)

            if len(image_data) != data_length:
                return None

            # 读取结束标记
            end_marker = self.read_line()
            if not end_marker.startswith("$END"):
                return None

            # 解析图像
            expected_gray_size = width * height

            if data_length == expected_gray_size:
                # 灰度图
                image = np.frombuffer(image_data, dtype=np.uint8).reshape((height, width))
            else:
                # JPEG
                image = cv2.imdecode(np.frombuffer(image_data, dtype=np.uint8), cv2.IMREAD_GRAYSCALE)

            return image

        except Exception as e:
            print(f"✗ 接收图像失败: {e}")
            return None

    def predict(self, image):
        """使用模型预测"""
        try:
            # 预处理
            input_tensor = self.transform(image).unsqueeze(0).to(self.device)

            # 推理
            with torch.no_grad():
                outputs = self.model(input_tensor)
                probabilities = torch.softmax(outputs, dim=1)
                confidence, predicted = torch.max(probabilities, 1)

                gesture_id = predicted.item()
                conf = confidence.item()
                all_probs = probabilities.cpu().numpy()[0]

            return gesture_id, conf, all_probs

        except Exception as e:
            print(f"✗ 推理失败: {e}")
            return None, None, None

    def draw_info(self, display_image, gesture_id, confidence, all_probs, fps):
        """在图像上绘制信息"""
        h, w = display_image.shape[:2]

        # 转换为彩色图像
        if len(display_image.shape) == 2:
            display_image = cv2.cvtColor(display_image, cv2.COLOR_GRAY2BGR)

        # 获取手势名称和颜色
        gesture_name = GESTURE_NAMES.get(gesture_id, f"UNKNOWN_{gesture_id}")
        color = GESTURE_COLORS.get(gesture_id, (255, 255, 255))

        # 绘制手势名称（大字体，顶部居中）
        font = cv2.FONT_HERSHEY_SIMPLEX
        text = f"{gesture_name.upper()}"
        font_scale = 1.5
        thickness = 3
        (text_w, text_h), _ = cv2.getTextSize(text, font, font_scale, thickness)

        # 背景框
        cv2.rectangle(display_image, (0, 0), (w, text_h + 20), (0, 0, 0), -1)

        # 文字
        text_x = (w - text_w) // 2
        cv2.putText(display_image, text, (text_x, text_h + 10),
                   font, font_scale, color, thickness)

        # 绘制置信度（右上角）
        conf_text = f"{confidence*100:.1f}%"
        cv2.putText(display_image, conf_text, (w - 120, 60),
                   font, 0.8, color, 2)

        # 绘制 FPS（左下角）
        fps_text = f"FPS: {fps:.1f}"
        cv2.putText(display_image, fps_text, (10, h - 20),
                   font, 0.6, (255, 255, 255), 2)

        # 绘制帧数（右下角）
        frame_text = f"Frame: {self.frame_count}"
        cv2.putText(display_image, frame_text, (w - 150, h - 20),
                   font, 0.6, (255, 255, 255), 2)

        return display_image

    def draw_confidence_bars(self, all_probs, current_id):
        """绘制置信度条形图"""
        # 创建条形图画布
        bar_height = 50
        bar_width = 400
        margin = 20
        canvas_height = len(GESTURE_NAMES) * (bar_height + 5) + margin * 2
        canvas = np.zeros((canvas_height, bar_width, 3), dtype=np.uint8)

        # 绘制每个手势的置信度条
        for i, (gid, gname) in enumerate(GESTURE_NAMES.items()):
            y = margin + i * (bar_height + 5)

            # 置信度值
            prob = all_probs[gid] if gid < len(all_probs) else 0.0
            bar_len = int((bar_width - 150) * prob)

            # 颜色（当前预测的手势高亮）
            color = GESTURE_COLORS[gid] if gid == current_id else (100, 100, 100)

            # 绘制条形
            cv2.rectangle(canvas, (100, y), (100 + bar_len, y + bar_height - 10), color, -1)

            # 绘制边框
            cv2.rectangle(canvas, (100, y), (bar_width - 20, y + bar_height - 10), (255, 255, 255), 1)

            # 绘制手势名称
            cv2.putText(canvas, f"{gid}:{gname[:3]}", (5, y + 30),
                       cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 1)

            # 绘制百分比
            cv2.putText(canvas, f"{prob*100:.1f}%", (bar_width - 80, y + 30),
                       cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 1)

        return canvas

    def save_frame(self, image, gesture_id, confidence):
        """保存当前帧"""
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S_%f")[:-3]
        gesture_name = GESTURE_NAMES.get(gesture_id, f"unknown_{gesture_id}")
        filename = f"{timestamp}_{gesture_name}_{confidence*100:.0f}.png"
        filepath = os.path.join(self.save_dir, filename)
        cv2.imwrite(filepath, image)
        print(f"✓ 已保存: {filename}")

    def run(self):
        """运行实时推理"""
        print("\n" + "="*60)
        print("实时推理工具")
        print("="*60)
        print("\n操作说明:")
        print("  's' - 保存当前帧")
        print("  'p' - 暂停/继续")
        print("  'q' - 退出")
        print("\n" + "="*60 + "\n")

        # 创建窗口
        cv2.namedWindow('Gesture Recognition', cv2.WINDOW_NORMAL)
        cv2.namedWindow('Confidence', cv2.WINDOW_NORMAL)

        # 调整窗口大小
        cv2.resizeWindow('Gesture Recognition', 640, 640)
        cv2.resizeWindow('Confidence', 400, 400)

        try:
            while True:
                # 接收图像
                image = self.receive_image()

                if image is None:
                    time.sleep(0.01)
                    continue

                # 更新帧计数
                self.frame_count += 1

                # 计算 FPS
                current_time = time.time()
                fps = 1.0 / (current_time - self.last_time) if (current_time - self.last_time) > 0 else 0
                self.last_time = current_time
                self.fps_queue.append(fps)
                avg_fps = np.mean(self.fps_queue)

                if not self.paused:
                    # 推理
                    gesture_id, confidence, all_probs = self.predict(image)

                    if gesture_id is not None:
                        # 放大图像用于显示
                        display_image = cv2.resize(image, (640, 640), interpolation=cv2.INTER_NEAREST)

                        # 绘制信息
                        display_image = self.draw_info(display_image, gesture_id, confidence, all_probs, avg_fps)

                        # 绘制置信度条形图
                        conf_bars = self.draw_confidence_bars(all_probs, gesture_id)

                        # 显示
                        cv2.imshow('Gesture Recognition', display_image)
                        cv2.imshow('Confidence', conf_bars)

                        # 控制台输出
                        gesture_name = GESTURE_NAMES.get(gesture_id, f"UNKNOWN_{gesture_id}")
                        print(f"\rFrame {self.frame_count:04d} | {gesture_name:12s} | {confidence*100:5.1f}% | FPS: {avg_fps:5.1f}", end='')
                else:
                    # 暂停状态：只显示图像，不推理
                    display_image = cv2.resize(image, (640, 640), interpolation=cv2.INTER_NEAREST)
                    display_image = cv2.cvtColor(display_image, cv2.COLOR_GRAY2BGR)

                    # 显示暂停提示
                    cv2.putText(display_image, "PAUSED (Press 'p' to continue)",
                               (50, 320), cv2.FONT_HERSHEY_SIMPLEX, 1.0, (0, 255, 255), 2)

                    cv2.imshow('Gesture Recognition', display_image)

                # 处理按键
                key = cv2.waitKey(1) & 0xFF

                if key == ord('q'):
                    print("\n\n用户退出")
                    break
                elif key == ord('s'):
                    if not self.paused and gesture_id is not None:
                        self.save_frame(image, gesture_id, confidence)
                elif key == ord('p'):
                    self.paused = not self.paused
                    status = "暂停" if self.paused else "继续"
                    print(f"\n[{status}]")

        except KeyboardInterrupt:
            print("\n\n用户中断")

        finally:
            cv2.destroyAllWindows()
            print(f"\n\n总帧数: {self.frame_count}")
            print(f"平均 FPS: {np.mean(self.fps_queue):.2f}" if len(self.fps_queue) > 0 else "")


def main():
    parser = argparse.ArgumentParser(description='实时推理工具 - PC端识别STM32发来的图像')
    parser.add_argument('--port', type=str, required=True, help='串口号（如 COM17）')
    parser.add_argument('--baudrate', type=int, default=921600, help='波特率（默认 921600）')
    parser.add_argument('--model', type=str, required=True, help='PyTorch 模型路径（.pth 文件）')
    parser.add_argument('--save-dir', type=str, default='realtime_frames', help='保存帧的目录')

    args = parser.parse_args()

    # 创建推理器
    inferencer = RealtimeInference(args.port, args.baudrate, args.model, args.save_dir)

    # 加载模型
    inferencer.load_model()

    # 连接串口
    if not inferencer.connect():
        return

    try:
        # 运行实时推理
        inferencer.run()
    finally:
        inferencer.disconnect()


if __name__ == "__main__":
    main()
