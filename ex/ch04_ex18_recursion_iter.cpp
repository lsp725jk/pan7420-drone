/**
 * ch04_ex18_recursion_iter.c —— 递归改迭代：阶乘、斐波那契、以及"何时必须递归"
 */
#include <stdio.h>
#include <stdint.h>

/**
 * ---- 1. 阶乘：递归 vs 迭代 vs 带溢出检查的迭代 ----
 */
static uint64_t fact_rec(int n)
{
    if (n <= 1) {
        return 1;
    }
    return (uint64_t)n * fact_rec(n - 1);
}

static uint64_t fact_iter(int n)
{
    uint64_t r = 1;
    for (int i = 2; i <= n; i++) {
        r *= (uint64_t)i;
    }
    return r;
}

/* 迭代版的优势：可以在每一步检查溢出 */
static int fact_iter_checked(int n, uint64_t *out)
{
    uint64_t r = 1;
    for (int i = 2; i <= n; i++) {
        if (r > UINT64_MAX / (uint64_t)i) {
            return -1;                                          /* 会溢出，拒绝计算 */
        }
        r *= (uint64_t)i;
    }
    *out = r;
    return 0;
}

/* ---- 2. 斐波那契：三种写法 ---- */
static long long fib_rec(int n)         /* 指数级，只用来对照 */
{
    if (n < 2) {
        return n;
    }
    return fib_rec(n - 1) + fib_rec(n - 2);
}

static long long fib_iter(int n)        /* O(n) 时间、O(1) 空间 */
{
    long long a = 0, b = 1;
    for (int i = 0; i < n; i++) {
        long long t = a + b;
        a = b;
        b = t;
    }
    return a;
}

static long long fib_memo(int n)        /* 记忆化：O(n) 时间、O(n) 空间 */
{
    static long long cache[64];
    if (n < 0 || n >= 64) {
        return -1;
    }
    if (n < 2) {
        return n;
    }
    if (cache[n] != 0) {
        return cache[n];
    }
    cache[n] = fib_memo(n - 1) + fib_memo(n - 2);
    return cache[n];
}

/* ---- 3. 什么时候必须递归：处理嵌套结构 ---- */
/* 用"自己维护栈"的方式做十进制位数求和（演示"递归能改迭代"） */
static int digit_sum_rec(uint32_t n)
{
    if (n < 10) {
        return (int)n;
    }
    return (int)(n % 10) + digit_sum_rec(n / 10);
}

static int digit_sum_iter(uint32_t n)
{
    int sum = 0;
    while (n >= 10) {
        sum += (int)(n % 10);
        n /= 10;
    }
    return sum + (int)n;
}

/* ---- 4. 递归深度受限的场景：显式限制层数 ---- */
static int depth_limited(int n, int depth)
{
    if (depth > 8) {
        return -1;                                      /* 主动限制：超过 8 层就放弃 */
    }
    if (n <= 1) {
        return n;
    }
    return depth_limited(n - 1, depth + 1) + depth_limited(n - 2, depth + 1);
}

int main(void)
{
    printf("---- 1. 阶乘：递归 / 迭代 / 带检查的迭代 ----\n");
    for (int i = 1; i <= 10; i += 3) {
        printf("  %2d!: rec=%llu  iter=%llu\n", i,
               (unsigned long long)fact_rec(i),
               (unsigned long long)fact_iter(i));
    }
    uint64_t v = 0;
    printf("  fact_iter_checked(20, &v) = %d, v = %llu\n",
           fact_iter_checked(20, &v), (unsigned long long)v);
    printf("  fact_iter_checked(21, &v) = %d  <-- 拒绝，因为 21! 超过 uint64_t\n",
           fact_iter_checked(21, &v));
    printf("  【关键】递归版没法方便地做这个检查，迭代版可以。\n");

    printf("---- 2. 斐波那契：三种写法结果一致 ----\n");
    for (int i = 0; i <= 10; i++) {
        printf("  fib(%2d): rec=%lld iter=%lld memo=%lld\n",
               i, fib_rec(i), fib_iter(i), fib_memo(i));
    }
    printf("  fib_iter(50) = %lld  （瞬间出来，O(1) 空间）\n", fib_iter(50));
    printf("  fib_iter(90) = %lld  <-- 接近 long long 上限，注意溢出\n",
           fib_iter(90));
    printf("  注意：fib_iter 不需要栈，也不需要缓存数组。\n");

    printf("---- 3. 递归改迭代：位数求和 ----\n");
    uint32_t nums[3] = { 1234u, 99999u, 0u };
    for (int i = 0; i < 3; i++) {
        printf("  digit_sum(%u): rec=%d iter=%d\n", nums[i],
               digit_sum_rec(nums[i]), digit_sum_iter(nums[i]));
    }
    printf("  两者结果完全一致，但迭代版不占栈。\n");

    printf("---- 4. 主动限制递归深度 ----\n");
    printf("  depth_limited(5, 0)  = %d\n", depth_limited(5, 0));
    printf("  depth_limited(12, 0) = %d  <-- 递归太深，中途碰到深度上限就返回 -1\n",
           depth_limited(12, 0));
    printf("  但请注意：这个 -1 是**最深层**返回的，它会沿着调用链传上来\n");
    printf("  （上面的 23 就是「部分结果 + 一个 -1」混在一起的产物）。\n");
    printf("  真实代码里遇到 -1 必须**立刻整体放弃**，不能用带 -1 的结果：\n");
    {
        int r = depth_limited(12, 0);
        if (r < 0) {
            printf("    -> 检测到 r < 0，拒绝使用这个结果，改为返回错误码\n");
        }
    }
    printf("  这是嵌入式里的实用技巧：**给递归加深度上限**，\n");
    printf("  宁可返回错误也不让栈溢出；但调用方必须把错误**向上传播**。\n");

    printf("---- 5. 选择建议 ----\n");
    printf("  a) 能用循环就用循环：不占栈、可加溢出检查、好调试；\n");
    printf("  b) 需要重复子问题：用记忆化 + 循环（或带缓存的递归）；\n");
    printf("  c) 处理嵌套/树形结构：用递归，但**必须限制深度**；\n");
    printf("  d) 单片机（栈 1~4KB）：递归深度 >10 就要重新考虑设计。\n");
    return 0;
}
