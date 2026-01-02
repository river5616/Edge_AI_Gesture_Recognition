#!/usr/bin/env python3
"""
推理调试工具 - 实时接收STM32推理时的图像并保存分析
用于诊断光照、预处理等问题
同时使用 ONNX 模型在 PC 端推理，对比 STM32 和 PC 的识别结果
"""

import serial
import numpy as np
import cv2
import os
import time
from datetime import datetime
import matplotlib.pyplot as plt

# 尝试导入 ONNX Runtime
try:
    import onnxruntime as ort
    ONNX_AVAILABLE = True
except ImportError:
    print("⚠️ 警告: onnxruntime 未安装，将无法使用 PC 端对比推理")
    print("   安装命令: pip install onnxruntime")
    ONNX_AVAILABLE = False

# 手势类别
GESTURE_NAMES = {
    0: "fist", 1: "palm", 2: "thumbs_up", 3: "ok", 4: "v_sign",
    5: "point", 6: "three", 7: "four", 8: "nothing"
}

class InferenceDebugger:
    def __init__(self, port, baudrate=921600, save_dir="debug_inference", onnx_model_path="gesture_model.onnx"):
        """
        初始化调试器

        Args:
            port: 串口号
            baudrate: 波特率
            save_dir: 调试数据保存目录
            onnx_model_path: ONNX 模型路径（用于 PC 端对比推理）
        """
        self.port = port
        self.baudrate = baudrate
        self.save_dir = save_dir
        self.ser = None

        # ONNX 模型
        self.onnx_session = None
        self.onnx_enabled = False

        if ONNX_AVAILABLE and os.path.exists(onnx_model_path):
            try:
                self.onnx_session = ort.InferenceSession(onnx_model_path)
                self.onnx_enabled = True
                print(f"✓ ONNX 模型已加载: {onnx_model_path}")
            except Exception as e:
                print(f"⚠️ 加载 ONNX 模型失败: {e}")
        elif ONNX_AVAILABLE and not os.path.exists(onnx_model_path):
            print(f"⚠️ ONNX 模型文件不存在: {onnx_model_path}")

        # 创建保存目录
        os.makedirs(save_dir, exist_ok=True)
        os.makedirs(os.path.join(save_dir, "images"), exist_ok=True)
        os.makedirs(os.path.join(save_dir, "analysis"), exist_ok=True)

        # 统计信息
        self.frame_count = 0
        self.results_log = []

        # 对比统计
        self.match_count = 0  # STM32 和 PC 识别一致的次数
        self.mismatch_count = 0  # 不一致的次数

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
                print(f"✗ 图像头格式错误: {line.strip()}")
                return None, None

            try:
                width = int(parts[1])
                height = int(parts[2])
                data_length = int(parts[3])
            except (ValueError, IndexError) as e:
                print(f"✗ 图像头解析失败: {line.strip()} | 错误: {e}")
                return None, None

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

    def run_onnx_inference(self, image):
        """
        使用 ONNX 模型在 PC 端运行推理

        Args:
            image: 输入图像 (numpy array, 灰度, 0-255)

        Returns:
            pc_gesture_id: PC 端识别的手势ID
            pc_confidence: PC 端的置信度
            pc_all_outputs: PC 端的所有输出（9个logits）
        """
        if not self.onnx_enabled:
            return None, None, None

        try:
            # 预处理：与 STM32 完全一致
            # 归一化到 [-1, 1]
            normalized = (image.astype(np.float32) / 255.0 - 0.5) / 0.5

            # 调整维度 (1, 1, 64, 64) - (batch, channel, height, width)
            input_tensor = normalized.reshape(1, 1, 64, 64).astype(np.float32)

            # 运行推理
            input_name = self.onnx_session.get_inputs()[0].name
            output_name = self.onnx_session.get_outputs()[0].name
            outputs = self.onnx_session.run([output_name], {input_name: input_tensor})

            # 获取输出
            output = outputs[0][0]  # shape: (9,)

            # 找到最大值的索引
            pc_gesture_id = int(np.argmax(output))
            pc_confidence = float(output[pc_gesture_id])

            return pc_gesture_id, pc_confidence, output

        except Exception as e:
            print(f"✗ ONNX 推理失败: {e}")
            return None, None, None

    def save_debug_frame(self, image, stm32_result_id, stm32_confidence, analysis):
        """
        保存调试帧（包括原图、分析图、统计信息、PC端对比）

        Args:
            image: 原始图像
            stm32_result_id: STM32 识别结果ID
            stm32_confidence: STM32 置信度
            analysis: 分析结果
        """
        self.frame_count += 1
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S_%f")[:-3]

        # PC 端推理（如果启用）
        pc_gesture_id, pc_confidence, pc_all_outputs = self.run_onnx_inference(image)

        # 判断是否一致
        match = False
        if pc_gesture_id is not None:
            match = (pc_gesture_id == stm32_result_id)
            if match:
                self.match_count += 1
            else:
                self.mismatch_count += 1

        # 保存原始图像
        match_marker = "MATCH" if match else "DIFF" if pc_gesture_id is not None else "NOPC"
        img_filename = f"frame_{self.frame_count:04d}_{GESTURE_NAMES[stm32_result_id]}_stm{stm32_confidence:.1f}_{match_marker}.png"
        img_path = os.path.join(self.save_dir, "images", img_filename)
        cv2.imwrite(img_path, image)

        # 创建可视化分析图
        fig, axes = plt.subplots(2, 2, figsize=(14, 12))

        # 1. 原始图像
        axes[0, 0].imshow(image, cmap='gray')
        title_text = f"原始图像 ({image.shape[1]}x{image.shape[0]})"
        if not match and pc_gesture_id is not None:
            title_text += " ⚠️ STM32 与 PC 不一致"
        axes[0, 0].set_title(title_text, fontsize=12)
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

        # 4. 统计信息（包括PC端对比）
        if pc_gesture_id is not None:
            # 有PC端对比
            match_symbol = "✓ 一致" if match else "✗ 不一致"
            info_text = f"""
STM32 识别结果: {GESTURE_NAMES[stm32_result_id]} (ID: {stm32_result_id})
STM32 置信度: {stm32_confidence:.2f}

PC 识别结果: {GESTURE_NAMES[pc_gesture_id]} (ID: {pc_gesture_id})
PC 置信度: {pc_confidence:.2f}

对比结果: {match_symbol}

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
        else:
            # 无PC端对比
            info_text = f"""
STM32 识别结果: {GESTURE_NAMES[stm32_result_id]} (ID: {stm32_result_id})
STM32 置信度: {stm32_confidence:.2f}

PC 对比: 未启用

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

        # 控制台输出（包括对比结果）
        if pc_gesture_id is not None:
            if match:
                print(f"✓ 帧 {self.frame_count}: {GESTURE_NAMES[stm32_result_id]} "
                      f"(STM32:{stm32_confidence:.1f} = PC:{pc_confidence:.1f}) "
                      f"| 亮度:{analysis['mean']:.1f} | {analysis['exposure']}")
            else:
                print(f"✗ 帧 {self.frame_count}: STM32={GESTURE_NAMES[stm32_result_id]}({stm32_confidence:.1f}) "
                      f"vs PC={GESTURE_NAMES[pc_gesture_id]}({pc_confidence:.1f}) "
                      f"| 亮度:{analysis['mean']:.1f} | {analysis['exposure']}")
        else:
            print(f"✓ 帧 {self.frame_count}: {GESTURE_NAMES[stm32_result_id]} ({stm32_confidence:.1f}) "
                  f"| 均值:{analysis['mean']:.1f} | {analysis['exposure']}")

        # 记录到日志（包括PC结果）
        log_entry = {
            'frame': self.frame_count,
            'timestamp': timestamp,
            'stm32_result_id': stm32_result_id,
            'stm32_result_name': GESTURE_NAMES[stm32_result_id],
            'stm32_confidence': stm32_confidence,
            'analysis': analysis
        }

        if pc_gesture_id is not None:
            log_entry['pc_result_id'] = pc_gesture_id
            log_entry['pc_result_name'] = GESTURE_NAMES[pc_gesture_id]
            log_entry['pc_confidence'] = pc_confidence
            log_entry['match'] = match

        self.results_log.append(log_entry)

    def save_summary(self):
        """保存调试总结（包括STM32 vs PC对比）"""
        if len(self.results_log) == 0:
            return

        summary_path = os.path.join(self.save_dir, "summary.txt")

        with open(summary_path, 'w', encoding='utf-8') as f:
            f.write("=" * 60 + "\n")
            f.write("推理调试总结\n")
            f.write("=" * 60 + "\n\n")

            f.write(f"总帧数: {len(self.results_log)}\n\n")

            # STM32 vs PC 对比统计
            if self.onnx_enabled and (self.match_count + self.mismatch_count) > 0:
                f.write("=" * 60 + "\n")
                f.write("STM32 vs PC 对比统计:\n")
                f.write("=" * 60 + "\n")
                total_compared = self.match_count + self.mismatch_count
                match_rate = self.match_count / total_compared * 100
                f.write(f"对比总数: {total_compared}\n")
                f.write(f"一致: {self.match_count} 次 ({match_rate:.1f}%)\n")
                f.write(f"不一致: {self.mismatch_count} 次 ({100-match_rate:.1f}%)\n\n")

                # 列出所有不一致的帧
                if self.mismatch_count > 0:
                    f.write("不一致的帧:\n")
                    for log in self.results_log:
                        if 'match' in log and not log['match']:
                            f.write(f"  帧 {log['frame']}: STM32={log['stm32_result_name']}({log['stm32_confidence']:.1f}) "
                                   f"vs PC={log['pc_result_name']}({log['pc_confidence']:.1f})\n")
                    f.write("\n")
            else:
                f.write("(未启用 PC 端对比)\n\n")

            # 统计各手势识别次数（STM32）
            f.write("=" * 60 + "\n")
            f.write("STM32 识别结果统计:\n")
            f.write("=" * 60 + "\n")
            gesture_counts = {}
            for log in self.results_log:
                gid = log['stm32_result_id']
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
                if 'match' in log:
                    # 有PC对比
                    match_str = "✓" if log['match'] else "✗"
                    f.write(f"帧 {log['frame']}: {match_str} STM32={log['stm32_result_name']}({log['stm32_confidence']:.1f}) "
                           f"PC={log['pc_result_name']}({log['pc_confidence']:.1f}) "
                           f"| 亮度: {log['analysis']['mean']:.1f} "
                           f"| {log['analysis']['exposure']}\n")
                else:
                    # 无PC对比
                    f.write(f"帧 {log['frame']}: {log['stm32_result_name']} "
                           f"(置信度: {log['stm32_confidence']:.1f}) "
                           f"| 亮度: {log['analysis']['mean']:.1f} "
                           f"| {log['analysis']['exposure']}\n")

        print(f"\n✓ 总结已保存到: {summary_path}")

        # 生成汇总可视化
        self.plot_summary()

    def plot_summary(self):
        """生成汇总图表（包括STM32 vs PC对比）"""
        if len(self.results_log) == 0:
            return

        # 根据是否有PC对比决定布局
        has_pc_comparison = self.onnx_enabled and (self.match_count + self.mismatch_count) > 0

        if has_pc_comparison:
            fig, axes = plt.subplots(2, 3, figsize=(20, 10))
            axes = axes.flatten()
        else:
            fig, axes = plt.subplots(2, 2, figsize=(14, 10))
            axes = axes.flatten()

        # 1. STM32识别结果分布
        gesture_counts = {}
        for log in self.results_log:
            gid = log['stm32_result_id']
            gesture_counts[gid] = gesture_counts.get(gid, 0) + 1

        gestures = [GESTURE_NAMES[gid] for gid in sorted(gesture_counts.keys())]
        counts = [gesture_counts[gid] for gid in sorted(gesture_counts.keys())]

        axes[0].bar(gestures, counts, color='skyblue')
        axes[0].set_title("STM32 识别结果分布")
        axes[0].set_xlabel("手势")
        axes[0].set_ylabel("次数")
        axes[0].tick_params(axis='x', rotation=45)
        axes[0].grid(True, alpha=0.3)

        # 2. STM32置信度分布
        confidences = [log['stm32_confidence'] for log in self.results_log]
        axes[1].hist(confidences, bins=20, color='orange', alpha=0.7)
        axes[1].axvline(np.mean(confidences), color='red', linestyle='--',
                          label=f'平均: {np.mean(confidences):.1f}')
        axes[1].set_title("STM32 置信度分布")
        axes[1].set_xlabel("置信度 (logits)")
        axes[1].set_ylabel("频率")
        axes[1].legend()
        axes[1].grid(True, alpha=0.3)

        # 3. 亮度变化趋势
        frames = [log['frame'] for log in self.results_log]
        brightness = [log['analysis']['mean'] for log in self.results_log]
        axes[2].plot(frames, brightness, marker='o', markersize=3, linewidth=1)
        axes[2].axhline(np.mean(brightness), color='red', linestyle='--',
                          label=f'平均: {np.mean(brightness):.1f}')
        axes[2].set_title("亮度变化趋势")
        axes[2].set_xlabel("帧序号")
        axes[2].set_ylabel("平均亮度")
        axes[2].legend()
        axes[2].grid(True, alpha=0.3)

        # 4. STM32置信度变化趋势
        axes[3].plot(frames, confidences, marker='o', markersize=3, linewidth=1, color='green')
        axes[3].axhline(np.mean(confidences), color='red', linestyle='--',
                          label=f'平均: {np.mean(confidences):.1f}')
        axes[3].set_title("STM32 置信度变化趋势")
        axes[3].set_xlabel("帧序号")
        axes[3].set_ylabel("置信度 (logits)")
        axes[3].legend()
        axes[3].grid(True, alpha=0.3)

        # 如果有PC对比，添加额外的图表
        if has_pc_comparison:
            # 5. 一致性趋势（滑动窗口）
            matches = [1 if log.get('match', False) else 0 for log in self.results_log]
            window_size = min(10, len(matches))  # 10帧滑动窗口
            if window_size > 1:
                match_rate_moving = np.convolve(matches, np.ones(window_size)/window_size, mode='valid')
                match_frames = frames[window_size-1:]
                axes[4].plot(match_frames, match_rate_moving * 100, marker='o', markersize=3, linewidth=1, color='purple')
                axes[4].axhline(np.mean(matches) * 100, color='red', linestyle='--',
                              label=f'平均一致率: {np.mean(matches)*100:.1f}%')
            else:
                axes[4].bar(['一致', '不一致'], [self.match_count, self.mismatch_count], color=['green', 'red'])
            axes[4].set_title("STM32 vs PC 一致性")
            axes[4].set_xlabel("帧序号")
            axes[4].set_ylabel("一致率 (%)")
            axes[4].legend()
            axes[4].grid(True, alpha=0.3)

            # 6. PC vs STM32 置信度对比
            pc_confidences = [log.get('pc_confidence', 0) for log in self.results_log if 'pc_confidence' in log]
            stm32_conf_for_pc = [log['stm32_confidence'] for log in self.results_log if 'pc_confidence' in log]

            if len(pc_confidences) > 0:
                axes[5].scatter(stm32_conf_for_pc, pc_confidences, alpha=0.6, color='blue')
                axes[5].plot([min(stm32_conf_for_pc + pc_confidences), max(stm32_conf_for_pc + pc_confidences)],
                           [min(stm32_conf_for_pc + pc_confidences), max(stm32_conf_for_pc + pc_confidences)],
                           'r--', label='y=x (完全一致)')
                axes[5].set_title("STM32 vs PC 置信度对比")
                axes[5].set_xlabel("STM32 置信度")
                axes[5].set_ylabel("PC 置信度")
                axes[5].legend()
                axes[5].grid(True, alpha=0.3)

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

        if self.onnx_enabled:
            print("✓ ONNX 对比模式：已启用 (将对比 STM32 和 PC 端推理结果)")
        else:
            print("○ ONNX 对比模式：未启用")

        print("=" * 60)

        last_result = None
        last_image = None

        try:
            while self.frame_count < max_frames:
                # 读取串口数据
                line = self.read_line()

                if not line:
                    time.sleep(0.01)
                    continue

                if line.startswith("$RESULT"):
                    # 识别结果
                    try:
                        parts = line.split(',')
                        if len(parts) == 3:
                            gesture_id = int(parts[1])
                            confidence = float(parts[2])

                            # 如果有图像，保存调试帧
                            if last_image is not None:
                                analysis = self.analyze_image(last_image)
                                self.save_debug_frame(last_image, gesture_id, confidence, analysis)
                                last_image = None
                            else:
                                print(f"⚠ 收到结果但无图像: {gesture_id}, {confidence:.2f}")
                    except (ValueError, IndexError) as e:
                        print(f"✗ 解析结果失败: {line.strip()} | 错误: {e}")

                elif line.startswith("$IMG"):
                    # 图像头：解析参数并接收图像数据
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
                                    last_image = image
                                    # print(f"✓ 接收图像: {width}x{height}")
                                else:
                                    print(f"✗ 图像解码失败")
                            else:
                                print(f"✗ 数据长度不匹配: 期望{data_length}, 实际{len(image_data)}")
                    except (ValueError, IndexError) as e:
                        print(f"✗ 图像头解析失败: {line.strip()} | 错误: {e}")
                    except Exception as e:
                        print(f"✗ 图像接收失败: {e}")

                elif line.startswith("$FPS"):
                    pass  # 忽略FPS消息（已禁用）

                time.sleep(0.001)  # 短暂延时

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

    parser = argparse.ArgumentParser(description="推理调试工具（支持ONNX对比）")
    parser.add_argument("--port", type=str, required=True, help="串口号")
    parser.add_argument("--baudrate", type=int, default=921600, help="波特率")
    parser.add_argument("--frames", type=int, default=100, help="采集帧数")
    parser.add_argument("--save-dir", type=str, default="debug_inference", help="保存目录")
    parser.add_argument("--onnx-model", type=str, default="gesture_model.onnx",
                       help="ONNX模型路径（用于PC端对比推理）")

    args = parser.parse_args()

    debugger = InferenceDebugger(args.port, args.baudrate, args.save_dir, args.onnx_model)

    if not debugger.connect():
        return

    try:
        debugger.run(max_frames=args.frames)
    finally:
        debugger.disconnect()

if __name__ == "__main__":
    main()
