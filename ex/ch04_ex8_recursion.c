/**
 * ch04_ex8_recursion.c —— 递归：自己调用自己
 */
#include <stdio.h>
#include <stdint.h>

/**
 * ---- 1. 阶乘：递归的教科书例子 ----
 */
static uint64_t factorial_rec(int n)
{
    if (n <= 1) {
        return 1;                               /**
         * 基准情形
         */
    }
    return (uint64_t)n * factorial_rec(n - 1);              /* 递推 */
}

/* 同样的事情用循环做：不占栈，还能顺便检查溢出 */
static uint64_t factorial_iter(int n)
{
    uint64_t r = 1;
    for (int i = 2; i <= n; i++) {
        r *= (uint64_t)i;
    }
    return r;
}

/* ---- 2. 斐波那契：展示"朴素递归为什么慢" ---- */
static long long fib_naive(int n)
{
    if (n < 2) {
        return n;
    }
    return fib_naive(n - 1) + fib_naive(n - 2);             /* 同一个子问题被算了很多遍 */
}

/* 记忆化（memoization）：把算过的结果存起来，避免重复计算 */
static long long fib_memo_inner(int n, long long *cache)
{
    if (n < 2) {
        return n;
    }
    if (cache[n] != 0) {
        return cache[n];                        /* 已经算过，直接用 */
    }
    cache[n] = fib_memo_inner(n - 1, cache) + fib_memo_inner(n - 2, cache);
    return cache[n];
}

static long long fib_memo(int n)
{
    static long long cache[64];                     /* 静态数组，初值全 0 */
    if (n < 0 || n >= 64) {
        return -1;
    }
    return fib_memo_inner(n, cache);
}

/* ---- 3. 递归处理"嵌套结构"：这才是递归真正不可替代的地方 ---- */
/* 用递归求一个整数的各位之和：1234 -> 1+2+3+4 = 10 */
static int digit_sum(int n)
{
    if (n < 10) {
        return n;
    }
    return n % 10 + digit_sum(n / 10);
}

/* 汉诺塔：移动步骤数 = 2^n - 1，是递归思想的经典体现 */
static long long hanoi_moves(int n)
{
    if (n <= 0) {
        return 0;
    }
    return 2 * hanoi_moves(n - 1) + 1;
}

/* ---- 4. 递归的深度限制：亲眼看看栈能撑多深 ---- */
static int depth_probe(int n)
{
    volatile char pad[64];                  /* 每层吃一点栈，模拟真实函数的局部变量 */
    pad[0] = (char)n;
    if (n == 0) {
        return pad[0];
    }
    return depth_probe(n - 1) + 0;          /* 返回值用一下，防止被优化掉 */
}

int main(void)
{
    printf("---- 1. 阶乘 ----\n");
    printf("递归:  ");
    for (int i = 1; i <= 10; i++) {
        printf("%d!=%llu ", i, (unsigned long long)factorial_rec(i));
    }
    printf("\n");
    printf("循环:  ");
    for (int i = 1; i <= 10; i++) {
        printf("%d!=%llu ", i, (unsigned long long)factorial_iter(i));
    }
    printf("\n两个结果完全一样。\n");
    printf("【注意】20! = 2432902008176640000，21! 就超过 uint64_t 了（溢出）。\n");
    printf("       12! 以后其实已经超过 32 位 int 的范围。\n");

    printf("---- 2. 斐波那契：朴素递归的代价 ----\n");
    for (int i = 0; i <= 10; i++) {
        printf("fib(%d)=%lld ", i, fib_naive(i));
    }
    printf("\n");
    printf("memo 版: ");
    for (int i = 0; i <= 10; i++) {
        printf("fib(%d)=%lld ", i, fib_memo(i));
    }
    printf("\n");
    printf("fib(35) 朴素递归 = %lld（约 3000 万次函数调用，慢到能感觉到）\n",
           fib_naive(35));
    printf("fib(35) memo     = %lld（瞬间出来）\n", fib_memo(35));
    printf("原因：朴素递归把 fib(33)、fib(32)... 反复重算，复杂度是指数级的。\n");

    printf("---- 3. 递归真正擅长的：嵌套结构 ----\n");
    printf("digit_sum(1234) = %d  （1+2+3+4）\n", digit_sum(1234));
    printf("digit_sum(99999) = %d\n", digit_sum(99999));
    printf("汉诺塔 3 层要 %lld 步，10 层要 %lld 步（= 2^n - 1）\n",
           hanoi_moves(3), hanoi_moves(10));

    printf("---- 4. 递归深度与栈 ----\n");
    int depth = depth_probe(2000);
    printf("递归 2000 层成功（返回值 %d）\n", depth);
    printf("【嵌入式警告】单片机的栈通常只有 1~4 KB，\n");
    printf("  一个函数栈帧若占 32 字节，几百层就爆了。\n");
    printf("  爆栈的症状：程序跑飞、HardFault、或者某个毫不相关的变量被改。\n");
    printf("  所以嵌入式里：递归深度必须能算出来，且要留足余量。\n");

    printf("---- 5. 写递归的四步检查 ----\n");
    printf("a) 有没有基准情形？没有就是无限递归；\n");
    printf("b) 每次递归规模是否**一定**变小？\n");
    printf("c) 最大深度是多少？栈够不够？\n");
    printf("d) 有没有重复计算？有就用记忆化或改成循环。\n");
    printf("（斐波那契的朴素递归就是 c 和 d 都踩了的典型）\n");
    return 0;
}
