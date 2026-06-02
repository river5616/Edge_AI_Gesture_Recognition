#include "uart_rx_task.h"
#include <string.h>
#include <stdio.h>

// 接收缓冲区（暴露给uart_callbacks.c使用）
uint8_t rx_buffer[256];
uint16_t rx_index = 0;
uint8_t rx_byte;  // 单字节接收缓冲

// 初始化
void UartRxTask_Init(void)
{
    memset(rx_buffer, 0, sizeof(rx_buffer));
    rx_index = 0;
}

// 解析命令（暴露给uart_callbacks.c使用）
void parse_command(uint8_t* cmd_str, uint16_t len)
{
    // 检查命令起始符
    if(cmd_str[0] != '$') return;

    // COLLECT命令: $COLLECT,<gesture_id>,<num_images>
    if(strncmp((char*)cmd_str, "$COLLECT,", 9) == 0)
    {
        int gesture_id, num_images;
        if(sscanf((char*)cmd_str, "$COLLECT,%d,%d", &gesture_id, &num_images) == 2)
        {
            if(gesture_id >= 0 && gesture_id <= 9 && num_images > 0)
            {
                g_gesture_id = (uint8_t)gesture_id;
                g_target_images = (uint16_t)num_images;
                g_current_images = 0;
                Task_SetMode(MODE_COLLECTING);

                // 发送确认
                char response[64];
                sprintf(response, "$OK,COLLECT,%d,%d\r\n", gesture_id, num_images);
                HAL_UART_Transmit(&huart1, (uint8_t*)response, strlen(response), 1000);
            }
        }
    }
    // STOP命令
    else if(strncmp((char*)cmd_str, "$STOP", 5) == 0)
    {
        Task_SetMode(MODE_IDLE);
        HAL_UART_Transmit(&huart1, (uint8_t*)"$OK,STOPPED\r\n", 13, 1000);
    }
    // STATUS命令
    else if(strncmp((char*)cmd_str, "$STATUS", 7) == 0)
    {
        char status[128];
        const char* mode_str;

        switch(Task_GetMode())
        {
            case MODE_IDLE:       mode_str = "IDLE"; break;
            case MODE_COLLECTING: mode_str = "COLLECTING"; break;
            case MODE_INFERENCE:  mode_str = "INFERENCE"; break;
            default:              mode_str = "UNKNOWN"; break;
        }

        sprintf(status, "$STATUS,%s,%d,%d,%d\r\n",
                mode_str, g_gesture_id, g_current_images, g_target_images);
        HAL_UART_Transmit(&huart1, (uint8_t*)status, strlen(status), 1000);
    }
    // INFER命令
    else if(strncmp((char*)cmd_str, "$INFER", 6) == 0)
    {
        Task_SetMode(MODE_INFERENCE);
        HAL_UART_Transmit(&huart1, (uint8_t*)"$OK,INFER_MODE\r\n", 16, 1000);
    }
    // STOP_INFER命令
    else if(strncmp((char*)cmd_str, "$STOP_INFER", 11) == 0)
    {
        Task_SetMode(MODE_IDLE);
        HAL_UART_Transmit(&huart1, (uint8_t*)"$OK,INFER_STOPPED\r\n", 19, 1000);
    }
    else
    {
        HAL_UART_Transmit(&huart1, (uint8_t*)"$ERROR,UNKNOWN_CMD\r\n", 20, 1000);
    }
}

/**
 * @brief  串口接收任务
 * @param  argument: 未使用
 * @retval None
 * @note   使用中断方式接收，实际接收在HAL_UART_RxCpltCallback中处理
 */
void AppTask_uart_rx(void *argument)
{
    TickType_t pxPreviousWakeTime = xTaskGetTickCount();

    UartRxTask_Init();

    // 启动UART1中断接收（上位机命令）
    HAL_UART_Receive_IT(&huart1, &rx_byte, 1);

    // 启动UART4中断接收（蓝牙命令）
    BT_Init();

    // 无限循环 - 主要用于监控和定期任务
    for(;;)
    {
        // 这里可以添加定期检查或心跳任务
        // 实际的串口接收在中断回调中处理

        // 定期延时
        vTaskDelayUntil(&pxPreviousWakeTime, pdMS_TO_TICKS(100));
    }
}

// 注意：HAL_UART_RxCpltCallback已移至uart_callbacks.c统一管理
