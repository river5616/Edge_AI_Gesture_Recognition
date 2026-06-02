#ifndef __SENSORS_H
#define __SENSORS_H

/* 包含所有传感器头文件 */
#include "BH1750.h"
#include "HCSR04.h"
#include "MQ2.h"
#include "MAX30102.h"
#include "DHT11.h"

/* 传感器初始化函数 */
void Sensors_Init(void);

#endif /* __SENSORS_H */
