#!/usr/bin/env python3
"""
USB webcam dataset collector for the 5-class gesture model.

It saves images in the same folder style used by train_model.py:
    dataset_real/0_fist/fist_0001.png
    dataset_real/1_palm/palm_0001.png
    ...

The saved image is the exact 64x64 grayscale model input generated from:
camera frame -> center square crop -> optional center ROI scale -> resize 64x64.
"""

import argparse
import os
import time
from datetime import datetime

import cv2
import numpy as np


GESTURE_NAMES = {
    0: "fist",
    1: "palm",
    2: "thumbs_up",
    3: "ok",
    4: "nothing",
}


def center_square_crop(frame_bgr, roi_scale=1.0):
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


def preprocess_frame(frame_bgr, roi_scale=1.0):
    roi_bgr, roi_box = center_square_crop(frame_bgr, roi_scale)
    gray = cv2.cvtColor(roi_bgr, cv2.COLOR_BGR2GRAY)
    small = cv2.resize(gray, (64, 64), interpolation=cv2.INTER_AREA)
    return small, roi_box


def ensure_dataset_dirs(dataset_dir):
    os.makedirs(dataset_dir, exist_ok=True)
    for gesture_id, gesture_name in GESTURE_NAMES.items():
        os.makedirs(os.path.join(dataset_dir, f"{gesture_id}_{gesture_name}"), exist_ok=True)


def next_index(gesture_dir, gesture_name):
    max_index = 0
    for filename in os.listdir(gesture_dir):
        if not filename.lower().endswith(".png"):
            continue
        stem, _ext = os.path.splitext(filename)
        prefix = f"{gesture_name}_"
        if stem.startswith(prefix):
            suffix = stem[len(prefix):]
            if suffix.isdigit():
                max_index = max(max_index, int(suffix))
    return max_index + 1


def draw_overlay(frame, roi_box, gesture_name, collected, target, fps, paused, auto_mode):
    x0, y0, side = roi_box
    display = frame.copy()
    cv2.rectangle(display, (x0, y0), (x0 + side, y0 + side), (0, 220, 255), 2)

    h, w = display.shape[:2]
    overlay = display.copy()
    cv2.rectangle(overlay, (0, 0), (w, 62), (0, 0, 0), -1)
    cv2.rectangle(overlay, (0, h - 34), (w, h), (0, 0, 0), -1)
    cv2.addWeighted(overlay, 0.45, display, 0.55, 0, display)

    cv2.putText(display, gesture_name.upper(), (14, 42),
                cv2.FONT_HERSHEY_SIMPLEX, 1.1, (0, 220, 255), 2)
    cv2.putText(display, f"{collected}/{target}", (w - 160, 42),
                cv2.FONT_HERSHEY_SIMPLEX, 0.95, (0, 220, 255), 2)

    state = "AUTO" if auto_mode else "MANUAL"
    if paused:
        state += " PAUSED"
    cv2.putText(display, f"{state} | fps {fps:4.1f} | q quit | p pause | s save",
                (10, h - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.52, (255, 255, 255), 1)
    return display


def show_preview(cap, args, gesture_id, gesture_name):
    print(f"请现在摆好手势: {gesture_name} (ID {gesture_id})")
    print("预览窗口会持续显示画面。按 Enter/空格开始采集，按 q 退出。")

    cv2.namedWindow("Collect Webcam Data", cv2.WINDOW_NORMAL)
    cv2.namedWindow("Saved Input (64x64)", cv2.WINDOW_NORMAL)
    cv2.resizeWindow("Collect Webcam Data", 720, 560)
    cv2.resizeWindow("Saved Input (64x64)", 256, 256)

    last_frame_time = time.time()
    while True:
        ok, frame = cap.read()
        if not ok:
            print("Camera read failed.")
            return False

        now = time.time()
        fps = 1.0 / max(now - last_frame_time, 1e-6)
        last_frame_time = now

        small, roi_box = preprocess_frame(frame, args.roi_scale)
        display = draw_overlay(
            frame,
            roi_box,
            gesture_name,
            0,
            args.num_images,
            fps,
            True,
            args.interval > 0,
        )
        cv2.putText(display, "PREVIEW - Enter/Space start, q quit", (14, 88),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 255), 2)

        input_vis = cv2.cvtColor(small, cv2.COLOR_GRAY2BGR)
        input_vis = cv2.resize(input_vis, (256, 256), interpolation=cv2.INTER_NEAREST)
        cv2.imshow("Collect Webcam Data", display)
        cv2.imshow("Saved Input (64x64)", input_vis)

        key = cv2.waitKey(1) & 0xFF
        if key in (13, 10, 32):
            return True
        if key == ord("q"):
            print("\nQuit requested.")
            return False


def show_countdown(cap, args, gesture_name):
    if args.countdown <= 0:
        return True

    print(f"Countdown: {args.countdown}s")
    end_time = time.time() + args.countdown
    last_frame_time = time.time()

    while True:
        ok, frame = cap.read()
        if not ok:
            print("Camera read failed.")
            return False

        now = time.time()
        remaining = max(0.0, end_time - now)
        fps = 1.0 / max(now - last_frame_time, 1e-6)
        last_frame_time = now

        small, roi_box = preprocess_frame(frame, args.roi_scale)
        display = draw_overlay(
            frame,
            roi_box,
            gesture_name,
            0,
            args.num_images,
            fps,
            True,
            args.interval > 0,
        )
        cv2.putText(display, f"STARTING IN {int(np.ceil(remaining))}", (14, 104),
                    cv2.FONT_HERSHEY_SIMPLEX, 1.0, (0, 255, 255), 2)

        input_vis = cv2.cvtColor(small, cv2.COLOR_GRAY2BGR)
        input_vis = cv2.resize(input_vis, (256, 256), interpolation=cv2.INTER_NEAREST)
        cv2.imshow("Collect Webcam Data", display)
        cv2.imshow("Saved Input (64x64)", input_vis)

        key = cv2.waitKey(1) & 0xFF
        if key == ord("q"):
            print("\nQuit requested.")
            return False
        if remaining <= 0:
            print("Starting now.")
            return True


def collect_gesture(cap, args, gesture_id, gesture_name):
    gesture_dir = os.path.join(args.dataset_dir, f"{gesture_id}_{gesture_name}")
    start_index = next_index(gesture_dir, gesture_name)

    print("\n" + "=" * 70)
    print(f"Collecting: {gesture_id} - {gesture_name}")
    print(f"Target: {args.num_images}")
    print(f"Start index: {start_index}")
    print(f"Save dir: {gesture_dir}")
    print(f"Interval: {args.interval:.2f}s")
    print("=" * 70)

    if not show_preview(cap, args, gesture_id, gesture_name):
        return False

    if not show_countdown(cap, args, gesture_name):
        return False

    collected = 0
    frame_count = 0
    last_frame_time = time.time()
    last_save_time = 0.0
    paused = False

    while collected < args.num_images:
        ok, frame = cap.read()
        if not ok:
            print("Camera read failed.")
            break

        now = time.time()
        fps = 1.0 / max(now - last_frame_time, 1e-6)
        last_frame_time = now

        small, roi_box = preprocess_frame(frame, args.roi_scale)
        auto_due = args.interval <= 0 or (now - last_save_time) >= args.interval

        key = cv2.waitKey(1) & 0xFF
        if key == ord("q"):
            print("\nQuit requested.")
            return False
        if key == ord("p"):
            paused = not paused
            print(f"\n{'Paused' if paused else 'Resumed'}")
        if key == ord("s") and not paused:
            auto_due = True

        if not paused and auto_due:
            filename = f"{gesture_name}_{start_index + collected:04d}.png"
            path = os.path.join(gesture_dir, filename)
            cv2.imwrite(path, small)
            collected += 1
            last_save_time = now
            print(f"\r[{collected:4d}/{args.num_images:4d}] saved {filename} "
                  f"mean={np.mean(small):5.1f} std={np.std(small):5.1f}",
                  end="", flush=True)

        display = draw_overlay(
            frame,
            roi_box,
            gesture_name,
            collected,
            args.num_images,
            fps,
            paused,
            args.interval > 0,
        )
        input_vis = cv2.cvtColor(small, cv2.COLOR_GRAY2BGR)
        input_vis = cv2.resize(input_vis, (256, 256), interpolation=cv2.INTER_NEAREST)
        cv2.imshow("Collect Webcam Data", display)
        cv2.imshow("Saved Input (64x64)", input_vis)
        frame_count += 1

    print(f"\nDone: {gesture_name}, collected {collected} images.")
    return True


def parse_gesture_ids(value):
    if value.lower() == "all":
        return list(GESTURE_NAMES.keys())

    gesture_ids = []
    for part in value.split(","):
        part = part.strip()
        if not part:
            continue
        gesture_id = int(part)
        if gesture_id not in GESTURE_NAMES:
            raise ValueError(f"Invalid gesture id: {gesture_id}")
        gesture_ids.append(gesture_id)
    return gesture_ids


def interactive_select_gestures():
    print("Gesture list:")
    for gesture_id, gesture_name in GESTURE_NAMES.items():
        print(f"  {gesture_id}: {gesture_name}")
    print()
    value = input("Gesture ids to collect, e.g. all or 0,2,4: ").strip()
    return parse_gesture_ids(value or "all")


def main():
    parser = argparse.ArgumentParser(description="Collect 5-class gesture data from a USB webcam.")
    parser.add_argument("--dataset-dir", default="dataset_real", help="Dataset output directory")
    parser.add_argument("--camera", type=int, default=0, help="Camera index")
    parser.add_argument("--width", type=int, default=640, help="Camera width")
    parser.add_argument("--height", type=int, default=480, help="Camera height")
    parser.add_argument("--roi-scale", type=float, default=1.0,
                        help="Center ROI scale before resizing to 64x64")
    parser.add_argument("--gesture", type=int, default=None,
                        help="Single gesture id to collect, e.g. 0 for fist")
    parser.add_argument("--num-images", type=int, default=200, help="Images per gesture")
    parser.add_argument("--interval", type=float, default=0.25,
                        help="Seconds between automatic captures. Use 0 for every frame.")
    parser.add_argument("--countdown", type=int, default=3, help="Countdown before each gesture")
    parser.add_argument("--gestures", default="all",
                        help="Gesture ids to collect, e.g. all, ask, or 0,2,4")
    args = parser.parse_args()

    if not (0.1 <= args.roi_scale <= 1.0):
        raise ValueError("--roi-scale must be between 0.1 and 1.0")

    if args.gesture is not None:
        if args.gesture not in GESTURE_NAMES:
            raise ValueError(f"Invalid gesture id: {args.gesture}")
        gesture_ids = [args.gesture]
    elif args.gestures.lower() == "ask":
        gesture_ids = interactive_select_gestures()
    else:
        gesture_ids = parse_gesture_ids(args.gestures)

    if args.interval < 0:
        raise ValueError("--interval must be >= 0")
    ensure_dataset_dirs(args.dataset_dir)

    print("Gesture list:")
    for gesture_id, gesture_name in GESTURE_NAMES.items():
        print(f"  {gesture_id}: {gesture_name}")
    print()
    print(f"Dataset dir: {args.dataset_dir}")
    print(f"Camera: {args.camera}")
    print(f"ROI scale: {args.roi_scale}")
    print(f"Images per gesture: {args.num_images}")
    print(f"Capture interval: {args.interval}s")
    print(f"Selected gestures: {', '.join(f'{gid}:{GESTURE_NAMES[gid]}' for gid in gesture_ids)}")
    print()

    cap = cv2.VideoCapture(args.camera, cv2.CAP_DSHOW)
    if not cap.isOpened():
        cap = cv2.VideoCapture(args.camera)
    if not cap.isOpened():
        raise RuntimeError(f"Cannot open camera {args.camera}")

    cap.set(cv2.CAP_PROP_FRAME_WIDTH, args.width)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, args.height)
    print(f"Camera opened: {int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))}x"
          f"{int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))}")

    try:
        for gesture_id in gesture_ids:
            gesture_name = GESTURE_NAMES[gesture_id]
            ok = collect_gesture(cap, args, gesture_id, gesture_name)
            if not ok:
                break
    except KeyboardInterrupt:
        print("\nInterrupted.")
    finally:
        cap.release()
        cv2.destroyAllWindows()
        print("Collector exited.")


if __name__ == "__main__":
    main()
