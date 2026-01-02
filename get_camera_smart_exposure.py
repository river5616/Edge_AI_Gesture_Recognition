import sensor, image, time, display
from pyb import UART

# ================= 配置区域 =================
# 1. 神经网络模型输入尺寸
NN_WIDTH  = 64
NN_HEIGHT = 64

# 2. 串口配置 (必须使用超高波特率)
UART_BAUD = 1500000

# 3. 曝光控制配置
TARGET_BRIGHTNESS = 120    # 目标亮度（推荐 100-140）
BRIGHTNESS_TOLERANCE = 20  # 亮度容差
MIN_EXPOSURE_US = 1000     # 最小曝光时间 (1ms)
MAX_EXPOSURE_US = 100000   # 最大曝光时间 (100ms)
MIN_GAIN_DB = 0.0          # 最小增益 (0dB)
MAX_GAIN_DB = 30.0         # 最大增益 (30dB)

# 是否启用自动曝光调整（推荐开启）
ENABLE_AUTO_ADJUST = True
# ===========================================

# 1. 初始化 LCD (使用新版 display 模块)
lcd = display.SPIDisplay()

# 2. 初始化 摄像头
sensor.reset()
sensor.set_pixformat(sensor.GRAYSCALE) # 灰度图 (1 byte/pixel)
sensor.set_framesize(sensor.QQVGA2)    # 128x160 (适合LCD显示)

# 初始曝光设置（从高曝光开始）
current_exposure = 50000  # 50ms
current_gain = 16.0       # 16dB

if ENABLE_AUTO_ADJUST:
    # 使用手动模式，后续动态调整
    sensor.set_auto_exposure(False, exposure_us=current_exposure)
    sensor.set_auto_gain(False, gain_db=current_gain)
    sensor.set_auto_whitebal(False)
    print("动态曝光调整：已启用")
else:
    # 固定高曝光
    sensor.set_auto_exposure(False, exposure_us=50000)
    sensor.set_auto_gain(False, gain_db=24.0)
    sensor.set_auto_whitebal(False)
    print("固定曝光模式：已启用")

sensor.skip_frames(time = 2000)        # 等待设置生效

# 3. 初始化 串口 (P4=TX, P5=RX)
uart = UART(3, UART_BAUD, timeout_char=10)

clock = time.clock()

# 曝光调整计数器（每N帧调整一次）
adjust_counter = 0
ADJUST_INTERVAL = 10  # 每10帧调整一次

# 辅助函数: 发送带校验的数据包
def send_image_packet(img_data):
    if not isinstance(img_data, bytearray):
        img_data = bytearray(img_data)

    size = len(img_data)
    checksum = 0
    for b in img_data:
        checksum = (checksum + b) & 0xFF

    packet = bytearray([
        0xAA, 0x55,
        (size >> 8) & 0xFF, size & 0xFF,
        checksum
    ])
    packet += img_data
    packet += bytearray([0x5A, 0xA5])

    uart.write(packet)

# 辅助函数: 动态调整曝光
def adjust_exposure(img, current_exp, current_g):
    """
    根据图像亮度动态调整曝光参数

    返回: (new_exposure, new_gain, adjusted)
    """
    # 计算图像平均亮度
    stats = img.get_statistics()
    mean_brightness = stats.mean()

    # 判断是否需要调整
    if abs(mean_brightness - TARGET_BRIGHTNESS) < BRIGHTNESS_TOLERANCE:
        return current_exp, current_g, False

    # 计算调整比例
    ratio = TARGET_BRIGHTNESS / max(mean_brightness, 1)

    # 优先调整曝光时间，其次调整增益
    new_exposure = current_exp * ratio
    new_gain = current_g

    # 限制曝光时间范围
    if new_exposure > MAX_EXPOSURE_US:
        # 曝光已达上限，增加增益
        new_exposure = MAX_EXPOSURE_US
        new_gain = min(current_g * ratio, MAX_GAIN_DB)
    elif new_exposure < MIN_EXPOSURE_US:
        # 曝光已达下限，减少增益
        new_exposure = MIN_EXPOSURE_US
        new_gain = max(current_g * ratio, MIN_GAIN_DB)

    # 限制增益范围
    new_gain = max(MIN_GAIN_DB, min(new_gain, MAX_GAIN_DB))

    return int(new_exposure), new_gain, True

print("系统启动: 智能曝光控制模式")
print(f"目标亮度: {TARGET_BRIGHTNESS} ± {BRIGHTNESS_TOLERANCE}")

while(True):
    clock.tick()

    # [1] 获取图像
    img = sensor.snapshot()

    # [2] 动态调整曝光（如果启用）
    if ENABLE_AUTO_ADJUST:
        adjust_counter += 1
        if adjust_counter >= ADJUST_INTERVAL:
            adjust_counter = 0

            new_exp, new_gain, adjusted = adjust_exposure(img, current_exposure, current_gain)

            if adjusted:
                current_exposure = new_exp
                current_gain = new_gain

                # 应用新的曝光参数
                sensor.set_auto_exposure(False, exposure_us=current_exposure)
                sensor.set_auto_gain(False, gain_db=current_gain)

                # 等待几帧让新设置生效
                sensor.skip_frames(time=100)

                # 获取调整后的图像
                img = sensor.snapshot()

                print(f"调整曝光: Exp={current_exposure}us, Gain={current_gain:.1f}dB, 亮度={img.get_statistics().mean():.1f}")

    # [3] 显示到 LCD（显示实际亮度）
    stats = img.get_statistics()
    mean_val = stats.mean()

    # 在图像上叠加亮度信息
    img.draw_string(2, 2, f"Br:{mean_val:.0f}", color=255, scale=2)

    # 如果欠曝或过曝，显示警告
    if mean_val < 50:
        img.draw_string(2, 20, "DARK!", color=255, scale=2)
    elif mean_val > 200:
        img.draw_string(2, 20, "BRIGHT!", color=255, scale=2)

    lcd.write(img)

    # [4] 缩放图像 (用于发送)
    img_small = img.copy(x_scale=NN_WIDTH/img.width(), y_scale=(NN_HEIGHT/img.height()) + 0.001)

    # [5] 通过高速串口发送
    send_image_packet(img_small.bytearray())

    # 打印帧率和亮度
    fps = clock.fps()
    if int(clock.seconds()) % 2 == 0:  # 每2秒打印一次
        print(f"FPS:{fps:.1f} | 亮度:{mean_val:.1f} | Exp:{current_exposure}us | Gain:{current_gain:.1f}dB")
