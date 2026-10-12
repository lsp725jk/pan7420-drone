/**
 * ch02_ex1_basic_types.c —— 基本类型、sizeof、取值范围
 */
#include <stdio.h>
#include <limits.h>
#include <float.h>

int main(void)
{
        /* ---- 1. 常见类型的 sizeof ---- 给一个数组，用"整个数组的字节数 / 一个元素的字节数"来数元素个数， 这是 sizeof 在 C 里最常用的套路。 */
    printf("---- sizeof （本机 MSVC x64）----\n");
    printf("sizeof(char)        = %zu\n", sizeof(char));
    printf("sizeof(short)       = %zu\n", sizeof(short));
    printf("sizeof(int)         = %zu\n", sizeof(int));
    printf("sizeof(long)        = %zu\n", sizeof(long));
    printf("sizeof(long long)   = %zu\n", sizeof(long long));
    printf("sizeof(float)       = %zu\n", sizeof(float));
    printf("sizeof(double)      = %zu\n", sizeof(double));
    printf("sizeof(long double) = %zu\n", sizeof(long double));
    printf("sizeof(void *)      = %zu  (指针宽度 = 地址总线宽度)\n",
           sizeof(void *));

                /* ---- 2. sizeof 的两种形态 ---- sizeof(类型)   由编译器在编译期直接给出数字 sizeof 表达式  不写括号也行，但**不会真的求值**那个表达式 */
    int arr[10];
    int x = 5;
    printf("---- sizeof 的两种形态 ----\n");
    printf("sizeof(arr)      = %zu\n", sizeof(arr));
    printf("元素个数         = %zu\n", sizeof(arr) / sizeof(arr[0]));
    printf("sizeof x         = %zu  (不写括号也可以)\n", sizeof x);
    printf("sizeof(x + 1)    = %zu  (x+1 被提升成 int,这里是 4,没有真的算)\n",
           sizeof(x + 1));
    printf("sizeof(x = 99)   = %zu  (赋值也没发生,x 还是 %d)\n",
           sizeof(x = 99), x);

                /* ---- 3. 取值范围：用标准宏，别手写数字 ---- INT_MAX 这类宏来自 <limits.h>，它们的类型保证能装下对应类型的所有值。 */
    printf("---- 取值范围 ----\n");
    printf("CHAR_BIT   = %d    (一个字节多少位)\n", CHAR_BIT);
    printf("char   : %d .. %d\n", CHAR_MIN, CHAR_MAX);
    printf("signed char  : %d .. %d\n", SCHAR_MIN, SCHAR_MAX);
    printf("unsigned char: 0 .. %u\n", (unsigned)UCHAR_MAX);
    printf("short  : %d .. %d\n", SHRT_MIN, SHRT_MAX);
    printf("ushort : 0 .. %u\n", (unsigned)USHRT_MAX);
    printf("int    : %d .. %d\n", INT_MIN, INT_MAX);
    printf("uint   : 0 .. %u\n", UINT_MAX);
    printf("llong  : %lld .. %lld\n", LLONG_MIN, LLONG_MAX);
    printf("ullong : 0 .. %llu\n", ULLONG_MAX);

        /* ---- 4. 浮点的精度和范围 ---- FLT_DIG/DBL_DIG 是"十进制下能保证的有效位数"， 这才是你真正该关心的数字：float 只有 6~7 位，double 有 15~16 位。 */
    printf("---- 浮点 ----\n");
    printf("FLT_DIG = %d  位十进制有效数字\n", FLT_DIG);
    printf("DBL_DIG = %d 位十进制有效数字\n", DBL_DIG);
    printf("FLT_MIN = %e   FLT_MAX = %e\n", (double)FLT_MIN, (double)FLT_MAX);
    printf("DBL_MIN = %e  DBL_MAX = %e\n", DBL_MIN, DBL_MAX);

                /* 用"不能精确表示的小数"体会一下精度 */
    float  f = 0.1f;
    double d = 0.1;
    printf("float  0.1 存进去再取出来 = %.10f\n", (double)f);
    printf("double 0.1 存进去再取出来 = %.20f\n", d);
    printf("float  0.1f == 0.1 吗?  %d  (0=否,1=是)\n", (f == 0.1));
    return 0;
}
