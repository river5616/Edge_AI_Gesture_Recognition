/**
 * @file DHT11.c
 * @brief DHT11温湿度传感器驱动（优化安全版本）
 * @note 针对O1优化做了特殊处理：使用DWT精确延时和禁用优化
 */
#include "DHT11.h"
#include <string.h>

/* DWT延时相关（使用DWT计数器实现精确延时） */
static inline void DHT11_delay_us(uint32_t us) __attribute__((always_inline));
static inline void DHT11_delay_us(uint32_t us)
{
    uint32_t startTick = DWT->CYCCNT;
    uint32_t delayTicks = us * (SystemCoreClock / 1000000);
    while ((DWT->CYCCNT - startTick) < delayTicks);
}

static inline void DHT11_delay_ms(uint32_t ms) __attribute__((always_inline));
static inline void DHT11_delay_ms(uint32_t ms)
{
    DHT11_delay_us(ms * 1000);
}

/* GPIO控制宏 */
#define DHT11_DQ_OUT(x) do { \
    if (x) HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_SET); \
    else HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_RESET); \
} while(0)

#define DHT11_DQ_IN() HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN)

/* GPIO方向切换 */
static void DHT11_GPIO_Out(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DHT11_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);
}

static void DHT11_GPIO_In(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DHT11_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);
}

/* 发送起始信号（禁用优化以保证时序） */
static uint8_t DHT11_Start(void)
{
    volatile uint16_t timeout;

    DHT11_GPIO_Out();
    DHT11_DQ_OUT(1);
    DHT11_delay_us(2);
    DHT11_DQ_OUT(0);
    DHT11_delay_ms(20);  // 拉低至少18ms
    DHT11_DQ_OUT(1);
    DHT11_delay_us(30);  // 等待20-40us

    DHT11_GPIO_In();

    // 等待DHT11响应（拉低总线）
    timeout = 0;
    while (DHT11_DQ_IN() && timeout < 100) {
        DHT11_delay_us(1);
        timeout++;
    }
    if (timeout >= 100) return 1;  // 超时

    // 等待DHT11拉高
    timeout = 0;
    while (!DHT11_DQ_IN() && timeout < 100) {
        DHT11_delay_us(1);
        timeout++;
    }
    if (timeout >= 100) return 1;  // 超时

    // 等待DHT11拉低（准备发送数据）
    timeout = 0;
    while (DHT11_DQ_IN() && timeout < 100) {
        DHT11_delay_us(1);
        timeout++;
    }
    if (timeout >= 100) return 1;  // 超时

    return 0;  // 成功
}

/* 读取一个字节（禁用优化以保证时序） */
static uint8_t DHT11_Read_Byte(void)
{
    volatile uint8_t i;
    volatile uint8_t byte = 0;
    volatile uint16_t timeout;

    for (i = 0; i < 8; i++)
    {
        // 等待低电平结束
        timeout = 0;
        while (!DHT11_DQ_IN() && timeout < 100) {
            DHT11_delay_us(1);
            timeout++;
        }

        // 延时50us后判断高低电平
        DHT11_delay_us(50);

        byte <<= 1;
        if (DHT11_DQ_IN()) {
            byte |= 0x01;
            // 等待高电平结束
            timeout = 0;
            while (DHT11_DQ_IN() && timeout < 100) {
                DHT11_delay_us(1);
                timeout++;
            }
        }
    }

    return byte;
}

/**
 * @brief 初始化DHT11
 */
void DHT11_Init(void)
{
    /* 使能DWT用于精确延时 */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    DWT->CYCCNT = 0;

    /* 使能GPIO时钟 */
    DHT11_CLK_ENABLE();

    /* 初始化为输出高电平 */
    DHT11_GPIO_Out();
    DHT11_DQ_OUT(1);
}

/**
 * @brief 读取DHT11数据
 * @param data DHT11数据结构指针
 * @retval 0:成功 1:失败
 */
uint8_t DHT11_Read(DHT11_Data_t *data)
{
    uint8_t buf[5];
    uint8_t i;
    uint8_t checksum;

    // 发送起始信号
    if (DHT11_Start() != 0) {
        data->valid = 0;
        return 1;
    }

    // 读取40位数据
    for (i = 0; i < 5; i++) {
        buf[i] = DHT11_Read_Byte();
    }

    // 校验和
    checksum = buf[0] + buf[1] + buf[2] + buf[3];
    if (checksum != buf[4]) {
        data->valid = 0;
        return 1;
    }

    // 保存数据
    data->humidity_int = buf[0];
    data->humidity_dec = buf[1];
    data->temperature_int = buf[2];
    data->temperature_dec = buf[3];
    data->valid = 1;

    return 0;
}
