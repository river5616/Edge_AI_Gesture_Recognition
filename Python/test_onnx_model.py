#!/usr/bin/env python3
"""
测试 ONNX 模型在 PC 端的推理结果
对比训练集图像和实际采集图像的识别效果
"""

import onnxruntime as ort
import numpy as np
from PIL import Image

# 手势类别
GESTURE_NAMES = {
    0: "fist", 1: "palm", 2: "thumbs_up", 3: "ok", 4: "v_sign",
    5: "point", 6: "three", 7: "four", 8: "nothing"
}

def preprocess_image(image_path):
    """预处理图像（与 STM32 端完全一致）"""
    # 读取图像并转为灰度
    img = Image.open(image_path).convert('L')

    # 确保尺寸为 64x64
    if img.size != (64, 64):
        img = img.resize((64, 64))

    # 转为 numpy 数组
    img_array = np.array(img, dtype=np.float32)

    # 归一化：(x / 255.0 - 0.5) / 0.5  映射到 [-1, 1]
    normalized = (img_array / 255.0 - 0.5) / 0.5

    # 调整维度为 (1, 1, 64, 64) - (batch, channel, height, width)
    input_tensor = normalized.reshape(1, 1, 64, 64).astype(np.float32)

    return input_tensor, img_array

def run_inference(session, input_tensor):
    """运行推理"""
    input_name = session.get_inputs()[0].name
    output_name = session.get_outputs()[0].name

    # 运行推理
    outputs = session.run([output_name], {input_name: input_tensor})

    # 获取输出
    output = outputs[0][0]  # shape: (9,)

    # 找到最大值的索引
    predicted_class = np.argmax(output)
    confidence = output[predicted_class]

    return predicted_class, confidence, output

def main():
    # 加载 ONNX 模型
    print("加载 ONNX 模型...")
    session = ort.InferenceSession("gesture_model.onnx")
    print(f"✓ 模型已加载")
    print()

    # 测试图像列表
    test_images = [
        ("训练集 - fist_0001", "dataset/0_fist/fist_0001.png", 0),
        ("训练集 - fist_0010", "dataset/0_fist/fist_0010.png", 0),
        ("训练集 - point_0001", "dataset/5_point/point_0001.png", 5),
        ("实际采集 - frame_0002 (应该是 fist)", "debug_inference/images/frame_0002_point_conf3.1.png", 0),
    ]

    print("=" * 80)
    for name, image_path, expected_class in test_images:
        try:
            # 预处理
            input_tensor, raw_image = preprocess_image(image_path)

            # 推理
            predicted_class, confidence, all_outputs = run_inference(session, input_tensor)

            # 显示结果
            correct = "✓" if predicted_class == expected_class else "✗"
            print(f"\n{correct} {name}")
            print(f"  路径: {image_path}")
            print(f"  期望: {expected_class} ({GESTURE_NAMES[expected_class]})")
            print(f"  预测: {predicted_class} ({GESTURE_NAMES[predicted_class]}) - 置信度: {confidence:.4f}")
            print(f"  图像均值: {np.mean(raw_image):.2f}")

            # 显示所有类别的输出
            print(f"  所有输出:")
            for i, val in enumerate(all_outputs):
                marker = " ← MAX" if i == predicted_class else ""
                print(f"    {i} ({GESTURE_NAMES[i]:12s}): {val:8.4f}{marker}")

            print("-" * 80)

        except Exception as e:
            print(f"✗ {name} - 错误: {e}")
            print("-" * 80)

    print("\n" + "=" * 80)
    print("测试完成")

if __name__ == "__main__":
    main()
