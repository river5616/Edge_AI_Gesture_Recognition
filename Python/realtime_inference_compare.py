#!/usr/bin/env python3
"""
实时推理对比工具 - 同时运行多个模型进行对比
从 STM32 接收图像并在 PC 端使用多个模型实时识别显示对比结果
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
    0: "fist", 1: "palm", 2: "v_sign", 3: "ok", 4: "nothing"
}

GESTURE_COLORS = {
    0: (255, 0, 0),      # 蓝色 - fist
    1: (0, 255, 0),      # 绿色 - palm
    2: (0, 0, 255),      # 红色 - point
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
    """改进版轻量级手势识别网络"""

    def __init__(self, num_classes=5):
        super(GestureNetV2, self).__init__()

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
    """轻量版改进型手势识别网络 - 减少RAM占用"""

    def __init__(self, num_classes=5):
        super(GestureNetV2Lite, self).__init__()

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


# ==================== 模型信息类 ====================
class ModelInfo:
    """存储单个模型的信息"""
    def __init__(self, name, model, color):
        self.name = name
        self.model = model
        self.color = color
        self.gesture_id = None
        self.confidence = 0.0
        self.all_probs = None
        self.inference_time = 0.0


# ==================== 实时推理对比类 ====================
class RealtimeInferenceCompare:
    def __init__(self, port, baudrate, model_configs, save_dir="realtime_frames_compare"):
        """
        Args:
            port: 串口号
            baudrate: 波特率
            model_configs: 模型配置列表，格式 [(model_path, model_class, display_name, color), ...]
            save_dir: 保存目录
        """
        self.port = port
        self.baudrate = baudrate
        self.model_configs = model_configs
        self.save_dir = save_dir
        self.ser = None
        self.models = []  # ModelInfo列表
        self.device = None
        self.transform = None

        # 创建保存目录
        os.makedirs(save_dir, exist_ok=True)

        # 统计信息
        self.frame_count = 0
        self.fps_queue = deque(maxlen=30)
        self.last_time = time.time()

        # 暂停标志
        self.paused = False

    def load_models(self):
        """加载所有模型"""
        print(f"\n加载 {len(self.model_configs)} 个模型...")

        # 检测设备
        self.device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
        print(f"使用设备: {self.device}")

        for model_path, model_class, display_name, color in self.model_configs:
            print(f"\n加载模型: {display_name}")
            print(f"  路径: {model_path}")

            try:
                # 创建模型实例
                model = model_class(num_classes=5)

                # 加载权重
                checkpoint = torch.load(model_path, map_location=self.device, weights_only=False)

                # 检查是否有嵌套的 state_dict
                if isinstance(checkpoint, dict) and 'state_dict' in checkpoint:
                    state_dict = checkpoint['state_dict']
                else:
                    state_dict = checkpoint

                model.load_state_dict(state_dict)
                model = model.to(self.device)
                model.eval()

                # 创建 ModelInfo
                model_info = ModelInfo(display_name, model, color)
                self.models.append(model_info)

                print(f"  ✓ 加载成功")

            except Exception as e:
                print(f"  ✗ 加载失败: {e}")
                continue

        if len(self.models) == 0:
            raise ValueError("没有成功加载任何模型！")

        print(f"\n✓ 成功加载 {len(self.models)} 个模型\n")

        # 数据预处理
        self.transform = transforms.Compose([
            transforms.ToPILImage(),
            transforms.Resize((64, 64)),
            transforms.ToTensor(),
            transforms.Normalize(mean=[0.5], std=[0.5])
        ])

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

    def predict_all(self, image):
        """使用所有模型预测"""
        try:
            # 预处理
            input_tensor = self.transform(image).unsqueeze(0).to(self.device)

            # 对每个模型进行推理
            for model_info in self.models:
                start_time = time.time()

                with torch.no_grad():
                    outputs = model_info.model(input_tensor)
                    probabilities = torch.softmax(outputs, dim=1)
                    confidence, predicted = torch.max(probabilities, 1)

                    model_info.gesture_id = predicted.item()
                    model_info.confidence = confidence.item()
                    model_info.all_probs = probabilities.cpu().numpy()[0]
                    model_info.inference_time = (time.time() - start_time) * 1000  # ms

        except Exception as e:
            print(f"✗ 推理失败: {e}")

    def draw_comparison(self, image, fps):
        """绘制对比显示"""
        # 创建大画布
        num_models = len(self.models)

        # 布局：顶部显示原始图像，下方显示各个模型的结果
        image_display_size = 320
        model_display_height = 280

        canvas_width = max(image_display_size * num_models, 960)
        canvas_height = image_display_size + model_display_height

        canvas = np.zeros((canvas_height, canvas_width, 3), dtype=np.uint8)

        # 1. 绘制原始图像（居中显示）
        img_resized = cv2.resize(image, (image_display_size, image_display_size),
                                 interpolation=cv2.INTER_NEAREST)
        img_colored = cv2.cvtColor(img_resized, cv2.COLOR_GRAY2BGR)

        # 添加标题和信息
        cv2.putText(img_colored, "Original Image", (10, 30),
                   cv2.FONT_HERSHEY_SIMPLEX, 0.8, (255, 255, 255), 2)
        cv2.putText(img_colored, f"Frame: {self.frame_count}", (10, image_display_size - 40),
                   cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 255, 255), 1)
        cv2.putText(img_colored, f"FPS: {fps:.1f}", (10, image_display_size - 15),
                   cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 255, 255), 1)

        # 将原始图像放在画布中央
        start_x = (canvas_width - image_display_size) // 2
        canvas[0:image_display_size, start_x:start_x+image_display_size] = img_colored

        # 2. 绘制每个模型的预测结果
        model_width = canvas_width // num_models

        for idx, model_info in enumerate(self.models):
            x_start = idx * model_width
            y_start = image_display_size

            # 创建模型结果区域
            result_area = canvas[y_start:y_start+model_display_height, x_start:x_start+model_width]

            # 背景色（根据模型颜色稍微调整）
            bg_color = tuple(int(c * 0.2) for c in model_info.color)
            result_area[:] = bg_color

            # 绘制模型名称
            cv2.putText(result_area, model_info.name, (10, 30),
                       cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 255), 2)

            if model_info.gesture_id is not None:
                # 获取手势信息
                gesture_name = GESTURE_NAMES.get(model_info.gesture_id, "UNKNOWN")
                gesture_color = GESTURE_COLORS.get(model_info.gesture_id, (255, 255, 255))

                # 绘制预测手势（大字）
                cv2.putText(result_area, gesture_name.upper(), (10, 80),
                           cv2.FONT_HERSHEY_SIMPLEX, 1.0, gesture_color, 2)

                # 绘制置信度
                conf_text = f"{model_info.confidence*100:.1f}%"
                cv2.putText(result_area, conf_text, (10, 120),
                           cv2.FONT_HERSHEY_SIMPLEX, 0.8, gesture_color, 2)

                # 绘制推理时间
                time_text = f"Time: {model_info.inference_time:.1f}ms"
                cv2.putText(result_area, time_text, (10, 150),
                           cv2.FONT_HERSHEY_SIMPLEX, 0.5, (200, 200, 200), 1)

                # 绘制置信度条形图（简化版）
                bar_y_start = 170
                bar_height = 15
                bar_max_width = model_width - 60

                # 显示前3个最高概率
                top3_indices = np.argsort(model_info.all_probs)[-3:][::-1]

                for i, gid in enumerate(top3_indices):
                    y = bar_y_start + i * (bar_height + 5)
                    prob = model_info.all_probs[gid]
                    bar_len = int(bar_max_width * prob)

                    # 绘制条形
                    color = GESTURE_COLORS.get(gid, (100, 100, 100))
                    cv2.rectangle(result_area, (50, y), (50 + bar_len, y + bar_height),
                                 color, -1)

                    # 绘制边框
                    cv2.rectangle(result_area, (50, y), (50 + bar_max_width, y + bar_height),
                                 (100, 100, 100), 1)

                    # 绘制标签
                    label = f"{gid}:{GESTURE_NAMES[gid][:4]}"
                    cv2.putText(result_area, label, (5, y + 12),
                               cv2.FONT_HERSHEY_SIMPLEX, 0.4, (255, 255, 255), 1)

                    # 绘制百分比
                    cv2.putText(result_area, f"{prob*100:.0f}%",
                               (55 + bar_max_width, y + 12),
                               cv2.FONT_HERSHEY_SIMPLEX, 0.4, (255, 255, 255), 1)

        return canvas

    def save_frame(self, image):
        """保存当前帧和所有模型预测结果"""
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S_%f")[:-3]

        # 保存原始图像
        img_filename = f"{timestamp}_original.png"
        img_filepath = os.path.join(self.save_dir, img_filename)
        cv2.imwrite(img_filepath, image)

        # 保存预测信息到文本文件
        txt_filename = f"{timestamp}_predictions.txt"
        txt_filepath = os.path.join(self.save_dir, txt_filename)

        with open(txt_filepath, 'w', encoding='utf-8') as f:
            f.write(f"Frame: {self.frame_count}\n")
            f.write(f"Timestamp: {timestamp}\n\n")

            for model_info in self.models:
                f.write(f"Model: {model_info.name}\n")
                if model_info.gesture_id is not None:
                    gesture_name = GESTURE_NAMES.get(model_info.gesture_id, "UNKNOWN")
                    f.write(f"  Prediction: {gesture_name} (ID: {model_info.gesture_id})\n")
                    f.write(f"  Confidence: {model_info.confidence*100:.2f}%\n")
                    f.write(f"  Inference Time: {model_info.inference_time:.2f}ms\n")
                    f.write(f"  All Probabilities:\n")
                    for i in range(5):
                        f.write(f"    {i} ({GESTURE_NAMES[i]}): {model_info.all_probs[i]*100:.2f}%\n")
                f.write("\n")

        print(f"\n✓ 已保存: {img_filename}, {txt_filename}")

    def run(self):
        """运行实时推理对比"""
        print("\n" + "="*60)
        print("实时推理对比工具")
        print("="*60)
        print(f"\n对比模型数量: {len(self.models)}")
        for model_info in self.models:
            print(f"  - {model_info.name}")
        print("\n操作说明:")
        print("  's' - 保存当前帧和预测结果")
        print("  'p' - 暂停/继续")
        print("  'q' - 退出")
        print("\n" + "="*60 + "\n")

        # 创建窗口
        cv2.namedWindow('Model Comparison', cv2.WINDOW_NORMAL)
        cv2.resizeWindow('Model Comparison', 1280, 720)

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
                    # 推理所有模型
                    self.predict_all(image)

                    # 绘制对比显示
                    display = self.draw_comparison(image, avg_fps)

                    # 显示
                    cv2.imshow('Model Comparison', display)

                    # 控制台输出
                    print(f"\rFrame {self.frame_count:04d} | ", end='')
                    for model_info in self.models:
                        if model_info.gesture_id is not None:
                            gesture_name = GESTURE_NAMES.get(model_info.gesture_id, "???")
                            print(f"{model_info.name[:6]}: {gesture_name:10s}({model_info.confidence*100:4.1f}%) | ", end='')
                    print(f"FPS: {avg_fps:5.1f}", end='')
                else:
                    # 暂停状态
                    display = self.draw_comparison(image, avg_fps)
                    cv2.putText(display, "PAUSED (Press 'p' to continue)",
                               (display.shape[1]//2 - 200, 50),
                               cv2.FONT_HERSHEY_SIMPLEX, 1.0, (0, 255, 255), 2)
                    cv2.imshow('Model Comparison', display)

                # 处理按键
                key = cv2.waitKey(1) & 0xFF

                if key == ord('q'):
                    print("\n\n用户退出")
                    break
                elif key == ord('s'):
                    if not self.paused:
                        self.save_frame(image)
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
    parser = argparse.ArgumentParser(description='实时推理对比工具 - 同时运行多个模型进行对比')
    parser.add_argument('--port', type=str, required=True, help='串口号（如 COM17）')
    parser.add_argument('--baudrate', type=int, default=921600, help='波特率（默认 921600）')
    parser.add_argument('--model1', type=str, required=True, help='模型1路径（如 gesture_v2_lite.pth）')
    parser.add_argument('--model1-type', type=str, default='lite',
                       choices=['v1', 'v2', 'lite'],
                       help='模型1类型')
    parser.add_argument('--model1-name', type=str, default='V2Lite', help='模型1显示名称')

    parser.add_argument('--model2', type=str, required=True, help='模型2路径（如 gesture_v1_focal.pth）')
    parser.add_argument('--model2-type', type=str, default='v1',
                       choices=['v1', 'v2', 'lite'],
                       help='模型2类型')
    parser.add_argument('--model2-name', type=str, default='V1', help='模型2显示名称')

    parser.add_argument('--model3', type=str, required=True, help='模型3路径（如 gesture_v2_focal.pth）')
    parser.add_argument('--model3-type', type=str, default='v2',
                       choices=['v1', 'v2', 'lite'],
                       help='模型3类型')
    parser.add_argument('--model3-name', type=str, default='V2', help='模型3显示名称')

    parser.add_argument('--save-dir', type=str, default='realtime_frames_compare',
                       help='保存帧的目录')

    args = parser.parse_args()

    # 模型类型映射
    model_class_map = {
        'v1': GestureNet,
        'v2': GestureNetV2,
        'lite': GestureNetV2Lite
    }

    # 颜色映射
    colors = [
        (100, 200, 255),  # 橙色
        (255, 150, 100),  # 蓝色
        (150, 255, 150)   # 绿色
    ]

    # 构建模型配置
    model_configs = [
        (args.model1, model_class_map[args.model1_type], args.model1_name, colors[0]),
        (args.model2, model_class_map[args.model2_type], args.model2_name, colors[1]),
        (args.model3, model_class_map[args.model3_type], args.model3_name, colors[2])
    ]

    # 创建推理器
    inferencer = RealtimeInferenceCompare(args.port, args.baudrate, model_configs, args.save_dir)

    # 加载模型
    inferencer.load_models()

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
