/**
 * 文件：imu/filter.c                 【角色】数字滤波器：给传感器数据降噪
 */



#include "filter.h"
#include <math.h>

/**
 * 数学常量：加 _FLOAT 后缀是为了表明用 float 版本参与运算 （M_LN2 = ln2，用于把"倍频程"换算成频率比）
 */
#define M_LN2_FLOAT 0.69314718055994530942f
#define M_PI_FLOAT  3.14159265358979323846f



#define BIQUAD_BANDWIDTH 1.9f           // bandwidth in octaves
#define BIQUAD_Q (1.0f / sqrtf(2.0f))   // 2nd order Butterworth Q



// #define UNUSED(x) (void)(x)

// ------------------------------------------------------------------------------------
/* ==================== 空滤波器 ==================== 直接原样返回输入。存在的意义是"占位"： 上层代码可能通过统一的函数指针调用滤波器， 某些通道不需要滤波时，就指向这个函数，避免写 if 判断。 */
// NULL filter
float nullFilterApply(void *filter, float input)
{
    UNUSED(filter);           /* 宏展开成 (void)(filter)，作用是"告诉编译器我故意不用这个参数"， 避免 -Wunused-parameter 警告。这是 C 里处理未用参数的惯用手法。 */
    return input;
}

// ------------------------------------------------------------------------------------

// PT1 Low Pass filter PT1低通滤波器
/* @brief  初始化 PT1 低通滤波器 【参数】 @param f_cut 截止频率（Hz）。低于它基本通过，高于它被衰减。 经验：陀螺仪常用 80~150Hz，具体看机架振动情况。 */
void pt1FilterInit(pt1Filter_t *filter, uint8_t f_cut, float dT)
{
    filter->RC = 1.0f / ( 2.0f * M_PI_FLOAT * f_cut );           /* RC 时间常数 */
    filter->dT = dT;
}

/* @brief  执行一次 PT1 滤波 【算法】y += dt/(RC+dt) * (x - y)，即上面说明里的递推式。 注意 state（上一次的输出）保存在结构体里，所以这是"有记忆"的滤波器， 第一次调用前必须把 state 清零（通常结构体定义时 = {0} 即可）。 */
float pt1FilterApply(pt1Filter_t *filter, float input)
{
    filter->state = filter->state + filter->dT / (filter->RC + filter->dT) * (input - filter->state);
    return filter->state;
}



float pt1FilterApply4(pt1Filter_t *filter, float input, uint8_t f_cut, float dT)
{
        // Pre calculate and store RC
    if (!filter->RC) {
        pt1FilterInit(filter, f_cut, dT);
    }
    filter->state = filter->state + filter->dT / (filter->RC + filter->dT) * (input - filter->state);
    return filter->state;
}

// ------------------------------------------------------------------------------------

static void biquadFilterInit(biquadFilter_t *filter, float filterFreq, uint32_t sampleDeltaUs, float Q, biquadFilterType_e filterType)
{
        // setup variables
                /* sampleHz：采样率，由采样周期（微秒）换算成 Hz。 0.000001f 是把微秒换成秒；1/周期 = 频率。 */
    const float sampleHz = 1 / ((float)sampleDeltaUs * 0.000001f);
                /* omega：截止频率对应的数字角频率（归一化到采样率），范围 0~π */
    const float omega = 2 * M_PI_FLOAT * filterFreq / sampleHz;
    const float sn = sinf(omega);
    const float cs = cosf(omega);
                /* alpha：决定滤波器"陡峭程度"与 Q 值的参数。 Q 越大峰越尖（可能谐振），Q=1/√2 是巴特沃斯（最平坦）。 */
    const float alpha = sn / (2 * Q);

    float b0, b1, b2, a0, a1, a2;

                /* 按滤波器类型套用不同的系数公式（这些是双线性变换的标准结果） */
    switch (filterType) {
        default:
        case FILTER_LPF:                      /* 低通：保留低频，衰减高频 —— 用于传感器降噪 */
            b0 = (1 - cs) / 2;
            b1 = 1 - cs;
            b2 = (1 - cs) / 2;
            a0 = 1 + alpha;
            a1 = -2 * cs;
            a2 = 1 - alpha;
            break;
        case FILTER_NOTCH:                    /* 陷波：只掐掉 filterFreq 附近很窄的一段 典型用途：电机转速引起的共振（例如 200Hz 的机体振动）， 用陷波精准掐掉，其余频段不受影响。 */
            b0 =  1;
            b1 = -2 * cs;
            b2 =  1;
            a0 =  1 + alpha;
            a1 = -2 * cs;
            a2 =  1 - alpha;
            break;
    }

        // precompute the coefficients
                /* 归一化：全部除以 a0，这样运行时就不需要再做除法（除法在 M0+ 上很慢） */
    filter->b0 = b0 / a0;
    filter->b1 = b1 / a0;
    filter->b2 = b2 / a0;
    filter->a1 = a1 / a0;
    filter->a2 = a2 / a0;

        // zero initial samples
                /* 清历史状态。不清的话第一次输出会因为残留值产生一个冲击 */
    filter->d1 = filter->d2 = 0;
}

/* @brief  按低通模式初始化双二阶滤波器（最常用入口） 【参数】 @param filterFreq    截止频率（Hz） @param sampleDeltaUs 采样周期（微秒）。控制周期 1ms 就传 1000。 */
/* sets up a biquad Filter */
void biquadFilterInitLPF(biquadFilter_t *filter, float filterFreq, uint32_t sampleDeltaUs)
{
    biquadFilterInit(filter, filterFreq, sampleDeltaUs, BIQUAD_Q, FILTER_LPF);
}



/* Computes a biquadFilter_t filter on a sample */
float biquadFilterApply(biquadFilter_t *filter, float input)
{
    const float result = filter->b0 * input + filter->d1;

    filter->d1 = filter->b1 * input - filter->a1 * result + filter->d2;
    filter->d2 = filter->b2 * input - filter->a2 * result;

#ifdef DEBUG_BIQUAD_INFINITY
    if (!isfinite(filter->d1) || !isfinite(filter->d2) ) {
        failureMode(FAILURE_DEVELOPER);
    }
#endif

    return result;
}



