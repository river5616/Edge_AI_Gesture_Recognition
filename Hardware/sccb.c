#include "sccb.h"

// 微秒延时函数(使用 volatile 防止编译器优化)
static void delay_us(uint32_t us)
{
    // 在 480MHz 时钟下，每微秒约 480 个时钟周期
    // 每次循环约 4-6 个时钟周期（取决于编译优化）
    // 使用 volatile 确保循环不被优化掉
    volatile uint32_t ticks = us * (SystemCoreClock / 1000000U) / 9U;
    while(ticks--);
}

// 配置SDA为输出模式
void SCCB_SDA_Mode_Out(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = SCCB_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(SCCB_SDA_PORT, &GPIO_InitStruct);
}

// 配置SDA为输入模式
void SCCB_SDA_Mode_In(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = SCCB_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(SCCB_SDA_PORT, &GPIO_InitStruct);
}

// 初始化SCCB接口
void SCCB_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 使能GPIOD时钟
    __HAL_RCC_GPIOD_CLK_ENABLE();

    // 配置SCL引脚
    GPIO_InitStruct.Pin = SCCB_SCL_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(SCCB_SCL_PORT, &GPIO_InitStruct);

    // 配置SDA引脚为输出模式
    SCCB_SDA_Mode_Out();

    // 初始状态设置为高电平
    SCCB_SCL_HIGH();
    SCCB_SDA_HIGH();
}

// SCCB起始信号
void SCCB_Start(void)
{
    SCCB_SDA_OUT();
    SCCB_SDA_HIGH();
    SCCB_SCL_HIGH();
    delay_us(50);
    SCCB_SDA_LOW();
    delay_us(50);
    SCCB_SCL_LOW();
}

// SCCB停止信号
void SCCB_Stop(void)
{
    SCCB_SDA_OUT();
    SCCB_SDA_LOW();
    delay_us(50);
    SCCB_SCL_HIGH();
    delay_us(50);
    SCCB_SDA_HIGH();
    delay_us(50);
}

// 产生NA信号
void SCCB_No_Ack(void)
{
    delay_us(50);
    SCCB_SDA_HIGH();
    SCCB_SCL_HIGH();
    delay_us(50);
    SCCB_SCL_LOW();
    delay_us(50);
    SCCB_SDA_LOW();
    delay_us(50);
}

// SCCB写入一个字节
// 返回值: 0-成功, 1-失败
uint8_t SCCB_WR_Byte(uint8_t dat)
{
    uint8_t j, res;

    SCCB_SDA_OUT();

    for(j = 0; j < 8; j++)
    {
        if(dat & 0x80)
            SCCB_SDA_HIGH();
        else
            SCCB_SDA_LOW();

        dat <<= 1;
        delay_us(50);
        SCCB_SCL_HIGH();
        delay_us(50);
        SCCB_SCL_LOW();
    }

    // 接收应答
    SCCB_SDA_IN();
    delay_us(50);
    SCCB_SCL_HIGH();
    delay_us(50);

    if(SCCB_SDA_READ())
        res = 1;  // 失败
    else
        res = 0;  // 成功

    SCCB_SCL_LOW();
    SCCB_SDA_OUT();

    return res;
}

// SCCB读取一个字节
uint8_t SCCB_RD_Byte(void)
{
    uint8_t temp = 0, j;

    SCCB_SDA_IN();

    for(j = 8; j > 0; j--)
    {
        delay_us(50);
        SCCB_SCL_HIGH();
        temp = temp << 1;

        if(SCCB_SDA_READ())
            temp++;

        delay_us(50);
        SCCB_SCL_LOW();
    }

    SCCB_SDA_OUT();
    return temp;
}

// 写寄存器
// 返回值: 0-成功, 1-失败
uint8_t SCCB_WR_Reg(uint8_t reg, uint8_t data)
{
    uint8_t res = 0;

    SCCB_Start();

    if(SCCB_WR_Byte(SCCB_ID))
        res = 1;

    delay_us(100);

    if(SCCB_WR_Byte(reg))
        res = 1;

    delay_us(100);

    if(SCCB_WR_Byte(data))
        res = 1;

    SCCB_Stop();

    return res;
}

// 读寄存器
uint8_t SCCB_RD_Reg(uint8_t reg)
{
    uint8_t val = 0;

    SCCB_Start();
    SCCB_WR_Byte(SCCB_ID);
    delay_us(100);
    SCCB_WR_Byte(reg);
    delay_us(100);
    SCCB_Stop();
    delay_us(100);

    // 重新启动并读取
    SCCB_Start();
    SCCB_WR_Byte(SCCB_ID | 0x01);
    delay_us(100);
    val = SCCB_RD_Byte();
    SCCB_No_Ack();
    SCCB_Stop();

    return val;
}
