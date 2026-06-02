# 手势系统更新：point → v_sign

## 更新日期
2026-01-03

## 更新原因
**问题**: fist（拳头）容易被错误识别为point（两指并拢）

**原因**: 两指并拢时的形状与握紧的拳头相似度较高，导致误识别

## 解决方案
将 **point（两指并拢）** 改为 **v_sign（V手势）**

## 手势对比

### 旧方案 - point（两指并拢）
- 食指和中指伸直并拢
- 形成"箭头"形状
- ❌ 问题：与fist相似度高，容易混淆

### 新方案 - v_sign（V手势）
- 食指和中指伸直分开
- 形成"V"字形（30-60度角）
- ✅ 优势：与fist形状差异明显，不易混淆
- ✅ 经典胜利手势，识别度高

## 最终手势集（5个）

| ID | 手势名称 | 中文名称 | 特征描述 |
|----|---------|---------|---------|
| 0 | fist | 拳头 | 五指完全握紧，紧凑圆形 |
| 1 | palm | 手掌 | 五指展开分开，扇形 |
| 2 | **v_sign** | **V手势** | **食指中指分开成V形** |
| 3 | ok | OK手势 | 拇指食指圈，圆圈形 |
| 4 | nothing | 无手势 | 空背景 |

## 修改文件清单

### Python文件（14个）✅
- gesture_definitions.py - 核心定义
- train_model.py, train_model_v2.py - 训练脚本
- data_collector.py, collect_training_data.py - 数据采集
- debug_inference.py - 调试推理
- realtime_inference.py, realtime_inference_compare.py - 实时推理
- test_onnx_model.py, test_pytorch_model.py, test_pytorch_model_v2.py - 测试
- augment_dataset.py - 数据增强
- convert_to_onnx_v2.py, test_serial.py

### C代码文件（4个）✅
- Hardware/disp.c - OLED显示改为"V"
- Hardware/disp.h
- Inc/app_x-cube-ai.h - 注释更新
- Src/app_x-cube-ai.c - 注释更新

## v_sign采集标准

### 正确姿势
- ✅ 食指和中指完全伸直
- ✅ 两指之间分开呈V字形（约30-60度角）
- ✅ 其他三指（拇指、无名指、小指）完全握拳
- ✅ 可以竖直向上或倾斜

### 要避免
- ❌ 两指并拢（变成point）
- ❌ 两指过度分开（>90度）
- ❌ 其他手指伸出
- ❌ 手指弯曲

### 采集建议
- 推荐数量：500-1000张
- 多角度：正面、侧面、45°斜角
- 多距离：近、中、远
- 多朝向：向上、向左、向右、倾斜

## 下一步操作

### 1. 重新采集v_sign数据
```bash
python data_collector.py --port COM17 --gesture 2 --num 500
```

### 2. 重新训练模型
```bash
python train_model_v2.py --model-version lite --epochs 50
```

### 3. 转换为ONNX
```bash
python convert_to_onnx_v2.py --input gesture_model_v2_lite.pth --output gesture_model_v_sign.onnx
```

### 4. 部署到STM32
- 使用X-CUBE-AI转换ONNX模型
- 烧录到STM32
- OLED将显示 "V" 代表v_sign手势

## 预期改进

✅ **fist与v_sign区别明显** - 握拳 vs V形张开，不会混淆

✅ **v_sign与palm区别清晰** - 两指 vs 五指，容易区分

✅ **手势形状更独特** - V形是经典手势，识别度高

✅ **提高整体准确率** - 解决最大的混淆问题

## 验证结果

✅ Python文件: 0个point残留

✅ C代码: 显示"V"，注释全部更新

✅ 所有18个文件修改完成

---

**修改完成日期：** 2026-01-03  
**状态：** ✅ 代码修改完成，等待数据采集和训练
