/**
 * @file bluetooth.c
 * @brief 蓝牙通信实现（UART4）
 */
#include "bluetooth.h"
#include "disp.h"
#include <stdio.h>
#include <string.h>

/* 外部UART句柄 */
extern UART_HandleTypeDef huart4;

/* 接收缓冲区 */
uint8_t bt_rx_byte = 0;
uint8_t bt_rx_buffer[64] = {0};
uint16_t bt_rx_index = 0;

/* 命令回调函数指针 */
static BT_CommandCallback_t command_callback = NULL;

/**
 * @brief 初始化蓝牙通信
 */
void BT_Init(void)
{
    memset(bt_rx_buffer, 0, sizeof(bt_rx_buffer));
    bt_rx_index = 0;

    /* 启动UART4中断接收 */
    HAL_UART_Receive_IT(&huart4, &bt_rx_byte, 1);
}

/**
 * @brief 发送传感器数据到蓝牙
 * @note 数据格式：温度,湿度,光照,距离,MQ2,心率,血氧\r\n
 *       异常数据用"--"代替
 */
void BT_SendSensorData(void)
{
    char buffer[128];
    char temp_str[8], humi_str[8], light_str[8], dist_str[8];
    char mq2_str[8], hr_str[8], spo2_str[8];

    /* 温度：检查DHT11有效性 */
    if (g_sensorData.dht11Valid && g_sensorData.temperature < 100) {
        sprintf(temp_str, "%d", g_sensorData.temperature);
    } else {
        strcpy(temp_str, "--");
    }

    /* 湿度：检查DHT11有效性 */
    if (g_sensorData.dht11Valid && g_sensorData.humidity <= 100) {
        sprintf(humi_str, "%d", g_sensorData.humidity);
    } else {
        strcpy(humi_str, "--");
    }

    /* 光照：检查范围 */
    if (g_sensorData.lightLux >= 0 && g_sensorData.lightLux < 65535) {
        sprintf(light_str, "%d", (uint16_t)g_sensorData.lightLux);
    } else {
        strcpy(light_str, "--");
    }

    /* 距离：检查有效范围（HCSR04有效范围2-500cm） */
    if (g_sensorData.distance >= 2 && g_sensorData.distance < 500) {
        sprintf(dist_str, "%d", g_sensorData.distance);
    } else {
        strcpy(dist_str, "--");
    }

    /* MQ2：检查有效范围 */
    if (g_sensorData.mq2PPM >= 0 && g_sensorData.mq2PPM < 10000) {
        sprintf(mq2_str, "%.1f", g_sensorData.mq2PPM);
    } else {
        strcpy(mq2_str, "--");
    }

    /* 心率：检查有效性和范围（正常心率30-250 bpm） */
    if (g_sensorData.heartRateValid && g_sensorData.heartRate >= 30 && g_sensorData.heartRate <= 250) {
        sprintf(hr_str, "%d", g_sensorData.heartRate);
    } else {
        strcpy(hr_str, "--");
    }

    /* 血氧：检查有效性和范围（正常血氧70-100%） */
    if (g_sensorData.spo2Valid && g_sensorData.spo2 >= 70 && g_sensorData.spo2 <= 100) {
        sprintf(spo2_str, "%d", g_sensorData.spo2);
    } else {
        strcpy(spo2_str, "--");
    }

    /* 组装CSV格式数据 */
    int len = sprintf(buffer, "%s,%s,%s,%s,%s,%s,%s\r\n",
                      temp_str, humi_str, light_str, dist_str,
                      mq2_str, hr_str, spo2_str);

    /* 发送数据 */
    HAL_UART_Transmit(&huart4, (uint8_t*)buffer, len, 1000);
}

/**
 * @brief 注册命令回调函数
 * @param callback 回调函数指针
 */
void BT_RegisterCommandCallback(BT_CommandCallback_t callback)
{
    command_callback = callback;
}

/**
 * @brief 处理接收到的命令
 * @param cmd 命令字符
 */
void BT_ProcessCommand(uint8_t cmd)
{
    /* 检查是否为有效的大写字母命令（A-Z） */
    if (cmd >= 'A' && cmd <= 'Z')
    {
        /* 发送命令确认 */
        char ack[16];
        sprintf(ack, "OK:%c\r\n", cmd);
        HAL_UART_Transmit(&huart4, (uint8_t*)ack, strlen(ack), 100);

        /* 调用回调函数 */
        if (command_callback != NULL)
        {
            command_callback((BT_Command_t)cmd);
        }
    }
}
