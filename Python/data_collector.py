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
            # 清空接收缓冲区，避免读到旧数据
            self.ser.reset_input_buffer()
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
        接收图像数据（支持JPEG和灰度两种格式）

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
            if len(parts) != 4:
                print(f"✗ 图像头部格式错误: {line}")
                return None

            width = int(parts[1])
            height = int(parts[2])
            data_length = int(parts[3])

            # 读取图像数据（精确读取data_length字节）
            image_data = self.ser.read(data_length)

            if len(image_data) != data_length:
                print(f"✗ 数据长度不匹配: 期望{data_length}, 实际{len(image_data)}")
                # 清空缓冲区
                self.ser.reset_input_buffer()
                return None

            # 读取结束标记（应该紧跟在图像数据后）
            end_marker = self.read_response()
            if not end_marker.startswith("$END"):
                print(f"✗ 未收到正确的结束标记")
                print(f"  期望: $END")
                print(f"  实际: {end_marker[:50]}...")  # 只显示前50字符
                # 清空缓冲区，准备下一帧
                self.ser.reset_input_buffer()
                return None

            # ⚠️ 改进格式检测：根据data_length判断而不是魔数判断
            # 如果data_length == width * height，则是灰度模式
            # 如果data_length < width * height，则可能是JPEG压缩模式
            expected_gray_size = width * height

            if data_length == expected_gray_size:
                # 灰度格式：直接转换
                image = np.frombuffer(image_data, dtype=np.uint8)
                if len(image) != expected_gray_size:
                    print(f"✗ 灰度图像大小不匹配: {len(image)} != {expected_gray_size}")
                    return None
                image = image.reshape((height, width))
                print(f"← 灰度图像: {width}x{height}, {data_length}字节")
            elif data_length < expected_gray_size:
                # 可能是JPEG格式（压缩后体积更小）
                # 验证JPEG魔数和结束标记
                if len(image_data) >= 2 and image_data[0] == 0xFF and image_data[1] == 0xD8:
                    if len(image_data) >= 2 and image_data[-2] == 0xFF and image_data[-1] == 0xD9:
                        # JPEG格式：使用OpenCV解码
                        image = cv2.imdecode(np.frombuffer(image_data, dtype=np.uint8), cv2.IMREAD_GRAYSCALE)
                        if image is None:
                            print(f"✗ JPEG解码失败，数据长度: {data_length}")
                            print(f"  起始: {image_data[:4].hex()}, 结束: {image_data[-4:].hex()}")
                            return None
                        print(f"← JPEG图像: {width}x{height}, 压缩后{data_length}字节, 解码为{image.shape}")
                    else:
                        print(f"✗ JPEG数据不完整（缺少结束标记0xFFD9）")
                        print(f"  起始: {image_data[:4].hex()}, 结束: {image_data[-4:].hex()}")
                        return None
                else:
                    print(f"✗ 未知格式: data_length={data_length}, 期望={expected_gray_size}")
                    print(f"  起始: {image_data[:4].hex()}")
                    return None
            else:
                # data_length > expected_gray_size，异常
                print(f"✗ 数据长度异常: {data_length} > {expected_gray_size}")
                return None

            return image

        except Exception as e:
            print(f"✗ 接收图像失败: {e}")
            import traceback
            traceback.print_exc()
            # 清空串口缓冲区
            if self.ser:
                self.ser.reset_input_buffer()
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

        # 先发送停止命令，确保STM32处于空闲状态
        self.send_command("$STOP")
        time.sleep(0.5)  # 等待STM32停止之前的操作

        # 发送采集命令
        self.send_command(f"$COLLECT,{gesture_id},{num_images}")

        # 等待响应，跳过可能的调试信息（如$FPS）
        max_retries = 10
        response = None
        for i in range(max_retries):
            line = self.read_response()
            if line:
                print(f"← {line}")
                if line.startswith("$OK"):
                    response = line
                    break
                elif line.startswith("$ERROR"):
                    print("✗ STM32返回错误")
                    return
            time.sleep(0.1)  # 等待一小段时间

        if not response or not response.startswith("$OK"):
            print("✗ STM32未准备好或响应超时")
            return

        # 接收图像
        gesture_dir = os.path.join(self.dataset_dir, f"{gesture_id}_{gesture_name}")

        # 获取当前文件夹中已有的图片数量，作为起始序号
        existing_files = [f for f in os.listdir(gesture_dir) if f.endswith('.png')]
        start_index = len(existing_files) + 1

        collected = 0
        failed_count = 0
        max_failures = 3  # 最多允许3次失败

        while collected < num_images:
            # 接收图像
            image = self.receive_image()

            if image is not None:
                # ⚠️ 缩放到96x96用于训练
                # JPEG模式接收240x240，灰度模式接收180x180，统一缩放到96x96
                # 使用INTER_AREA插值，适合缩小图像
                image_96 = cv2.resize(image, (96, 96), interpolation=cv2.INTER_AREA)

                # 保存缩放后的图像 - 使用序号命名
                image_index = start_index + collected
                filename = f"{gesture_name}_{image_index:04d}.png"
                filepath = os.path.join(gesture_dir, filename)
                cv2.imwrite(filepath, image_96)  # 保存96x96

                collected += 1
                failed_count = 0  # 重置失败计数
                print(f"✓ 已保存: {filename} ({image.shape[1]}x{image.shape[0]}→96x96) ({collected}/{num_images})")

                # 显示原始图像
                cv2.imshow(f"Gesture {gesture_id}: {gesture_name}", image)
                cv2.waitKey(1)
            else:
                # 接收失败，计数
                failed_count += 1
                print(f"✗ 图像接收失败 ({failed_count}/{max_failures})")

                if failed_count >= max_failures:
                    print(f"✗ 连续失败{max_failures}次，放弃采集")
                    cv2.destroyAllWindows()
                    return

                # 等待一小段时间后重试
                time.sleep(0.1)

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
        # 发送停止命令并等待STM32完全停止
        collector.send_command("$STOP")
        time.sleep(0.5)  # 等待STM32处理停止命令
        # 清空缓冲区中残留的数据
        if collector.ser and collector.ser.is_open:
            collector.ser.reset_input_buffer()
            collector.ser.reset_output_buffer()
        collector.disconnect()

if __name__ == "__main__":
    main()
