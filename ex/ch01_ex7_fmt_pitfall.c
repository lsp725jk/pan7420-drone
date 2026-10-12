/**
 * ch01_ex7_fmt_pitfall.c —— 格式串不匹配：编译期可能只有警告，运行期就出事
 */
#include <stdio.h>

int main(void)
{
    printf("---- 1. 正确写法 ----\n");
    printf("int 用 %%d: %d\n", 42);
    printf("double 用 %%f: %f\n", 3.5);

    printf("---- 2. 类型不匹配（本文件故意保留，编译时会看到警告）----\n");
                /* 下面两行会触发 warning C4477 / C4473 一类的提示。 输出是什么完全不可预测（值来自栈/寄存器的错误解释）， 所以这里只写注释，不真的跑它们。 */
                /* printf("%d\n", 3.5);     错：%d 要 int，却给了 double */
                /* printf("%f\n", 3);       错：%f 要 double，却给了 int */
    printf("  %%d 配 double、%%f 配 int 都是错的，编译器会给 C4477 之类的警告。\n");
    printf("  【症状】打印出 0、-1073741824、1.2e-308 这类莫名其妙的数字。\n");

    printf("---- 3. 用条件编译演示能编过但结果不对 ----\n");
                /* 同一个 printf，参数类型由宏决定：两种配置都能编译， 但从输出就能看出哪种是对的。 */
#define USE_CORRECT_FMT 1
    double ratio = 3.5;
#if USE_CORRECT_FMT
    printf("  USE_CORRECT_FMT=1: printf(\"%%f\", 3.5)     -> %f\n", ratio);
#else
    printf("  USE_CORRECT_FMT=0: printf(\"%%d\", 3.5)     -> %d（错误解释）\n",
           ratio);
#endif
    printf("  想亲眼看错误版本，把 USE_CORRECT_FMT 改成 0 重新编译。\n");

    printf("---- 4. 长度修饰符也不能错 ----\n");
    long long big = 1234567890123LL;
    printf("  long long 必须用 %%lld: %lld\n", big);
    printf("  用 %%d 打印 long long 会只读到低 32 位，得到错误的值。\n");
    return 0;
}
