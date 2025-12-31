/**
 ******************************************************************************
 * @file    uart_callbacks.c
 * @brief   UART回调函数统一管理
 * @note    所有UART的HAL回调函数都在此文件中实现，避免符号重复定义
 ******************************************************************************
 */

#include "uart_callbacks.h"
#include "uart_rx_task.h"
#include "openmv_rx_task.h"
#include <string.h>

// DMA缓冲区大小（与openmv_rx_task.c中的定义一致）
#define DMA_BUFFER_SIZE     (OPENMV_PACKET_SIZE * 2)

/**
 * @brief  UART接收完成回调
 * @param  huart: UART句柄
 * @retval None
 * @note   由HAL库自动调用
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    // ========== USART1：上位机命令接收（中断模式） ==========
    if(huart->Instance == USART1)
    {
        // 将接收到的字节添加到缓冲区
        if(rx_index < 256 - 1)
        {
            rx_buffer[rx_index++] = rx_byte;

            // 检查是否接收到完整命令（以\n结尾）
            if(rx_byte == '\n')
            {
                rx_buffer[rx_index] = '\0';

                // 解析命令
                parse_command(rx_buffer, rx_index);

                // 重置缓冲区
                rx_index = 0;
            }
        }
        else
        {
            // 缓冲区溢出，重置
            rx_index = 0;
        }

        // 继续接收下一个字节
        HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
    }

    // ========== USART3：OpenMV数据接收（DMA模式） ==========
    // 注意：当前主要使用空闲中断模式，此回调仅在DMA缓冲区满时触发（很少发生）
    else if(huart->Instance == USART3)
    {
        // DMA缓冲区满，处理数据
        SCB_InvalidateDCache_by_Addr((uint32_t*)dma_buffer, DMA_BUFFER_SIZE);
        parse_receive_buffer(dma_buffer, DMA_BUFFER_SIZE);

        // 通知任务
        if(openmv_task_handle != NULL)
        {
            osThreadFlagsSet(openmv_task_handle, 0x01);
        }
    }
}

/**
 * @brief  UART接收半完成回调
 * @param  huart: UART句柄
 * @retval None
 */
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
    // USART1不使用半完成回调

    // USART3半完成回调（可选）
    if(huart->Instance == USART3)
    {
        // 可以在这里处理半缓冲区的数据
        // 但要注意不要和完成回调冲突
    }
}

/**
 * @brief  UART错误回调
 * @param  huart: UART句柄
 * @retval None
 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)
    {
        // USART1错误处理：重新启动接收
        HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
    }
    else if(huart->Instance == USART3)
    {
        // USART3错误处理：重新启动DMA接收
        HAL_UART_DMAStop(&huart3);
        HAL_UART_Receive_DMA(&huart3, dma_buffer, DMA_BUFFER_SIZE);
    }
}

