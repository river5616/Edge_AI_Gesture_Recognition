#!/usr/bin/env python3
"""
推理调试工具 - 实时接收STM32推理时的图像并保存分析
用于诊断光照、预处理等问题
"""

import serial
import numpy as np
import cv2
import os
import time
from datetime import datetime
import matplotlib.pyplot as plt

# 手势类别
GESTURE_NAMES = {
    0: "fist", 1: "palm", 2: "thumbs_up", 3: "ok", 4: "v_sign",
    5: "point", 6: "three", 7: "four", 8: "nothing"
}

class InferenceDebugger:
    def __init__(self, port, baudrate=921600, save_dir="debug_inference"):
        """
        初始化调试器

        Args:
            port: 串口号
            baudrate: 波特率
            save_dir: 调试数据保存目录
        """
        self.port = port
        self.baudrate = baudrate
        self.save_dir = save_dir
        self.ser = None

        # 创建保存目录
        os.makedirs(save_dir, exist_ok=True)
        os.makedirs(os.path.join(save_dir, "images"), exist_ok=True)
        os.makedirs(os.path.join(save_dir, "analysis"), exist_ok=True)

        # 统计信息
        self.frame_count = 0
        self.results_log = []

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

    def receive_image(self):
        """
        接收图像数据

        Returns:
            numpy array: 图像 (height, width)
            dict: 元数据
        """
        try:
            # 读取图像头部
            while True:
                line = self.read_line()
                if line.startswith("$IMG"):
                    break
                elif line.startswith("$RESULT"):
                    # 记录识别结果
                    parts = line.split(',')
                    if len(parts) == 3:
                        gesture_id = int(parts[1])
                        confidence = float(parts[2])
                        return None, {"result": (gesture_id, confidence)}

            # 解析参数
            parts = line.split(',')
            if len(parts) != 4:
                return None, None

            width = int(parts[1])
            height = int(parts[2])
            data_length = int(parts[3])

            # 读取图像数据
            image_data = self.ser.read(data_length)

            if len(image_data) != data_length:
                print(f"✗ 数据长度不匹配")
                return None, None

            # 读取结束标记
            end_marker = self.read_line()
            if not end_marker.startswith("$END"):
                return None, None

            # 解析图像
            expected_gray_size = width * height

            if data_length == expected_gray_size:
                # 灰度图
                image = np.frombuffer(image_data, dtype=np.uint8).reshape((height, width))
            else:
                # JPEG
                image = cv2.imdecode(np.frombuffer(image_data, dtype=np.uint8), cv2.IMREAD_GRAYSCALE)

            if image is None:
                return None, None

            metadata = {
                "width": width,
                "height": height,
                "data_length": data_length,
                "timestamp": datetime.now().strftime("%Y%m%d_%H%M%S_%f")
            }

            return image, metadata

        except Exception as e:
            print(f"✗ 接收失败: {e}")
            return None, None

    def analyze_image(self, image):
        """
        分析图像特征

        Args:
            image: 输入图像

        Returns:
            dict: 分析结果
        """
        analysis = {}

        # 基本统计
        analysis['mean'] = float(np.mean(image))
        analysis['std'] = float(np.std(image))
        analysis['min'] = int(np.min(image))
        analysis['max'] = int(np.max(image))
        analysis['median'] = float(np.median(image))

        # 亮度分布
        hist, bins = np.histogram(image, bins=256, range=(0, 256))
        analysis['histogram'] = hist.tolist()

        # 对比度评估
        analysis['contrast'] = analysis['max'] - analysis['min']

        # 判断曝光情况
        if analysis['mean'] > 200:
            analysis['exposure'] = "过曝"
        elif analysis['mean'] < 50:
            analysis['exposure'] = "欠曝"
        else:
            analysis['exposure'] = "正常"

        return analysis

    def save_debug_frame(self, image, result_id, confidence, analysis):
        """
        保存调试帧（包括原图、分析图、统计信息）

        Args:
            image: 原始图像
            result_id: 识别结果ID
            confidence: 置信度
            analysis: 分析结果
        """
        self.frame_count += 1
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S_%f")[:-3]

        # 保存原始图像
        img_filename = f"frame_{self.frame_count:04d}_{GESTURE_NAMES[result_id]}_conf{confidence:.1f}.png"
        img_path = os.path.join(self.save_dir, "images", img_filename)
        cv2.imwrite(img_path, image)

        # 创建可视化分析图
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))

        # 1. 原始图像
        axes[0, 0].imshow(image, cmap='gray')
        axes[0, 0].set_title(f"原始图像 ({image.shape[1]}x{image.shape[0]})")
        axes[0, 0].axis('off')

        # 2. 亮度直方图
        axes[0, 1].hist(image.ravel(), bins=256, range=(0, 256), color='blue', alpha=0.7)
        axes[0, 1].axvline(analysis['mean'], color='red', linestyle='--', label=f'均值: {analysis["mean"]:.1f}')
        axes[0, 1].axvline(analysis['median'], color='green', linestyle='--', label=f'中位数: {analysis["median"]:.1f}')
        axes[0, 1].set_title("亮度分布")
        axes[0, 1].set_xlabel("像素值")
        axes[0, 1].set_ylabel("频率")
        axes[0, 1].legend()
        axes[0, 1].grid(True, alpha=0.3)

        # 3. 归一化后的图像（模拟训练预处理）
        normalized = (image.astype(np.float32) / 255.0 - 0.5) / 0.5  # [-1, 1]
        normalized_display = ((normalized + 1.0) / 2.0 * 255).astype(np.uint8)  # 转回[0, 255]显示
        axes[1, 0].imshow(normalized_display, cmap='gray')
        axes[1, 0].set_title("预处理后图像（归一化到[-1,1]）")
        axes[1, 0].axis('off')

        # 4. 统计信息
        info_text = f"""
识别结果: {GESTURE_NAMES[result_id]} (ID: {result_id})
置信度: {confidence:.2f}%

图像统计:
- 尺寸: {image.shape[1]}x{image.shape[0]}
- 均值: {analysis['mean']:.2f}
- 标准差: {analysis['std']:.2f}
- 最小值: {analysis['min']}
- 最大值: {analysis['max']}
- 中位数: {analysis['median']:.2f}
- 对比度: {analysis['contrast']}
- 曝光: {analysis['exposure']}

训练时期望:
- 归一化范围: [-1, 1]
- 输入尺寸: 64x64
        """
        axes[1, 1].text(0.1, 0.5, info_text, fontsize=10, family='monospace',
                       verticalalignment='center')
        axes[1, 1].axis('off')

        plt.tight_layout()

        # 保存分析图
        analysis_filename = f"analysis_{self.frame_count:04d}.png"
        analysis_path = os.path.join(self.save_dir, "analysis", analysis_filename)
        plt.savefig(analysis_path, dpi=100)
        plt.close()

        print(f"✓ 帧 {self.frame_count}: {GESTURE_NAMES[result_id]} ({confidence:.1f}%) "
              f"| 均值:{analysis['mean']:.1f} | {analysis['exposure']}")

        # 记录到日志
        self.results_log.append({
            'frame': self.frame_count,
            'timestamp': timestamp,
            'result_id': result_id,
            'result_name': GESTURE_NAMES[result_id],
            'confidence': confidence,
            'analysis': analysis
        })

    def save_summary(self):
        """保存调试总结"""
        if len(self.results_log) == 0:
            return

        summary_path = os.path.join(self.save_dir, "summary.txt")

        with open(summary_path, 'w', encoding='utf-8') as f:
            f.write("=" * 60 + "\n")
            f.write("推理调试总结\n")
            f.write("=" * 60 + "\n\n")

            f.write(f"总帧数: {len(self.results_log)}\n\n")

            # 统计各手势识别次数
            f.write("识别结果统计:\n")
            gesture_counts = {}
            for log in self.results_log:
                gid = log['result_id']
                gesture_counts[gid] = gesture_counts.get(gid, 0) + 1

            for gid in sorted(gesture_counts.keys()):
                count = gesture_counts[gid]
                percentage = count / len(self.results_log) * 100
                f.write(f"  {gid} ({GESTURE_NAMES[gid]}): {count} 次 ({percentage:.1f}%)\n")

            f.write("\n")

            # 亮度统计
            mean_brightness = np.mean([log['analysis']['mean'] for log in self.results_log])
            std_brightness = np.std([log['analysis']['mean'] for log in self.results_log])

            f.write(f"平均亮度: {mean_brightness:.2f} ± {std_brightness:.2f}\n")

            # 曝光情况统计
            exposure_counts = {}
            for log in self.results_log:
                exp = log['analysis']['exposure']
                exposure_counts[exp] = exposure_counts.get(exp, 0) + 1

            f.write("\n曝光情况:\n")
            for exp, count in exposure_counts.items():
                percentage = count / len(self.results_log) * 100
                f.write(f"  {exp}: {count} 次 ({percentage:.1f}%)\n")

            f.write("\n" + "=" * 60 + "\n")
            f.write("详细日志:\n")
            f.write("=" * 60 + "\n\n")

            for log in self.results_log:
                f.write(f"帧 {log['frame']}: {log['result_name']} "
                       f"(置信度: {log['confidence']:.1f}%) "
                       f"| 亮度: {log['analysis']['mean']:.1f} "
                       f"| {log['analysis']['exposure']}\n")

        print(f"\n✓ 总结已保存到: {summary_path}")

        # 生成汇总可视化
        self.plot_summary()

    def plot_summary(self):
        """生成汇总图表"""
        if len(self.results_log) == 0:
            return

        fig, axes = plt.subplots(2, 2, figsize=(14, 10))

        # 1. 识别结果分布
        gesture_counts = {}
        for log in self.results_log:
            gid = log['result_id']
            gesture_counts[gid] = gesture_counts.get(gid, 0) + 1

        gestures = [GESTURE_NAMES[gid] for gid in sorted(gesture_counts.keys())]
        counts = [gesture_counts[gid] for gid in sorted(gesture_counts.keys())]

        axes[0, 0].bar(gestures, counts, color='skyblue')
        axes[0, 0].set_title("识别结果分布")
        axes[0, 0].set_xlabel("手势")
        axes[0, 0].set_ylabel("次数")
        axes[0, 0].tick_params(axis='x', rotation=45)
        axes[0, 0].grid(True, alpha=0.3)

        # 2. 置信度分布
        confidences = [log['confidence'] for log in self.results_log]
        axes[0, 1].hist(confidences, bins=20, color='orange', alpha=0.7)
        axes[0, 1].axvline(np.mean(confidences), color='red', linestyle='--',
                          label=f'平均: {np.mean(confidences):.1f}%')
        axes[0, 1].set_title("置信度分布")
        axes[0, 1].set_xlabel("置信度 (%)")
        axes[0, 1].set_ylabel("频率")
        axes[0, 1].legend()
        axes[0, 1].grid(True, alpha=0.3)

        # 3. 亮度变化趋势
        frames = [log['frame'] for log in self.results_log]
        brightness = [log['analysis']['mean'] for log in self.results_log]
        axes[1, 0].plot(frames, brightness, marker='o', markersize=3, linewidth=1)
        axes[1, 0].axhline(np.mean(brightness), color='red', linestyle='--',
                          label=f'平均: {np.mean(brightness):.1f}')
        axes[1, 0].set_title("亮度变化趋势")
        axes[1, 0].set_xlabel("帧序号")
        axes[1, 0].set_ylabel("平均亮度")
        axes[1, 0].legend()
        axes[1, 0].grid(True, alpha=0.3)

        # 4. 置信度变化趋势
        axes[1, 1].plot(frames, confidences, marker='o', markersize=3, linewidth=1, color='green')
        axes[1, 1].axhline(np.mean(confidences), color='red', linestyle='--',
                          label=f'平均: {np.mean(confidences):.1f}%')
        axes[1, 1].set_title("置信度变化趋势")
        axes[1, 1].set_xlabel("帧序号")
        axes[1, 1].set_ylabel("置信度 (%)")
        axes[1, 1].legend()
        axes[1, 1].grid(True, alpha=0.3)

        plt.tight_layout()

        summary_plot_path = os.path.join(self.save_dir, "summary_plot.png")
        plt.savefig(summary_plot_path, dpi=150)
        plt.close()

        print(f"✓ 汇总图表已保存到: {summary_plot_path}")

    def run(self, max_frames=100):
        """
        运行调试会话

        Args:
            max_frames: 最多采集帧数
        """
        print(f"\n开始推理调试 (最多 {max_frames} 帧)")
        print("=" * 60)

        last_result = None
        last_image = None

        try:
            while self.frame_count < max_frames:
                # 读取串口数据
                line = self.read_line()

                if line.startswith("$RESULT"):
                    # 识别结果
                    parts = line.split(',')
                    if len(parts) == 3:
                        gesture_id = int(parts[1])
                        confidence = float(parts[2])
                        last_result = (gesture_id, confidence)

                        # 如果有图像，保存调试帧
                        if last_image is not None:
                            analysis = self.analyze_image(last_image)
                            self.save_debug_frame(last_image, gesture_id, confidence, analysis)
                            last_image = None

                elif line.startswith("$IMG"):
                    # 图像数据
                    # 将指针回退，让receive_image重新读取
                    self.ser.reset_input_buffer()
                    # 重新接收
                    time.sleep(0.01)
                    continue

                elif line.startswith("$FPS"):
                    print(f"← {line}")

                # 尝试接收图像
                image, metadata = self.receive_image()
                if image is not None and 'width' in (metadata or {}):
                    last_image = image

                time.sleep(0.01)

        except KeyboardInterrupt:
            print("\n\n用户中断")

        finally:
            print("\n" + "=" * 60)
            print("调试会话结束")
            print("=" * 60)

            # 保存总结
            self.save_summary()

def main():
    import argparse

    parser = argparse.ArgumentParser(description="推理调试工具")
    parser.add_argument("--port", type=str, required=True, help="串口号")
    parser.add_argument("--baudrate", type=int, default=921600, help="波特率")
    parser.add_argument("--frames", type=int, default=100, help="采集帧数")
    parser.add_argument("--save-dir", type=str, default="debug_inference", help="保存目录")

    args = parser.parse_args()

    debugger = InferenceDebugger(args.port, args.baudrate, args.save_dir)

    if not debugger.connect():
        return

    try:
        debugger.run(max_frames=args.frames)
    finally:
        debugger.disconnect()

if __name__ == "__main__":
    main()
