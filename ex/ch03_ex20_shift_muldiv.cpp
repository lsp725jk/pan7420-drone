/**
 * ch03_ex20_shift_muldiv.c —— 用移位代替乘除 2 的幂：什么时候对、什么时候错
 */
#include <stdio.h>
#include <limits.h>
#include <stdint.h>

/**
 * 无符号：位移和除法完全等价
 */
static int check_unsigned(void)
{
    int all_same = 1;
    printf("  无符号数：x >> n 与 x / 2^n 是否一致\n");
    uint32_t xs[4] = { 0u, 1u, 100u, 0xFFFFFFFFu };
    for (int i = 0; i < 4; i++) {
        for (int n = 0; n <= 3; n++) {
            uint32_t a = xs[i] >> n;
            uint32_t b = xs[i] / (1u << n);
            if (a != b) { all_same = 0; }
            if (n == 1) {
                printf("    %10u >> 1 = %10u, /2 = %10u  %s\n",
                       xs[i], a, b, (a == b) ? "一致" : "不一致");
            }
        }
    }
    printf("  全部一致：%s\n", all_same ? "是" : "否");
    return all_same;
}

/* 有符号：位移和除法在负数上不一致 */
static void check_signed(void)
{
    printf("  有符号数：x >> n 与 x / 2^n 是否一致\n");
    int xs[5] = { 7, -7, 8, -8, -1 };
    for (int i = 0; i < 5; i++) {
        int a = xs[i] >> 1;
        int b = xs[i] / 2;
        printf("    %4d >> 1 = %4d, /2 = %4d  %s\n",
               xs[i], a, b, (a == b) ? "一致" : "**不一致**");
    }
    printf("  【规律】能整除时一致；不能整除时右移比除法小 1（向下 vs 向零）。\n");
}

/* 有符号：想要"向零取整的右移"要自己做修正 */
static int sar_toward_zero(int x, int n)
{
    if (x >= 0) {
        return x >> n;
    }
                /* 负数：先加偏移再右移，把向下取整修正成向零取整 */
    return (int)((x + ((1 << n) - 1)) >> n);
}

int main(void)
{
    printf("---- 1. 无符号：完全等价 ----\n");
    (void)check_unsigned();

    printf("---- 2. 有符号：负数上不等价 ----\n");
    check_signed();

    printf("---- 3. 想要向零取整的移位除法 ----\n");
    int xs[5] = { 7, -7, -8, -1, -100 };
    for (int i = 0; i < 5; i++) {
        printf("  %5d: >> 1 = %5d, 修正后 = %5d, /2 = %5d  %s\n",
               xs[i], xs[i] >> 1, sar_toward_zero(xs[i], 1), xs[i] / 2,
               (sar_toward_zero(xs[i], 1) == xs[i] / 2) ? "一致" : "**不一致**");
    }
    printf("  【建议】有符号数想要除法语义就直接写 /，别自己拼移位。\n");

    printf("---- 4. 移位与乘法的溢出风险 ----\n");
    uint32_t v = 1u;
    printf("  1u << 30 = 0x%08X（安全）\n", v << 30);
    printf("  1u << 31 = 0x%08X（无符号，安全，等于 2^31）\n", v << 31);
    printf("  【注意】有符号的 1 << 31 是 UB（见 03.4 的实测），一律用 1u。\n");
    int big = 1 << 30;
    printf("  int 1<<30 = %d；再乘 2 会溢出：\n", big);
                /* 不用真的去算 big * 2（那是 UB），而是先检查 */
    if (big > INT_MAX / 2) {
        printf("    检查到 big > INT_MAX/2，拒绝乘 2（否则就是 UB）\n");
    }

    printf("---- 5. 什么时候值得用移位 ----\n");
    printf("  a) 无符号数除以 2 的幂：编译器本来就会优化成移位，自己写只是让意图更明显；\n");
    printf("  b) Cortex-M0+ 没有硬件除法器：除法很贵，所以能改成移位就改确实有意义；\n");
    printf("  c) 但**可读性优先**：如果编译器已经能优化，就别手工拼移位；\n");
    printf("  d) 唯一必须手写的场合：循环移位、位掩码构造、协议拼拆包。\n");
    return 0;
}
