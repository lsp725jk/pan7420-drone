/**
 * ch02_ex22_promotion.c —— 整型提升与浮点提升的实际影响
 */
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>

/**
 * 用可变参数看 float 是否被提升成 double
 */
static void probe_varargs(int count, ...)
{
    va_list ap;
    va_start(ap, count);
    for (int i = 0; i < count; i++) {
                                /* 可变参数位置上 float 一定被提升为 double， 所以这里必须用 double 取，用 float 取是错的。 */
        double d = va_arg(ap, double);
        printf("    第 %d 个参数（按 double 取）= %.6f\n", i + 1, d);
    }
    va_end(ap);
}

/* 函数返回值也会做提升/转换 */
static double half_of(int x)
{
                /* 没有强制转换时，x / 2 是**整数除法**，然后才转 double。 想得到 2.5 必须先把 x 提升成浮点。 */
    return x / 2;
}

static double half_of_ok(int x)
{
    return (double)x / 2.0;
}

int main(void)
{
                /* ---- 1. 窄类型做算术都提升成 int ---- */
    printf("---- 1. 窄类型的算术都先提升成 int ----\n");
    int8_t  a8  = 100, b8 = 100;
    int16_t a16 = 30000, b16 = 30000;
    printf("int8_t  100 * 100 = %d   (sizeof=%zu，结果是 int)\n",
           a8 * b8, sizeof(a8 * b8));
    printf("int16_t 30000 + 30000 = %d   (sizeof=%zu)\n",
           a16 + b16, sizeof(a16 + b16));
    printf("所以中间结果不会提前溢出，**存回去**才会截断：\n");
    printf("  (int8_t)(100 * 100)   = %d\n", (int8_t)(a8 * b8));
    printf("  (int16_t)(30000+30000)= %d\n", (int16_t)(a16 + b16));

                /* ---- 2. 无符号窄类型提升成 int 还是 unsigned ---- */
    printf("---- 2. 无符号窄类型提升成什么 ----\n");
    uint8_t  u8  = 200;
    uint16_t u16 = 60000;
    printf("uint8_t 200 + 200 = %d（int），所以不是 144\n", u8 + u8);
    printf("uint16_t 60000 + 60000 = %d（int，因为 int 装得下 uint16_t）\n",
           u16 + u16);
    printf("【可移植性提醒】在 int 只有 16 位的机器上，\n");
    printf("uint16_t 会被提升成 unsigned int，那时 60000+60000 会回绕成 54464。\n");
    printf("同一份代码换平台结果不同 —— 跨平台代码要显式转换。\n");

                /* ---- 3. float 在可变参数里提升成 double ---- */
    printf("---- 3. 可变参数里的 float → double ----\n");
    probe_varargs(3, 1.5f, 2.25f, 3.125f);              /* 传的是 float，取的是 double */
    printf("  这就是 printf(\"%%f\", someFloat) 能正常工作的原因。\n");

                /* ---- 4. 整数除法与提升的经典组合坑 ---- */
    printf("---- 4. 除法里的提升 ----\n");
    printf("half_of(5)    = %f   <-- 错：5/2 先算成整数 2\n", half_of(5));
    printf("half_of_ok(5) = %f   <-- 对：先提升成 double\n", half_of_ok(5));
    printf("half_of(4)    = %f   （偶数看不出问题，所以这个 bug 很隐蔽）\n",
           half_of(4));
    printf("【症状】奇数输入时结果少 0.5，偶数输入时一切正常。\n");
    return 0;
}
