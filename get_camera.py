import sensor, image, time, display
from pyb import UART

# ================= 配置区域 =================
# 1. 神经网络模型输入尺寸
NN_WIDTH  = 64
NN_HEIGHT = 64

# 2. 串口配置 (必须使用超高波特率)
# OpenMV H7 Plus 支持 1.5Mbps 甚至 3Mbps
# 注意：STM32 端也要配置成一模一样的波特率！
UART_BAUD = 1500000
# UART_BAUD = 921600

# 3. 帧率控制 (设置发送间隔，单位ms)
# 0 = 全速发送 (~32 FPS)
# 30 = 约10 FPS
# 62 = 约8 FPS
# 100 = 约5 FPS
FRAME_DELAY_MS = 62  # 默认全速，根据需要修改
# ===========================================

# 1. 初始化 LCD (使用新版 display 模块)
lcd = display.SPIDisplay()

# 2. 初始化 摄像头
sensor.reset()
sensor.set_pixformat(sensor.GRAYSCALE) # 灰度图 (1 byte/pixel)
sensor.set_framesize(sensor.QQVGA2)    # 128x160 (适合LCD显示)
sensor.skip_frames(time = 2000)        # 等待自动增益稳定

# 3. 初始化 串口 (P4=TX, P5=RX)
# timeout_char 设置小一点，保证发送不阻塞太久
uart = UART(3, UART_BAUD, timeout_char=10)

clock = time.clock()

# 辅助函数: 发送带校验的数据包
def send_image_packet(img_data):
    # 协议格式: [头0xAA,0x55] [长度2字节] [校验1字节] [数据...] [尾0x5A,0xA5]

    # 确保img_data是bytearray类型
    if not isinstance(img_data, bytearray):
        img_data = bytearray(img_data)

    # 构建完整数据包（一次性发送，避免触发STM32的空闲中断）
    size = len(img_data)

    # 计算校验和
    checksum = 0
    for b in img_data:
        checksum = (checksum + b) & 0xFF

    # 组装完整数据包
    packet = bytearray([
        0xAA, 0x55,                    # 帧头
        (size >> 8) & 0xFF, size & 0xFF,  # 长度（Big Endian）
        checksum                        # 校验和
    ])
    packet += img_data                  # 图像数据（使用+=而不是extend）
    packet += bytearray([0x5A, 0xA5])   # 帧尾

    # 关键：一次性发送完整数据包
    uart.write(packet)

print("系统启动: High-Speed UART Mode")

while(True):
    clock.tick()

    # [1] 获取原始图像 (干净的)
    img = sensor.snapshot()

    # [2] 立即生成缩放后的图像 (用于发送给STM32)
    # 在画字之前先Copy出来，保证 img_small 是干净的！
    img_small = img.copy(x_scale=NN_WIDTH/img.width(), y_scale=(NN_HEIGHT/img.height()) + 0.001)

    # [3] 计算亮度并在原图上叠加信息 (仅用于 LCD 显示)
    stats = img.get_statistics()
    mean_val = stats.mean()
    # 这行代码修改的是 img，但此时 img_small 已经生成好了，互不影响
    img.draw_string(1, 1, f"Br:{mean_val:.0f}", color=255, scale=1)

    # [4] 显示带文字的原图到 LCD (给人看)
    lcd.write(img)

    # [5] 通过高速串口发送 (发送的是干净的小图)
    send_image_packet(img_small.bytearray())

    # [6] 帧率控制延时
    if FRAME_DELAY_MS > 0:
        time.sleep_ms(FRAME_DELAY_MS)

    # 打印帧率
    print(clock.fps())
