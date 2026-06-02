#!/usr/bin/env python3
"""
将改进版 PyTorch 模型转换为 ONNX 格式
支持 GestureNetV2 和 GestureNetV2Lite
"""

import torch
import torch.onnx
import argparse
from test_pytorch_model_v2 import GestureNetV2, GestureNetV2Lite, GestureNet


class GestureNetV2Spatial(GestureNetV2):
    """GestureNetV2 with 2x2 spatial pooling head for ONNX export."""

    def __init__(self, num_classes=5):
        import torch.nn as nn
        super().__init__(num_classes=num_classes)
        # block3 output is fixed 8x8 for 64x64 input; AvgPool2d(4) exports to ONNX reliably.
        self.global_avg_pool = nn.AvgPool2d(kernel_size=4, stride=4)
        self.fc = nn.Sequential(
            nn.Dropout(0.5),
            nn.Linear(128 * 2 * 2, 64),
            nn.ReLU(inplace=True),
            nn.Dropout(0.3),
            nn.Linear(64, num_classes)
        )


def convert_to_onnx(pytorch_model_path, onnx_model_path, model_type='v2'):
    """
    将PyTorch模型转换为ONNX格式

    Args:
        pytorch_model_path: PyTorch模型路径 (.pth)
        onnx_model_path: 输出ONNX模型路径 (.onnx)
        model_type: 模型类型 ('v1', 'v2', 'lite')
    """
    print(f"加载PyTorch模型: {pytorch_model_path}")

    # 加载模型
    device = torch.device("cpu")  # ONNX转换使用CPU

    # 根据模型类型选择架构
    if model_type == 'lite':
        model = GestureNetV2Lite(num_classes=5).to(device)
        print("使用模型架构: GestureNetV2Lite")
    elif model_type == 'spatial':
        model = GestureNetV2Spatial(num_classes=5).to(device)
        print("使用模型架构: GestureNetV2Spatial")
    elif model_type == 'v2':
        model = GestureNetV2(num_classes=5).to(device)
        print("使用模型架构: GestureNetV2")
    elif model_type == 'v1':
        model = GestureNet(num_classes=5).to(device)
        print("使用模型架构: GestureNet")
    else:
        raise ValueError(f"未知的模型类型: {model_type}")

    model.load_state_dict(torch.load(pytorch_model_path, map_location=device, weights_only=False))
    model.eval()

    print("✓ 模型加载成功")

    # 打印模型信息
    total_params = sum(p.numel() for p in model.parameters())
    print(f"模型参数量: {total_params:,}")
    print(f"模型大小: {total_params * 4 / 1024:.2f} KB")

    # 创建示例输入
    dummy_input = torch.randn(1, 1, 64, 64, device=device)

    # 测试模型前向传播
    print("\n测试模型前向传播...")
    with torch.no_grad():
        output = model(dummy_input)
        print(f"输入形状: {dummy_input.shape}")
        print(f"输出形状: {output.shape}")
        print(f"输出维度: {output.shape[1]} (应为5)")

    if output.shape[1] != 5:
        print(f"❌ 错误: 输出维度应为5，实际为{output.shape[1]}")
        return False

    # 导出ONNX
    print(f"\n导出ONNX模型: {onnx_model_path}")
    torch.onnx.export(
        model,                          # 模型
        dummy_input,                    # 示例输入
        onnx_model_path,               # 输出路径
        export_params=True,             # 导出参数
        opset_version=11,               # ONNX算子集版本
        do_constant_folding=True,       # 常量折叠优化
        input_names=['input'],          # 输入名称
        output_names=['output'],        # 输出名称
        dynamic_axes={                  # 动态轴（支持不同批量大小）
            'input': {0: 'batch_size'},
            'output': {0: 'batch_size'}
        }
    )

    print("✓ ONNX模型导出成功")

    # 验证ONNX模型
    print("\n验证ONNX模型...")
    try:
        import onnx
        onnx_model = onnx.load(onnx_model_path)
        onnx.checker.check_model(onnx_model)
        print("✓ ONNX模型验证通过")

        # 打印模型信息
        print(f"\n输入:")
        for input_tensor in onnx_model.graph.input:
            print(f"  名称: {input_tensor.name}")
            print(f"  形状: {[dim.dim_value for dim in input_tensor.type.tensor_type.shape.dim]}")

        print(f"\n输出:")
        for output_tensor in onnx_model.graph.output:
            print(f"  名称: {output_tensor.name}")
            print(f"  形状: {[dim.dim_value for dim in output_tensor.type.tensor_type.shape.dim]}")

    except ImportError:
        print("⚠️ 警告: onnx 库未安装，跳过验证")
        print("   安装命令: pip install onnx")

    # 使用ONNX Runtime测试
    print("\n使用ONNX Runtime测试...")
    try:
        import onnxruntime as ort
        import numpy as np

        # 创建推理会话
        ort_session = ort.InferenceSession(onnx_model_path)

        # 准备输入
        input_data = np.random.randn(1, 1, 64, 64).astype(np.float32)

        # 推理
        outputs = ort_session.run(
            None,
            {'input': input_data}
        )

        print(f"✓ ONNX Runtime 推理成功")
        print(f"  输出形状: {outputs[0].shape}")
        print(f"  输出样例: {outputs[0][0][:3]}...")

        # 对比PyTorch和ONNX输出
        with torch.no_grad():
            pytorch_output = model(torch.from_numpy(input_data)).numpy()

        max_diff = np.abs(pytorch_output - outputs[0]).max()
        print(f"\nPyTorch vs ONNX 最大误差: {max_diff:.6f}")

        if max_diff < 1e-5:
            print("✓ 转换精度验证通过")
        else:
            print("⚠️ 警告: 转换存在精度差异")

    except ImportError:
        print("⚠️ 警告: onnxruntime 库未安装，跳过运行时测试")
        print("   安装命令: pip install onnxruntime")

    print(f"\n✓ 转换完成！ONNX模型已保存到: {onnx_model_path}")
    return True


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='将PyTorch模型转换为ONNX格式')
    parser.add_argument('--input', type=str, default='gesture_v2.pth',
                        help='输入PyTorch模型路径')
    parser.add_argument('--output', type=str, default='gesture_v2.onnx',
                        help='输出ONNX模型路径')
    parser.add_argument('--model-type', type=str, default='v2',
                        choices=['v1', 'v2', 'lite', 'spatial'],
                        help='模型类型: v1=GestureNet, v2=GestureNetV2, lite=GestureNetV2Lite, spatial=V2+2x2 head')
    args = parser.parse_args()

    success = convert_to_onnx(args.input, args.output, args.model_type)

    if success:
        print("\n下一步:")
        print("1. 使用 X-CUBE-AI 导入 ONNX 模型")
        print("2. 验证输出维度为 7")
        print("3. 生成 C 代码并集成到 STM32 项目")
    else:
        print("\n❌ 转换失败，请检查错误信息")
