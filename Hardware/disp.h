#ifndef __DISP_H
#define __DISP_H

#include "headfile.h"

#define line1 0
#define line2 8
#define line3 16
#define line4 24
#define line5 32
#define line6 40
#define line7 48
#define line8 56

// 显示视图定义
#define VIEW_STATUS     0  // 状态信息视图
#define VIEW_IMAGE      1  // 图像显示视图
#define VIEW_UART_STATS 2  // UART接收统计视图
#define VIEW_ALL_CONF   3  // 所有手势置信度视图

// 外部变量声明
extern u8 view;

void oled_disp(void);

/*应用函数*/
void OLED_ShowGestureResult(uint8_t GestureID, float Confidence);















#endif
