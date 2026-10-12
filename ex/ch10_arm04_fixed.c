/**
 * ch10_arm04_fixed.c —— 无 FPU 的代价：浮点 vs Q16.16 定点
 */
#include <stdint.h>

/**
 * 浮点版：会去调用 __aeabi_fmul / __aeabi_fadd
 */
float scale_float(float x, float k)
{
    return x * k + 1.0f;
}

/* 定点版 Q16.16：把"1.0"表示成 65536。 乘法之后要右移 16 位把小数位"缩回来"； 中间的乘积用 int64_t 存，避免溢出。 */
int32_t scale_q16(int32_t x, int32_t k)
{
    int64_t t = (int64_t)x * (int64_t)k;              /* Q16.16 * Q16.16 = Q32.32 */
    t >>= 16;                                         /* 缩回 Q16.16 */
    return (int32_t)t + (1 << 16);                    /* 加 1.0 */
}

/* 变式：不用 64 位乘法也能做（先各取高 16 位），代价是精度略低 */
int32_t scale_q16_fast(int32_t x, int32_t k)
{
    return (int32_t)(((int64_t)x * k) >> 16);
}
