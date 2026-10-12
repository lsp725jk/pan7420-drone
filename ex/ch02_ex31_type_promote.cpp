/**
 * ch02_ex31_type_promote.c —— 类型提升的可移植性：int 宽度不同的影响
 */
#include <stdio.h>
#include <stdint.h>
#include <limits.h>

/**
 * 可移植的"求和"：先统一转成 int64_t，结果与 int 宽度无关
 */
static int64_t sum_portable(const uint16_t *v, int n)
{
    int64_t total = 0;
    for (int i = 0; i < n; i++) {
        total += (int64_t)v[i];
    }
    return total;
}

/* 直接用 uint16_t 相加：在 16 位 int 的平台上会提升成 unsigned int（16 位）而回绕 */
static uint32_t sum_native(const uint16_t *v, int n)
{
    uint32_t total = 0;
    for (int i = 0; i < n; i++) {
        total += v[i];                              /* 提升规则依平台而变 */
    }
    return total;
}

int main(void)
{
    printf("---- 1. 本机的平台参数 ----\n");
    printf("  CHAR_BIT  = %d\n", CHAR_BIT);
    printf("  INT_MAX   = %d\n", INT_MAX);
    printf("  sizeof(int)      = %zu\n", sizeof(int));
    printf("  sizeof(uint16_t) = %zu\n", sizeof(uint16_t));
    printf("  本机 int 是 %d 位，能装下 uint16_t 的全部取值。\n",
           (int)(sizeof(int) * CHAR_BIT));
    printf("  【推论】所以本机 uint16_t 参与运算时提升成 **int**（有符号）。\n");
    printf("  如果 int 只有 16 位，就会提升成 unsigned int —— 结果可能不同！\n");

    printf("---- 2. 两种求和在极端数据上的差别 ----\n");
    uint16_t v[4] = { 60000, 60000, 60000, 60000 };
    printf("  数据: 4 个 60000（总和应为 240000）\n");
    printf("  sum_native   = %u   （本机正确，因为提升成 32 位 int）\n", sum_native(v, 4));
    printf("  sum_portable = %lld  （显式 int64_t，任何平台都对）\n",
           (long long)sum_portable(v, 4));
    printf("  【重点】在 16 位 int 的平台上，sum_native 会在第二次加法时就回绕，\n");
    printf("         得到完全不同的结果 —— 这就是可移植性陷阱。\n");

    printf("---- 3. 单个数相加时的差别 ----\n");
    uint16_t a = 60000, b = 60000;
    printf("  uint16_t 60000 + 60000 用 %%d 打印 = %d\n", a + b);
    printf("    （本机提升成 int，得到 120000）\n");
    printf("  如果写成 (uint16_t)(a + b) 会截断成 %u\n", (unsigned)(uint16_t)(a + b));
    printf("  【写法建议】两种意图要写清楚：\n");
    printf("    · 想要数学上的和       -> 先转 int32_t/int64_t；\n");
    printf("    · 想要模 2^16 的和     -> 显式写 (uint16_t)(a + b)；\n");
    printf("    含糊地写 a + b 会让不同平台给出不同结果。\n");

    printf("---- 4. 用宏判断平台能力 ----\n");
#if INT_MAX == 32767
    printf("  本机 int 是 16 位（INT_MAX = 32767）\n");
#elif INT_MAX == 2147483647
    printf("  本机 int 是 32 位（INT_MAX = 2147483647）\n");
#else
    printf("  本机 int 是别的宽度：INT_MAX = %d\n", INT_MAX);
#endif
    printf("  【工程做法】需要至少 N 位时用 int_leastN_t；\n");
    printf("             需要恰好 N 位时用 intN_t（本机都有）。\n");
    return 0;
}