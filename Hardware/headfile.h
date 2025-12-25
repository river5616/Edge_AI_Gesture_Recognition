#ifndef __HEADFILE_H
#define __HEADFILE_H


// 标准库文件
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"
#include "stdint.h"
#include "math.h"
#include "ctype.h"
#include "stdbool.h"
#include "time.h"

// cubeMX生成的文件
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

// 自定义文件
#include "led.h" // LED灯
#include "key.h" // 按键
#include "OLED.h" // OLED功能接口
#include "disp.h" // OLED显示


// OLED
extern u8 view; // 显示页面
extern u8 cnt;









#endif
