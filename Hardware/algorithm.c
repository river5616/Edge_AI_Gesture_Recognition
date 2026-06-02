/**
 * @file algorithm.c
 * @brief 心率血氧算法（基于FFT）
 */
#include "algorithm.h"
#include <math.h>
#include <string.h>

#define XPI (3.1415926535897932384626433832795)
#define XENTRY (100)
#define XINCL (XPI/2/XENTRY)
#define PI 3.1415926535897932384626433832795028841971

// 全局FFT缓冲区
static struct compx s1[FFT_N+16];
static struct compx s2[FFT_N+16];

// 正弦查找表（节省计算时间）
static const double XSinTbl[] = {
    0.00000000000000000, 0.015707317311820675, 0.031410759078128292, 0.047106450709642665,
    0.062790519529313374, 0.078459095727844944, 0.094108313318514325, 0.10973431109104528,
    0.12533323356430426, 0.14090123193758267, 0.15643446504023087, 0.17192910027940955,
    0.18738131458572463, 0.20278729535651249, 0.21814324139654256, 0.23344536385590542,
    0.24868988716485479, 0.26387304996537292, 0.27899110603922928, 0.29404032523230400,
    0.30901699437494740, 0.32391741819814940, 0.33873792024529142, 0.35347484377925714,
    0.36812455268467797, 0.38268343236508978, 0.39714789063478062, 0.41151435860510882,
    0.42577929156507272, 0.43993916985591514, 0.45399049973954680, 0.46792981426057340,
    0.48175367410171532, 0.49545866843240760, 0.50904141575037132, 0.52249856471594880,
    0.53582679497899666, 0.54902281799813180, 0.56208337785213058, 0.57500525204327857,
    0.58778525229247314, 0.60042022532588402, 0.61290705365297649, 0.62524265633570519,
    0.63742398974868975, 0.64944804833018377, 0.66131186532365183, 0.67301251350977331,
    0.68454710592868873, 0.69591279659231442, 0.70710678118654757, 0.71812629776318881,
    0.72896862742141155, 0.73963109497860968, 0.75011106963045959, 0.76040596560003104,
    0.77051324277578925, 0.78043040733832969, 0.79015501237569041, 0.79968465848709058,
    0.80901699437494745, 0.81814971742502351, 0.82708057427456183, 0.83580736136827027,
    0.84432792550201508, 0.85264016435409218, 0.86074202700394364, 0.86863151443819120,
    0.87630668004386369, 0.88376563008869347, 0.89100652418836779, 0.89802757576061565,
    0.90482705246601958, 0.91140327663544529, 0.91775462568398114, 0.92387953251128674,
    0.92977648588825146, 0.93544403082986738, 0.94088076895422557, 0.94608535882754530,
    0.95105651629515353, 0.95579301479833012, 0.96029368567694307, 0.96455741845779808,
    0.96858316112863108, 0.97236992039767667, 0.97591676193874743, 0.97922281062176575,
    0.98228725072868872, 0.98510932615477398, 0.98768834059513777, 0.99002365771655754,
    0.99211470131447788, 0.99396095545517971, 0.99556196460308000, 0.99691733373312796,
    0.99802672842827156, 0.99888987496197001, 0.99950656036573160, 0.99987663248166059,
    1.00000000000000000
};

// 向下取整
double my_floor(double x) {
    double y = x;
    if (((*((int *)&y) + 1) & 0x80000000) != 0)
        return (float)((int)x) - 1;
    else
        return (float)((int)x);
}

// 取余
double my_fmod(double x, double y) {
    double temp, ret;
    if (y == 0.0)
        return 0.0;
    temp = my_floor(x / y);
    ret = x - temp * y;
    if ((x < 0.0) != (y < 0.0))
        ret = ret - y;
    return ret;
}

// 正弦函数
double XSin(double x) {
    int s = 0, n;
    double dx, sx, cx;
    if (x < 0)
        s = 1, x = -x;
    x = my_fmod(x, 2 * XPI);
    if (x > XPI)
        s = !s, x -= XPI;
    if (x > XPI / 2)
        x = XPI - x;
    n = (int)(x / XINCL);
    dx = x - n * XINCL;
    if (dx > XINCL / 2)
        ++n, dx -= XINCL;
    sx = XSinTbl[n];
    cx = XSinTbl[XENTRY - n];
    x = sx + dx * cx - (dx * dx) * sx / 2
        - (dx * dx * dx) * cx / 6
        + (dx * dx * dx * dx) * sx / 24;
    return s ? -x : x;
}

// 余弦函数
double XCos(double x) {
    return XSin(x + XPI / 2);
}

// 复数乘法
struct compx EE(struct compx a, struct compx b) {
    struct compx c;
    c.real = a.real * b.real - a.imag * b.imag;
    c.imag = a.real * b.imag + a.imag * b.real;
    return c;
}

// 快速傅里叶变换
void FFT(struct compx *xin) {
    int f, m, nv2, nm1, i, k, l, j = 0;
    struct compx u, w, t;

    nv2 = FFT_N / 2;
    nm1 = FFT_N - 1;
    
    // 变址运算
    for (i = 0; i < nm1; i++) {
        if (i < j) {
            t = xin[j];
            xin[j] = xin[i];
            xin[i] = t;
        }
        k = nv2;
        while (k <= j) {
            j = j - k;
            k = k / 2;
        }
        j = j + k;
    }

    // FFT运算核
    int le, lei, ip;
    f = FFT_N;
    for (l = 1; (f = f / 2) != 1; l++);
    
    for (m = 1; m <= l; m++) {
        le = 2 << (m - 1);
        lei = le / 2;
        u.real = 1.0;
        u.imag = 0.0;
        w.real = XCos(PI / lei);
        w.imag = -XSin(PI / lei);
        
        for (j = 0; j <= lei - 1; j++) {
            for (i = j; i <= FFT_N - 1; i = i + le) {
                ip = i + lei;
                t = EE(xin[ip], u);
                xin[ip].real = xin[i].real - t.real;
                xin[ip].imag = xin[i].imag - t.imag;
                xin[i].real = xin[i].real + t.real;
                xin[i].imag = xin[i].imag + t.imag;
            }
            u = EE(u, w);
        }
    }
}

// 查找最大值索引
int find_max_num_index(struct compx *data, int count) {
    int i = START_INDEX;
    int max_num_index = i;
    float temp = data[i].real;
    
    for (i = START_INDEX; i < count; i++) {
        if (temp < data[i].real) {
            temp = data[i].real;
            max_num_index = i;
        }
    }
    return max_num_index;
}

// 心率血氧计算（使用FFT方法）
void maxim_heart_rate_and_oxygen_saturation(uint32_t *pun_ir_buffer, int32_t n_ir_buffer_length,
                                            uint32_t *pun_red_buffer, int32_t *pn_spo2,
                                            int8_t *pch_spo2_valid, int32_t *pn_heart_rate,
                                            int8_t *pch_hr_valid) {
    int32_t i;
    float n_denom;
    float dc_red = 0, dc_ir = 0;
    float ac_red = 0, ac_ir = 0;

    // 默认值
    *pn_spo2 = -999;
    *pch_spo2_valid = 0;
    *pn_heart_rate = -999;
    *pch_hr_valid = 0;

    // 检查缓冲区大小
    if (n_ir_buffer_length < FFT_N)
        return;

    // 清零FFT缓冲区
    memset(s1, 0, sizeof(s1));
    memset(s2, 0, sizeof(s2));

    // 复制数据到FFT缓冲区
    for (i = 0; i < FFT_N; i++) {
        s1[i].real = (float)pun_red_buffer[i];
        s1[i].imag = 0;
        s2[i].real = (float)pun_ir_buffer[i];
        s2[i].imag = 0;
    }

    // 计算DC分量（平均值）
    for (i = 0; i < FFT_N; i++) {
        dc_red += s1[i].real;
        dc_ir += s2[i].real;
    }
    dc_red = dc_red / FFT_N;
    dc_ir = dc_ir / FFT_N;

    // 在去除DC前先计算AC分量（时域峰峰值）
    float red_max = -999999, red_min = 999999;
    float ir_max = -999999, ir_min = 999999;

    for (i = 0; i < FFT_N; i++) {
        if (s1[i].real > red_max) red_max = s1[i].real;
        if (s1[i].real < red_min) red_min = s1[i].real;
        if (s2[i].real > ir_max) ir_max = s2[i].real;
        if (s2[i].real < ir_min) ir_min = s2[i].real;
    }

    ac_red = red_max - red_min;
    ac_ir = ir_max - ir_min;

    // 去除DC分量（用于FFT心率计算）
    for (i = 0; i < FFT_N; i++) {
        s1[i].real = s1[i].real - dc_red;
        s2[i].real = s2[i].real - dc_ir;
    }

    // 移动平均滤波
    for (i = 1; i < FFT_N - 1; i++) {
        n_denom = (s1[i - 1].real + 2 * s1[i].real + s1[i + 1].real);
        s1[i].real = n_denom / 4.00;

        n_denom = (s2[i - 1].real + 2 * s2[i].real + s2[i + 1].real);
        s2[i].real = n_denom / 4.00;
    }

    // 八点平均滤波
    for (i = 0; i < FFT_N - 8; i++) {
        n_denom = (s1[i].real + s1[i + 1].real + s1[i + 2].real + s1[i + 3].real +
                   s1[i + 4].real + s1[i + 5].real + s1[i + 6].real + s1[i + 7].real);
        s1[i].real = n_denom / 8.00;

        n_denom = (s2[i].real + s2[i + 1].real + s2[i + 2].real + s2[i + 3].real +
                   s2[i + 4].real + s2[i + 5].real + s2[i + 6].real + s2[i + 7].real);
        s2[i].real = n_denom / 8.00;
    }

    // 执行FFT
    FFT(s1);
    FFT(s2);

    // 计算幅度
    for (i = 0; i < FFT_N; i++) {
        s1[i].real = sqrtf(s1[i].real * s1[i].real + s1[i].imag * s1[i].imag);
        s2[i].real = sqrtf(s2[i].real * s2[i].real + s2[i].imag * s2[i].imag);
    }

    // 检查信号质量
    for (i = 0; i < 50; i++) {
        if (s1[i].real <= 10)
            break;
    }

    // 查找峰值（用于心率计算）
    int s1_max_index = find_max_num_index(s1, 60);

    // 检查信号有效性
    if (i >= 45) {
        // 计算心率
        uint16_t Heart_Rate = 60.00 * SAMPLES_PER_SECOND * s1_max_index / FFT_N;
        *pn_heart_rate = Heart_Rate;

        // 心率范围校验
        if (*pn_heart_rate >= 40 && *pn_heart_rate <= 200) {
            *pch_hr_valid = 1;
        } else {
            *pn_heart_rate = -999;
            *pch_hr_valid = 0;
        }

        // 计算血氧（使用时域AC/DC比值）
        if (ac_red > 0 && dc_ir > 0 && ac_ir > 0 && dc_red > 0) {
            // R = (AC_red/DC_red) / (AC_ir/DC_ir)
            float R = (ac_red / dc_red) / (ac_ir / dc_ir);
            float sp02_num = -45.060 * R * R + 30.354 * R + 94.845;

            // 先赋值（便于调试）
            *pn_spo2 = (int32_t)sp02_num;

            // 血氧有效性判断
            if (sp02_num >= 70.0 && sp02_num <= 120.0) {
                *pch_spo2_valid = 1;
            } else {
                *pch_spo2_valid = 0;
            }
        }
    } else {
        // 信号质量差
        *pn_heart_rate = -999;
        *pch_hr_valid = 0;
        *pn_spo2 = -999;
        *pch_spo2_valid = 0;
    }
}
