#ifndef __HEADFILE_H
#define __HEADFILE_H


// ��׼���ļ�
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"
#include "stdint.h"
#include "math.h"
#include "ctype.h"
#include "stdbool.h"
#include "time.h"

// cubeMX���ɵ��ļ�
#include "main.h"
#include "cmsis_os.h"
#include "memorymap.h"
#include "gpio.h"

typedef uint8_t     u8;
typedef int8_t      s8;
typedef uint16_t   u16;
typedef int16_t    s16;
typedef uint32_t   u32;
typedef int32_t    s32;

// �Զ����ļ�
#include "led.h" // LED��
#include "key.h" // ����
#include "OLED.h" // OLED���ܽӿ�
#include "disp.h" // OLED��ʾ

// ����ͷ���
#include "uart_tx_task.h"
#include "uart_rx_task.h"
#include "task_common.h"     // ����������
#include "inference_task.h"
#include "openmv_rx_task.h"
#include "uart_callbacks.h"

// OLED
extern u8 view; // ��ʾҳ��
extern u8 cnt1,cnt2;

extern uint8_t camera_init_flag;
extern uint8_t capture_status;  // capture_frame返回值
extern uint8_t capture_success_cnt;  // 采集成功次数
extern uint8_t capture_fail_reason;  // 失败原因代码
extern uint8_t href_state;  // HREF引脚状态（调试用）







#endif
