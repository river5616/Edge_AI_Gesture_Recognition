/**
 * @file gesture_control.c
 * @brief 手势控制LED和风扇，传感器报警系统实现
 */
#include "gesture_control.h"
#include "tim.h"
#include "usart.h"
#include "bluetooth.h"
#include "disp.h"
#include <stdio.h>
#include <string.h>

/* 私有变量 */
static ControlTarget_t currentTarget = CONTROL_TARGET_LED;  // 当前控制目标

/* 报警状态 - 分别记录3种报警 */
static bool isLightAlarm = false;   // 光照报警
static bool isDistanceAlarm = false; // 距离报警
static bool isMQ2Alarm = false;      // MQ2报警

/* 报警去抖动计数器 */
static uint8_t distanceAlarmCount = 0;     // 距离报警进入计数器
static uint8_t distanceClearCount = 0;     // 距离报警解除计数器

/* 手势滤波相关 */
static uint8_t gestureBuffer[GESTURE_FILTER_SIZE] = {GESTURE_NONE};  // 手势缓冲区
static uint8_t bufferIndex = 0;                              // 缓冲区索引
static uint8_t lastStableGesture = GESTURE_NONE;            // 上次稳定手势
static uint8_t gestureStableCount = 0;                       // 手势稳定计数

/* OK手势防抖 */
static bool okGestureProcessed = false;                      // OK手势已处理标志

/* 软件PWM变量 - LED和BUZZ独立控制 */
static uint8_t pwm_counter = 0;      // PWM计数器 (0-99)
static uint8_t led_duty = 0;         // LED占空比 (0-100) - 独立保存
static uint8_t buzz_duty = 0;        // BUZZ占空比 (0-100) - 独立保存

/* TIM2定时器（用于PWM中断） */
extern TIM_HandleTypeDef htim2;

/**
 * @brief 初始化手势控制系统
 */
void GestureControl_Init(void)
{
    // 初始化手势缓冲区
    for (int i = 0; i < GESTURE_FILTER_SIZE; i++)
    {
        gestureBuffer[i] = GESTURE_NONE;
    }

    // 设置初始状态 - LED和BUZZ各自独立的占空比
    currentTarget = CONTROL_TARGET_LED;
    led_duty = 0;
    buzz_duty = 0;

    // 配置GPIO为输出模式（LED3和风扇）
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // LED3 (PB14) - 已在CubeMX中配置，这里确保是推挽输出
    GPIO_InitStruct.Pin = GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);

    // 蜂鸣器 (PG6) - 已在CubeMX中配置
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_SET);

    // 启动TIM2定时器中断（用于软件PWM，频率约1kHz）
    // 假设TIM2已配置为1kHz更新频率（1ms周期，PWM周期=100ms）
    HAL_TIM_Base_Start_IT(&htim2);

    // 关闭所有RGB报警灯
    RGB_LED_OFF();

    // 通过蓝牙发送初始化信息
    char msg[64];
    sprintf(msg, "GC Init: Target=LED, LED=0%%, BUZZ=0%%\r\n");
    HAL_UART_Transmit(&huart4, (uint8_t*)msg, strlen(msg), 100);
}

/**
 * @brief TIM2中断回调函数 - 实现软件PWM
 * @note 在stm32h7xx_it.c中调用，或在HAL_TIM_PeriodElapsedCallback中处理
 */
void GestureControl_PWM_Update(void)
{
    // PWM计数器递增 (0-99循环)
    pwm_counter++;
    if (pwm_counter >= 100)
    {
        pwm_counter = 0;
    }

    // LED3 PWM控制 (PB14)
    if (pwm_counter < led_duty)
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
    }

    // 蜂鸣器 PWM控制 (PG6)
    if (pwm_counter < buzz_duty)
    {
        HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_RESET);
    }
    else
    {
        HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_SET);
    }
}

/**
 * @brief 手势滤波 - 返回稳定的手势ID
 * @param gesture_id 当前检测到的手势ID
 * @return 滤波后的稳定手势ID，如果不稳定返回GESTURE_NONE
 */
static uint8_t GestureFilter(uint8_t gesture_id)
{
    // 更新缓冲区
    gestureBuffer[bufferIndex] = gesture_id;
    bufferIndex = (bufferIndex + 1) % GESTURE_FILTER_SIZE;

    // 统计各手势出现次数
    uint8_t counts[5] = {0};  // fist, palm, v, ok, none
    for (int i = 0; i < GESTURE_FILTER_SIZE; i++)
    {
        if (gestureBuffer[i] < 5)
        {
            counts[gestureBuffer[i]]++;
        }
    }

    // 找到出现最多的手势
    uint8_t maxCount = 0;
    uint8_t mostFrequentGesture = GESTURE_NONE;
    for (int i = 0; i < 5; i++)
    {
        if (counts[i] > maxCount)
        {
            maxCount = counts[i];
            mostFrequentGesture = i;
        }
    }

    // 判断是否稳定（超过阈值）
    if (maxCount >= GESTURE_STABLE_COUNT)
    {
        return mostFrequentGesture;
    }
    else
    {
        return GESTURE_NONE;
    }
}

/**
 * @brief 设置PWM占空比
 * @param duty 占空比（0-100）
 */
static void SetPWMDuty(uint8_t duty)
{
    if (currentTarget == CONTROL_TARGET_LED)
    {
        // 控制LED3
        led_duty = duty;
    }
    else
    {
        // 控制蜂鸣器
        buzz_duty = duty;
    }
}

/**
 * @brief 切换控制目标
 */
void GestureControl_SetTarget(ControlTarget_t target)
{
    if (currentTarget != target)
    {
        currentTarget = target;

        // 通过蓝牙发送切换信息（显示当前目标及其占空比）
        char msg[64];
        if (target == CONTROL_TARGET_LED)
        {
            sprintf(msg, "Target: LED3 (Duty=%d%%)\r\n", led_duty);
        }
        else
        {
            sprintf(msg, "Target: BUZZ (Duty=%d%%)\r\n", buzz_duty);
        }
        HAL_UART_Transmit(&huart4, (uint8_t*)msg, strlen(msg), 100);
    }
}

/**
 * @brief 设置占空比等级
 */
void GestureControl_SetDuty(DutyLevel_t duty)
{
    SetPWMDuty(duty);

    // 通过蓝牙发送占空比信息
    char msg[64];
    const char* targetName = (currentTarget == CONTROL_TARGET_LED) ? "LED3" : "BUZZ";
    sprintf(msg, "%s Duty: %d%%\r\n", targetName, duty);
    HAL_UART_Transmit(&huart4, (uint8_t*)msg, strlen(msg), 100);
}

/**
 * @brief 处理手势输入
 * @param gesture_id 手势ID
 * @param confidence 置信度
 */
void GestureControl_HandleGesture(uint8_t gesture_id, float confidence)
{
    // 滤波处理
    uint8_t filteredGesture = GestureFilter(gesture_id);

    // 如果不稳定，返回
    if (filteredGesture == GESTURE_NONE)
    {
        // 手势消失，重置OK手势处理标志
        if (lastStableGesture == GESTURE_OK)
        {
            okGestureProcessed = false;
        }
        lastStableGesture = filteredGesture;
        return;
    }

    // 如果手势与上次相同，检查是否需要处理
    if (filteredGesture == lastStableGesture)
    {
        // OK手势持续，不重复切换
        if (filteredGesture == GESTURE_OK && okGestureProcessed)
        {
            return;
        }

        // 其他手势持续，不重复设置
        if (filteredGesture != GESTURE_OK)
        {
            return;
        }
    }

    // 处理新的稳定手势
    lastStableGesture = filteredGesture;

    switch (filteredGesture)
    {
        case GESTURE_FIST:  // 握拳 -> 0%
            GestureControl_SetDuty(DUTY_0_PERCENT);
            break;

        case GESTURE_V:     // thumbs_up点赞手势 -> 10%
            GestureControl_SetDuty(DUTY_10_PERCENT);
            break;

        case GESTURE_PALM:  // 张开手掌 -> 30%
            GestureControl_SetDuty(DUTY_30_PERCENT);
            break;

        case GESTURE_OK:    // OK手势 -> 切换目标
            if (!okGestureProcessed)
            {
                ControlTarget_t newTarget = (currentTarget == CONTROL_TARGET_LED) ?
                                            CONTROL_TARGET_FAN : CONTROL_TARGET_LED;
                GestureControl_SetTarget(newTarget);
                okGestureProcessed = true;
            }
            break;

        default:
            break;
    }
}

/**
 * @brief 处理蓝牙命令
 * @param cmd 蓝牙命令
 */
void GestureControl_HandleBluetoothCmd(BT_Command_t cmd)
{
    switch (cmd)
    {
        case BT_CMD_A:  // 切换控制目标
        {
            ControlTarget_t newTarget = (currentTarget == CONTROL_TARGET_LED) ?
                                        CONTROL_TARGET_FAN : CONTROL_TARGET_LED;
            GestureControl_SetTarget(newTarget);
            break;
        }

        case BT_CMD_B:  // 占空比 0%
            GestureControl_SetDuty(DUTY_0_PERCENT);
            break;

        case BT_CMD_C:  // 占空比 10%
            GestureControl_SetDuty(DUTY_10_PERCENT);
            break;

        case BT_CMD_D:  // 占空比 30%
            GestureControl_SetDuty(DUTY_30_PERCENT);
            break;

        default:
            break;
    }
}

/**
 * @brief 检查传感器报警
 */
void GestureControl_CheckSensorAlarm(void)
{
    extern SensorData_t g_sensorData;

    bool light_alarm_now = false;
    bool distance_alarm_now = false;
    bool mq2_alarm_now = false;

    // 检查光照过大 → 红灯
    if (g_sensorData.lightLux > LIGHT_ALARM_THRESHOLD)
    {
        light_alarm_now = true;
    }

    // 检查距离过小 → 绿灯（带滞后阈值和去抖动）
    if (g_sensorData.distance > 0)
    {
        if (!isDistanceAlarm)
        {
            // 当前未报警，检查是否需要进入报警
            if (g_sensorData.distance < DISTANCE_ALARM_THRESHOLD)
            {
                distanceAlarmCount++;
                distanceClearCount = 0;  // 重置解除计数器

                if (distanceAlarmCount >= ALARM_DEBOUNCE_COUNT)
                {
                    distance_alarm_now = true;
                    distanceAlarmCount = 0;  // 重置计数器
                }
            }
            else
            {
                distanceAlarmCount = 0;  // 重置进入计数器
            }
        }
        else
        {
            // 当前正在报警，检查是否需要解除报警
            if (g_sensorData.distance > DISTANCE_CLEAR_THRESHOLD)
            {
                distanceClearCount++;
                distanceAlarmCount = 0;  // 重置进入计数器

                if (distanceClearCount >= ALARM_DEBOUNCE_COUNT)
                {
                    distance_alarm_now = false;
                    distanceClearCount = 0;  // 重置计数器
                }
                else
                {
                    distance_alarm_now = true;  // 保持报警状态
                }
            }
            else
            {
                distance_alarm_now = true;  // 保持报警状态
                distanceClearCount = 0;  // 重置解除计数器
            }
        }
    }
    else
    {
        // 距离无效，重置计数器
        distanceAlarmCount = 0;
        distanceClearCount = 0;
    }

    // 检查MQ2浓度过大 → 蓝灯
    if (g_sensorData.mq2PPM > MQ2_ALARM_THRESHOLD)
    {
        mq2_alarm_now = true;
    }

    // 光照报警状态变化
    if (light_alarm_now != isLightAlarm)
    {
        isLightAlarm = light_alarm_now;

        if (light_alarm_now)
        {
            // 开启红灯
            RGB_RED_ON;

            // 发送报警信息
            char alarmMsg[128];
            sprintf(alarmMsg, "ALARM: Light too high (%.1f Lux)!\r\n", g_sensorData.lightLux);
            HAL_UART_Transmit(&huart4, (uint8_t*)alarmMsg, strlen(alarmMsg), 100);
        }
        else
        {
            // 关闭红灯
            RGB_RED_OFF;

            // 发送解除报警信息
            const char* msg = "ALARM: Light cleared\r\n";
            HAL_UART_Transmit(&huart4, (uint8_t*)msg, strlen(msg), 100);
        }
    }

    // 距离报警状态变化
    if (distance_alarm_now != isDistanceAlarm)
    {
        isDistanceAlarm = distance_alarm_now;

        if (distance_alarm_now)
        {
            // 开启绿灯
            RGB_GREEN_ON;

            // 发送报警信息
            char alarmMsg[128];
            sprintf(alarmMsg, "ALARM: Distance too close (%d cm)!\r\n", g_sensorData.distance);
            HAL_UART_Transmit(&huart4, (uint8_t*)alarmMsg, strlen(alarmMsg), 100);
        }
        else
        {
            // 关闭绿灯
            RGB_GREEN_OFF;

            // 发送解除报警信息
            const char* msg = "ALARM: Distance cleared\r\n";
            HAL_UART_Transmit(&huart4, (uint8_t*)msg, strlen(msg), 100);
        }
    }

    // MQ2报警状态变化
    if (mq2_alarm_now != isMQ2Alarm)
    {
        isMQ2Alarm = mq2_alarm_now;

        if (mq2_alarm_now)
        {
            // 开启蓝灯
            RGB_BLUE_ON;

            // 发送报警信息
            char alarmMsg[128];
            sprintf(alarmMsg, "ALARM: MQ2 too high (%.1f PPM)!\r\n", g_sensorData.mq2PPM);
            HAL_UART_Transmit(&huart4, (uint8_t*)alarmMsg, strlen(alarmMsg), 100);
        }
        else
        {
            // 关闭蓝灯
            RGB_BLUE_OFF;

            // 发送解除报警信息
            const char* msg = "ALARM: MQ2 cleared\r\n";
            HAL_UART_Transmit(&huart4, (uint8_t*)msg, strlen(msg), 100);
        }
    }
}

/**
 * @brief 定期处理函数（在主循环或任务中调用）
 */
void GestureControl_Process(void)
{
    // 检查传感器报警
    GestureControl_CheckSensorAlarm();
}

/**
 * @brief 获取当前控制目标
 */
ControlTarget_t GestureControl_GetTarget(void)
{
    return currentTarget;
}

/**
 * @brief 获取当前占空比
 */
uint8_t GestureControl_GetDuty(void)
{
    if (currentTarget == CONTROL_TARGET_LED)
    {
        return led_duty;
    }
    else
    {
        return buzz_duty;
    }
}

/**
 * @brief 获取报警状态
 */
bool GestureControl_IsAlarm(void)
{
    return (isLightAlarm || isDistanceAlarm || isMQ2Alarm);
}
