#include "MAX30102.h"

/* MAX30102使用软件I2C */
/* I2C引脚定义 */
#define MAX30102_IIC_SCL_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOB_CLK_ENABLE()
#define MAX30102_IIC_SDA_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOB_CLK_ENABLE()
#define MAX30102_IIC_SCL_PORT               GPIOB
#define MAX30102_IIC_SCL_PIN                GPIO_PIN_4
#define MAX30102_IIC_SDA_PORT               GPIOB
#define MAX30102_IIC_SDA_PIN                GPIO_PIN_5

/* 延时函数 */
static void delay_us(uint32_t us);

/* 软件I2C函数 */
static void MAX30102_IIC_Init(void);
static void MAX30102_IIC_Start(void);
static void MAX30102_IIC_Stop(void);
static void MAX30102_IIC_SendByte(uint8_t data);
static uint8_t MAX30102_IIC_ReadByte(uint8_t ack);
static uint8_t MAX30102_IIC_WaitAck(void);
static void MAX30102_IIC_Ack(void);
static void MAX30102_IIC_NAck(void);
static void MAX30102_SDA_OUT(void);
static void MAX30102_SDA_IN(void);

/* 微秒延时函数 */
static void delay_us(uint32_t us)
{
    uint32_t startTick = DWT->CYCCNT;
    uint32_t delayTicks = us * (SystemCoreClock / 1000000);

    while ((DWT->CYCCNT - startTick) < delayTicks);
}

/* 设置SDA为输出 */
static void MAX30102_SDA_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = MAX30102_IIC_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(MAX30102_IIC_SDA_PORT, &GPIO_InitStruct);
}

/* 设置SDA为输入 */
static void MAX30102_SDA_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = MAX30102_IIC_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(MAX30102_IIC_SDA_PORT, &GPIO_InitStruct);
}

/* 软件I2C初始化 */
static void MAX30102_IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* 使能GPIO时钟 */
    MAX30102_IIC_SCL_GPIO_CLK_ENABLE();
    MAX30102_IIC_SDA_GPIO_CLK_ENABLE();

    /* 配置SCL和SDA */
    GPIO_InitStruct.Pin = MAX30102_IIC_SCL_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(MAX30102_IIC_SCL_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = MAX30102_IIC_SDA_PIN;
    HAL_GPIO_Init(MAX30102_IIC_SDA_PORT, &GPIO_InitStruct);

    /* 初始化为低电平 */
    HAL_GPIO_WritePin(MAX30102_IIC_SCL_PORT, MAX30102_IIC_SCL_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MAX30102_IIC_SDA_PORT, MAX30102_IIC_SDA_PIN, GPIO_PIN_RESET);
}

/* 产生IIC起始信号 */
static void MAX30102_IIC_Start(void)
{
    MAX30102_SDA_OUT();
    HAL_GPIO_WritePin(MAX30102_IIC_SDA_PORT, MAX30102_IIC_SDA_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(MAX30102_IIC_SCL_PORT, MAX30102_IIC_SCL_PIN, GPIO_PIN_SET);
    delay_us(4);
    HAL_GPIO_WritePin(MAX30102_IIC_SDA_PORT, MAX30102_IIC_SDA_PIN, GPIO_PIN_RESET);
    delay_us(4);
    HAL_GPIO_WritePin(MAX30102_IIC_SCL_PORT, MAX30102_IIC_SCL_PIN, GPIO_PIN_RESET);
}

/* 产生IIC停止信号 */
static void MAX30102_IIC_Stop(void)
{
    MAX30102_SDA_OUT();
    HAL_GPIO_WritePin(MAX30102_IIC_SCL_PORT, MAX30102_IIC_SCL_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MAX30102_IIC_SDA_PORT, MAX30102_IIC_SDA_PIN, GPIO_PIN_RESET);
    delay_us(4);
    HAL_GPIO_WritePin(MAX30102_IIC_SCL_PORT, MAX30102_IIC_SCL_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(MAX30102_IIC_SDA_PORT, MAX30102_IIC_SDA_PIN, GPIO_PIN_SET);
    delay_us(4);
}

/* 等待应答信号 */
static uint8_t MAX30102_IIC_WaitAck(void)
{
    uint8_t errTime = 0;

    MAX30102_SDA_IN();
    HAL_GPIO_WritePin(MAX30102_IIC_SDA_PORT, MAX30102_IIC_SDA_PIN, GPIO_PIN_SET);
    delay_us(1);
    HAL_GPIO_WritePin(MAX30102_IIC_SCL_PORT, MAX30102_IIC_SCL_PIN, GPIO_PIN_SET);
    delay_us(1);

    while (HAL_GPIO_ReadPin(MAX30102_IIC_SDA_PORT, MAX30102_IIC_SDA_PIN) == GPIO_PIN_SET)
    {
        errTime++;
        if (errTime > 250)
        {
            MAX30102_IIC_Stop();
            return 1;
        }
    }

    HAL_GPIO_WritePin(MAX30102_IIC_SCL_PORT, MAX30102_IIC_SCL_PIN, GPIO_PIN_RESET);
    return 0;
}

/* 产生ACK应答 */
static void MAX30102_IIC_Ack(void)
{
    HAL_GPIO_WritePin(MAX30102_IIC_SCL_PORT, MAX30102_IIC_SCL_PIN, GPIO_PIN_RESET);
    MAX30102_SDA_OUT();
    HAL_GPIO_WritePin(MAX30102_IIC_SDA_PORT, MAX30102_IIC_SDA_PIN, GPIO_PIN_RESET);
    delay_us(2);
    HAL_GPIO_WritePin(MAX30102_IIC_SCL_PORT, MAX30102_IIC_SCL_PIN, GPIO_PIN_SET);
    delay_us(2);
    HAL_GPIO_WritePin(MAX30102_IIC_SCL_PORT, MAX30102_IIC_SCL_PIN, GPIO_PIN_RESET);
}

/* 不产生ACK应答 */
static void MAX30102_IIC_NAck(void)
{
    HAL_GPIO_WritePin(MAX30102_IIC_SCL_PORT, MAX30102_IIC_SCL_PIN, GPIO_PIN_RESET);
    MAX30102_SDA_OUT();
    HAL_GPIO_WritePin(MAX30102_IIC_SDA_PORT, MAX30102_IIC_SDA_PIN, GPIO_PIN_SET);
    delay_us(2);
    HAL_GPIO_WritePin(MAX30102_IIC_SCL_PORT, MAX30102_IIC_SCL_PIN, GPIO_PIN_SET);
    delay_us(2);
    HAL_GPIO_WritePin(MAX30102_IIC_SCL_PORT, MAX30102_IIC_SCL_PIN, GPIO_PIN_RESET);
}

/* IIC发送一个字节 */
static void MAX30102_IIC_SendByte(uint8_t data)
{
    uint8_t t;

    MAX30102_SDA_OUT();
    HAL_GPIO_WritePin(MAX30102_IIC_SCL_PORT, MAX30102_IIC_SCL_PIN, GPIO_PIN_RESET);

    for (t = 0; t < 8; t++)
    {
        if ((data & 0x80) >> 7)
            HAL_GPIO_WritePin(MAX30102_IIC_SDA_PORT, MAX30102_IIC_SDA_PIN, GPIO_PIN_SET);
        else
            HAL_GPIO_WritePin(MAX30102_IIC_SDA_PORT, MAX30102_IIC_SDA_PIN, GPIO_PIN_RESET);

        data <<= 1;
        delay_us(2);
        HAL_GPIO_WritePin(MAX30102_IIC_SCL_PORT, MAX30102_IIC_SCL_PIN, GPIO_PIN_SET);
        delay_us(2);
        HAL_GPIO_WritePin(MAX30102_IIC_SCL_PORT, MAX30102_IIC_SCL_PIN, GPIO_PIN_RESET);
        delay_us(2);
    }
}

/* IIC读取一个字节 */
static uint8_t MAX30102_IIC_ReadByte(uint8_t ack)
{
    uint8_t i, receive = 0;

    MAX30102_SDA_IN();
    for (i = 0; i < 8; i++)
    {
        HAL_GPIO_WritePin(MAX30102_IIC_SCL_PORT, MAX30102_IIC_SCL_PIN, GPIO_PIN_RESET);
        delay_us(2);
        HAL_GPIO_WritePin(MAX30102_IIC_SCL_PORT, MAX30102_IIC_SCL_PIN, GPIO_PIN_SET);
        receive <<= 1;
        if (HAL_GPIO_ReadPin(MAX30102_IIC_SDA_PORT, MAX30102_IIC_SDA_PIN) == GPIO_PIN_SET)
            receive++;
        delay_us(1);
    }

    if (!ack)
        MAX30102_IIC_NAck();
    else
        MAX30102_IIC_Ack();

    return receive;
}

/* 初始化MAX30102的GPIO */
static void MAX30102_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* 使能GPIO时钟 */
    MAX30102_INT_GPIO_CLK_ENABLE();

    /* 配置INT引脚为输入浮空 */
    GPIO_InitStruct.Pin = MAX30102_INT_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(MAX30102_INT_PORT, &GPIO_InitStruct);
}

/* 写寄存器 */
int8_t MAX30102_WriteReg(uint8_t regAddr, uint8_t data)
{
    uint8_t ack1, ack2, ack3;

    MAX30102_IIC_Start();
    MAX30102_IIC_SendByte(MAX30102_I2C_WRITE_ADDR);
    ack1 = MAX30102_IIC_WaitAck();

    MAX30102_IIC_SendByte(regAddr);
    ack2 = MAX30102_IIC_WaitAck();

    MAX30102_IIC_SendByte(data);
    ack3 = MAX30102_IIC_WaitAck();

    MAX30102_IIC_Stop();

    if (ack1 || ack2 || ack3)
        return -1;
    else
        return 0;
}

/* 读寄存器 */
int8_t MAX30102_ReadReg(uint8_t regAddr, uint8_t *pData)
{
    uint8_t ack1, ack2, ack3;

    MAX30102_IIC_Start();
    MAX30102_IIC_SendByte(MAX30102_I2C_WRITE_ADDR);
    ack1 = MAX30102_IIC_WaitAck();

    MAX30102_IIC_SendByte(regAddr);
    ack2 = MAX30102_IIC_WaitAck();

    MAX30102_IIC_Start();
    MAX30102_IIC_SendByte(MAX30102_I2C_READ_ADDR);
    ack3 = MAX30102_IIC_WaitAck();

    *pData = MAX30102_IIC_ReadByte(0);
    MAX30102_IIC_Stop();

    if (ack1 || ack2 || ack3)
        return -1;
    else
        return 0;
}

/* 初始化MAX30102 */
void MAX30102_Init(void)
{
    /* 使能DWT用于微秒延时 */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    DWT->CYCCNT = 0;

    /* 初始化GPIO */
    MAX30102_GPIO_Init();

    /* 初始化I2C */
    MAX30102_IIC_Init();

    /* 配置MAX30102寄存器 */
    MAX30102_WriteReg(MAX30102_REG_INTR_ENABLE_1, 0xC0);  // 中断使能
    MAX30102_WriteReg(MAX30102_REG_INTR_ENABLE_2, 0x00);
    MAX30102_WriteReg(MAX30102_REG_FIFO_WR_PTR, 0x00);
    MAX30102_WriteReg(MAX30102_REG_OVF_COUNTER, 0x00);
    MAX30102_WriteReg(MAX30102_REG_FIFO_RD_PTR, 0x00);
    MAX30102_WriteReg(MAX30102_REG_FIFO_CONFIG, 0x4F);    // 采样平均=4, FIFO滚动=使能
    MAX30102_WriteReg(MAX30102_REG_MODE_CONFIG, 0x03);    // SpO2模式
    MAX30102_WriteReg(MAX30102_REG_SPO2_CONFIG, 0x27);    // ADC范围=4096nA, 采样率100Hz, 脉宽400us
    MAX30102_WriteReg(MAX30102_REG_LED1_PA, 0x3F);        // LED1电流~6.2mA（适中亮度）
    MAX30102_WriteReg(MAX30102_REG_LED2_PA, 0x3F);        // LED2电流~6.2mA（适中亮度）
    MAX30102_WriteReg(MAX30102_REG_PILOT_PA, 0x7F);       // Pilot LED电流~25mA
}

/* 复位MAX30102 */
int8_t MAX30102_Reset(void)
{
    if (MAX30102_WriteReg(MAX30102_REG_MODE_CONFIG, 0x40))
        return -1;
    else
        return 0;
}

/* 读取FIFO */
int8_t MAX30102_ReadFIFO(uint32_t *pRedLED, uint32_t *pIRLED)
{
    uint32_t un_temp;
    uint8_t uch_temp;
    uint8_t ack1, ack2, ack3;

    *pRedLED = 0;
    *pIRLED = 0;

    /* 读取并清除状态寄存器 */
    MAX30102_ReadReg(MAX30102_REG_INTR_STATUS_1, &uch_temp);
    MAX30102_ReadReg(MAX30102_REG_INTR_STATUS_2, &uch_temp);

    /* 读取FIFO数据 */
    MAX30102_IIC_Start();
    MAX30102_IIC_SendByte(MAX30102_I2C_WRITE_ADDR);
    ack1 = MAX30102_IIC_WaitAck();

    MAX30102_IIC_SendByte(MAX30102_REG_FIFO_DATA);
    ack2 = MAX30102_IIC_WaitAck();

    MAX30102_IIC_Start();
    MAX30102_IIC_SendByte(MAX30102_I2C_READ_ADDR);
    ack3 = MAX30102_IIC_WaitAck();

    /* 读取红光LED数据 */
    un_temp = MAX30102_IIC_ReadByte(1);
    un_temp <<= 16;
    *pRedLED += un_temp;

    un_temp = MAX30102_IIC_ReadByte(1);
    un_temp <<= 8;
    *pRedLED += un_temp;

    un_temp = MAX30102_IIC_ReadByte(1);
    *pRedLED += un_temp;

    /* 读取红外LED数据 */
    un_temp = MAX30102_IIC_ReadByte(1);
    un_temp <<= 16;
    *pIRLED += un_temp;

    un_temp = MAX30102_IIC_ReadByte(1);
    un_temp <<= 8;
    *pIRLED += un_temp;

    un_temp = MAX30102_IIC_ReadByte(0);
    *pIRLED += un_temp;

    MAX30102_IIC_Stop();

    *pRedLED &= 0x03FFFF;  // 掩码MSB [23:18]
    *pIRLED &= 0x03FFFF;   // 掩码MSB [23:18]

    if (ack1 || ack2 || ack3)
        return -1;
    else
        return 0;
}
