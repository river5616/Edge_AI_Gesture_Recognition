#!/usr/bin/env python3
"""
数据增强工具 - 对现有数据集进行增强
用于扩充训练数据，提高模型泛化能力
"""

import cv2
import numpy as np
import os
from pathlib import Path

def augment_image(image):
    """
    对单张图像进行多种增强

    Args:
        image: 输入灰度图像 (64x64)

    Returns:
        augmented: 增强后的图像列表 [(名称, 图像), ...]
    """
    augmented = []

    # 1. 原图
    augmented.append(('original', image.copy()))

    # 2. 轻微旋转 (-15° 到 +15°)
    for angle in [-15, -10, -5, 5, 10, 15]:
        h, w = image.shape
        M = cv2.getRotationMatrix2D((w/2, h/2), angle, 1.0)
        rotated = cv2.warpAffine(image, M, (w, h), borderValue=int(np.mean(image)))
        augmented.append((f'rot{angle:+d}', rotated))

    # 3. 平移 (-8 到 +8 像素)
    for dx, dy in [(-8, -8), (-8, 0), (-8, 8), (0, -8), (0, 8), (8, -8), (8, 0), (8, 8)]:
        h, w = image.shape
        M = np.float32([[1, 0, dx], [0, 1, dy]])
        shifted = cv2.warpAffine(image, M, (w, h), borderValue=int(np.mean(image)))
        augmented.append((f'shift{dx:+d}{dy:+d}', shifted))

    # 4. 缩放 (0.85 到 1.15)
    for scale in [0.85, 0.90, 0.95, 1.05, 1.10, 1.15]:
        h, w = image.shape
        new_h, new_w = int(h * scale), int(w * scale)
        scaled = cv2.resize(image, (new_w, new_h))

        # 裁剪或填充到原始尺寸
        if scale > 1.0:
            # 裁剪中心
            start_y = (new_h - h) // 2
            start_x = (new_w - w) // 2
            result = scaled[start_y:start_y+h, start_x:start_x+w]
        else:
            # 填充边缘
            result = np.full((h, w), int(np.mean(image)), dtype=np.uint8)
            start_y = (h - new_h) // 2
            start_x = (w - new_w) // 2
            result[start_y:start_y+new_h, start_x:start_x+new_w] = scaled

        augmented.append((f'scale{int(scale*100)}', result))

    # 5. 亮度调整 (-30 到 +30)
    for delta in [-30, -20, -10, 10, 20, 30]:
        adjusted = cv2.add(image, delta)
        augmented.append((f'bright{delta:+d}', adjusted))

    # 6. 对比度调整 (0.7 到 1.3)
    for alpha in [0.7, 0.8, 0.9, 1.1, 1.2, 1.3]:
        adjusted = cv2.convertScaleAbs(image, alpha=alpha, beta=0)
        augmented.append((f'contrast{int(alpha*100)}', adjusted))

    # 7. 高斯噪声 (sigma=5, 10)
    for sigma in [5, 10]:
        noise = np.random.normal(0, sigma, image.shape)
        noisy = np.clip(image + noise, 0, 255).astype(np.uint8)
        augmented.append((f'noise{sigma}', noisy))

    # 8. 高斯模糊 (kernel=3, 5)
    for ksize in [3, 5]:
        blurred = cv2.GaussianBlur(image, (ksize, ksize), 0)
        augmented.append((f'blur{ksize}', blurred))

    # 9. 水平翻转（小心使用，某些手势不对称）
    # thumbs_up (2), ok (3), point (5), three (6) 不应翻转
    flipped = cv2.flip(image, 1)
    augmented.append(('flip', flipped))

    return augmented

def augment_dataset(input_dir, output_dir, augment_factor=10, exclude_flip_for=[2, 3, 5, 6]):
    """
    增强整个数据集

    Args:
        input_dir: 输入数据集目录
        output_dir: 输出增强后的数据集目录
        augment_factor: 每张图像生成多少个增强版本
        exclude_flip_for: 不进行水平翻转的手势ID列表
    """
    input_path = Path(input_dir)
    output_path = Path(output_dir)
    output_path.mkdir(parents=True, exist_ok=True)

    total_original = 0
    total_augmented = 0

    # 遍历每个手势类别
    for gesture_dir in sorted(input_path.iterdir()):
        if not gesture_dir.is_dir():
            continue

        gesture_name = gesture_dir.name
        output_gesture_dir = output_path / gesture_name
        output_gesture_dir.mkdir(exist_ok=True)

        # 提取手势ID
        gesture_id = int(gesture_name.split('_')[0]) if '_' in gesture_name else -1

        print(f"\n{'='*60}")
        print(f"处理手势: {gesture_name}")
        print(f"{'='*60}")

        # 获取所有图像
        image_files = list(gesture_dir.glob("*.png")) + list(gesture_dir.glob("*.jpg"))
        total = len(image_files)
        total_original += total

        print(f"原始图像数量: {total}")

        for idx, img_file in enumerate(image_files):
            # 读取图像
            image = cv2.imread(str(img_file), cv2.IMREAD_GRAYSCALE)

            if image is None:
                print(f"  ✗ 无法读取: {img_file.name}")
                continue

            # 确保尺寸为 64x64
            if image.shape != (64, 64):
                image = cv2.resize(image, (64, 64))

            # 生成增强图像
            augmented_images = augment_image(image)

            # 如果是不对称手势，移除水平翻转
            if gesture_id in exclude_flip_for:
                augmented_images = [(name, img) for name, img in augmented_images if name != 'flip']

            # 只保存前 augment_factor 个增强版本
            saved_count = 0
            for aug_idx, (aug_name, aug_img) in enumerate(augmented_images):
                if saved_count >= augment_factor:
                    break

                output_filename = f"{img_file.stem}_aug{aug_idx:02d}_{aug_name}.png"
                output_path_full = output_gesture_dir / output_filename
                cv2.imwrite(str(output_path_full), aug_img)
                saved_count += 1

            if (idx + 1) % 10 == 0 or (idx + 1) == total:
                print(f"  进度: {idx+1}/{total} ({(idx+1)/total*100:.1f}%)")

        final_count = len(list(output_gesture_dir.glob("*.png")))
        total_augmented += final_count
        print(f"  ✓ 完成: 原始 {total} 张 → 增强后 {final_count} 张 (x{final_count/total:.1f})")

    print(f"\n{'='*60}")
    print(f"总计:")
    print(f"  原始图像: {total_original}")
    print(f"  增强后图像: {total_augmented}")
    print(f"  增强倍数: x{total_augmented/total_original:.1f}")
    print(f"{'='*60}")

def preview_augmentation(image_path, output_dir="augmentation_preview"):
    """
    预览单张图像的增强效果

    Args:
        image_path: 输入图像路径
        output_dir: 预览图像保存目录
    """
    import matplotlib.pyplot as plt

    # 读取图像
    image = cv2.imread(image_path, cv2.IMREAD_GRAYSCALE)
    if image is None:
        print(f"✗ 无法读取图像: {image_path}")
        return

    if image.shape != (64, 64):
        image = cv2.resize(image, (64, 64))

    # 生成增强图像
    augmented_images = augment_image(image)

    # 创建输出目录
    os.makedirs(output_dir, exist_ok=True)

    # 选择前20个进行可视化
    num_to_show = min(20, len(augmented_images))

    fig, axes = plt.subplots(4, 5, figsize=(15, 12))
    axes = axes.flatten()

    for i in range(num_to_show):
        name, img = augmented_images[i]
        axes[i].imshow(img, cmap='gray')
        axes[i].set_title(name, fontsize=8)
        axes[i].axis('off')

    plt.tight_layout()
    preview_path = os.path.join(output_dir, "augmentation_preview.png")
    plt.savefig(preview_path, dpi=150, bbox_inches='tight')
    plt.close()

    print(f"✓ 预览图已保存: {preview_path}")

    # 保存所有增强图像
    for i, (name, img) in enumerate(augmented_images):
        filename = f"{i:02d}_{name}.png"
        filepath = os.path.join(output_dir, filename)
        cv2.imwrite(filepath, img)

    print(f"✓ 所有增强图像已保存到: {output_dir}")

if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description="数据增强工具")
    parser.add_argument("--input", type=str, default="dataset", help="输入数据集目录")
    parser.add_argument("--output", type=str, default="dataset_augmented", help="输出目录")
    parser.add_argument("--factor", type=int, default=10, help="每张图像生成多少个增强版本 (最多约70种)")
    parser.add_argument("--preview", type=str, help="预览单张图像的增强效果")
    parser.add_argument("--exclude-flip", type=int, nargs='+', default=[2, 3, 5, 6],
                       help="不进行水平翻转的手势ID (默认: 2 3 5 6)")

    args = parser.parse_args()

    print("="*60)
    print("数据增强工具")
    print("="*60)

    if args.preview:
        # 预览模式
        print(f"预览模式: {args.preview}")
        print("="*60)
        preview_augmentation(args.preview)
    else:
        # 批量增强模式
        print(f"输入: {args.input}")
        print(f"输出: {args.output}")
        print(f"增强倍数: {args.factor}")
        print(f"不翻转的手势ID: {args.exclude_flip}")
        print("="*60)

        if not os.path.exists(args.input):
            print(f"✗ 输入目录不存在: {args.input}")
            exit(1)

        augment_dataset(args.input, args.output, args.factor, args.exclude_flip)

        print("\n" + "="*60)
        print("✓ 数据增强完成！")
        print(f"✓ 增强后的数据集保存在: {args.output}")
        print("="*60)
        print("\n下一步:")
        print("1. 检查增强后的数据质量")
        print("2. 使用增强后的数据训练模型")
        print("3. 评估模型性能")
