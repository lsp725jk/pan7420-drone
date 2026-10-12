/**
 * ch09_ex01.c —— printf 的格式化说明符（conversion specification）总览
 */
#include <stdio.h>
#include <stddef.h>

int main(void)
{
    char                c  = 'A';
    short               h  = -2;
    int                 i  = -123;
    unsigned int        u  = 4000000000u;
    long                l  = -123456789L;
    long long           ll = 1234567890123LL;
    unsigned long long  ull = 18446744073709551615ULL;
    float               f  = 3.14159f;
    double              d  = 2.718281828459045;
    long double         ld = 1.5L;
    size_t              sz = sizeof(int);
    const char         *s  = "hello";
    int                *p  = &i;

    printf("有符号十进制  %%d   = %d\n", i);
    printf("无符号十进制  %%u   = %u\n", u);
    printf("十六进制      %%x   = %x , 大写 %%X = %X\n", u, u);
    printf("八进制        %%o   = %o\n", 64);
    printf("字符          %%c   = %c\n", c);
    printf("字符串        %%s   = %s\n", s);
    printf("百分号本身    %%%%   = %%\n");
    printf("指针          %%p   = %p\n", (void *)p);

                /* 长度修饰符（length modifier）：告诉 printf "这个实参多宽" */
    printf("短整型        %%hd  = %hd\n", h);
    printf("长整型        %%ld  = %ld\n", l);
    printf("长长整型      %%lld = %lld\n", ll);
    printf("无符号长长    %%llu = %llu\n", ull);
    printf("size_t        %%zu  = %zu\n", sz);

                /* 浮点：%f 定点、%e 科学计数、%g 自动挑短的 */
    printf("浮点 %%-10.4f = %-10.4f|\n", f);
    printf("科学 %%-14.6e = %-14.6e|\n", d);
    printf("自动 %%-10.6g = %-10.6g|\n", d);
    printf("double        = %.15f\n", d);
    printf("long double   = %Lf\n", ld);

                /* 变式：字段宽度可以用 * 从参数里取，中英文对齐时很有用 */
    printf("宽度来自参数  %%*d = [%*d]\n", 8, 42);
    return 0;
}
