#include "BH1750.h"

/* 延时函数声明 */
static void delay_us(uint32_t us);

/* 微秒延时函数 - 使用DWT */
static void delay_us(uint32_t us)
{
    uint32_t startTick = DWT->CYCCNT;
    uint32_t delayTicks = us * (SystemCoreClock / 1000000);

    while ((DWT->CYCCNT - startTick) < delayTicks);
}

/* 设置SDA为输出 */
static void SDA_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = BH1750_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(BH1750_SDA_PORT, &GPIO_InitStruct);
}

/* 设置SDA为输入 */
static void SDA_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = BH1750_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(BH1750_SDA_PORT, &GPIO_InitStruct);
}

/* BH1750初始化 */
void BH1750_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* 使能DWT用于微秒延时 */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    DWT->CYCCNT = 0;
    /* 使能SCL和SDA时钟 */
    BH1750_SCL_CLK_ENABLE();
    BH1750_SDA_CLK_ENABLE();

    /* 配置SCL和SDA为输出 */
    GPIO_InitStruct.Pin = BH1750_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(BH1750_SDA_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = BH1750_SCL_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(BH1750_SCL_PORT, &GPIO_InitStruct);

    /* 初始化为高电平 */
    HAL_GPIO_WritePin(BH1750_SDA_PORT, BH1750_SDA_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(BH1750_SCL_PORT, BH1750_SCL_PIN, GPIO_PIN_SET);

    /* BH1750上电、复位、配置 */
    BH1750_SendCommand(BH1750_POWER_ON);      // 上电
    BH1750_SendCommand(BH1750_RESET);         // 复位
    BH1750_SendCommand(BH1750_MODE_H2);       // 高分辨率模式2
}

/* 产生IIC起始信号 */
void BH1750_IIC_Start(void)
{
    SDA_OUT();
    HAL_GPIO_WritePin(BH1750_SDA_PORT, BH1750_SDA_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(BH1750_SCL_PORT, BH1750_SCL_PIN, GPIO_PIN_SET);
    delay_us(4);
    HAL_GPIO_WritePin(BH1750_SDA_PORT, BH1750_SDA_PIN, GPIO_PIN_RESET);
    delay_us(4);
    HAL_GPIO_WritePin(BH1750_SCL_PORT, BH1750_SCL_PIN, GPIO_PIN_RESET);
}

/* 产生IIC停止信号 */
void BH1750_IIC_Stop(void)
{
    SDA_OUT();
    HAL_GPIO_WritePin(BH1750_SCL_PORT, BH1750_SCL_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(BH1750_SDA_PORT, BH1750_SDA_PIN, GPIO_PIN_RESET);
    delay_us(4);
    HAL_GPIO_WritePin(BH1750_SCL_PORT, BH1750_SCL_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(BH1750_SDA_PORT, BH1750_SDA_PIN, GPIO_PIN_SET);
    delay_us(4);
}

/* 等待应答信号 */
uint8_t BH1750_IIC_WaitAck(void)
{
    uint8_t errTime = 0;

    SDA_IN();
    HAL_GPIO_WritePin(BH1750_SDA_PORT, BH1750_SDA_PIN, GPIO_PIN_SET);
    delay_us(1);
    HAL_GPIO_WritePin(BH1750_SCL_PORT, BH1750_SCL_PIN, GPIO_PIN_SET);
    delay_us(1);

    while (HAL_GPIO_ReadPin(BH1750_SDA_PORT, BH1750_SDA_PIN) == GPIO_PIN_SET)
    {
        errTime++;
        if (errTime > 250)
        {
            BH1750_IIC_Stop();
            return 1;
        }
    }

    HAL_GPIO_WritePin(BH1750_SCL_PORT, BH1750_SCL_PIN, GPIO_PIN_RESET);
    return 0;
}

/* 产生ACK应答 */
void BH1750_IIC_Ack(void)
{
    HAL_GPIO_WritePin(BH1750_SCL_PORT, BH1750_SCL_PIN, GPIO_PIN_RESET);
    SDA_OUT();
    HAL_GPIO_WritePin(BH1750_SDA_PORT, BH1750_SDA_PIN, GPIO_PIN_RESET);
    delay_us(2);
    HAL_GPIO_WritePin(BH1750_SCL_PORT, BH1750_SCL_PIN, GPIO_PIN_SET);
    delay_us(2);
    HAL_GPIO_WritePin(BH1750_SCL_PORT, BH1750_SCL_PIN, GPIO_PIN_RESET);
}

/* 不产生ACK应答 */
void BH1750_IIC_NAck(void)
{
    HAL_GPIO_WritePin(BH1750_SCL_PORT, BH1750_SCL_PIN, GPIO_PIN_RESET);
    SDA_OUT();
    HAL_GPIO_WritePin(BH1750_SDA_PORT, BH1750_SDA_PIN, GPIO_PIN_SET);
    delay_us(2);
    HAL_GPIO_WritePin(BH1750_SCL_PORT, BH1750_SCL_PIN, GPIO_PIN_SET);
    delay_us(2);
    HAL_GPIO_WritePin(BH1750_SCL_PORT, BH1750_SCL_PIN, GPIO_PIN_RESET);
}

/* IIC发送一个字节 */
void BH1750_IIC_SendByte(uint8_t data)
{
    uint8_t t;

    SDA_OUT();
    HAL_GPIO_WritePin(BH1750_SCL_PORT, BH1750_SCL_PIN, GPIO_PIN_RESET);

    for (t = 0; t < 8; t++)
    {
        if ((data & 0x80) >> 7)
            HAL_GPIO_WritePin(BH1750_SDA_PORT, BH1750_SDA_PIN, GPIO_PIN_SET);
        else
            HAL_GPIO_WritePin(BH1750_SDA_PORT, BH1750_SDA_PIN, GPIO_PIN_RESET);

        data <<= 1;
        delay_us(2);
        HAL_GPIO_WritePin(BH1750_SCL_PORT, BH1750_SCL_PIN, GPIO_PIN_SET);
        delay_us(2);
        HAL_GPIO_WritePin(BH1750_SCL_PORT, BH1750_SCL_PIN, GPIO_PIN_RESET);
        delay_us(2);
    }
}

/* IIC读取一个字节 */
uint8_t BH1750_IIC_ReadByte(uint8_t ack)
{
    uint8_t i, receive = 0;

    SDA_IN();
    for (i = 0; i < 8; i++)
    {
        HAL_GPIO_WritePin(BH1750_SCL_PORT, BH1750_SCL_PIN, GPIO_PIN_RESET);
        delay_us(2);
        HAL_GPIO_WritePin(BH1750_SCL_PORT, BH1750_SCL_PIN, GPIO_PIN_SET);
        receive <<= 1;
        if (HAL_GPIO_ReadPin(BH1750_SDA_PORT, BH1750_SDA_PIN) == GPIO_PIN_SET)
            receive++;
        delay_us(1);
    }

    if (!ack)
        BH1750_IIC_NAck();
    else
        BH1750_IIC_Ack();

    return receive;
}

/* 发送命令 */
void BH1750_SendCommand(uint8_t command)
{
    do {
        BH1750_IIC_Start();
        BH1750_IIC_SendByte(BH1750_ADDR_WRITE);
    } while (BH1750_IIC_WaitAck());

    BH1750_IIC_SendByte(command);
    BH1750_IIC_WaitAck();
    BH1750_IIC_Stop();
}

/* 读取数据 */
uint16_t BH1750_ReadData(void)
{
    uint16_t buf;

    BH1750_IIC_Start();
    BH1750_IIC_SendByte(BH1750_ADDR_READ);
    BH1750_IIC_WaitAck();

    buf = BH1750_IIC_ReadByte(1);
    buf = buf << 8;
    buf += (0x00FF & BH1750_IIC_ReadByte(0));

    BH1750_IIC_Stop();

    return buf;
}
