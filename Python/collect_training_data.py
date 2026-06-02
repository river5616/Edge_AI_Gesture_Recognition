#!/usr/bin/env python3
"""
训练数据收集工具
用于从 STM32 采集真实部署环境的图像数据
"""

import serial
import numpy as np
import cv2
import os
import time
from datetime import datetime

# 手势类别（按照模型训练时的顺序） - 5个手势版本
GESTURE_NAMES = {
    0: "fist",           # 拳头
    1: "palm",           # 手掌
    2: "v_sign",         # V手势（食指中指分开成V形）
    3: "ok",             # OK手势
    4: "nothing"         # 无手势/背景
}

class DataCollector:
    def __init__(self, port, baudrate=921600, dataset_dir="dataset_real"):
        self.port = port
        self.baudrate = baudrate
        self.dataset_dir = dataset_dir
        self.ser = None

        # 为每个手势创建目录
        for gesture_id, gesture_name in GESTURE_NAMES.items():
            gesture_dir = os.path.join(dataset_dir, f"{gesture_id}_{gesture_name}")
            os.makedirs(gesture_dir, exist_ok=True)

        print(f"✓ 数据集目录已创建: {dataset_dir}")

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
            print("✓ 串口已断开")

    def read_line(self):
        """读取一行"""
        if self.ser and self.ser.is_open:
            return self.ser.readline().decode('utf-8', errors='ignore').strip()
        return ""

    def collect_gesture(self, gesture_id, num_images=100, countdown=5):
        """
        采集指定手势的图像

        Args:
            gesture_id: 手势ID (0-6)
            num_images: 采集图像数量
            countdown: 开始采集前的倒计时秒数（默认5秒）
        """
        if gesture_id not in GESTURE_NAMES:
            print(f"✗ 无效的手势ID: {gesture_id}")
            return

        gesture_name = GESTURE_NAMES[gesture_id]
        gesture_dir = os.path.join(self.dataset_dir, f"{gesture_id}_{gesture_name}")

        # 获取当前已有图像的最大编号，从最大编号+1开始，避免覆盖
        existing_files = [f for f in os.listdir(gesture_dir) if f.endswith('.png')]
        max_index = 0
        for filename in existing_files:
            try:
                # 从文件名中提取编号，格式: {gesture_name}_{序号:04d}.png
                # 例如: palm_0123.png -> 123
                number_part = filename.split('_')[-1].split('.')[0]
                index = int(number_part)
                max_index = max(max_index, index)
            except (ValueError, IndexError):
                # 忽略无法解析的文件名
                pass
        start_index = max_index + 1

        print(f"\n{'='*60}")
        print(f"开始采集手势: {gesture_name} (ID: {gesture_id})")
        print(f"目标数量: {num_images}")
        print(f"起始序号: {start_index}")
        print(f"保存目录: {gesture_dir}")
        print(f"{'='*60}\n")
        print("请保持手势稳定，尽量变化角度、位置、距离...")
        print("按 Ctrl+C 可提前结束\n")

        # 倒计时，给用户准备时间
        print(f"⏱️  准备倒计时（左手拿摄像头，右手准备做手势）：")
        for i in range(countdown, 0, -1):
            print(f"   {i} 秒...", end='\r')
            time.sleep(1)
        print("   开始采集！      \n")

        collected = 0
        last_image = None

        try:
            while collected < num_images:
                line = self.read_line()

                if not line:
                    time.sleep(0.01)
                    continue

                # 接收图像
                if line.startswith("$IMG"):
                    try:
                        parts = line.split(',')
                        if len(parts) == 4:
                            width = int(parts[1])
                            height = int(parts[2])
                            data_length = int(parts[3])

                            # 读取图像数据
                            image_data = self.ser.read(data_length)

                            if len(image_data) == data_length:
                                # 读取结束标记
                                end_marker = self.read_line()

                                # 解析图像
                                if data_length == width * height:
                                    # 灰度图
                                    image = np.frombuffer(image_data, dtype=np.uint8).reshape((height, width))
                                else:
                                    # JPEG
                                    image = cv2.imdecode(np.frombuffer(image_data, dtype=np.uint8), cv2.IMREAD_GRAYSCALE)

                                if image is not None:
                                    # 保存图像
                                    filename = f"{gesture_name}_{start_index + collected:04d}.png"
                                    filepath = os.path.join(gesture_dir, filename)
                                    cv2.imwrite(filepath, image)

                                    collected += 1
                                    print(f"✓ [{collected}/{num_images}] 已保存: {filename} "
                                          f"| 亮度: {np.mean(image):.1f}")

                                    # 短暂延时，避免采集到相同的图像
                                    time.sleep(0.1)
                    except Exception as e:
                        print(f"✗ 图像接收失败: {e}")

                elif line.startswith("$RESULT"):
                    pass  # 忽略识别结果

                elif line.startswith("$FPS"):
                    pass  # 忽略FPS

        except KeyboardInterrupt:
            print(f"\n\n⚠️ 用户中断，已采集 {collected} 张图像")

        print(f"\n{'='*60}")
        print(f"✓ 采集完成！")
        print(f"  手势: {gesture_name}")
        print(f"  采集数量: {collected}")
        print(f"  保存位置: {gesture_dir}")
        print(f"{'='*60}\n")

def main():
    import argparse

    parser = argparse.ArgumentParser(description="训练数据收集工具")
    parser.add_argument("--port", type=str, required=True, help="串口号")
    parser.add_argument("--baudrate", type=int, default=921600, help="波特率")
    parser.add_argument("--dataset-dir", type=str, default="dataset_real", help="数据集保存目录")

    args = parser.parse_args()

    collector = DataCollector(args.port, args.baudrate, args.dataset_dir)

    if not collector.connect():
        return

    try:
        print("\n" + "="*60)
        print("手势识别训练数据收集工具")
        print("="*60)
        print("\n手势列表：")
        for gid, gname in GESTURE_NAMES.items():
            print(f"  {gid} - {gname}")
        print("\n每个手势建议采集：")
        print("  - 最少：200 张（快速测试）")
        print("  - 推荐：500 张（较好效果）")
        print("  - 最佳：1000+ 张（最佳效果）")
        print("\n采集要求：")
        print("  1. 多角度：正面、侧面、斜视")
        print("  2. 多距离：远、中、近")
        print("  3. 多位置：左、中、右、上、下")
        print("  4. 手指变化：稍微弯曲、展开、收紧")
        print("  5. 背景变化：移动手部位置改变背景")
        print("\n特别注意易混淆的手势：")
        print("  - v_sign (2) vs palm (1)：v_sign只有两指伸出，其他三指完全握拳")
        print("  - v_sign (2)：两指要分开呈V形，不要并拢")
        print("  - 这些手势要特别多采集不同角度和距离！")
        print("="*60 + "\n")

        while True:
            try:
                gesture_input = input("请输入要采集的手势ID (0-4)，或输入 'q' 退出: ").strip()

                if gesture_input.lower() == 'q':
                    break

                gesture_id = int(gesture_input)

                if gesture_id not in GESTURE_NAMES:
                    print("✗ 无效的手势ID，请输入 0-4")
                    continue

                num_input = input(f"请输入采集数量（建议至少200，推荐500+）: ").strip()
                num_images = int(num_input)

                if num_images <= 0:
                    print("✗ 数量必须大于0")
                    continue

                # 询问倒计时时长
                countdown_input = input(f"准备时间（秒，默认2秒，直接回车使用默认值）: ").strip()
                countdown = int(countdown_input) if countdown_input else 2

                # 开始采集
                collector.collect_gesture(gesture_id, num_images, countdown)

                # 询问是否继续
                continue_input = input("\n是否继续采集其他手势？(y/n): ").strip().lower()
                if continue_input != 'y':
                    break

            except ValueError:
                print("✗ 请输入有效的数字")
            except KeyboardInterrupt:
                print("\n\n⚠️ 用户中断")
                break

    finally:
        collector.disconnect()
        print("\n采集工具已退出")

if __name__ == "__main__":
    main()
