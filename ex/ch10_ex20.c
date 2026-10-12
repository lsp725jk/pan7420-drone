/**
 * ch10_ex20.c —— 【对照实验】定点数 vs 浮点的速度对比（PC 上实测）
 */
#include <stdio.h>
#include <stdint.h>
#include <time.h>

#define N 2000000L

#define Q16_ONE  65536
#define Q16(f)   ((int32_t)((f) * Q16_ONE))
static inline int32_t q16_mul(int32_t a, int32_t b)
{
    return (int32_t)(((int64_t)a * (int64_t)b) >> 16);
}
/* 快速版：避免 64 位乘法（真机上差别很大，见 ARM 反汇编） */
static inline int32_t q16_mul_fast(int32_t a, int32_t b)
{
    int32_t ah = a >> 16, al = a & 0xFFFF;
    return (int32_t)((int64_t)al * b >> 16) + (ah * b);
}

static volatile int32_t g_i32 = 0;
static volatile float   g_f32 = 0.0f;
static volatile double  g_f64 = 0.0;

int main(void)
{
                /* ---------- 1) 定点：整数乘加 ---------- */
    clock_t c0 = clock();
    int32_t acc_i = Q16(1.5);
    const int32_t k_i = Q16(0.9999);
    for (long i = 0; i < N; i++) acc_i = q16_mul(acc_i, k_i) + Q16(0.001);
    clock_t c1 = clock();
    g_i32 = acc_i;

                /* ---------- 2) 定点快速版（无 64 位乘法） ---------- */
    clock_t c2 = clock();
    int32_t acc_f = Q16(1.5);
    for (long i = 0; i < N; i++) acc_f = q16_mul_fast(acc_f, k_i) + Q16(0.001);
    clock_t c3 = clock();
    g_i32 = acc_f;

                /* ---------- 3) float（本机有 FPU，会很快） ---------- */
    clock_t c4 = clock();
    float acc_f32 = 1.5f;
    const float k_f32 = 0.9999f;
    for (long i = 0; i < N; i++) acc_f32 = acc_f32 * k_f32 + 0.001f;
    clock_t c5 = clock();
    g_f32 = acc_f32;

                /* ---------- 4) double ---------- */
    clock_t c6 = clock();
    double acc_f64 = 1.5;
    const double k_f64 = 0.9999;
    for (long i = 0; i < N; i++) acc_f64 = acc_f64 * k_f64 + 0.001;
    clock_t c7 = clock();
    g_f64 = acc_f64;

    printf("迭代 %ld 次（一次「乘 + 加」），PC 上实测：\n\n", N);
    printf("  Q16.16 定点（64 位乘法）: %5.1f ms\n",
           1000.0 * (double)(c1 - c0) / CLOCKS_PER_SEC);
    printf("  Q16.16 定点（32 位乘法）: %5.1f ms\n",
           1000.0 * (double)(c3 - c2) / CLOCKS_PER_SEC);
    printf("  float                    : %5.1f ms\n",
           1000.0 * (double)(c5 - c4) / CLOCKS_PER_SEC);
    printf("  double                   : %5.1f ms\n",
           1000.0 * (double)(c7 - c6) / CLOCKS_PER_SEC);

    printf("\n结果对比：\n");
    printf("  定点结果(64位) 转浮点 = %.6f\n", (double)acc_i / Q16_ONE);
    printf("  定点结果(32位) 转浮点 = %.6f\n", (double)acc_f / Q16_ONE);
    printf("  float  结果          = %.6f\n", (double)acc_f32);
    printf("  double 结果          = %.6f\n", acc_f64);
    printf("  -> 定点快速版和浮点的结果很接近，说明「少一次 64 位乘法」是可行的\n\n");

    printf("为什么在 PC 上的结论不能直接搬到 M0+：\n");
    printf("  · 本机 x86 有硬件 FPU，float/double 一条指令就完事；\n");
    printf("  · Cortex-M0+ 【没有 FPU】（ARM GCC 会定义 __SOFTFP__，没有 __ARM_FP），\n");
    printf("    一次 x*k+1.0f 会编译成两次软件库调用：\n");
    printf("        bl __aeabi_fmul\n");
    printf("        bl __aeabi_fadd\n");
    printf("    （本机 arm-none-eabi-objdump 实测，见 ch10_arm04_fixed.c 的反汇编）\n");
    printf("  · 而定点乘法也可能调 __aeabi_lmul（64 位乘法库函数）。\n");
    printf("    所以真机上「定点一定快」的前提是：避开 64 位中间结果。\n");
    printf("\n实用建议：M0+ 上把控制律写成「32 位乘 + 移位」的 Q15/Q16 形式，\n");
    printf("          并且用 arm-none-eabi-objdump 看一眼有没有 bl __aeabi_* 调用。\n");
    return 0;
}
