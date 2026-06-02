#!/usr/bin/env python3
"""
Evaluate an ONNX gesture model on a 5-class folder dataset.

Expected folder layout:
    dataset_hard_test/
      0_fist/
      1_palm/
      2_thumbs_up/
      3_ok/
      4_nothing/

Example:
    python eval_onnx_dataset.py --model gesture_v2_5_augment_new_int8.onnx --dataset dataset_hard_test
"""

import argparse
import csv
from pathlib import Path

import cv2
import numpy as np
import onnxruntime as ort
from sklearn.metrics import classification_report, confusion_matrix


GESTURE_NAMES = {
    0: "fist",
    1: "palm",
    2: "thumbs_up",
    3: "ok",
    4: "nothing",
}


def collect_samples(dataset_dir):
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

    return samples


def infer_layout(session):
    shape = session.get_inputs()[0].shape
    if len(shape) == 4:
        if shape[1] == 1:
            return "NCHW"
        if shape[3] == 1:
            return "NHWC"
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


def main():
    parser = argparse.ArgumentParser(description="Evaluate ONNX gesture model on a dataset.")
    parser.add_argument("--model", required=True, help="ONNX model path")
    parser.add_argument("--dataset", required=True, help="Dataset directory")
    parser.add_argument("--output-prefix", default=None,
                        help="Output prefix for confusion matrix CSV and wrong samples CSV")
    parser.add_argument("--top-k-wrong", type=int, default=30,
                        help="Print first N wrong samples")
    args = parser.parse_args()

    model_path = Path(args.model)
    dataset_dir = Path(args.dataset)
    output_prefix = args.output_prefix or f"{model_path.stem}_{dataset_dir.name}"

    session = ort.InferenceSession(str(model_path), providers=["CPUExecutionProvider"])
    input_name = session.get_inputs()[0].name
    output_name = session.get_outputs()[0].name
    layout = infer_layout(session)

    samples = collect_samples(dataset_dir)
    y_true = []
    y_pred = []
    wrong_rows = []

    print(f"Model: {model_path}")
    print(f"Dataset: {dataset_dir}")
    print(f"Samples: {len(samples)}")
    print(f"Input: {input_name}, layout={layout}")
    print(f"Output: {output_name}")
    print()

    for image_path, label in samples:
        input_tensor = preprocess_image(image_path, layout)
        output = session.run([output_name], {input_name: input_tensor})[0][0]
        probs = softmax(output)
        pred = int(np.argmax(probs))
        confidence = float(probs[pred])

        y_true.append(label)
        y_pred.append(pred)

        if pred != label:
            sorted_idx = np.argsort(probs)
            top2 = int(sorted_idx[-2]) if len(sorted_idx) > 1 else pred
            margin = float(probs[pred] - probs[top2])
            wrong_rows.append({
                "path": str(image_path),
                "true_id": label,
                "true_name": GESTURE_NAMES[label],
                "pred_id": pred,
                "pred_name": GESTURE_NAMES.get(pred, f"id_{pred}"),
                "confidence": confidence,
                "top2_id": top2,
                "top2_name": GESTURE_NAMES.get(top2, f"id_{top2}"),
                "top2_prob": float(probs[top2]),
                "margin": margin,
            })

    y_true = np.array(y_true)
    y_pred = np.array(y_pred)
    acc = float(np.mean(y_true == y_pred))

    labels = list(GESTURE_NAMES.keys())
    names = [GESTURE_NAMES[i] for i in labels]
    cm = confusion_matrix(y_true, y_pred, labels=labels)

    print(f"Accuracy: {acc * 100:.2f}% ({int(np.sum(y_true == y_pred))}/{len(y_true)})")
    print()
    print("Classification report:")
    print(classification_report(y_true, y_pred, labels=labels, target_names=names, digits=4))
    print("Confusion matrix rows=true cols=pred:")
    print("          " + " ".join(f"{name[:7]:>7s}" for name in names))
    for i, row in enumerate(cm):
        print(f"{names[i][:9]:>9s} " + " ".join(f"{value:7d}" for value in row))

    print()
    print(f"Wrong samples: {len(wrong_rows)}")
    for row in wrong_rows[:args.top_k_wrong]:
        print(
            f"{row['true_name']:>9s} -> {row['pred_name']:<9s} "
            f"conf={row['confidence']:.3f} margin={row['margin']:.3f} "
            f"{row['path']}"
        )

    cm_path = Path(f"{output_prefix}_confusion_matrix.csv")
    wrong_path = Path(f"{output_prefix}_wrong_samples.csv")

    with cm_path.open("w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(["true\\pred"] + names)
        for i, row in enumerate(cm):
            writer.writerow([names[i]] + row.tolist())

    with wrong_path.open("w", newline="", encoding="utf-8") as f:
        fieldnames = [
            "path", "true_id", "true_name", "pred_id", "pred_name",
            "confidence", "top2_id", "top2_name", "top2_prob", "margin",
        ]
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(wrong_rows)

    print()
    print(f"Saved confusion matrix: {cm_path}")
    print(f"Saved wrong samples: {wrong_path}")


if __name__ == "__main__":
    main()
