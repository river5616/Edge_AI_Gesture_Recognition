# OpenMV 曝光问题完整解决方案

## 📊 问题诊断

### 当前症状
- **平均亮度**: 22.48（正常应为 120-140）
- **欠曝程度**: 仅有正常亮度的 **1/6**
- **识别结果**: 全部误判为 palm（因为图像太暗，模型看不清细节）
- **STM32-PC 一致性**: 100%（说明两端都看到的是黑图像）

### 根本原因
OpenMV 摄像头的**自动曝光在当前光照条件下失效**，导致图像严重欠曝。

---

## 🔧 解决方案

### 方案 1：快速修复 - 固定高曝光 ⭐ **最简单**

**已修改**: `get_camera.py`（第 24-32 行）

**关键代码**:
```python
# 关闭自动曝光，使用固定高曝光值
sensor.set_auto_exposure(False, exposure_us=50000)  # 50ms曝光时间
sensor.set_auto_gain(False, gain_db=24.0)           # 24dB增益
sensor.set_auto_whitebal(False)                     # 关闭自动白平衡
```

**参数说明**:
- `exposure_us`: 曝光时间（微秒）
  - 当前: 50000us = **50ms**
  - 范围: 1000us (1ms) ~ 100000us (100ms)
  - 调整原则: 太暗加大，太亮减小

- `gain_db`: 增益（分贝）
  - 当前: **24dB**（高增益）
  - 范围: 0dB ~ 30dB
  - 注意: 增益过高会增加噪点

**使用方法**:
1. 将修改后的 `get_camera.py` 上传到 OpenMV
2. 重启 OpenMV
3. 观察 LCD 显示的亮度
4. 根据实际效果调整参数

**调试技巧**:
```python
# 如果仍然太暗，增大这些值：
sensor.set_auto_exposure(False, exposure_us=80000)  # 增加到 80ms
sensor.set_auto_gain(False, gain_db=28.0)           # 增加到 28dB

# 如果过亮，减小这些值：
sensor.set_auto_exposure(False, exposure_us=30000)  # 减少到 30ms
sensor.set_auto_gain(False, gain_db=16.0)           # 减少到 16dB

# 还可以调整对比度和亮度（取消注释）：
sensor.set_contrast(2)    # 对比度 +2 (-3 到 +3)
sensor.set_brightness(1)  # 亮度 +1 (-3 到 +3)
```

---

### 方案 2：智能曝光调整 ⭐ **推荐使用**

**文件**: `get_camera_smart_exposure.py`（已创建）

**特点**:
- ✅ 自动检测图像亮度
- ✅ 动态调整曝光和增益
- ✅ 适应不同光照条件
- ✅ 在 LCD 上显示实时亮度
- ✅ 显示欠曝/过曝警告

**核心逻辑**:
```python
# 目标亮度配置
TARGET_BRIGHTNESS = 120    # 目标亮度（推荐 100-140）
BRIGHTNESS_TOLERANCE = 20  # 容差范围

# 每10帧检查一次亮度
if abs(current_brightness - TARGET_BRIGHTNESS) > BRIGHTNESS_TOLERANCE:
    # 需要调整
    ratio = TARGET_BRIGHTNESS / current_brightness
    new_exposure = current_exposure * ratio
    new_gain = current_gain * ratio

    # 应用新设置
    sensor.set_auto_exposure(False, exposure_us=new_exposure)
    sensor.set_auto_gain(False, gain_db=new_gain)
```

**使用方法**:
1. 将 `get_camera_smart_exposure.py` 上传到 OpenMV
2. 重命名为 `main.py`（让 OpenMV 自动运行）
3. 重启 OpenMV
4. 观察串口输出和 LCD 显示

**输出示例**:
```
系统启动: 智能曝光控制模式
目标亮度: 120 ± 20
FPS:25.3 | 亮度:25.4 | Exp:50000us | Gain:16.0dB
调整曝光: Exp:80000us, Gain:20.0dB, 亮度:95.2
调整曝光: Exp=95000us, Gain=22.0dB, 亮度=118.5
FPS:24.8 | 亮度:120.1 | Exp:95000us | Gain:22.0dB  ← 已稳定
```

**LCD 显示**:
```
+-------------------+
| Br:120            |  ← 实时亮度
|                   |
|     [图像]        |
|                   |
+-------------------+
```

如果太暗会显示:
```
+-------------------+
| Br:22             |
| DARK!             |  ← 警告
|     [图像]        |
+-------------------+
```

---

### 方案 3：改善硬件光照条件 ⭐ **治本方案**

#### A. 增加光源
- **LED 补光灯**: 在摄像头附近增加白色 LED 灯
  - 推荐: 5-10W 白光 LED
  - 距离: 30-50cm
  - 角度: 45° 斜照（避免直射反光）

- **环境照明**: 提高房间整体亮度
  - 打开更多灯光
  - 拉开窗帘（白天）

#### B. 调整摄像头位置
- **避免逆光**: 摄像头不要对着窗户或灯光
- **均匀照明**: 确保手部区域光照均匀
- **减少阴影**: 光源应从侧前方照射

#### C. 背景选择
- **浅色背景**: 白色或浅灰色背景反光更好
- **避免深色**: 黑色背景会降低整体亮度
- **均匀背景**: 避免明暗对比强烈的背景

---

## 📋 测试流程

### 步骤 1: 应用修复
```bash
# 方法 A: 使用快速修复（推荐新手）
# 1. 已修改的 get_camera.py 直接上传到 OpenMV
# 2. 重启 OpenMV

# 方法 B: 使用智能曝光（推荐有经验用户）
# 1. 上传 get_camera_smart_exposure.py 到 OpenMV
# 2. 重命名为 main.py
# 3. 重启 OpenMV
```

### 步骤 2: 验证亮度
```bash
# 运行调试工具，采集50帧
python debug_inference.py --port COM17 --frames 50

# 检查 summary.txt 中的亮度：
# 平均亮度: XXX
# 曝光情况: 正常/欠曝/过曝
```

**期望结果**:
- ✅ 平均亮度: **100-140**
- ✅ 曝光情况: **正常 100%**
- ✅ 识别结果: **多样化**（不再全是 palm）

### 步骤 3: 微调参数（如果需要）

**如果亮度仍在 50-80（仍然偏暗）**:
```python
# 修改 get_camera.py
sensor.set_auto_exposure(False, exposure_us=70000)  # 增加到 70ms
sensor.set_auto_gain(False, gain_db=26.0)           # 增加到 26dB
```

**如果亮度在 160-200（过亮）**:
```python
# 修改 get_camera.py
sensor.set_auto_exposure(False, exposure_us=30000)  # 减少到 30ms
sensor.set_auto_gain(False, gain_db=16.0)           # 减少到 16dB
```

**如果亮度在 100-140（理想）**:
- ✅ 曝光已正确，可以继续数据采集

---

## 🎯 曝光参数速查表

| 环境光照 | 曝光时间 (us) | 增益 (dB) | 预期亮度 |
|----------|---------------|-----------|----------|
| 很暗（夜晚，无灯） | 80000-100000 | 26-30 | 100-120 |
| 暗（室内，一盏灯） | 50000-70000 | 20-26 | 110-130 |
| 中等（室内，多盏灯） | 30000-50000 | 16-20 | 120-140 |
| 明亮（室内，强光） | 10000-30000 | 10-16 | 130-150 |
| 很亮（窗边，白天） | 5000-10000 | 0-10 | 140-160 |

---

## ⚠️ 常见问题

### Q1: 修改后仍然很暗怎么办？

**A1**: 按照优先级尝试：

1. **增大曝光时间** (最有效):
   ```python
   sensor.set_auto_exposure(False, exposure_us=80000)  # 从 50ms 增加到 80ms
   ```

2. **增大增益**:
   ```python
   sensor.set_auto_gain(False, gain_db=28.0)  # 从 24dB 增加到 28dB
   ```

3. **调整亮度**:
   ```python
   sensor.set_brightness(2)  # 增加软件亮度 +2
   ```

4. **改善硬件光照** (最根本):
   - 增加 LED 补光灯
   - 打开房间所有灯
   - 移动到窗边（白天）

### Q2: 图像有噪点怎么办？

**A2**: 增益过高导致，减小增益：

```python
sensor.set_auto_gain(False, gain_db=16.0)  # 从 24dB 减少到 16dB

# 如果仍然太暗，增加曝光时间补偿：
sensor.set_auto_exposure(False, exposure_us=70000)  # 增加曝光时间
```

### Q3: 帧率下降怎么办？

**A3**: 曝光时间过长导致，优化设置：

```python
# 当前: 50ms 曝光 → 最大 20 FPS
# 优化: 30ms 曝光 + 更高增益
sensor.set_auto_exposure(False, exposure_us=30000)  # 减少到 30ms
sensor.set_auto_gain(False, gain_db=26.0)           # 增加增益补偿
# 预期: 最大 33 FPS
```

### Q4: 使用智能曝光后亮度波动很大？

**A4**: 调整参数：

```python
# 增大容差范围（减少调整频率）
BRIGHTNESS_TOLERANCE = 30  # 从 20 增加到 30

# 增大调整间隔（减少调整次数）
ADJUST_INTERVAL = 20  # 从 10 帧增加到 20 帧
```

### Q5: OpenMV 显示亮度正常，但 STM32 收到的图像很暗？

**A5**: 可能是串口传输问题或 STM32 端图像处理问题，检查：

1. **串口波特率**: 确保 OpenMV 和 STM32 一致
   ```python
   # OpenMV: get_camera.py
   UART_BAUD = 1500000

   # STM32: 在 CubeMX 中配置 UART3 为 1500000
   ```

2. **图像格式**: 确保是灰度图
   ```python
   sensor.set_pixformat(sensor.GRAYSCALE)
   ```

3. **检查接收**: 使用 debug_inference.py 保存图像，手动查看

---

## ✅ 验证清单

修复后，请确认：

- [ ] OpenMV LCD 显示图像亮度正常（能清晰看到手部轮廓）
- [ ] LCD 上显示 "Br:XXX" 数值在 100-140 范围
- [ ] 没有 "DARK!" 警告
- [ ] STM32 串口输出 FPS 正常（15-30 FPS）
- [ ] debug_inference.py 采集的图像亮度在 100-140
- [ ] summary.txt 显示 "曝光情况: 正常 100%"
- [ ] 识别结果多样化（不再全是 palm）

---

## 📞 下一步

曝光修复后：

1. **验证曝光效果**:
   ```bash
   python debug_inference.py --port COM17 --frames 50
   # 检查 summary.txt 中的亮度和曝光情况
   ```

2. **开始数据采集** (如果曝光正常):
   ```bash
   python collect_training_data.py --port COM17
   # 采集每个手势 500+ 张图像
   ```

3. **训练新模型**:
   ```bash
   python train_model_v2.py --dataset dataset_real --epochs 200
   ```

---

## 🎓 技术说明

### 曝光三要素

**1. 曝光时间 (Exposure Time)**
- 含义: 传感器收集光线的时间
- 单位: 微秒 (us) 或毫秒 (ms)
- 影响:
  - 增加 → 图像更亮，但帧率降低
  - 减少 → 图像更暗，但帧率提高
- 典型值: 10ms (室内) ~ 100ms (暗处)

**2. 增益 (Gain)**
- 含义: 信号放大倍数
- 单位: 分贝 (dB)
- 影响:
  - 增加 → 图像更亮，但噪点增加
  - 减少 → 图像更暗，但噪点减少
- 典型值: 0dB (明亮) ~ 30dB (黑暗)

**3. ISO (传感器灵敏度)**
- OpenMV 通过增益模拟 ISO
- ISO 100 ≈ 0dB gain
- ISO 1600 ≈ 24dB gain

### 自动曝光算法

OpenMV 自动曝光的工作原理：
```
1. 计算图像平均亮度
2. 与目标亮度比较
3. 调整曝光时间和增益
4. 重复直到稳定
```

**失效原因**:
- 光照过暗，自动调整范围不够
- 背景复杂，平均亮度计算不准
- 逆光情况，背景过亮前景过暗

**手动控制优势**:
- 精确控制曝光参数
- 避免自动调整延迟
- 适应特定光照环境
- 保持帧间亮度一致

---

## 📚 参考资料

- OpenMV 官方文档: https://docs.openmv.io/
- sensor 模块: https://docs.openmv.io/library/omv.sensor.html
- 曝光控制: https://docs.openmv.io/library/omv.sensor.html#sensor.set_auto_exposure

---

祝修复顺利！🚀
