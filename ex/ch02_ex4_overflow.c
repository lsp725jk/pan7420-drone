/**
 * ch02_ex4_overflow.c —— 溢出：无符号回绕 vs 有符号溢出（UB）
 */
#include <stdio.h>
#include <limits.h>
#include <stdint.h>

/**
 * 一个"看起来没问题"的溢出判断：在 UB 的世界里它是错的
 */
static int looks_safe(int x)
{
                /* 本意：判断 x+1 会不会溢出。但 x+1 本身已经 UB 了， 编译器可以假装不会溢出，于是这里的比较可能被优化成恒真。 */
    return (x + 1) > x;
}

int main(void)
{
                /* ---- 1. 无符号回绕：行为完全确定 ---- */
    printf("---- 无符号回绕（有定义）----\n");
    uint8_t  u8  = 255u;
    uint16_t u16 = 65535u;
    uint32_t u32 = 4294967295u;
    printf("uint8_t  255 + 1 = %u\n", (unsigned)(uint8_t)(u8 + 1u));
    printf("uint16_t 65535 + 1 = %u\n", (unsigned)(uint16_t)(u16 + 1u));
    printf("uint32_t 4294967295 + 1 = %u\n", u32 + 1u);
    printf("0u - 1u = %u\n", 0u - 1u);
    printf("结论：无符号的回绕是标准保证的，可以放心用来做计时器差值。\n");

                /* ---- 2. 有符号溢出：UB，只能"观察"，不能"依赖" ---- */
    printf("---- 有符号溢出（UB）----\n");
    int imax = INT_MAX;
    volatile int one = 1;                       /* volatile 阻止编译期常量折叠 */
    int over = imax + one;
    printf("INT_MAX       = %d\n", INT_MAX);
    printf("INT_MAX + 1   = %d   <-- MSVC 此处表现回绕，但语言层面是 UB\n", over);

                /* ---- 3. 用"宽类型 + 提前判断"来真正避免溢出 ---- */
    printf("---- 正确做法：先转换成更宽的类型再运算 ----\n");
    int a = INT_MAX - 5;
    int b = 100;
    int64_t wide = (int64_t)a + (int64_t)b;              /* 在 64 位里算，绝不会溢出 */
    printf("a=%d b=%d  (int)a+b 会溢出，用 int64_t 算 = %lld\n",
           a, b, (long long)wide);
    printf("再检查是否超出 int 范围，然后才转回来：%s\n",
           (wide > INT_MAX || wide < INT_MIN) ? "超出，拒绝转换" : "安全");

                /* ---- 4. 累加/平均这类循环里最常见的溢出 ---- */
    printf("---- 常见场景：求和时先累加会溢出 ----\n");
    int v[4] = { 2000000000, 2000000000, 10, 20 };
    int bad_sum = 0;
    int64_t good_sum = 0;
    for (int k = 0; k < 4; k++) {
        bad_sum  += v[k];                               /* int 累加：第 2 个元素就溢出了（UB） */
        good_sum += v[k];                               /* int64_t 累加：安全 */
    }
    printf("用 int 累加     = %d   <-- 已经错了\n", bad_sum);
    printf("用 int64_t 累加 = %lld  <-- 正确\n", (long long)good_sum);

                /* ---- 5. 顺带看一个"看似恒真"的优化陷阱 ---- */
    printf("---- 编译器的推理 ----\n");
    printf("looks_safe(INT_MAX) = %d  (本机返回 0，但它本来就是 UB 表达式)\n",
           looks_safe(INT_MAX));
    printf("looks_safe(0)       = %d\n", looks_safe(0));
    return 0;
}
