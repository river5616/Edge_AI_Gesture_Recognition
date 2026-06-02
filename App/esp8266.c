#include "esp8266.h"


void ESP8266_Init(void)
{
    char string[200];
    // sprintf(string, "AT+RST\r\n");
    // HAL_UART_Transmit(&huart5, (uint8_t *)string, strlen(string), 50);
    // HAL_Delay(500);
    // sprintf(string, "AT+CWMODE=1\r\n");
    // HAL_UART_Transmit(&huart5, (uint8_t *)string, strlen(string), 50);
    // sprintf(string, "AT+CWDHCP=1,1\r\n");
    // HAL_UART_Transmit(&huart5, (uint8_t *)string, strlen(string), 50);
    // sprintf(string, "AT+CWJAP=\"river\",\"river5616\"\r\n");
    // HAL_UART_Transmit(&huart5, (uint8_t *)string, strlen(string), 50);
    sprintf(string, "AT+MQTTUSERCFG=0,1,\"device1\",\"AmnaH12M2R\",\"version=2018-10-31&res=products%%2FAmnaH12M2R%%2Fdevices%%2Fdevice1&et=2813050644&method=md5&sign=9RAhtS9HzjhZzFDRSk5AIw%%3D%%3D\",0,0,\"\"\r\n");
    HAL_UART_Transmit(&huart5, (uint8_t *)string, strlen(string), 50);
    HAL_Delay(200);
    sprintf(string, "AT+MQTTUSERCFG=0,1,\"device1\",\"AmnaH12M2R\",\"version=2018-10-31&res=products%%2FAmnaH12M2R%%2Fdevices%%2Fdevice1&et=2813050644&method=md5&sign=9RAhtS9HzjhZzFDRSk5AIw%%3D%%3D\",0,0,\"\"\r\n");
    HAL_UART_Transmit(&huart5, (uint8_t *)string, strlen(string), 50);
    HAL_Delay(200);
    sprintf(string, "AT+MQTTCONN=0,\"mqtts.heclouds.com\",1883,1\r\n");
    HAL_UART_Transmit(&huart5, (uint8_t *)string, strlen(string), 50);
    HAL_Delay(100);
    sprintf(string, "AT+MQTTCONN=0,\"mqtts.heclouds.com\",1883,1\r\n");
    HAL_UART_Transmit(&huart5, (uint8_t *)string, strlen(string), 50);
    HAL_Delay(100);
    sprintf(string, "AT+MQTTSUB=0,\"$sys/AmnaH12M2R/device1/thing/property/post/reply\",1\r\n");
    HAL_UART_Transmit(&huart5, (uint8_t *)string, strlen(string), 50);
    HAL_Delay(100);
    sprintf(string, "AT+MQTTSUB=0,\"$sys/AmnaH12M2R/device1/thing/property/post/reply\",1\r\n");
    HAL_UART_Transmit(&huart5, (uint8_t *)string, strlen(string), 50);
}


void ESP8266_SendMQTTMessage(void)
{
    char string[200];
    if(g_sensorData.dht11Valid)
    {
        sprintf(string, "AT+MQTTPUB=0,\"$sys/AmnaH12M2R/device1/thing/property/post\",\"{\\\"id\\\":\\\"123\\\"\\,\\\"params\\\":{\\\"temperature\\\":{\\\"value\\\":%d\\}}}\",0,0\r\n", g_sensorData.temperature);
        HAL_UART_Transmit(&huart5, (uint8_t *)string, strlen(string), 50);
        osDelay(200);
        sprintf(string, "AT+MQTTPUB=0,\"$sys/AmnaH12M2R/device1/thing/property/post\",\"{\\\"id\\\":\\\"123\\\"\\,\\\"params\\\":{\\\"humidity\\\":{\\\"value\\\":%d\\}}}\",0,0\r\n", g_sensorData.humidity);
        HAL_UART_Transmit(&huart5, (uint8_t *)string, strlen(string), 50);
        osDelay(200);
    }
    sprintf(string, "AT+MQTTPUB=0,\"$sys/AmnaH12M2R/device1/thing/property/post\",\"{\\\"id\\\":\\\"123\\\"\\,\\\"params\\\":{\\\"lightLux\\\":{\\\"value\\\":%d\\}}}\",0,0\r\n", (int)g_sensorData.lightLux);
    HAL_UART_Transmit(&huart5, (uint8_t *)string, strlen(string), 50);
    osDelay(200);
    sprintf(string, "AT+MQTTPUB=0,\"$sys/AmnaH12M2R/device1/thing/property/post\",\"{\\\"id\\\":\\\"123\\\"\\,\\\"params\\\":{\\\"distance\\\":{\\\"value\\\":%d\\}}}\",0,0\r\n", g_sensorData.distance);
    HAL_UART_Transmit(&huart5, (uint8_t *)string, strlen(string), 50);
    osDelay(200);
    sprintf(string, "AT+MQTTPUB=0,\"$sys/AmnaH12M2R/device1/thing/property/post\",\"{\\\"id\\\":\\\"123\\\"\\,\\\"params\\\":{\\\"mq2PPM\\\":{\\\"value\\\":%.1f\\}}}\",0,0\r\n", g_sensorData.mq2PPM);
    HAL_UART_Transmit(&huart5, (uint8_t *)string, strlen(string), 50);
    osDelay(200);
    if(g_sensorData.heartRateValid)
    {
        sprintf(string, "AT+MQTTPUB=0,\"$sys/AmnaH12M2R/device1/thing/property/post\",\"{\\\"id\\\":\\\"123\\\"\\,\\\"params\\\":{\\\"heartRate\\\":{\\\"value\\\":%d\\}}}\",0,0\r\n", g_sensorData.heartRate);
        HAL_UART_Transmit(&huart5, (uint8_t *)string, strlen(string), 50);
        osDelay(200);
    }
    if(g_sensorData.spo2Valid)
    {
        sprintf(string, "AT+MQTTPUB=0,\"$sys/AmnaH12M2R/device1/thing/property/post\",\"{\\\"id\\\":\\\"123\\\"\\,\\\"params\\\":{\\\"spo2\\\":{\\\"value\\\":%d\\}}}\",0,0\r\n", g_sensorData.spo2);
        HAL_UART_Transmit(&huart5, (uint8_t *)string, strlen(string), 50);
        osDelay(200);
    }
}

