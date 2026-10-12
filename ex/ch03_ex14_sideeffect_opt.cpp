/**
 * ch03_ex14_sideeffect_opt.c —— 副作用的编译期折叠：用 /O2 对照观察
 */
#include <stdio.h>

/**
 * 用 volatile 变量让"读"这件事真的发生
 */
volatile int g_sensor = 42;

/**
 * 纯函数：编译器可以随便优化，因为它没有副作用
 */
static int square(int x)
{
    return x * x;
}

int main(void)
{
    printf("---- 1. 纯表达式：编译期就折叠了 ----\n");
    int a = 3 + 4 * 5;                      /* 编译期算成 23，运行期没有计算 */
    printf("3 + 4 * 5 = %d   （这条在编译期就算完了）\n", a);
    printf("square(7)  = %d   （/O2 下大概率直接内联成常量 49）\n", square(7));
    printf("sizeof 类运算、字符串长度、常量表的地址，也都在编译期算好。\n");

    printf("---- 2. 有副作用的表达式：不能被折叠 ----\n");
    int calls = 0;
    int r1 = (calls++, calls + 1);              /* 逗号运算符：先自增再取值 */
    printf("(calls++, calls+1) = %d, calls = %d\n", r1, calls);
    printf("  因为 calls++ 改了内存，编译器不能把它优化掉。\n");

    printf("---- 3. volatile：强制每次读都发生 ----\n");
    printf("第一次读 g_sensor = %d\n", g_sensor);
    int x1 = g_sensor;
    int x2 = g_sensor;
    printf("连读两次：%d, %d   （volatile 保证两次都真的读了内存）\n", x1, x2);
    printf("  如果 g_sensor 是普通变量，/O2 下编译器可能只读一次然后复用。\n");

    printf("---- 4. 观察优化效果的正确方法 ----\n");
    printf("  a) 用 /Od 和 /O2 各编译一次，比较输出是否一致；\n");
    printf("  b) 想看机器码：cl /O2 /FA ch03_ex14_sideeffect_opt.c 生成 .asm；\n");
    printf("  c) 想看优化决策：MSVC 的 /Qvec-report、/d1reportAllClassLayout 等；\n");
    printf("  d) 性能测量必须用 Release 版 —— Debug 版的耗时没有参考价值。\n");

    printf("---- 5. 编译期能算 vs 必须运行期算 ----\n");
#define COMPILE_TIME_CONST (2 * 3 * 4 * 5)
    printf("  #define 的常量表达式   = %d（编译期）\n", COMPILE_TIME_CONST);
    printf("  g_sensor + 1              = %d（运行期，因为有 volatile 读）\n",
           g_sensor + 1);
    printf("  字符串字面量的长度        = %zu（编译期，sizeof 不产生运行期代码）\n",
           sizeof("hello"));
    return 0;
}
