#!/usr/bin/env python3
"""
PC 端实时手势识别 - 使用 USB 摄像头作为输入源

模拟 STM32 上的推理流程：抓帧 -> 灰度 -> 中心裁剪 -> resize 64x64
-> 归一化 (x/255-0.5)/0.5 -> 模型推理 -> softmax -> 显示

用法:
    python realtime_inference_webcam.py --model gesture_v1_5.pth
    python realtime_inference_webcam.py --model gesture_v2_5_augment_int8.onnx
    python realtime_inference_webcam.py --model gesture_v1_5_sobel.onnx --preprocess sobel --sobel-threshold 40 --roi-scale 0.8
    python realtime_inference_webcam.py --model gesture_v1_5.pth --camera 1
"""

import argparse
import os
import time
from collections import deque
from datetime import datetime

import cv2
import numpy as np
import torch
import torch.nn as nn
import torchvision.transforms as transforms

try:
    import onnxruntime as ort
except ImportError:
    ort = None

# 复用 realtime_inference.py 里已经定义好的模型类（softmax 和 FPS 都是对的）
from realtime_inference import (
    GestureNet,
    GestureNetV2,
    GestureNetV2Lite,
    GestureNetV2Spatial,
)

# 默认 5 类（对应 train_model.py 的训练设定）
DEFAULT_GESTURE_NAMES = {
    0: "fist", 1: "palm", 2: "thumbs_up", 3: "ok", 4: "nothing"
}

GESTURE_COLORS = {
    0: (255, 100, 100),   # fist - 蓝
    1: (100, 255, 100),   # palm - 绿
    2: (100, 100, 255),   # thumbs_up - 红
    3: (100, 255, 255),   # ok - 黄
    4: (180, 180, 180),   # nothing - 灰
    5: (200, 100, 255),
    6: (255, 200, 100),
    7: (100, 200, 255),
    8: (200, 200, 200),
}


def load_model(model_path, num_classes, device):
    """按顺序尝试三种架构加载 .pth"""
    print(f"加载模型: {model_path}")
    print(f"设备: {device}")

    checkpoint = torch.load(model_path, map_location=device, weights_only=False)
    if isinstance(checkpoint, dict) and "state_dict" in checkpoint:
        state_dict = checkpoint["state_dict"]
    else:
        state_dict = checkpoint

    last_err = None
    for cls in (GestureNet, GestureNetV2Lite, GestureNetV2, GestureNetV2Spatial):
        try:
            m = cls(num_classes=num_classes)
            m.load_state_dict(state_dict)
            m.to(device).eval()
            print(f"  -> 成功匹配架构: {cls.__name__}")
            return m
        except Exception as e:
            last_err = e
            print(f"  -> {cls.__name__} 不匹配: {str(e)[:80]}")

    raise RuntimeError(f"所有架构均加载失败: {last_err}")


class TorchRunner:
    def __init__(self, model_path, num_classes, device):
        self.device = device
        self.model = load_model(model_path, num_classes, device)

    @torch.no_grad()
    def infer(self, input_array):
        tensor = torch.from_numpy(input_array)
        logits = self.model(tensor.to(self.device))
        probs = torch.softmax(logits, dim=1)[0].cpu().numpy()
        top_id = int(np.argmax(probs))
        return top_id, float(probs[top_id]), probs


class OnnxRunner:
    def __init__(self, model_path):
        if ort is None:
            raise RuntimeError("onnxruntime 未安装，请先执行: pip install onnxruntime")

        providers = ["CPUExecutionProvider"]
        self.session = ort.InferenceSession(model_path, providers=providers)
        self.input_name = self.session.get_inputs()[0].name
        self.output_name = self.session.get_outputs()[0].name
        self.layout = self._infer_layout()
        print(f"加载 ONNX 模型: {model_path}")
        print(f"  -> 输入: {self.input_name}, layout={self.layout}")
        print(f"  -> 输出: {self.output_name}")

    def _infer_layout(self):
        shape = self.session.get_inputs()[0].shape
        if len(shape) == 4:
            if shape[1] == 1:
                return "NCHW"
            if shape[3] == 1:
                return "NHWC"
        return "NCHW"

    def infer(self, input_array):
        if self.layout == "NHWC":
            input_array = np.transpose(input_array, (0, 2, 3, 1))

        output = self.session.run([self.output_name], {self.input_name: input_array})[0][0]
        probs = softmax_np(output)
        top_id = int(np.argmax(probs))
        return top_id, float(probs[top_id]), probs


def softmax_np(logits):
    logits = logits.astype(np.float32)
    logits = logits - np.max(logits)
    exp = np.exp(logits)
    return exp / np.sum(exp)


def create_runner(model_path, num_classes, device):
    suffix = os.path.splitext(model_path)[1].lower()
    if suffix == ".onnx":
        return OnnxRunner(model_path)
    if suffix == ".pth":
        return TorchRunner(model_path, num_classes, device)
    raise ValueError(f"不支持的模型格式: {suffix}，请使用 .pth 或 .onnx")


def center_square_crop(frame_bgr, roi_scale=1.0):
    """从摄像头帧中心裁剪一个正方形 ROI（保证 STM32 那边的 64x64 不会因比例失真）"""
    h, w = frame_bgr.shape[:2]
    side = min(h, w)
    y0 = (h - side) // 2
    x0 = (w - side) // 2
    if roi_scale < 1.0:
        scaled_side = max(1, int(side * roi_scale))
        x0 += (side - scaled_side) // 2
        y0 += (side - scaled_side) // 2
        side = scaled_side
    return frame_bgr[y0:y0 + side, x0:x0 + side], (x0, y0, side)


def sobel_magnitude_u8(gray, threshold=40, blur=True):
    """Soft Sobel: blur -> |Gx| + |Gy| -> suppress weak edges."""
    src = cv2.GaussianBlur(gray, (3, 3), 0) if blur else gray
    gx = cv2.Sobel(src, cv2.CV_16S, 1, 0, ksize=3)
    gy = cv2.Sobel(src, cv2.CV_16S, 0, 1, ksize=3)
    mag = np.abs(gx).astype(np.int32) + np.abs(gy).astype(np.int32)
    if threshold > 0:
        mag[mag < threshold] = 0
    return np.clip(mag, 0, 255).astype(np.uint8)


def canny_u8(gray):
    blurred = cv2.GaussianBlur(gray, (3, 3), 0)
    return cv2.Canny(blurred, 50, 150)


def preprocess(roi_bgr, mode, sobel_threshold, sobel_no_blur, per_image_norm):
    """Preprocess ROI to the model input. mode must match the model training input."""
    gray = cv2.cvtColor(roi_bgr, cv2.COLOR_BGR2GRAY)
    small = cv2.resize(gray, (64, 64), interpolation=cv2.INTER_AREA)

    if mode == "gray":
        model_image = small
    elif mode == "sobel":
        model_image = sobel_magnitude_u8(
            small,
            threshold=sobel_threshold,
            blur=not sobel_no_blur,
        )
    elif mode == "canny":
        model_image = canny_u8(small)
    else:
        raise ValueError(f"Unsupported preprocess mode: {mode}")

    arr = model_image.astype(np.float32)
    if per_image_norm:
        mean = float(np.mean(arr))
        std = float(np.std(arr))
        arr = (arr - mean) / max(std, 1.0)
        arr = np.clip(arr / 3.0, -1.0, 1.0)
    else:
        arr = arr / 255.0
        arr = (arr - 0.5) / 0.5

    input_array = arr.reshape(1, 1, 64, 64).astype(np.float32)
    return model_image, input_array


def apply_stability(raw_id, raw_probs, history, num_classes,
                    vote_window, conf_threshold, margin_threshold,
                    unknown_id):
    """Apply confidence/margin gates and optional majority vote."""
    sorted_probs = np.sort(raw_probs)
    top1 = float(sorted_probs[-1]) if len(sorted_probs) else 0.0
    top2 = float(sorted_probs[-2]) if len(sorted_probs) > 1 else 0.0
    accepted_id = raw_id

    if top1 < conf_threshold or (top1 - top2) < margin_threshold:
        accepted_id = unknown_id

    if vote_window <= 1:
        return accepted_id, top1, accepted_id, top1 - top2

    history.append(accepted_id)
    counts = np.bincount(np.array(history, dtype=np.int32), minlength=num_classes)
    voted_id = int(np.argmax(counts))
    voted_conf = counts[voted_id] / len(history)
    return voted_id, voted_conf, accepted_id, top1 - top2


def draw_main(display, gesture_name, confidence, infer_fps, capture_fps, paused,
              preprocess_mode, raw_name=None):
    """在主显示窗口绘制信息覆盖层"""
    h, w = display.shape[:2]
    color = (0, 200, 255)
    overlay = display.copy()

    # Semi-transparent strips keep status readable without hiding the camera view.
    cv2.rectangle(overlay, (0, 0), (w, 58), (0, 0, 0), -1)
    cv2.rectangle(overlay, (0, h - 30), (w, h), (0, 0, 0), -1)
    cv2.addWeighted(overlay, 0.45, display, 0.55, 0, display)

    cv2.putText(display, gesture_name.upper(), (15, 42),
                cv2.FONT_HERSHEY_SIMPLEX, 1.15, color, 2)
    cv2.putText(display, f"{confidence * 100:5.1f}%", (w - 150, 40),
                cv2.FONT_HERSHEY_SIMPLEX, 0.85, color, 2)

    cv2.putText(display, f"infer: {infer_fps:5.1f} fps", (10, h - 8),
                cv2.FONT_HERSHEY_SIMPLEX, 0.55, (255, 255, 255), 1)
    cv2.putText(display, f"cap: {capture_fps:5.1f} fps", (180, h - 8),
                cv2.FONT_HERSHEY_SIMPLEX, 0.55, (255, 255, 255), 1)
    cv2.putText(display, f"mode: {preprocess_mode}", (330, h - 8),
                cv2.FONT_HERSHEY_SIMPLEX, 0.55, (255, 255, 255), 1)
    if raw_name and raw_name != gesture_name:
        cv2.putText(display, f"raw:{raw_name}", (w - 180, h - 8),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.55, (0, 255, 255), 1)
    if paused:
        cv2.putText(display, "PAUSED", (w - 130, h - 8),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.55, (0, 255, 255), 2)
    return display


def draw_bars(all_probs, gesture_names, current_id):
    """右侧置信度条形图"""
    n = len(gesture_names)
    row_h = 38
    canvas_w = 360
    canvas_h = n * row_h + 20
    canvas = np.zeros((canvas_h, canvas_w, 3), dtype=np.uint8)

    bar_x0 = 110
    bar_x1 = canvas_w - 60
    bar_max = bar_x1 - bar_x0

    for i, gid in enumerate(sorted(gesture_names.keys())):
        y = 10 + i * row_h
        prob = all_probs[gid] if gid < len(all_probs) else 0.0
        color = GESTURE_COLORS.get(gid, (200, 200, 200))
        if gid != current_id:
            color = tuple(int(c * 0.45) for c in color)

        bar_len = int(bar_max * float(prob))
        cv2.rectangle(canvas, (bar_x0, y), (bar_x0 + bar_len, y + 22), color, -1)
        cv2.rectangle(canvas, (bar_x0, y), (bar_x1, y + 22), (90, 90, 90), 1)

        label = gesture_names[gid][:9]
        cv2.putText(canvas, f"{gid}:{label}", (5, y + 17),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 1)
        cv2.putText(canvas, f"{prob * 100:5.1f}%", (bar_x1 + 2, y + 17),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.45, (255, 255, 255), 1)
    return canvas


def main():
    ap = argparse.ArgumentParser(description="PC 端 USB 摄像头实时手势识别")
    ap.add_argument("--model", required=True, help=".pth 或 .onnx 模型文件路径")
    ap.add_argument("--camera", type=int, default=0, help="摄像头索引（默认 0）")
    ap.add_argument("--num-classes", type=int, default=5, help="类别数（默认 5）")
    ap.add_argument("--names", type=str, default=None,
                    help="逗号分隔的类别名，例如 fist,palm,thumbs_up,ok,nothing")
    ap.add_argument("--save-dir", type=str, default="webcam_frames", help="保存目录")
    ap.add_argument("--width", type=int, default=640, help="摄像头采集宽度")
    ap.add_argument("--height", type=int, default=480, help="摄像头采集高度")
    ap.add_argument("--roi-scale", type=float, default=1.0,
                    help="中心正方形 ROI 二次裁剪比例，例如 0.8 表示只取中心 80%%")
    ap.add_argument("--preprocess", choices=["gray", "sobel", "canny"], default="gray",
                    help="输入预处理方式；必须和模型训练方式一致")
    ap.add_argument("--per-image-norm", action="store_true",
                    help="对每张 64x64 输入做 mean/std 标准化，再裁剪到 [-1,1]")
    ap.add_argument("--sobel-threshold", type=int, default=40,
                    help="Sobel 弱边缘抑制阈值，0 表示关闭")
    ap.add_argument("--sobel-no-blur", action="store_true",
                    help="关闭 Sobel 前的 3x3 GaussianBlur")
    ap.add_argument("--vote-window", type=int, default=1,
                    help="最近 N 帧多数投票，1 表示关闭")
    ap.add_argument("--conf-threshold", type=float, default=0.0,
                    help="top1 置信度低于该值时输出 unknown/nothing")
    ap.add_argument("--margin-threshold", type=float, default=0.0,
                    help="top1-top2 小于该值时输出 unknown/nothing")
    ap.add_argument("--unknown-id", type=int, default=None,
                    help="拒识类别 ID，默认使用最后一类")
    args = ap.parse_args()

    if args.names:
        names = [s.strip() for s in args.names.split(",") if s.strip()]
        gesture_names = {i: n for i, n in enumerate(names)}
    else:
        gesture_names = {k: v for k, v in DEFAULT_GESTURE_NAMES.items() if k < args.num_classes}

    unknown_id = args.unknown_id if args.unknown_id is not None else args.num_classes - 1
    if unknown_id < 0 or unknown_id >= args.num_classes:
        raise ValueError("--unknown-id 必须在类别范围内")
    if not (0.1 <= args.roi_scale <= 1.0):
        raise ValueError("--roi-scale 必须在 0.1 到 1.0 之间")

    os.makedirs(args.save_dir, exist_ok=True)
    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    runner = create_runner(args.model, args.num_classes, device)

    # 打开摄像头（Windows 上 CAP_DSHOW 启动更快）
    cap = cv2.VideoCapture(args.camera, cv2.CAP_DSHOW)
    if not cap.isOpened():
        cap = cv2.VideoCapture(args.camera)
    if not cap.isOpened():
        raise RuntimeError(f"无法打开摄像头 {args.camera}")
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, args.width)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, args.height)
    print(f"摄像头 {args.camera} 已打开: "
          f"{int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))}x"
          f"{int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))}")

    cv2.namedWindow("Gesture", cv2.WINDOW_NORMAL)
    cv2.namedWindow("Confidence", cv2.WINDOW_NORMAL)
    cv2.namedWindow("Model Input (64x64)", cv2.WINDOW_NORMAL)
    cv2.resizeWindow("Gesture", 640, 640)
    cv2.resizeWindow("Confidence", 360, 240)
    cv2.resizeWindow("Model Input (64x64)", 256, 256)

    print("\n按键: q 退出 | p 暂停推理 | s 保存当前帧\n")
    print(f"预处理: {args.preprocess}")
    print(f"ROI scale: {args.roi_scale:.2f}")
    print(f"Per-image norm: {args.per_image_norm}")
    if args.preprocess == "sobel":
        print(f"Sobel阈值: {args.sobel_threshold}")
        print(f"Sobel前模糊: {not args.sobel_no_blur}")
    print(f"投票窗口: {args.vote_window}")
    print(f"置信度阈值: {args.conf_threshold:.2f}")
    print(f"margin阈值: {args.margin_threshold:.2f}")
    print(f"拒识ID: {unknown_id} ({gesture_names.get(unknown_id, f'id_{unknown_id}')})\n")

    paused = False
    frame_count = 0
    cap_times = deque(maxlen=30)
    infer_times = deque(maxlen=30)
    last_cap_t = time.time()

    last_top_id = 4
    last_conf = 0.0
    last_probs = np.zeros(args.num_classes, dtype=np.float32)
    last_input = np.zeros((64, 64), dtype=np.uint8)
    vote_history = deque(maxlen=max(args.vote_window, 1))
    last_raw_id = last_top_id
    last_raw_conf = last_conf
    last_margin = 0.0

    try:
        while True:
            ok, frame = cap.read()
            if not ok:
                print("读取摄像头失败，退出")
                break

            now = time.time()
            cap_times.append(now - last_cap_t)
            last_cap_t = now

            roi_bgr, (x0, y0, side) = center_square_crop(frame, args.roi_scale)
            small, input_array = preprocess(
                roi_bgr,
                args.preprocess,
                args.sobel_threshold,
                args.sobel_no_blur,
                args.per_image_norm,
            )
            last_input = small

            if not paused:
                t0 = time.time()
                last_raw_id, last_raw_conf, last_probs = runner.infer(input_array)
                last_top_id, last_conf, _accepted_id, last_margin = apply_stability(
                    last_raw_id,
                    last_probs,
                    vote_history,
                    args.num_classes,
                    args.vote_window,
                    args.conf_threshold,
                    args.margin_threshold,
                    unknown_id,
                )
                infer_times.append(time.time() - t0)

            # 计算 FPS（取窗口内时间均值的倒数，比逐帧倒数稳定得多）
            cap_fps = 1.0 / max(np.mean(cap_times), 1e-6) if cap_times else 0.0
            infer_fps = 1.0 / max(np.mean(infer_times), 1e-6) if infer_times else 0.0

            # 主窗口：原始帧 + ROI 框
            display = frame.copy()
            cv2.rectangle(display, (x0, y0), (x0 + side, y0 + side), (0, 200, 255), 2)
            name = gesture_names.get(last_top_id, f"id_{last_top_id}")
            raw_name = gesture_names.get(last_raw_id, f"id_{last_raw_id}")
            display = draw_main(display, name, last_conf, infer_fps, cap_fps, paused,
                                args.preprocess, raw_name)

            # 副窗口
            bars = draw_bars(last_probs, gesture_names, last_top_id)
            input_vis = cv2.cvtColor(last_input, cv2.COLOR_GRAY2BGR)
            input_vis = cv2.resize(input_vis, (256, 256), interpolation=cv2.INTER_NEAREST)

            cv2.imshow("Gesture", display)
            cv2.imshow("Confidence", bars)
            cv2.imshow("Model Input (64x64)", input_vis)

            frame_count += 1
            print(f"\rframe {frame_count:5d} | {name:>10s} | "
                  f"{last_conf * 100:5.1f}% | raw {raw_name:>10s} "
                  f"{last_raw_conf * 100:5.1f}% | margin {last_margin:4.2f} | "
                  f"infer {infer_fps:5.1f} | cap {cap_fps:5.1f}",
                  end="", flush=True)

            key = cv2.waitKey(1) & 0xFF
            if key == ord("q"):
                break
            elif key == ord("p"):
                paused = not paused
                print(f"\n[{'暂停' if paused else '继续'}]")
            elif key == ord("s"):
                ts = datetime.now().strftime("%Y%m%d_%H%M%S_%f")[:-3]
                fn = f"{ts}_{name}_{int(last_conf * 100)}.png"
                cv2.imwrite(os.path.join(args.save_dir, fn), last_input)
                print(f"\n已保存 -> {fn}")

    except KeyboardInterrupt:
        pass
    finally:
        cap.release()
        cv2.destroyAllWindows()
        print(f"\n总帧数: {frame_count}")


if __name__ == "__main__":
    main()
