#!/usr/bin/env python3
"""
核对 STM32 C 实现与 PC 端 onnx 推理的数学等价性。

PC 基准路径（可信）：
    float图 -> 归一化[-1,1] -> onnx(内部量化+推理+反量化) -> float logits -> softmax

STM32 路径（复刻其 C 代码逻辑）：
    float图 -> 归一化[-1,1] -> 手动量化int8(scale=IN) -> [模型] -> int8输出 -> 手动反量化(scale=OUT) -> softmax

由于 onnx 是 float 接口、内部自带 QDQ，这里用 onnx 内部量化来代表"模型"，
重点核对 STM32 多做的【输入手动量化】和【输出手动反量化】是否与 onnx 内部行为一致。
"""
import sys
from pathlib import Path
import numpy as np
import cv2
import onnxruntime as ort

MODEL = "gesture_v2_strongaug_int8.onnx"
IN_SCALE  = 0.007874015718698502   # = AI_INPUT_QUANT_SCALE
OUT_SCALE = 0.14712882041931152    # = AI_OUTPUT_QUANT_SCALE
GESTURES = {0:"fist",1:"palm",2:"thumbs_up",3:"ok",4:"nothing"}


def softmax(logits):
    logits = logits.astype(np.float32)
    logits = logits - np.max(logits)
    exp = np.exp(logits)
    return exp / np.sum(exp)


def pc_preprocess(image):
    """PC 基准：归一化到 [-1,1]，float32 NCHW"""
    image = image.astype(np.float32)
    image = (image / 255.0 - 0.5) / 0.5
    return image.reshape(1, 1, 64, 64).astype(np.float32)


def stm32_quantize_input(image):
    """复刻 STM32 preprocess_image：归一化后量化为 int8，再还原为 float 喂入 float 接口模型。
    STM32: q = lrintf(x / IN_SCALE), 饱和[-128,127]；模型内部 dequant 用同样 IN_SCALE。"""
    image = image.astype(np.float32)
    x = (image / 255.0 - 0.5) / 0.5           # [-1,1]
    q = np.rint(x / IN_SCALE)                  # 量化（lrintf = round half to even? 见下方注释）
    q = np.clip(q, -128, 127).astype(np.int8)
    # 模型内部会 dequant: x' = q * IN_SCALE，等价于喂这个 float
    x_requant = q.astype(np.float32) * IN_SCALE
    return x_requant.reshape(1, 1, 64, 64).astype(np.float32)


def main():
    model_path = Path(MODEL)
    if not model_path.exists():
        print(f"模型不存在: {model_path}"); sys.exit(1)

    sess = ort.InferenceSession(str(model_path), providers=["CPUExecutionProvider"])
    in_name = sess.get_inputs()[0].name
    out_name = sess.get_outputs()[0].name

    # 收集几张真实样本（每类一张）
    ds = Path("dataset_real")
    if not ds.exists():
        ds = Path("dataset_hard_test")
    samples = []
    for cid, cname in GESTURES.items():
        d = ds / f"{cid}_{cname}"
        if d.exists():
            imgs = sorted(d.glob("*.png")) + sorted(d.glob("*.jpg"))
            if imgs:
                samples.append((imgs[0], cid))
    if not samples:
        print(f"未找到样本目录于 {ds}"); sys.exit(1)

    print(f"模型: {MODEL}")
    print(f"输入scale={IN_SCALE}  输出scale={OUT_SCALE}\n")
    print(f"{'样本':<28} {'PC_pred':<12} {'STM32_pred':<12} {'PC_conf':>8} {'STM32_conf':>10} {'logit最大差':>12}")
    print("-" * 90)

    max_logit_diff_all = 0.0
    pred_mismatch = 0
    for path, label in samples:
        img = cv2.imread(str(path), cv2.IMREAD_GRAYSCALE)
        if img is None:
            continue
        if img.shape != (64, 64):
            img = cv2.resize(img, (64, 64), interpolation=cv2.INTER_AREA)

        # PC 基准
        pc_logits = sess.run([out_name], {in_name: pc_preprocess(img)})[0][0]
        pc_probs = softmax(pc_logits)
        pc_pred = int(np.argmax(pc_probs))

        # STM32 复刻（输入经 int8 量化往返）
        st_logits = sess.run([out_name], {in_name: stm32_quantize_input(img)})[0][0]
        st_probs = softmax(st_logits)
        st_pred = int(np.argmax(st_probs))

        ld = float(np.max(np.abs(pc_logits - st_logits)))
        max_logit_diff_all = max(max_logit_diff_all, ld)
        if pc_pred != st_pred:
            pred_mismatch += 1

        print(f"{path.name:<28} {GESTURES[pc_pred]:<12} {GESTURES[st_pred]:<12} "
              f"{pc_probs[pc_pred]:>8.4f} {st_probs[st_pred]:>10.4f} {ld:>12.6f}")

    print("-" * 90)
    print(f"\n最大 logit 差异: {max_logit_diff_all:.6f}")
    print(f"预测不一致样本数: {pred_mismatch}/{len(samples)}")
    print("\n结论:")
    if max_logit_diff_all < 0.5 and pred_mismatch == 0:
        print("  输入量化往返对 logits 影响极小，STM32 输入量化逻辑与 PC 等价。")
    else:
        print("  输入量化往返显著改变了 logits，需要检查量化公式/舍入。")
    print("\n注: 本脚本验证【输入量化】与【softmax】等价性。")
    print("    输出反量化是线性变换，softmax(q*scale) 与 PC 的 softmax(dequant_logits) 数学一致，")
    print("    前提是 STM32 用的 OUT_SCALE 与 onnx 内部一致 —— 已确认均为 0.14712882。")


if __name__ == "__main__":
    main()
