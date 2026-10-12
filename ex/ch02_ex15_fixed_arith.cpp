/**
 * ch02_ex15_fixed_arith.c —— 定长类型做算术：先升宽再截断
 */
#include <stdio.h>
#include <stdint.h>

/**
 * 把一个可能超出范围的值饱和到 [lo, hi]，全程用宽类型比较
 */
static uint16_t saturate_u16(int32_t v, int32_t lo, int32_t hi)
{
    if (v < lo) { return (uint16_t)lo; }
    if (v > hi) { return (uint16_t)hi; }
    return (uint16_t)v;
}

/* 这就是"舵机输出"里最常见的限幅：输入可能是超范围的 */
static uint16_t pwm_clamp(int32_t raw)
{
    return saturate_u16(raw, 1000, 2000);           /* 1000~2000us */
}

int main(void)
{
                /* ---- 1. 表达式的类型由"提升后"决定 ---- */
    printf("---- 1. 提升让中间结果变大 ----\n");
    uint8_t a = 200, b = 200;
    printf("uint8_t 200 + 200 = %d   <-- 结果是 int，不是 8 位的 144\n", a + b);
    printf("sizeof(a + b) = %zu   <-- 4 字节，int\n", sizeof(a + b));
    printf("但存回 uint8_t 就截断了：\n");
    uint8_t c = (uint8_t)(a + b);
    printf("(uint8_t)(200 + 200) = %u   (400 & 0xFF = 144)\n", (unsigned)c);

                /* ---- 2. 累加超范围时，要么升宽要么饱和 ---- */
    printf("---- 2. 累加与饱和 ----\n");
    uint16_t acc = 65000;                       /* 接近 uint16_t 上限 */
    printf("uint16_t acc 初始 = %u\n", (unsigned)acc);
    printf("acc 再加 5000，先用 32 位算 = %d\n", (int)acc + 5000);
    printf("硬塞回 uint16_t            = %u   (70000 超出 65535，回绕成 4464)\n",
           (unsigned)(uint16_t)((int)acc + 5000));
    printf("饱和处理（限幅到上限）      = %u   <-- 这才是嵌入式里要的行为\n",
           (unsigned)saturate_u16((int32_t)acc + 5000, 0, 65535));

                /* ---- 3. 限幅（饱和）在嵌入式里到处都要用 ---- */
    printf("---- 3. 限幅 ----\n");
    int32_t tests[6] = { 500, 999, 1000, 1500, 2001, 8000 };
    for (int i = 0; i < 6; i++) {
        printf("  输入 %5d -> 限幅后 %u us\n",
               tests[i], (unsigned)pwm_clamp(tests[i]));
    }
    printf("注意 pwm_clamp 的参数是 int32_t：如果用 uint16_t 接 8000 也没事，\n");
    printf("但如果上游算出负数（比如 -200），用无符号接就会变成一个巨大的正数，\n");
    printf("限幅逻辑就彻底失效了。**限幅一律用有符号宽类型做。**\n");

                /* ---- 4. 有符号/无符号混用的定长类型 ---- */
    printf("---- 4. int8_t 与 uint8_t 混算 ----\n");
    int8_t  s = -5;
    uint8_t u = 3;
                /* 两者都会提升成 int，所以结果是 -2，不会出怪事 */
    printf("int8_t(-5) + uint8_t(3) = %d   (都提升成 int 再算)\n", s + u);
                /* 但如果把结果直接存进 uint8_t，-2 会变成 254 */
    uint8_t r = (uint8_t)(s + u);
    printf("(uint8_t)(-5 + 3) = %u   <-- 254，这就是负数变巨大正数\n", (unsigned)r);
    printf("所以：无符号类型里**不要**放可能为负的中间结果。\n");
    return 0;
}
