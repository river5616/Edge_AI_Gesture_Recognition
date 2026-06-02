#!/usr/bin/env python3
"""
Static INT8 quantization for the 5-class GestureNet ONNX model.

Default usage:
    python quantize_onnx_int8.py --model gesture_v1_5.onnx --dataset dataset_real

The dataset directory is expected to contain train_model.py-style folders:
    0_fist, 1_palm, 2_thumbs_up, 3_ok, 4_nothing
"""

import argparse
import random
from pathlib import Path

import cv2
import numpy as np

try:
    import onnx
    import onnxruntime as ort
    from onnxruntime.quantization import (
        CalibrationDataReader,
        CalibrationMethod,
        QuantFormat,
        QuantType,
        quantize_static,
    )
except ImportError as exc:
    raise SystemExit(
        "Missing dependency. Install with:\n"
        "  pip install onnx onnxruntime\n"
    ) from exc


GESTURE_NAMES = {
    0: "fist",
    1: "palm",
    2: "thumbs_up",
    3: "ok",
    4: "nothing",
}


def collect_images(dataset_dir, max_images=None, seed=42):
    dataset_dir = Path(dataset_dir)
    samples = []

    for class_id, class_name in GESTURE_NAMES.items():
        class_dir = dataset_dir / f"{class_id}_{class_name}"
        if not class_dir.exists():
            print(f"Warning: missing class directory: {class_dir}")
            continue

        for path in sorted(class_dir.glob("*")):
            if path.suffix.lower() in {".png", ".jpg", ".jpeg", ".bmp"}:
                samples.append((path, class_id))

    if not samples:
        raise FileNotFoundError(f"No images found in {dataset_dir}")

    random.Random(seed).shuffle(samples)
    if max_images is not None and max_images > 0:
        samples = samples[:max_images]

    return samples


def infer_input_layout(session):
    input_shape = session.get_inputs()[0].shape

    # Common exported PyTorch shape: [1, 1, 64, 64]
    if len(input_shape) == 4:
        if input_shape[1] == 1:
            return "NCHW"
        if input_shape[3] == 1:
            return "NHWC"

    # Fallback for dynamic shapes from torch.onnx.export.
    return "NCHW"


def preprocess_image(image_path, layout):
    image = cv2.imread(str(image_path), cv2.IMREAD_GRAYSCALE)
    if image is None:
        raise ValueError(f"Cannot read image: {image_path}")

    if image.shape != (64, 64):
        image = cv2.resize(image, (64, 64), interpolation=cv2.INTER_AREA)

    image = image.astype(np.float32)
    image = (image / 255.0 - 0.5) / 0.5

    if layout == "NHWC":
        return image.reshape(1, 64, 64, 1).astype(np.float32)

    return image.reshape(1, 1, 64, 64).astype(np.float32)


def softmax(logits):
    logits = logits.astype(np.float32)
    logits = logits - np.max(logits)
    exp = np.exp(logits)
    return exp / np.sum(exp)


class GestureCalibrationReader(CalibrationDataReader):
    def __init__(self, image_paths, input_name, layout):
        self.image_paths = list(image_paths)
        self.input_name = input_name
        self.layout = layout
        self.index = 0

    def get_next(self):
        if self.index >= len(self.image_paths):
            return None

        image_path = self.image_paths[self.index]
        self.index += 1
        return {self.input_name: preprocess_image(image_path, self.layout)}


def evaluate_model(model_path, samples, layout=None, providers=None):
    providers = providers or ["CPUExecutionProvider"]
    session = ort.InferenceSession(str(model_path), providers=providers)
    input_name = session.get_inputs()[0].name
    output_name = session.get_outputs()[0].name
    layout = layout or infer_input_layout(session)

    correct = 0
    predictions = []

    for image_path, label in samples:
        input_tensor = preprocess_image(image_path, layout)
        output = session.run([output_name], {input_name: input_tensor})[0][0]
        probs = softmax(output)
        pred = int(np.argmax(probs))
        confidence = float(probs[pred])
        correct += int(pred == label)
        predictions.append((pred, confidence, output))

    accuracy = correct / len(samples)
    return accuracy, predictions


def compare_outputs(float_model, int8_model, samples, layout):
    float_session = ort.InferenceSession(str(float_model), providers=["CPUExecutionProvider"])
    int8_session = ort.InferenceSession(str(int8_model), providers=["CPUExecutionProvider"])
    float_input = float_session.get_inputs()[0].name
    int8_input = int8_session.get_inputs()[0].name
    float_output = float_session.get_outputs()[0].name
    int8_output = int8_session.get_outputs()[0].name

    max_abs_diff = 0.0
    mean_abs_diffs = []
    same_top1 = 0

    for image_path, _label in samples:
        input_tensor = preprocess_image(image_path, layout)
        y_float = float_session.run([float_output], {float_input: input_tensor})[0][0]
        y_int8 = int8_session.run([int8_output], {int8_input: input_tensor})[0][0]

        diff = np.abs(y_float.astype(np.float32) - y_int8.astype(np.float32))
        max_abs_diff = max(max_abs_diff, float(np.max(diff)))
        mean_abs_diffs.append(float(np.mean(diff)))
        same_top1 += int(np.argmax(y_float) == np.argmax(y_int8))

    return {
        "max_abs_diff": max_abs_diff,
        "mean_abs_diff": float(np.mean(mean_abs_diffs)),
        "same_top1_rate": same_top1 / len(samples),
    }


def get_model_opset(model_path):
    model = onnx.load(str(model_path))
    ai_onnx_opsets = [
        opset.version
        for opset in model.opset_import
        if opset.domain in ("", "ai.onnx")
    ]
    return max(ai_onnx_opsets) if ai_onnx_opsets else None


def main():
    parser = argparse.ArgumentParser(description="Quantize GestureNet ONNX to INT8.")
    parser.add_argument("--model", default="gesture_v1_5.onnx", help="Input float32 ONNX model")
    parser.add_argument("--output", default="gesture_v1_5_int8.onnx", help="Output INT8 ONNX model")
    parser.add_argument("--dataset", default="dataset_real", help="5-class calibration/eval dataset")
    parser.add_argument("--calib-images", type=int, default=300, help="Max calibration images")
    parser.add_argument("--eval-images", type=int, default=0, help="Max eval images, 0 means all")
    parser.add_argument("--per-channel", action="store_true",
                        help="Use per-channel weight quantization. Requires QDQ model opset >= 13.")
    parser.add_argument("--seed", type=int, default=42)
    args = parser.parse_args()

    model_path = Path(args.model)
    output_path = Path(args.output)
    if not model_path.exists():
        raise FileNotFoundError(model_path)

    all_samples = collect_images(args.dataset, max_images=None, seed=args.seed)
    eval_samples = all_samples
    if args.eval_images > 0:
        eval_samples = all_samples[: args.eval_images]

    calib_samples = all_samples[: args.calib_images] if args.calib_images > 0 else all_samples

    session = ort.InferenceSession(str(model_path), providers=["CPUExecutionProvider"])
    input_name = session.get_inputs()[0].name
    layout = infer_input_layout(session)
    opset = get_model_opset(model_path)
    use_per_channel = args.per_channel

    if use_per_channel and opset is not None and opset < 13:
        print(
            f"Warning: model opset is {opset}; QDQ per-channel quantization "
            "requires opset >= 13. Falling back to per-tensor quantization."
        )
        use_per_channel = False

    print(f"Float model: {model_path}")
    print(f"INT8 output: {output_path}")
    print(f"Dataset: {args.dataset}")
    print(f"Input name: {input_name}")
    print(f"Input layout: {layout}")
    print(f"ONNX opset: {opset}")
    print(f"Per-channel quantization: {use_per_channel}")
    print(f"Calibration images: {len(calib_samples)}")
    print(f"Evaluation images: {len(eval_samples)}")

    calibration_reader = GestureCalibrationReader(
        [path for path, _label in calib_samples],
        input_name=input_name,
        layout=layout,
    )

    # QDQ format is generally the safest ONNX format for later tool conversion.
    # X-CUBE-AI support depends on the installed version; if import fails there,
    # use CubeMX/X-CUBE-AI built-in quantization with this same dataset instead.
    quantize_static(
        model_input=str(model_path),
        model_output=str(output_path),
        calibration_data_reader=calibration_reader,
        quant_format=QuantFormat.QDQ,
        activation_type=QuantType.QInt8,
        weight_type=QuantType.QInt8,
        calibrate_method=CalibrationMethod.MinMax,
        per_channel=use_per_channel,
        extra_options={"ActivationSymmetric": True, "WeightSymmetric": True},
    )

    float_acc, _ = evaluate_model(model_path, eval_samples, layout=layout)
    int8_acc, _ = evaluate_model(output_path, eval_samples, layout=layout)
    diff = compare_outputs(model_path, output_path, eval_samples, layout)

    print("\nQuantization done.")
    print(f"Float accuracy: {float_acc * 100:.2f}%")
    print(f"INT8 accuracy : {int8_acc * 100:.2f}%")
    print(f"Top-1 match   : {diff['same_top1_rate'] * 100:.2f}%")
    print(f"Mean abs diff : {diff['mean_abs_diff']:.6f}")
    print(f"Max abs diff  : {diff['max_abs_diff']:.6f}")
    print(f"Saved: {output_path}")


if __name__ == "__main__":
    main()
