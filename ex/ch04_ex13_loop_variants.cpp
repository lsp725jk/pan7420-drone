/**
 * ch04_ex13_loop_variants.c —— 循环的替代写法与一个真实的性能陷阱
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>

/**
 * 正着数的循环
 */
static int sum_forward(int n)
{
    int s = 0;
    for (int i = 0; i < n; i++) {
        s += i;
    }
    return s;
}

/* 倒着数的循环：结束条件是"i != 0"，省掉了每次和 n 的比较 */
static int sum_backward(int n)
{
    int s = 0;
    for (int i = n; i != 0; i--) {
        s += (i - 1);
    }
    return s;
}

int main(void)
{
    printf("---- 1. 正着数与倒着数：结果必须一致 ----\n");
    for (int n = 0; n <= 5; n++) {
        printf("  n=%d: forward=%2d backward=%2d\n",
               n, sum_forward(n), sum_backward(n));
    }
    printf("  两种写法结果相同。倒着数在 ARM 上可以少一条比较指令，\n");
    printf("  但可读性略差 —— 只在热点循环里这么做。\n");

    printf("---- 2. 循环里别重复算同样的东西 ----\n");
    const char *msg = "hello, embedded world";
    int total = 0;
                /* 反例写法：每次都调 strlen（O(n) 的循环里套 O(n) 的函数 => O(n^2)） */
    for (int i = 0; i < (int)strlen(msg); i++) {
        total += msg[i];
    }
    printf("  反例（每次调 strlen）: total = %d\n", total);

                /* 正例写法：长度算一次，存起来 */
    size_t len = strlen(msg);
    total = 0;
    for (size_t i = 0; i < len; i++) {
        total += msg[i];
    }
    printf("  正例（先算长度）    : total = %d   len = %zu\n", total, len);
    printf("  【注意】现代优化器多半能自动把 strlen 提到循环外（因为它是纯函数），\n");
    printf("  但那属于优化器的好心，不该当成设计依据。\n");

    printf("---- 3. sizeof 与 strlen 在循环里的区别 ----\n");
    char buf[32] = "abc";
    printf("  sizeof(buf) = %zu（整个数组，编译期常量）\n", sizeof(buf));
    printf("  strlen(buf) = %zu（到 '\\0' 为止，运行期要遍历）\n", strlen(buf));
    printf("  遍历定长数组用 sizeof，遍历字符串用 strlen（且只算一次）。\n");

    printf("---- 4. volatile 计数器：做粗略延时的笨办法 ----\n");
    volatile uint32_t spin = 0;
    for (uint32_t i = 0; i < 1000; i++) {
        spin++;                                 /* volatile 保证循环不会被整个删掉 */
    }
    printf("  volatile 计数循环跑完了，spin = %u\n", (unsigned)spin);
    printf("  【为什么不推荐】延时时长与主频、优化等级、编译器都有关，无法预测；\n");
    printf("  正规做法是用硬件定时器或 _delay_loop 之类的库函数。\n");
    printf("  但如果只是等一小会儿又不想引入定时器，volatile 计数是最省事的。\n");
    return 0;
}
