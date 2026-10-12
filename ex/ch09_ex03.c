/**
 * ch09_ex03.c —— 【坑】格式化说明符与实参类型不匹配
 */
#include <stdio.h>

int main(void)
{
                /**
         * 1) 用 %d 打印 long long：只取到低 32 位，另一半被忽略
         */
    long long big = 0x0000000100000002LL;
    printf("[错] %%d  打印 long long : %d\n", big);
    printf("[对] %%lld 打印 long long : %lld\n", big);

                /* 2) 用 %d 打印 double：double 走的是浮点寄存器，%d 却去整数寄存器取数 */
    double d = 3.5;
    printf("[错] %%d  打印 double     : %d\n", d);
    printf("[对] %%f  打印 double     : %f\n", d);

                /* 3) 用 %f 打印 int：同理，取到的是垃圾位 */
    int i = 7;
    printf("[错] %%f  打印 int        : %f\n", i);
    printf("[对] %%d  打印 int        : %d\n", i);

                /* 4) 用 %u 打印负数：位模式没变，只是解释方式变了 */
    int neg = -1;
    printf("[错] %%u  打印 -1         : %u\n", neg);
    printf("[对] %%d  打印 -1         : %d\n", neg);

                /* 5) 用 %s 打印非字符串：直接访问野地址。这里故意用一个安全的方式演示"类型不对"， 真正写错会崩溃 —— 所以不实际演示 */
    printf("[危险] 用 %%s 打印 int 会把它当地址去读，通常直接崩溃\n");

                /* 6) 变式：size_t 在 64 位平台上比 int 宽。小数值看不出问题， 数值一大就只留下低 32 位 */
    printf("[错] %%d  打印 size_t 大数 : %d\n", (size_t)0x100000002ULL);
    printf("[对] %%zu 打印 size_t 大数 : %zu\n", (size_t)0x100000002ULL);
    printf("[对] 小数值加 %%d 强转也安全: %d\n", (int)sizeof(int));
    printf("[对] 小数值用 %%zu 更规范  : %zu\n", sizeof(int));
    return 0;
}
