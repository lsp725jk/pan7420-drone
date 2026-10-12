/**
 * ch08_ex08.c —— 多语句宏的三种写法：花括号 / do-while(0) / 逗号表达式
 */
#include <stdio.h>

/**
 * 写法 A（错）：裸花括号。末尾那个分号会把它变成"一条语句 + 一个空语句"
 */
#define SWAP_A(a, b) { int t = (a); (a) = (b); (b) = t; }

/**
 * 写法 B（对）：do { } while (0)。整个宏是一条语句，末尾必须写分号
 */
#define SWAP_B(a, b) do { int t = (a); (a) = (b); (b) = t; } while (0)

/**
 * 写法 C：逗号表达式。要求 sw_t 在外面先声明好，不够独立
 */
#define SWAP_C(a, b) ((sw_t) = (a), (a) = (b), (b) = (sw_t))

int main(void)
{
                /* volatile 只是为了让编译器别把 if 条件直接算成常量（第 10 章细讲）， 对本节的宏演示没有影响 */
    volatile int x = 1, y = 2;

                /* 不用 if 时，三种写法看起来都对 —— 这正是坑难被发现的原因 */
    SWAP_A(x, y);
    printf("写法 A 单用：x = %d , y = %d\n", x, y);



    x = 1; y = 2;
    if (x < y)
        SWAP_B(x, y);                            /* 写法 B：编译通过，else 正常匹配 */
    else
        printf("不会走到这里\n");
    printf("写法 B 在 if 里：x = %d , y = %d\n", x, y);

                /* 关键差别 2：写法 A / B 都能把临时变量"锁"在宏内部的块作用域里， 写法 C 的临时变量会泄漏到外面，还要求名字不能冲突 */
    x = 5; y = 6;
    int sw_t = 0;
    SWAP_C(x, y);
    printf("写法 C 单用：x = %d , y = %d , 外泄的 sw_t = %d\n", x, y, sw_t);

                /* 变式：如果宏体里需要提前跳出，do-while(0) 反而是唯一正确的写法， 因为 break 会跳出这个"只执行一次的循环"，而不是外层真正的循环 */
    x = 7; y = 8;
    SWAP_B(x, y);
    printf("在循环/多语句场景里也安全：x = %d , y = %d\n", x, y);
    return 0;
}
