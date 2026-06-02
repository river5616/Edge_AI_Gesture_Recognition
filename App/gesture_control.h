/**
 * @file gesture_control.h
 * @brief 手势控制LED和蜂鸣器，传感器报警系统
 */
#ifndef __GESTURE_CONTROL_H
#define __GESTURE_CONTROL_H

#include "stm32h7xx_hal.h"
#include "bluetooth.h"  // 包含蓝牙头文件以使用 BT_Command_t 类型
#include <stdint.h>
#include <stdbool.h>

/* 控制目标枚举 */
typedef enum {
    CONTROL_TARGET_LED = 0,   // 控制LED3
    CONTROL_TARGET_FAN = 1    // 控制蜂鸣器(BUZZ)
} ControlTarget_t;

/* 占空比等级枚举 */
typedef enum {
    DUTY_0_PERCENT = 0,    // 0% - fist手势
    DUTY_10_PERCENT = 10,  // 10% - v手势
    DUTY_30_PERCENT = 30   // 30% - palm手势
} DutyLevel_t;

/* 手势ID定义（与推理输出一致） */
#define GESTURE_FIST  0
#define GESTURE_PALM  1
#define GESTURE_V     2
#define GESTURE_OK    3
#define GESTURE_NONE  4

/* RGB LED控制宏 */
#define RGB_RED_ON      HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_SET)
#define RGB_RED_OFF     HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_RESET)
#define RGB_GREEN_ON    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7,  GPIO_PIN_SET)
#define RGB_GREEN_OFF   HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7,  GPIO_PIN_RESET)
#define RGB_BLUE_ON     HAL_GPIO_WritePin(GPIOG, GPIO_PIN_4,  GPIO_PIN_SET)
#define RGB_BLUE_OFF    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_4,  GPIO_PIN_RESET)

#define RGB_LED_ON()    do { RGB_RED_ON; RGB_GREEN_ON; RGB_BLUE_ON; } while(0)
#define RGB_LED_OFF()   do { RGB_RED_OFF; RGB_GREEN_OFF; RGB_BLUE_OFF; } while(0)

/* 报警阈值 */
#define LIGHT_ALARM_THRESHOLD    1000.0f  // 光照过大阈值(Lux)
#define DISTANCE_ALARM_THRESHOLD 10        // 距离过小阈值(cm) - 进入报警
#define DISTANCE_CLEAR_THRESHOLD 15        // 距离报警解除阈值(cm) - 退出报警
#define MQ2_ALARM_THRESHOLD      10.0f   // MQ2浓度过大阈值(PPM)

/* 去抖动参数 */
#define ALARM_DEBOUNCE_COUNT     3         // 报警去抖动计数阈值

/* 滤波参数 */
#define GESTURE_FILTER_SIZE      5         // 滤波窗口大小
#define GESTURE_STABLE_COUNT     3         // 手势稳定判定次数

/* 函数声明 */
void GestureControl_Init(void);
void GestureControl_Process(void);
void GestureControl_HandleGesture(uint8_t gesture_id, float confidence);
void GestureControl_HandleBluetoothCmd(BT_Command_t cmd);
void GestureControl_SetTarget(ControlTarget_t target);
void GestureControl_SetDuty(DutyLevel_t duty);
void GestureControl_CheckSensorAlarm(void);

/* PWM更新函数（在TIM2中断中调用） */
void GestureControl_PWM_Update(void);

/* 状态查询 */
ControlTarget_t GestureControl_GetTarget(void);
uint8_t GestureControl_GetDuty(void);
bool GestureControl_IsAlarm(void);

#endif /* __GESTURE_CONTROL_H */
