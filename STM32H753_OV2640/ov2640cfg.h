#ifndef __OV2640CFG_H
#define __OV2640CFG_H

#include <stdint.h>

// OV2640 UXGA初始化寄存器表 (1600*1200)
extern const uint8_t ov2640_uxga_init_reg_tbl[][2];

// OV2640 SVGA初始化寄存器表 (800*600)
extern const uint8_t ov2640_svga_init_reg_tbl[][2];

// YUV422格式寄存器配置
extern const uint8_t ov2640_yuv422_reg_tbl[][2];

// JPEG格式寄存器配置
extern const uint8_t ov2640_jpeg_reg_tbl[][2];

// RGB565格式寄存器配置
extern const uint8_t ov2640_rgb565_reg_tbl[][2];

// 寄存器表大小
#define OV2640_UXGA_INIT_REG_TBL_SIZE    (sizeof(ov2640_uxga_init_reg_tbl) / 2)
#define OV2640_SVGA_INIT_REG_TBL_SIZE    (sizeof(ov2640_svga_init_reg_tbl) / 2)
#define OV2640_YUV422_REG_TBL_SIZE       (sizeof(ov2640_yuv422_reg_tbl) / 2)
#define OV2640_JPEG_REG_TBL_SIZE         (sizeof(ov2640_jpeg_reg_tbl) / 2)
#define OV2640_RGB565_REG_TBL_SIZE       (sizeof(ov2640_rgb565_reg_tbl) / 2)

#endif
