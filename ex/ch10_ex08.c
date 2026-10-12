/**
 * ch10_ex08.c —— 定点数（fixed-point）vs 浮点：没有 FPU 时怎么算小数
 */
#include <stdio.h>
#include <stdint.h>

#define Q16_ONE   65536                 /**
 * 1.0 的定点表示
 */
#define Q16(f)    ((int32_t)((f) * Q16_ONE + ((f) >= 0 ? 0.5 : -0.5)))
#define Q16_TO_F(x) ((double)(x) / Q16_ONE)

/* 定点乘法：乘积是 Q32.32，右移 16 位缩回 Q16.16。 注意要用 int64_t 存中间结果，否则两个 Q16.16 相乘立刻就溢出了。 */
static int32_t q16_mul(int32_t a, int32_t b)
{
    return (int32_t)(((int64_t)a * (int64_t)b) >> 16);
}

/* 定点除法：先左移 16 位再除 */
static int32_t q16_div(int32_t a, int32_t b)
{
    return (int32_t)((((int64_t)a) << 16) / b);
}

int main(void)
{
    printf("Q16.16 的 1.0 = %d\n", Q16_ONE);

                /* 1) 基本转换 */
    static const double vals[] = { 1.0, -1.0, 0.5, 3.14159265, -273.15, 0.0001 };
    printf("\n浮点 -> Q16.16 -> 浮点（看精度损失）：\n");
    for (int k = 0; k < 6; k++) {
        int32_t q = Q16(vals[k]);
        printf("  %12.6f -> %11d -> %12.6f  误差 %.8f\n",
               vals[k], q, Q16_TO_F(q), Q16_TO_F(q) - vals[k]);
    }

                /* 2) 乘法：和浮点比一比 */
    printf("\n乘法对比：\n");
    static const double pairs[][2] = { { 1.5, 2.25 }, { -3.75, 0.125 }, { 100.0, 3.0 } };
    for (int k = 0; k < 3; k++) {
        double a = pairs[k][0], b = pairs[k][1];
        double f = a * b;
        int32_t q = q16_mul(Q16(a), Q16(b));
        printf("  %8.4f * %8.4f : 浮点 = %12.6f , 定点 = %12.6f , 差 = %.8f\n",
               a, b, f, Q16_TO_F(q), Q16_TO_F(q) - f);
    }

                /* 3) 除法 */
    printf("\n除法对比：\n");
    double a = 1.0, b = 3.0;
    printf("  1.0 / 3.0 : 浮点 = %.10f , 定点 = %.10f\n",
           a / b, Q16_TO_F(q16_div(Q16(a), Q16(b))));
    printf("  定点比浮点精度低，是因为 Q16.16 只有 16 位小数位\n");

                /* 4) 溢出警告：Q16.16 的整数部分只有 15 位（含符号） */
    printf("\n范围与溢出：\n");
    printf("  Q16.16 能表示的最大值 = %.4f（32767.99998）\n", Q16_TO_F(0x7FFFFFFF));
    int32_t big = Q16(30000.0);
    printf("  30000.0 定点 = %d\n", big);
    printf("  再乘 2.0 -> %d  = %.4f  <-- 溢出了！结果是负的\n",
           q16_mul(big, Q16(2.0)), Q16_TO_F(q16_mul(big, Q16(2.0))));
    printf("  浮点版不会溢出，这就是定点数的代价：要自己盯着范围。\n");

                /* 5) 变式：精度不够时的办法 —— 用 Q8.24（小数位更多，范围更小） */
    printf("\n变式：如果只要 0~256 的范围但要更高精度，用 Q8.24：\n");
    printf("  Q8.24 的 1.0 = %d ，最小分辨率 = %.10f\n", 1 << 24, 1.0 / (1 << 24));
    printf("  飞控里的「角度」一般 ±180°，用 Q16.16 足够；\n");
    printf("  「陀螺仪零偏」这种极小量才需要 Q8.24 或 Q24.8。\n");
    return 0;
}
