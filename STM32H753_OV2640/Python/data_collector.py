#!/usr/bin/env python3
"""
STM32 OV2640手势识别 - 数据采集脚本
用于通过串口接收STM32发送的图像数据并保存为训练数据集
"""

import serial
import numpy as np
import cv2
import os
import argparse
from datetime import datetime
import time

# 手势类别名称
GESTURE_NAMES = {
    0: "fist",           # 拳头
    1: "palm",           # 张开手掌
    2: "thumbs_up",      # 竖起大拇指
    3: "ok",             # OK手势
    4: "v_sign",         # V字手势
    5: "point",          # 指向手势
    6: "three",          # 数字3
    7: "four",           # 数字4
    8: "nothing",        # 无
}

class DataCollector:
    def __init__(self, port, baudrate=921600, dataset_dir="dataset"):
        """
        初始化数据采集器

        Args:
            port: 串口号(如 'COM3' 或 '/dev/ttyUSB0')
            baudrate: 波特率
            dataset_dir: 数据集保存目录
        """
        self.port = port
        self.baudrate = baudrate
        self.dataset_dir = dataset_dir
        self.ser = None

        # 创建数据集目录
        os.makedirs(dataset_dir, exist_ok=True)
        for gesture_id, gesture_name in GESTURE_NAMES.items():
            gesture_dir = os.path.join(dataset_dir, f"{gesture_id}_{gesture_name}")
            os.makedirs(gesture_dir, exist_ok=True)

    def connect(self):
        """连接串口"""
        try:
            self.ser = serial.Serial(self.port, self.baudrate, timeout=1)
            print(f"✓ 已连接到 {self.port}, 波特率 {self.baudrate}")
            time.sleep(2)  # 等待连接稳定
            return True
        except Exception as e:
            print(f"✗ 连接失败: {e}")
            return False

    def disconnect(self):
        """断开串口"""
        if self.ser and self.ser.is_open:
            self.ser.close()
            print("✓ 串口已断开")

    def send_command(self, command):
        """
        发送命令到STM32

        Args:
            command: 命令字符串
        """
        if self.ser and self.ser.is_open:
            cmd = command + "\r\n"
            self.ser.write(cmd.encode())
            print(f"→ 发送命令: {command}")

    def read_response(self):
        """读取响应"""
        if self.ser and self.ser.is_open:
            line = self.ser.readline().decode('utf-8', errors='ignore').strip()
            return line
        return ""

    def receive_image(self):
        """
        接收图像数据

        Returns:
            numpy array: 灰度图像，形状为(height, width)
        """
        try:
            # 读取图像头部: $IMG,<width>,<height>,<data_length>
            while True:
                line = self.read_response()
                if line.startswith("$IMG"):
                    break
                elif line:
                    print(f"← {line}")

            # 解析图像参数
            parts = line.split(',')
            width = int(parts[1])
            height = int(parts[2])
            data_length = int(parts[3])

            # 读取图像数据
            image_data = self.ser.read(data_length)

            # 读取结束标记
            end_marker = self.read_response()
            if end_marker != "$END":
                print(f"警告: 未收到正确的结束标记: {end_marker}")

            # 转换为numpy数组
            image = np.frombuffer(image_data, dtype=np.uint8)
            image = image.reshape((height, width))

            return image

        except Exception as e:
            print(f"✗ 接收图像失败: {e}")
            return None

    def collect_gesture(self, gesture_id, num_images):
        """
        采集指定手势的数据

        Args:
            gesture_id: 手势ID (0-9)
            num_images: 要采集的图像数量
        """
        if gesture_id not in GESTURE_NAMES:
            print(f"✗ 无效的手势ID: {gesture_id}")
            return

        gesture_name = GESTURE_NAMES[gesture_id]
        print(f"\n开始采集手势 {gesture_id}: {gesture_name}, 目标数量: {num_images}")

        # 发送采集命令
        self.send_command(f"$COLLECT,{gesture_id},{num_images}")

        # 等待响应
        response = self.read_response()
        print(f"← {response}")

        if not response.startswith("$OK"):
            print("✗ STM32未准备好")
            return

        # 接收图像
        gesture_dir = os.path.join(self.dataset_dir, f"{gesture_id}_{gesture_name}")

        # 获取当前文件夹中已有的图片数量，作为起始序号
        existing_files = [f for f in os.listdir(gesture_dir) if f.endswith('.png')]
        start_index = len(existing_files) + 1

        collected = 0

        while collected < num_images:
            # 接收图像
            image = self.receive_image()

            if image is not None:
                # 保存图像 - 使用序号命名
                image_index = start_index + collected
                filename = f"{gesture_name}_{image_index:04d}.png"
                filepath = os.path.join(gesture_dir, filename)
                cv2.imwrite(filepath, image)

                collected += 1
                print(f"✓ 已保存: {filename} ({collected}/{num_images})")

                # 显示图像
                cv2.imshow(f"Gesture {gesture_id}: {gesture_name}",
                          cv2.resize(image, (288, 288)))  # 放大3倍显示
                cv2.waitKey(1)

        # 等待完成消息
        while True:
            line = self.read_response()
            if not line:
                break
            print(f"← {line}")

            if line.startswith("$COMPLETE"):
                print(f"✓ 采集完成!")
                cv2.destroyAllWindows()
                return

    def preview_mode(self):
        """
        预览模式 - 实时显示摄像头图像
        """
        print("\n进入预览模式...")
        print("按 'q' 退出, 按 's' 保存当前帧")

        # 发送推理模式命令（用于连续获取图像）
        self.send_command("$INFER")

        saved_count = 0

        try:
            while True:
                # 接收图像
                image = self.receive_image()

                if image is not None:
                    # 放大显示
                    display_image = cv2.resize(image, (288, 288))
                    cv2.imshow("Camera Preview", display_image)

                    # 处理按键
                    key = cv2.waitKey(1) & 0xFF
                    if key == ord('q'):
                        break
                    elif key == ord('s'):
                        # 保存当前帧
                        filename = f"preview_{datetime.now().strftime('%Y%m%d_%H%M%S')}.png"
                        cv2.imwrite(filename, image)
                        saved_count += 1
                        print(f"✓ 已保存: {filename}")

                # 读取其他信息
                line = self.read_response()
                if line and line.startswith("$FPS"):
                    print(f"← {line}")

        except KeyboardInterrupt:
            print("\n用户中断")

        finally:
            self.send_command("$STOP_INFER")
            cv2.destroyAllWindows()
            print(f"预览模式结束，共保存 {saved_count} 张图片")

    def query_status(self):
        """查询STM32状态"""
        self.send_command("$STATUS")
        response = self.read_response()
        print(f"← {response}")

def main():
    parser = argparse.ArgumentParser(description="STM32 OV2640手势识别数据采集工具")
    parser.add_argument("--port", type=str, required=True, help="串口号(如 COM3 或 /dev/ttyUSB0)")
    parser.add_argument("--baudrate", type=int, default=921600, help="波特率(默认921600)")
    parser.add_argument("--mode", type=str, default="collect", choices=["collect", "preview", "status"],
                       help="工作模式: collect(采集), preview(预览), status(状态)")
    parser.add_argument("--gesture", type=int, help="手势ID (0-9)")
    parser.add_argument("--num", type=int, default=100, help="采集图像数量(默认100)")
    parser.add_argument("--dataset", type=str, default="dataset", help="数据集保存目录")

    args = parser.parse_args()

    # 创建采集器
    collector = DataCollector(args.port, args.baudrate, args.dataset)

    # 连接串口
    if not collector.connect():
        return

    try:
        if args.mode == "collect":
            if args.gesture is None:
                print("错误: collect模式需要指定 --gesture 参数")
                print("\n可用的手势ID:")
                for gid, gname in GESTURE_NAMES.items():
                    print(f"  {gid}: {gname}")
            else:
                collector.collect_gesture(args.gesture, args.num)

        elif args.mode == "preview":
            collector.preview_mode()

        elif args.mode == "status":
            collector.query_status()

    except KeyboardInterrupt:
        print("\n\n用户中断")

    finally:
        # 发送停止命令
        collector.send_command("$STOP")
        collector.disconnect()

if __name__ == "__main__":
    main()
