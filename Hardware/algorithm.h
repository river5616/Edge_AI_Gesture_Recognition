/**
 * @file algorithm.h
 * @brief 心率血氧算法（FFT方法）
 */
#ifndef __ALGORITHM_H
#define __ALGORITHM_H

#include <stdint.h>

#define FFT_N 1024    // FFT点数
#define START_INDEX 8 // 低频过滤阈值
#define SAMPLES_PER_SECOND 100 // 采样率100Hz

// 复数结构
struct compx {
    float real;
    float imag;
};

// 数学函数
double my_floor(double x);
double my_fmod(double x, double y);
double XSin(double x);
double XCos(double x);

// FFT相关函数
struct compx EE(struct compx a, struct compx b);
void FFT(struct compx *xin);
int find_max_num_index(struct compx *data, int count);

// 心率血氧计算函数
void maxim_heart_rate_and_oxygen_saturation(uint32_t *pun_ir_buffer, int32_t n_ir_buffer_length,
                                            uint32_t *pun_red_buffer, int32_t *pn_spo2,
                                            int8_t *pch_spo2_valid, int32_t *pn_heart_rate,
                                            int8_t *pch_hr_valid);

#endif /* __ALGORITHM_H */
