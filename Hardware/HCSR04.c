#include "HCSR04.h"

/* 延时函数声明 */
static void delay_us(uint32_t us);

/* 微秒延时函数 - 使用DWT */
static void delay_us(uint32_t us)
{
    uint32_t startTick = DWT->CYCCNT;
    uint32_t delayTicks = us * (SystemCoreClock / 1000000);

    while ((DWT->CYCCNT - startTick) < delayTicks);
}

/* HC-SR04初始化 */
void HCSR04_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* 使能GPIO时钟 */
    HCSR04_TRIG_GPIO_CLK_ENABLE();
    HCSR04_ECHO_GPIO_CLK_ENABLE();

    /* 配置Trig为输出 */
    GPIO_InitStruct.Pin = HCSR04_TRIG_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(HCSR04_TRIG_PORT, &GPIO_InitStruct);

    /* 配置Echo为输入浮空 */
    GPIO_InitStruct.Pin = HCSR04_ECHO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(HCSR04_ECHO_PORT, &GPIO_InitStruct);

    /* 初始化Trig为低电平 */
    HAL_GPIO_WritePin(HCSR04_TRIG_PORT, HCSR04_TRIG_PIN, GPIO_PIN_RESET);

    /* 确保DWT用于延时 */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    DWT->CYCCNT = 0;
}

/* 获取距离（单位：厘米） */
uint16_t HCSR04_GetDistance(void)
{
    uint32_t startTime, endTime;
    uint32_t timeout = 0;

    /* 发送触发信号 */
    HAL_GPIO_WritePin(HCSR04_TRIG_PORT, HCSR04_TRIG_PIN, GPIO_PIN_SET);
    delay_us(20);
    HAL_GPIO_WritePin(HCSR04_TRIG_PORT, HCSR04_TRIG_PIN, GPIO_PIN_RESET);

    /* 等待Echo变高，带超时保护 */
    while (HAL_GPIO_ReadPin(HCSR04_ECHO_PORT, HCSR04_ECHO_PIN) == GPIO_PIN_RESET)
    {
        timeout++;
        if (timeout > 100000)
            return 888;  // 超时返回错误值
    }

    /* 开始计时 */
    startTime = DWT->CYCCNT;

    /* 等待Echo变低，带超时保护 */
    timeout = 0;
    while (HAL_GPIO_ReadPin(HCSR04_ECHO_PORT, HCSR04_ECHO_PIN) == GPIO_PIN_SET)
    {
        timeout++;
        if (timeout > 500000)
            return 777;  // 超时返回错误值
    }

    endTime = DWT->CYCCNT;

    /* 计算距离（厘米）
     * 时间 = (endTime - startTime) / (SystemCoreClock / 1000000) 微秒
     * 距离 = 时间 * 340m/s / 2 / 10000 = 时间 / 58.8 ≈ 时间 / 58
     */
    uint32_t timeUs = (endTime - startTime) / (SystemCoreClock / 1000000);
    return (uint16_t)(timeUs / 58);
}
