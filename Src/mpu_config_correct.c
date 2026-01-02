/**
 * @file    mpu_config_correct.c
 * @brief   STM32H7正确的MPU配置（用于DMA + Cache场景）
 * @note    如果禁用MPU后断电重连问题解决，则使用此配置替换main.c中的MPU_Config()
 */

#include "main.h"

/**
 * @brief  正确配置MPU，支持DMA访问RAM
 */
void MPU_Config(void)
{
    MPU_Region_InitTypeDef MPU_InitStruct = {0};

    /* 禁用MPU */
    HAL_MPU_Disable();

    /* ==================== Region 0: D1 RAM (AXI SRAM) ====================
     * 基地址: 0x24000000
     * 大小: 512KB
     * 用途: DMA缓冲区（dma_buffer所在区域）
     * 配置: 可缓存、可写、允许DMA访问
     */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER0;
    MPU_InitStruct.BaseAddress = 0x24000000;
    MPU_InitStruct.Size = MPU_REGION_SIZE_512KB;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;       // 允许Cache
    MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;     // 允许写缓冲

    HAL_MPU_ConfigRegion(&MPU_InitStruct);

    /* ==================== Region 1: D2 RAM (SRAM1/2/3) ====================
     * 基地址: 0x30000000
     * 大小: 288KB
     * 用途: DMA缓冲区备选区域
     * 配置: 可缓存、可写、允许DMA访问
     */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER1;
    MPU_InitStruct.BaseAddress = 0x30000000;
    MPU_InitStruct.Size = MPU_REGION_SIZE_256KB;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;

    HAL_MPU_ConfigRegion(&MPU_InitStruct);

    /* ==================== Region 2: DTCM RAM ====================
     * 基地址: 0x20000000
     * 大小: 128KB
     * 用途: CPU快速访问（不支持DMA）
     * 配置: 不可缓存（DTCM本身就很快）
     * 注意: 不要将DMA缓冲区放在DTCM！
     */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER2;
    MPU_InitStruct.BaseAddress = 0x20000000;
    MPU_InitStruct.Size = MPU_REGION_SIZE_128KB;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;   // DTCM不需要Cache
    MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

    HAL_MPU_ConfigRegion(&MPU_InitStruct);

    /* ==================== Region 3: Flash ====================
     * 基地址: 0x08000000
     * 大小: 2MB
     * 用途: 代码段
     * 配置: 可缓存、只读、可执行
     */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER3;
    MPU_InitStruct.BaseAddress = 0x08000000;
    MPU_InitStruct.Size = MPU_REGION_SIZE_2MB;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
    MPU_InitStruct.AccessPermission = MPU_REGION_PRIV_RO;    // 只读
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE; // 允许执行代码
    MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;

    HAL_MPU_ConfigRegion(&MPU_InitStruct);

    /* 启用MPU */
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}
