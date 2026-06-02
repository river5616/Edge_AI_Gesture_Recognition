#include "MQ2.h"

/* ADC句柄（需要用户在CubeMX中配置ADC1） */
extern ADC_HandleTypeDef hadc3;

/* 延时函数 */
extern void osDelay(uint32_t ms);

uint32_t g_adc3Buffer[MQ2_READ_TIMES]__attribute__((section(".ram_d1"), aligned(32)));

/* MQ2初始化 */
void MQ2_Init(void)
{
#if MQ2_MODE
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* 使能GPIO时钟 */
    MQ2_AO_GPIO_CLK_ENABLE();

    /* 配置ADC引脚为模拟输入 */
    GPIO_InitStruct.Pin = MQ2_AO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(MQ2_AO_PORT, &GPIO_InitStruct);

    /* ADC需要在CubeMX中配置并初始化 */
#else
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* 使能GPIO时钟 */
    MQ2_DO_GPIO_CLK_ENABLE();

    /* 配置DO引脚为输入上拉 */
    GPIO_InitStruct.Pin = MQ2_DO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(MQ2_DO_PORT, &GPIO_InitStruct);
#endif
}

#if MQ2_MODE
/* 读取ADC值 */
static uint16_t MQ2_ADC_Read(void)
{
    ADC_ChannelConfTypeDef sConfig = {0};

    /* 配置ADC通道 */
    sConfig.Channel = MQ2_ADC_CHANNEL;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_64CYCLES_5;
    sConfig.SingleDiff = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset = 0;

    if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
    {
        return 0;
    }

    /* 启动ADC转换 */
    HAL_ADC_Start(&hadc3);

    /* 等待转换完成 */
    if (HAL_ADC_PollForConversion(&hadc3, 10) == HAL_OK)
    {
        return (uint16_t)HAL_ADC_GetValue(&hadc3);
    }

    return 0;
}
#endif

/* 获取MQ2数据（ADC值或数字值） */
uint16_t MQ2_GetData(void)
{
#if MQ2_MODE
    uint32_t tempData = 0;
    uint8_t i;

    /* 多次读取求平均 */
    for (i = 0; i < MQ2_READ_TIMES; i++)
    {
        tempData += MQ2_ADC_Read();
    }

    tempData /= MQ2_READ_TIMES;
    return (uint16_t)tempData;
#else
    /* 数字模式：低电平表示检测到烟雾 */
    uint16_t tempData;
    tempData = !HAL_GPIO_ReadPin(MQ2_DO_PORT, MQ2_DO_PIN);
    return tempData;
#endif
}

/* 获取MQ2数据（PPM浓度值） */
float MQ2_GetDataPPM(void)
{
#if MQ2_MODE
    float tempData = 0.0f;
    uint8_t i;
    float Vol, RS, R0, ppm;

    /* 多次读取求平均 */
    for (i = 0; i < MQ2_READ_TIMES; i++)
    {
        tempData += MQ2_ADC_Read();
    }
    tempData /= MQ2_READ_TIMES;

    /* STM32H7的ADC是16位，但实际分辨率配置可能是12位或16位
     * 假设使用12位分辨率，满量程4096
     * 参考电压3.3V（根据实际硬件调整）
     */
    Vol = (tempData * 3.3f) / 65535.0f;
    RS = (3.3f - Vol) / (Vol * 0.5f);  // 假设负载电阻0.5kΩ
    R0 = 6.64f;  // 校准值，需要根据实际硬件设定

    /* 计算浓度PPM */
    if (RS / R0 > 0)
    {
        ppm = powf(11.5428f * R0 / RS, 0.6549f);
    }
    else
    {
        ppm = 0;
    }

    return ppm;
#else
    return 0;  // 数字模式不返回PPM值
#endif
}
