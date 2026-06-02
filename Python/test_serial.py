#!/usr/bin/env python3
"""
串口测试工具 - 查看原始数据
"""

import serial
import time

def test_serial(port, baudrate=921600):
    """测试串口连接，显示所有接收的数据"""
    try:
        ser = serial.Serial(port, baudrate, timeout=1)
        print(f"✓ 已连接到 {port}")
        print("=" * 60)
        print("开始监听串口数据...(按 Ctrl+C 退出)")
        print("=" * 60)

        line_count = 0
        byte_count = 0

        while True:
            # 读取一行
            if ser.in_waiting > 0:
                try:
                    line = ser.readline().decode('utf-8', errors='ignore').strip()
                    if line:
                        line_count += 1
                        byte_count += len(line)

                        # 显示接收到的数据
                        if line.startswith('$'):
                            print(f"[{line_count:04d}] {line}")
                        else:
                            print(f"[{line_count:04d}] {line[:80]}...")  # 只显示前80字符

                except Exception as e:
                    print(f"✗ 解码错误: {e}")

            time.sleep(0.01)

    except KeyboardInterrupt:
        print(f"\n\n收到 {line_count} 行数据，共 {byte_count} 字节")
        print("测试结束")
    except Exception as e:
        print(f"✗ 错误: {e}")
    finally:
        if 'ser' in locals() and ser.is_open:
            ser.close()

if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description="串口测试工具")
    parser.add_argument("--port", type=str, required=True, help="串口号")
    parser.add_argument("--baudrate", type=int, default=921600, help="波特率")

    args = parser.parse_args()

    test_serial(args.port, args.baudrate)
