#include "ov2640.h"
#include "ov2640cfg.h"
#include "sccb.h"
#include <string.h>

// 延时函数
static void delay_ms(uint32_t ms)
{
    HAL_Delay(ms);
}

// 初始化OV2640摄像头
uint8_t OV2640_Init(void)
{
    uint16_t i;
    uint16_t reg;

    // ⚠️ GPIO已在gpio.c（CubeMX）中配置，不需要重复配置
    // 只需要控制上电和复位时序

    // 复位序列 - 参考F103例程时序
    OV2640_PWDN_LOW();      // Power ON
    delay_ms(10);           // 等待上电稳定
    OV2640_RST_LOW();       // 复位
    delay_ms(10);           // 保持复位状态
    OV2640_RST_HIGH();      // 释放复位
    delay_ms(10);           // 等待摄像头就绪

    // 初始化SCCB
    SCCB_Init();

    // 软件复位OV2640
    SCCB_WR_Reg(OV2640_DSP_RA_DLMT, 0x01);  // 选择sensor寄存器
    SCCB_WR_Reg(OV2640_SENSOR_COM7, 0x80);  // 软件复位
    delay_ms(100);  // 增加延时，等待软件复位完成

    // 读取制造商ID
    reg = SCCB_RD_Reg(OV2640_SENSOR_MIDH);
    reg <<= 8;
    reg |= SCCB_RD_Reg(OV2640_SENSOR_MIDL);

    if(reg != OV2640_MID)
    {
        return 1;  // ID不匹配
    }

    // 读取产品ID
    reg = SCCB_RD_Reg(OV2640_SENSOR_PIDH);
    reg <<= 8;
    reg |= SCCB_RD_Reg(OV2640_SENSOR_PIDL);

    if(reg != OV2640_PID)
    {
        return 2;  // ID不匹配
    }

    // 初始化OV2640，使用UXGA分辨率
    for(i = 0; i < OV2640_UXGA_INIT_REG_TBL_SIZE; i++)
    {
        SCCB_WR_Reg(ov2640_uxga_init_reg_tbl[i][0],
                    ov2640_uxga_init_reg_tbl[i][1]);
    }

    return 0;  // 成功
}

// 切换到RGB565模式
void OV2640_RGB565_Mode(void)
{
    uint16_t i;

    for(i = 0; i < OV2640_RGB565_REG_TBL_SIZE; i++)
    {
        SCCB_WR_Reg(ov2640_rgb565_reg_tbl[i][0],
                    ov2640_rgb565_reg_tbl[i][1]);
    }
}

// 切换到JPEG模式
void OV2640_JPEG_Mode(void)
{
    uint16_t i;

    // 先设置YUV422
    for(i = 0; i < OV2640_YUV422_REG_TBL_SIZE; i++)
    {
        SCCB_WR_Reg(ov2640_yuv422_reg_tbl[i][0],
                    ov2640_yuv422_reg_tbl[i][1]);
    }

    // 再设置JPEG
    for(i = 0; i < OV2640_JPEG_REG_TBL_SIZE; i++)
    {
        SCCB_WR_Reg(ov2640_jpeg_reg_tbl[i][0],
                    ov2640_jpeg_reg_tbl[i][1]);
    }
}

// 设置输出图像大小
uint8_t OV2640_OutSize_Set(uint16_t width, uint16_t height)
{
    uint16_t outh;
    uint16_t outw;
    uint8_t temp;

    if(width % 4) return 1;
    if(height % 4) return 2;

    outw = width / 4;
    outh = height / 4;

    SCCB_WR_Reg(0xFF, 0x00);
    SCCB_WR_Reg(0xE0, 0x04);
    SCCB_WR_Reg(0x5A, outw & 0xFF);        // OUTW低8位
    SCCB_WR_Reg(0x5B, outh & 0xFF);        // OUTH低8位
    temp = (outw >> 8) & 0x03;
    temp |= (outh >> 6) & 0x04;
    SCCB_WR_Reg(0x5C, temp);               // OUTH/OUTW高位
    SCCB_WR_Reg(0xE0, 0x00);

    return 0;
}

// 设置图像窗口大小
uint8_t OV2640_ImageWin_Set(uint16_t offx, uint16_t offy, uint16_t width, uint16_t height)
{
    uint16_t hsize;
    uint16_t vsize;
    uint8_t temp;

    if(width % 4) return 1;
    if(height % 4) return 2;

    hsize = width / 4;
    vsize = height / 4;

    SCCB_WR_Reg(0xFF, 0x00);
    SCCB_WR_Reg(0xE0, 0x04);
    SCCB_WR_Reg(0x51, hsize & 0xFF);       // H_SIZE低8位
    SCCB_WR_Reg(0x52, vsize & 0xFF);       // V_SIZE低8位
    SCCB_WR_Reg(0x53, offx & 0xFF);        // offx低8位
    SCCB_WR_Reg(0x54, offy & 0xFF);        // offy低8位
    temp = (vsize >> 1) & 0x80;
    temp |= (offy >> 4) & 0x70;
    temp |= (hsize >> 5) & 0x08;
    temp |= (offx >> 8) & 0x07;
    SCCB_WR_Reg(0x55, temp);               // 高位
    SCCB_WR_Reg(0x57, (hsize >> 2) & 0x80);
    SCCB_WR_Reg(0xE0, 0x00);

    return 0;
}

// 设置图像尺寸
uint8_t OV2640_ImageSize_Set(uint16_t width, uint16_t height)
{
    uint8_t temp;

    SCCB_WR_Reg(0xFF, 0x00);
    SCCB_WR_Reg(0xE0, 0x04);
    SCCB_WR_Reg(0xC0, (width) >> 3 & 0xFF);     // HSIZE[10:3]
    SCCB_WR_Reg(0xC1, (height) >> 3 & 0xFF);    // VSIZE[10:3]
    temp = (width & 0x07) << 3;
    temp |= height & 0x07;
    temp |= (width >> 4) & 0x80;
    SCCB_WR_Reg(0x8C, temp);
    SCCB_WR_Reg(0xE0, 0x00);

    return 0;
}

// 自动曝光等级设置
void OV2640_Auto_Exposure(uint8_t level)
{
    const uint8_t exposure_tbl[5][8] = {
        {0xFF, 0x01, 0x24, 0x20, 0x25, 0x18, 0x26, 0x60},
        {0xFF, 0x01, 0x24, 0x34, 0x25, 0x1c, 0x26, 0x00},
        {0xFF, 0x01, 0x24, 0x3e, 0x25, 0x38, 0x26, 0x81},
        {0xFF, 0x01, 0x24, 0x48, 0x25, 0x40, 0x26, 0x81},
        {0xFF, 0x01, 0x24, 0x58, 0x25, 0x50, 0x26, 0x92},
    };

    uint8_t i;
    const uint8_t *p;

    if(level > 4) level = 4;
    p = exposure_tbl[level];

    for(i = 0; i < 4; i++)
    {
        SCCB_WR_Reg(p[i * 2], p[i * 2 + 1]);
    }
}

// 白平衡设置
void OV2640_Light_Mode(uint8_t mode)
{
    uint8_t regccval = 0x5E;  // Sunny
    uint8_t regcdval = 0x41;
    uint8_t regceval = 0x54;

    switch(mode)
    {
        case 0:  // auto
            SCCB_WR_Reg(0xFF, 0x00);
            SCCB_WR_Reg(0xC7, 0x00);  // AWB ON
            return;
        case 2:  // cloudy
            regccval = 0x65;
            regcdval = 0x41;
            regceval = 0x4F;
            break;
        case 3:  // office
            regccval = 0x52;
            regcdval = 0x41;
            regceval = 0x66;
            break;
        case 4:  // home
            regccval = 0x42;
            regcdval = 0x3F;
            regceval = 0x71;
            break;
    }

    SCCB_WR_Reg(0xFF, 0x00);
    SCCB_WR_Reg(0xC7, 0x40);  // AWB OFF
    SCCB_WR_Reg(0xCC, regccval);
    SCCB_WR_Reg(0xCD, regcdval);
    SCCB_WR_Reg(0xCE, regceval);
}

// 色彩饱和度
void OV2640_Color_Saturation(uint8_t sat)
{
    uint8_t reg7dval = ((sat + 2) << 4) | 0x08;

    SCCB_WR_Reg(0xFF, 0x00);
    SCCB_WR_Reg(0x7C, 0x00);
    SCCB_WR_Reg(0x7D, 0x02);
    SCCB_WR_Reg(0x7C, 0x03);
    SCCB_WR_Reg(0x7D, reg7dval);
    SCCB_WR_Reg(0x7D, reg7dval);
}

// 亮度设置
void OV2640_Brightness(uint8_t bright)
{
    SCCB_WR_Reg(0xFF, 0x00);
    SCCB_WR_Reg(0x7C, 0x00);
    SCCB_WR_Reg(0x7D, 0x04);
    SCCB_WR_Reg(0x7C, 0x09);
    SCCB_WR_Reg(0x7D, bright << 4);
    SCCB_WR_Reg(0x7D, 0x00);
}

// 对比度设置
void OV2640_Contrast(uint8_t contrast)
{
    uint8_t reg7d0val = 0x20;
    uint8_t reg7d1val = 0x20;

    switch(contrast)
    {
        case 0:  // -2
            reg7d0val = 0x18;
            reg7d1val = 0x34;
            break;
        case 1:  // -1
            reg7d0val = 0x1C;
            reg7d1val = 0x2A;
            break;
        case 3:  // +1
            reg7d0val = 0x24;
            reg7d1val = 0x16;
            break;
        case 4:  // +2
            reg7d0val = 0x28;
            reg7d1val = 0x0C;
            break;
    }

    SCCB_WR_Reg(0xFF, 0x00);
    SCCB_WR_Reg(0x7C, 0x00);
    SCCB_WR_Reg(0x7D, 0x04);
    SCCB_WR_Reg(0x7C, 0x07);
    SCCB_WR_Reg(0x7D, 0x20);
    SCCB_WR_Reg(0x7D, reg7d0val);
    SCCB_WR_Reg(0x7D, reg7d1val);
    SCCB_WR_Reg(0x7D, 0x06);
}

// 特殊效果
void OV2640_Special_Effects(uint8_t eft)
{
    uint8_t reg7d0val = 0x00;
    uint8_t reg7d1val = 0x80;
    uint8_t reg7d2val = 0x80;

    switch(eft)
    {
        case 1:  // 负片
            reg7d0val = 0x40;
            break;
        case 2:  // 黑白
            reg7d0val = 0x18;
            break;
        case 3:  // 偏红色
            reg7d0val = 0x18;
            reg7d1val = 0x40;
            reg7d2val = 0xC0;
            break;
        case 4:  // 偏绿色
            reg7d0val = 0x18;
            reg7d1val = 0x40;
            reg7d2val = 0x40;
            break;
        case 5:  // 偏蓝色
            reg7d0val = 0x18;
            reg7d1val = 0xA0;
            reg7d2val = 0x40;
            break;
        case 6:  // 复古
            reg7d0val = 0x18;
            reg7d1val = 0x40;
            reg7d2val = 0xA6;
            break;
    }

    SCCB_WR_Reg(0xFF, 0x00);
    SCCB_WR_Reg(0x7C, 0x00);
    SCCB_WR_Reg(0x7D, reg7d0val);
    SCCB_WR_Reg(0x7C, 0x05);
    SCCB_WR_Reg(0x7D, reg7d1val);
    SCCB_WR_Reg(0x7D, reg7d2val);
}
