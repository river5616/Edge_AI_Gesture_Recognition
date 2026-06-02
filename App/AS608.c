#include "AS608.h"
#include <string.h>

uint32_t AS608Addr = 0XFFFFFFFF; // 默认地址

// UART接收缓冲区
#define AS608_RX_BUF_SIZE 128
static uint8_t AS608_RxBuffer[AS608_RX_BUF_SIZE];
volatile uint16_t AS608_RxLen = 0;  // 移除static，允许外部访问

// 串口发送一个字节
static void Com_SendData(uint8_t data)
{
    HAL_UART_Transmit(&huart6, &data, 1, 100);
}

// 发送包头
static void SendHead(void)
{
    Com_SendData(0xEF);
    Com_SendData(0x01);
}

// 发送地址
static void SendAddr(void)
{
    Com_SendData(AS608Addr >> 24);
    Com_SendData(AS608Addr >> 16);
    Com_SendData(AS608Addr >> 8);
    Com_SendData(AS608Addr);
}

// 发送包标识
static void SendFlag(uint8_t flag)
{
    Com_SendData(flag);
}

// 发送包长度
static void SendLength(int length)
{
    Com_SendData(length >> 8);
    Com_SendData(length);
}

// 发送指令码
static void Sendcmd(uint8_t cmd)
{
    Com_SendData(cmd);
}

// 发送校验和
static void SendCheck(uint16_t check)
{
    Com_SendData(check >> 8);
    Com_SendData(check);
}

// 判断中断接收到的数据是不是应答包
// waittime为等待中断接收数据的时间（单位1ms）
// 返回值：数据包的地址
static uint8_t *JudgeStr(uint16_t waittime)
{
    char *data;
    uint8_t str[8];
    str[0] = 0xef;
    str[1] = 0x01;
    str[2] = AS608Addr >> 24;
    str[3] = AS608Addr >> 16;
    str[4] = AS608Addr >> 8;
    str[5] = AS608Addr;
    str[6] = 0x07;
    str[7] = '\0';

    // 清空接收缓冲区
    AS608_RxLen = 0;
    memset(AS608_RxBuffer, 0, AS608_RX_BUF_SIZE);

    // 使用轮询方式接收（更可靠）
    uint8_t rxByte;
    uint16_t rxIndex = 0;
    uint32_t startTime = HAL_GetTick();

    while ((HAL_GetTick() - startTime) < waittime)
    {
        // 单字节接收，超时20ms（增加超时时间）
        if (HAL_UART_Receive(&huart6, &rxByte, 1, 20) == HAL_OK)
        {
            if (rxIndex < AS608_RX_BUF_SIZE)
            {
                AS608_RxBuffer[rxIndex++] = rxByte;
                AS608_RxLen = rxIndex;

                // 检查是否接收到完整的包头（至少12字节）
                if (rxIndex >= 12 && AS608_RxBuffer[0] == 0xEF && AS608_RxBuffer[1] == 0x01)
                {
                    // 等待一小段时间确保包接收完整
                    HAL_Delay(30);

                    data = strstr((const char *)AS608_RxBuffer, (const char *)str);
                    if (data)
                        return (uint8_t *)data;
                }
            }
        }
    }
    return 0;
}

// UART6接收完成回调（需要在stm32h7xx_it.c中调用）
void AS608_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART6)
    {
        AS608_RxLen = AS608_RX_BUF_SIZE - huart->RxXferCount;
    }
}

// 录入图像 GZ_GetImage
// 功能:探测手指，探测到后录入指纹图像存于ImageBuffer。
// 模块返回确认字
uint8_t GZ_GetImage(void)
{
    uint16_t temp;
    uint8_t ensure;
    uint8_t *data;

    SendHead();
    SendAddr();
    SendFlag(0x01); // 命令包标识
    SendLength(0x03);
    Sendcmd(0x01);
    temp = 0x01 + 0x03 + 0x01;
    SendCheck(temp);

    data = JudgeStr(2000);
    if (data)
        ensure = data[9];
    else
        ensure = 0xff;
    return ensure;
}

// 生成特征 GZ_GenChar
// 功能:将ImageBuffer中的原始图像生成指纹特征文件存于CharBuffer1或CharBuffer2
// 参数:BufferID --> charBuffer1:0x01	charBuffer2:0x02
// 模块返回确认字
uint8_t GZ_GenChar(uint8_t BufferID)
{
    uint16_t temp;
    uint8_t ensure;
    uint8_t *data;

    SendHead();
    SendAddr();
    SendFlag(0x01); // 命令包标识
    SendLength(0x04);
    Sendcmd(0x02);
    Com_SendData(BufferID);
    temp = 0x01 + 0x04 + 0x02 + BufferID;
    SendCheck(temp);

    data = JudgeStr(2000);
    if (data)
        ensure = data[9];
    else
        ensure = 0xff;
    return ensure;
}

// 精确比对两枚指纹特征 GZ_Match
// 功能:精确比对CharBuffer1 与CharBuffer2 中的特征文件
// 模块返回确认字
uint8_t GZ_Match(void)
{
    uint16_t temp;
    uint8_t ensure;
    uint8_t *data;

    SendHead();
    SendAddr();
    SendFlag(0x01); // 命令包标识
    SendLength(0x03);
    Sendcmd(0x03);
    temp = 0x01 + 0x03 + 0x03;
    SendCheck(temp);

    data = JudgeStr(2000);
    if (data)
        ensure = data[9];
    else
        ensure = 0xff;
    return ensure;
}

// 搜索指纹 GZ_Search
// 功能:以CharBuffer1或CharBuffer2中的特征文件搜索整个或部分指纹库。若搜索到，则返回页码。
// 参数:  BufferID @ref CharBuffer1	CharBuffer2
// 说明:  模块返回确认字，页码（相配指纹模板）
uint8_t GZ_Search(uint8_t BufferID, uint16_t StartPage, uint16_t PageNum, SearchResult *p)
{
    uint16_t temp;
    uint8_t ensure;
    uint8_t *data;

    SendHead();
    SendAddr();
    SendFlag(0x01); // 命令包标识
    SendLength(0x08);
    Sendcmd(0x04);
    Com_SendData(BufferID);
    Com_SendData(StartPage >> 8);
    Com_SendData(StartPage);
    Com_SendData(PageNum >> 8);
    Com_SendData(PageNum);
    temp = 0x01 + 0x08 + 0x04 + BufferID + (StartPage >> 8) + (uint8_t)StartPage + (PageNum >> 8) + (uint8_t)PageNum;
    SendCheck(temp);

    data = JudgeStr(2000);
    if (data)
    {
        ensure = data[9];
        p->pageID = (data[10] << 8) + data[11];
        p->mathscore = (data[12] << 8) + data[13];
    }
    else
        ensure = 0xff;
    return ensure;
}

// 合并特征（生成模板）GZ_RegModel
// 功能:将CharBuffer1与CharBuffer2中的特征文件合并生成 模板,结果存于CharBuffer1与CharBuffer2
// 说明:  模块返回确认字
uint8_t GZ_RegModel(void)
{
    uint16_t temp;
    uint8_t ensure;
    uint8_t *data;

    SendHead();
    SendAddr();
    SendFlag(0x01); // 命令包标识
    SendLength(0x03);
    Sendcmd(0x05);
    temp = 0x01 + 0x03 + 0x05;
    SendCheck(temp);

    data = JudgeStr(2000);
    if (data)
        ensure = data[9];
    else
        ensure = 0xff;
    return ensure;
}

// 储存模板 GZ_StoreChar
// 功能:将 CharBuffer1 或 CharBuffer2 中的模板文件存到 PageID 号flash数据库位置。
// 参数:  BufferID @ref charBuffer1:0x01	charBuffer2:0x02
//       PageID：指纹库位置号
// 说明:  模块返回确认字
uint8_t GZ_StoreChar(uint8_t BufferID, uint16_t PageID)
{
    uint16_t temp;
    uint8_t ensure;
    uint8_t *data;

    SendHead();
    SendAddr();
    SendFlag(0x01); // 命令包标识
    SendLength(0x06);
    Sendcmd(0x06);
    Com_SendData(BufferID);
    Com_SendData(PageID >> 8);
    Com_SendData(PageID);
    temp = 0x01 + 0x06 + 0x06 + BufferID + (PageID >> 8) + (uint8_t)PageID;
    SendCheck(temp);

    data = JudgeStr(2000);
    if (data)
        ensure = data[9];
    else
        ensure = 0xff;
    return ensure;
}

// 删除模板 GZ_DeletChar
// 功能:  删除flash数据库中指定ID号开始的N个指纹模板
// 参数:  PageID(指纹库模板号)，N删除的模板个数。
// 说明:  模块返回确认字
uint8_t GZ_DeletChar(uint16_t PageID, uint16_t N)
{
    uint16_t temp;
    uint8_t ensure;
    uint8_t *data;

    SendHead();
    SendAddr();
    SendFlag(0x01); // 命令包标识
    SendLength(0x07);
    Sendcmd(0x0C);
    Com_SendData(PageID >> 8);
    Com_SendData(PageID);
    Com_SendData(N >> 8);
    Com_SendData(N);
    temp = 0x01 + 0x07 + 0x0C + (PageID >> 8) + (uint8_t)PageID + (N >> 8) + (uint8_t)N;
    SendCheck(temp);

    data = JudgeStr(2000);
    if (data)
        ensure = data[9];
    else
        ensure = 0xff;
    return ensure;
}

// 清空指纹库 GZ_Empty
// 功能:  删除flash数据库中所有指纹模板
// 参数:  无
// 说明:  模块返回确认字
uint8_t GZ_Empty(void)
{
    uint16_t temp;
    uint8_t ensure;
    uint8_t *data;

    SendHead();
    SendAddr();
    SendFlag(0x01); // 命令包标识
    SendLength(0x03);
    Sendcmd(0x0D);
    temp = 0x01 + 0x03 + 0x0D;
    SendCheck(temp);

    data = JudgeStr(2000);
    if (data)
        ensure = data[9];
    else
        ensure = 0xff;
    return ensure;
}

// 写系统寄存器 GZ_WriteReg
// 功能:  写模块寄存器
// 参数:  寄存器序号RegNum:4\5\6
// 说明:  模块返回确认字
uint8_t GZ_WriteReg(uint8_t RegNum, uint8_t DATA)
{
    uint16_t temp;
    uint8_t ensure;
    uint8_t *data;

    SendHead();
    SendAddr();
    SendFlag(0x01); // 命令包标识
    SendLength(0x05);
    Sendcmd(0x0E);
    Com_SendData(RegNum);
    Com_SendData(DATA);
    temp = RegNum + DATA + 0x01 + 0x05 + 0x0E;
    SendCheck(temp);

    data = JudgeStr(2000);
    if (data)
        ensure = data[9];
    else
        ensure = 0xff;
    return ensure;
}

// 读系统基本参数 GZ_ReadSysPara
// 功能:  读取模块的基本参数（波特率，包大小等)
// 参数:  无
// 说明:  模块返回确认字 + 基本参数（16bytes）
uint8_t GZ_ReadSysPara(SysPara *p)
{
    uint16_t temp;
    uint8_t ensure;
    uint8_t *data;

    SendHead();
    SendAddr();
    SendFlag(0x01); // 命令包标识
    SendLength(0x03);
    Sendcmd(0x0F);
    temp = 0x01 + 0x03 + 0x0F;
    SendCheck(temp);

    data = JudgeStr(1000);
    if (data)
    {
        ensure = data[9];
        p->GZ_max = (data[14] << 8) + data[15];
        p->GZ_level = data[17];
        p->GZ_addr = (data[18] << 24) + (data[19] << 16) + (data[20] << 8) + data[21];
        p->GZ_size = data[23];
        p->GZ_N = data[25];
    }
    else
        ensure = 0xff;
    return ensure;
}

// 设置模块地址 GZ_SetAddr
// 功能:  设置模块地址
// 参数:  GZ_addr
// 说明:  模块返回确认字
uint8_t GZ_SetAddr(uint32_t GZ_addr)
{
    uint16_t temp;
    uint8_t ensure;
    uint8_t *data;

    SendHead();
    SendAddr();
    SendFlag(0x01); // 命令包标识
    SendLength(0x07);
    Sendcmd(0x15);
    Com_SendData(GZ_addr >> 24);
    Com_SendData(GZ_addr >> 16);
    Com_SendData(GZ_addr >> 8);
    Com_SendData(GZ_addr);
    temp = 0x01 + 0x07 + 0x15 + (uint8_t)(GZ_addr >> 24) + (uint8_t)(GZ_addr >> 16) + (uint8_t)(GZ_addr >> 8) + (uint8_t)GZ_addr;
    SendCheck(temp);
    AS608Addr = GZ_addr; // 更新本指纹模块地址

    data = JudgeStr(2000);
    if (data)
        ensure = data[9];
    else
        ensure = 0xff;

    AS608Addr = GZ_addr;
    return ensure;
}

// 功能：模块内部为用户开辟了256bytes的FLASH空间用于存用户记事本,
//	该记事本逻辑上被分成 16 个页。
// 参数:  NotePageNum(0~15),Byte32(要写入的数据：32个字节)
// 说明:  模块返回确认字
uint8_t GZ_WriteNotepad(uint8_t NotePageNum, uint8_t *Byte32)
{
    uint16_t temp = 0;
    uint8_t ensure, i;
    uint8_t *data;

    SendHead();
    SendAddr();
    SendFlag(0x01); // 命令包标识
    SendLength(36);
    Sendcmd(0x18);
    Com_SendData(NotePageNum);
    for (i = 0; i < 32; i++)
    {
        Com_SendData(Byte32[i]);
        temp += Byte32[i];
    }
    temp = 0x01 + 36 + 0x18 + NotePageNum + temp;
    SendCheck(temp);

    data = JudgeStr(2000);
    if (data)
        ensure = data[9];
    else
        ensure = 0xff;
    return ensure;
}

// 读记事GZ_ReadNotepad
// 功能：  读取FLASH用户区的128bytes数据
// 参数:  NotePageNum(0~15)
// 说明:  模块返回确认字+用户信息
uint8_t GZ_ReadNotepad(uint8_t NotePageNum, uint8_t *Byte32)
{
    uint16_t temp;
    uint8_t ensure, i;
    uint8_t *data;

    SendHead();
    SendAddr();
    SendFlag(0x01); // 命令包标识
    SendLength(0x04);
    Sendcmd(0x19);
    Com_SendData(NotePageNum);
    temp = 0x01 + 0x04 + 0x19 + NotePageNum;
    SendCheck(temp);

    data = JudgeStr(2000);
    if (data)
    {
        ensure = data[9];
        for (i = 0; i < 32; i++)
        {
            Byte32[i] = data[10 + i];
        }
    }
    else
        ensure = 0xff;
    return ensure;
}

// 高速搜索GZ_HighSpeedSearch
// 功能：以 CharBuffer1或CharBuffer2中的特征文件高速搜索整个或部分指纹库。
//		  若搜索到，则返回页码,该指纹所在的页码、指纹库中此 枚指纹的录入时候可以
//		  更好的指纹，（保存指纹模板时候）
// 参数:  BufferID， StartPage(起始页)，PageNum（页数）
// 说明:  模块返回确认字+页码（相配指纹模板）
uint8_t GZ_HighSpeedSearch(uint8_t BufferID, uint16_t StartPage, uint16_t PageNum, SearchResult *p)
{
    uint16_t temp;
    uint8_t ensure;
    uint8_t *data;

    SendHead();
    SendAddr();
    SendFlag(0x01); // 命令包标识
    SendLength(0x08);
    Sendcmd(0x1b);
    Com_SendData(BufferID);
    Com_SendData(StartPage >> 8);
    Com_SendData(StartPage);
    Com_SendData(PageNum >> 8);
    Com_SendData(PageNum);
    temp = 0x01 + 0x08 + 0x1b + BufferID + (StartPage >> 8) + (uint8_t)StartPage + (PageNum >> 8) + (uint8_t)PageNum;
    SendCheck(temp);

    data = JudgeStr(2000);
    if (data)
    {
        ensure = data[9];
        p->pageID = (data[10] << 8) + data[11];
        p->mathscore = (data[12] << 8) + data[13];
    }
    else
        ensure = 0xff;
    return ensure;
}

// 读有效模板个数 GZ_ValidTempleteNum
// 功能：读有效模板个数
// 参数: 无
// 说明: 模块返回确认字+有效模板个数ValidN
uint8_t GZ_ValidTempleteNum(uint16_t *ValidN)
{
    uint16_t temp;
    uint8_t ensure;
    uint8_t *data;

    SendHead();
    SendAddr();
    SendFlag(0x01); // 命令包标识
    SendLength(0x03);
    Sendcmd(0x1d);
    temp = 0x01 + 0x03 + 0x1d;
    SendCheck(temp);

    data = JudgeStr(2000);
    if (data)
    {
        ensure = data[9];
        *ValidN = (data[10] << 8) + data[11];
    }
    else
        ensure = 0xff;

    return ensure;
}

// 与AS608握手 GZ_HandShake
// 参数: GZ_Addr地址指针
// 说明: 模块返下地址（正确地址）
uint8_t GZ_HandShake(uint32_t *GZ_Addr)
{
    // 清空接收缓冲区
    AS608_RxLen = 0;
    memset(AS608_RxBuffer, 0, AS608_RX_BUF_SIZE);

    // 发送握手命令
    SendHead();
    SendAddr();
    Com_SendData(0X01);
    Com_SendData(0X00);
    Com_SendData(0X00);

    // 使用轮询方式接收握手响应
    uint8_t rxByte;
    uint16_t rxIndex = 0;
    uint32_t startTime = HAL_GetTick();

    while ((HAL_GetTick() - startTime) < 1000)  // 1秒超时
    {
        // 单字节接收，超时20ms（增加超时时间）
        if (HAL_UART_Receive(&huart6, &rxByte, 1, 20) == HAL_OK)
        {
            if (rxIndex < AS608_RX_BUF_SIZE)
            {
                AS608_RxBuffer[rxIndex++] = rxByte;

                // 检查是否接收到至少12字节（接受12或13字节）
                if (rxIndex >= 12)
                {
                    if (AS608_RxBuffer[0] == 0XEF && AS608_RxBuffer[1] == 0X01 && AS608_RxBuffer[6] == 0X07)
                    {
                        *GZ_Addr = (AS608_RxBuffer[2] << 24) + (AS608_RxBuffer[3] << 16) +
                                   (AS608_RxBuffer[4] << 8) + (AS608_RxBuffer[5]);

                        // 等待一小段时间，确保所有数据都收到
                        HAL_Delay(50);
                        AS608_RxLen = rxIndex;  // 保存接收长度
                        return 0;  // 握手成功
                    }
                }
            }
        }
    }
    AS608_RxLen = rxIndex;  // 保存接收长度（即使失败）
    return 1;  // 握手失败
}

// 模块应答确认码错误消息函数
// 功能：解析确认码错误消息
// 参数: ensure
const char *EnsureMessage(uint8_t ensure)
{
    const char *p;
    switch (ensure)
    {
    case 0x00:
        p = "OK";
        break;
    case 0x01:
        p = "数据包接收错误";
        break;
    case 0x02:
        p = "传感器上没有手指";
        break;
    case 0x03:
        p = "录入指纹图像失败";
        break;
    case 0x04:
        p = "指纹图像太干、太淡而生不成特征";
        break;
    case 0x05:
        p = "指纹图像太湿、太糊而生不成特征";
        break;
    case 0x06:
        p = "指纹图像太乱而生不成特征";
        break;
    case 0x07:
        p = "指纹图像正常，但特征点太少（或面积太小）而生不成特征";
        break;
    case 0x08:
        p = "指纹不匹配";
        break;
    case 0x09:
        p = "没搜索到指纹";
        break;
    case 0x0a:
        p = "特征合并失败";
        break;
    case 0x0b:
        p = "访问指纹库时地址序号超出指纹库范围";
        break;
    case 0x10:
        p = "删除模板失败";
        break;
    case 0x11:
        p = "清空指纹库失败";
        break;
    case 0x15:
        p = "缓冲区内没有有效原始图而生不成图像";
        break;
    case 0x18:
        p = "读写 FLASH 出错";
        break;
    case 0x19:
        p = "未定义错误";
        break;
    case 0x1a:
        p = "无效寄存器号";
        break;
    case 0x1b:
        p = "寄存器设定内容错误";
        break;
    case 0x1c:
        p = "记事本页码指定错误";
        break;
    case 0x1f:
        p = "指纹库满";
        break;
    case 0x20:
        p = "地址错误";
        break;
    default:
        p = "模块返回确认码错误";
        break;
    }
    return p;
}
