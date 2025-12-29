#include "uart_protocol.h"
#include <string.h>
#include <stdio.h>

// 全局协议句柄
static Protocol_Handle_t protocol_handle;

// 初始化协议
void Protocol_Init(void)
{
    memset(&protocol_handle, 0, sizeof(Protocol_Handle_t));
    protocol_handle.mode = MODE_IDLE;
    protocol_handle.cmd = CMD_NONE;
}

// 发送字符串
static void Protocol_SendString(const char* str)
{
    HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), 1000);
}

// 解析命令
void Protocol_ProcessCommand(uint8_t* data, uint16_t len)
{
    char cmd_buffer[256];

    // 复制数据到临时缓冲区
    if(len >= sizeof(cmd_buffer)) len = sizeof(cmd_buffer) - 1;
    memcpy(cmd_buffer, data, len);
    cmd_buffer[len] = '\0';

    // 检查命令起始符
    if(cmd_buffer[0] != '$')
    {
        Protocol_SendError(1, "Invalid command format");
        return;
    }

    // 解析COLLECT命令: $COLLECT,<gesture_id>,<num_images>
    if(strncmp(cmd_buffer, "$COLLECT,", 9) == 0)
    {
        int gesture_id, num_images;
        if(sscanf(cmd_buffer, "$COLLECT,%d,%d", &gesture_id, &num_images) == 2)
        {
            if(gesture_id >= 0 && gesture_id <= 9 && num_images > 0)
            {
                protocol_handle.cmd = CMD_COLLECT;
                protocol_handle.mode = MODE_COLLECTING;
                protocol_handle.gesture_id = (uint8_t)gesture_id;
                protocol_handle.target_images = (uint16_t)num_images;
                protocol_handle.current_images = 0;

                char response[64];
                sprintf(response, "$OK,COLLECT,%d,%d\r\n", gesture_id, num_images);
                Protocol_SendString(response);
            }
            else
            {
                Protocol_SendError(2, "Invalid parameters");
            }
        }
        else
        {
            Protocol_SendError(3, "Parse error");
        }
    }
    // 解析STOP命令
    else if(strncmp(cmd_buffer, "$STOP", 5) == 0)
    {
        protocol_handle.cmd = CMD_STOP;
        protocol_handle.mode = MODE_IDLE;
        Protocol_SendString("$OK,STOPPED\r\n");
    }
    // 解析STATUS命令
    else if(strncmp(cmd_buffer, "$STATUS", 7) == 0)
    {
        protocol_handle.cmd = CMD_STATUS;
        Protocol_SendStatus();
    }
    // 解析INFER命令
    else if(strncmp(cmd_buffer, "$INFER", 6) == 0)
    {
        protocol_handle.cmd = CMD_INFER;
        protocol_handle.mode = MODE_INFERENCE;
        Protocol_SendString("$OK,INFER_MODE\r\n");
    }
    // 解析STOP_INFER命令
    else if(strncmp(cmd_buffer, "$STOP_INFER", 11) == 0)
    {
        protocol_handle.cmd = CMD_STOP_INFER;
        protocol_handle.mode = MODE_IDLE;
        Protocol_SendString("$OK,INFER_STOPPED\r\n");
    }
    else
    {
        Protocol_SendError(4, "Unknown command");
    }
}

// 发送图像数据
void Protocol_SendImage(uint8_t* image_data, uint16_t width, uint16_t height)
{
    char header[128];
    uint32_t data_length = width * height;

    // 发送头部
    sprintf(header, "$IMG,%d,%d,%lu\r\n", width, height, data_length);
    Protocol_SendString(header);

    // 发送图像数据（使用DMA可以更高效）
    HAL_UART_Transmit(&huart1, image_data, data_length, 5000);

    // 发送结束标记
    Protocol_SendString("$END\r\n");
}

// 发送采集进度
void Protocol_SendProgress(uint16_t current, uint16_t total)
{
    char progress[64];
    sprintf(progress, "$PROGRESS,%d,%d\r\n", current, total);
    Protocol_SendString(progress);
}

// 发送推理结果
void Protocol_SendResult(uint8_t gesture_id, float confidence)
{
    char result[64];
    sprintf(result, "$RESULT,%d,%.2f\r\n", gesture_id, confidence);
    Protocol_SendString(result);
}

// 发送FPS信息
void Protocol_SendFPS(float fps)
{
    char fps_str[32];
    sprintf(fps_str, "$FPS,%.2f\r\n", fps);
    Protocol_SendString(fps_str);
}

// 发送错误信息
void Protocol_SendError(uint8_t error_code, const char* error_msg)
{
    char error[128];
    sprintf(error, "$ERROR,%d,%s\r\n", error_code, error_msg);
    Protocol_SendString(error);
}

// 发送状态信息
void Protocol_SendStatus(void)
{
    char status[128];
    const char* mode_str;

    switch(protocol_handle.mode)
    {
        case MODE_IDLE:
            mode_str = "IDLE";
            break;
        case MODE_COLLECTING:
            mode_str = "COLLECTING";
            break;
        case MODE_INFERENCE:
            mode_str = "INFERENCE";
            break;
        default:
            mode_str = "UNKNOWN";
            break;
    }

    sprintf(status, "$STATUS,%s,%d,%d,%d\r\n",
            mode_str,
            protocol_handle.gesture_id,
            protocol_handle.current_images,
            protocol_handle.target_images);

    Protocol_SendString(status);
}

// 获取工作模式
Work_Mode_t Protocol_GetMode(void)
{
    return protocol_handle.mode;
}

// 获取手势ID
uint8_t Protocol_GetGestureID(void)
{
    return protocol_handle.gesture_id;
}

// 获取目标图像数
uint16_t Protocol_GetTargetImages(void)
{
    return protocol_handle.target_images;
}

// 增加图像计数
void Protocol_IncrementImageCount(void)
{
    protocol_handle.current_images++;

    // 发送进度
    Protocol_SendProgress(protocol_handle.current_images, protocol_handle.target_images);

    // 检查是否完成
    if(protocol_handle.current_images >= protocol_handle.target_images)
    {
        protocol_handle.mode = MODE_IDLE;
        Protocol_SendString("$COMPLETE\r\n");
    }
}
