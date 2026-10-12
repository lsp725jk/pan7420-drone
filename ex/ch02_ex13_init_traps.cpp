/**
 * ch02_ex13_init_traps.c —— 初始化的四个坑：溢出、未初始化、数组长度、数字分隔
 */
#include <stdio.h>
#include <limits.h>

/**
 * 把 INT_MIN 取负是 UB（-(-2147483648) 超出 int 范围），这里演示"取绝对值"的坑
 */
static int abs_bad(int x)
{
    return x < 0 ? -x : x;                  /* x == INT_MIN 时是 UB */
}

static long long abs_good(int x)
{
                /* 先转成更宽的类型再取负，就不会溢出。 注意返回类型也必须是 long long：INT_MIN 的绝对值是 2147483648， 它**超出 int 的范围**，用 int 返回照样是错的。 */
    long long v = x;
    return (v < 0 ? -v : v);
}

int main(void)
{
                /* ---- 1. 求最大值/最小值时的初始化坑 ---- */
    printf("---- 1. 求最大值的初始化 ----\n");
    int data[5] = { -3, -7, -1, -9, -5 };              /* 全是负数 */
    int bad_max = 0;                                   /* 错：用 0 初始化，结果永远是 0 */
    int good_max = INT_MIN;                            /* 对：用该类型的最小值初始化 */
    for (int i = 0; i < 5; i++) {
        if (data[i] > bad_max)  { bad_max  = data[i]; }
        if (data[i] > good_max) { good_max = data[i]; }
    }
    printf("用 0 初始化     -> max = %d   <-- 错了，负数永远不会大于 0\n", bad_max);
    printf("用 INT_MIN 初始化 -> max = %d   <-- 正确\n", good_max);
    printf("（求最小值就用 INT_MAX 初始化，对称的。）\n");

                /* ---- 2. 未初始化 + 取绝对值溢出 ---- */
    printf("---- 2. 未初始化与 INT_MIN ----\n");
    int uninit;                            /* 没初始化：值不确定 */
    uninit = 0;                            /* 演示代码里立刻赋值，避免真读未初始化值 */
    printf("未初始化的自动变量不打印（值是垃圾，本机 /Od 下常是 0xCCCCCCCC）\n");
    printf("INT_MIN = %d\n", INT_MIN);
    printf("abs_bad(INT_MIN)  = %d  <-- 已经是 UB（结果不可依赖）\n",
           abs_bad(INT_MIN));
    printf("abs_good(INT_MIN) = %lld  <-- 先转 long long 再取负，安全\n",
           abs_good(INT_MIN));

                /* ---- 3. 数组长度必须编译期常量 ---- */
    printf("---- 3. 数组长度 ----\n");
#define N 4
    enum { M = 5 };                        /* enum 常量也是编译期常量 */
    int a1[N];                             /* 宏：可以 */
    int a2[M];                             /* enum：可以 */
    int a3[4];                             /* 字面量：可以 */
    printf("sizeof(a1)=%zu sizeof(a2)=%zu sizeof(a3)=%zu  —— 三种都合法\n",
           sizeof(a1), sizeof(a2), sizeof(a3));

    printf("用 const int 当数组长度会报 error C2057: 应输入常量表达式（MSVC 不支持 VLA）\n");

                /* ---- 4. 元素个数只能靠 sizeof 求 ---- */
    printf("---- 4. 数组元素个数 ----\n");
    int arr[7] = { 1, 2, 3, 4, 5, 6, 7 };
    printf("sizeof(arr) = %zu, sizeof(arr[0]) = %zu\n",
           sizeof(arr), sizeof(arr[0]));
    printf("元素个数 = %zu   <-- 标准写法\n", sizeof(arr) / sizeof(arr[0]));
    printf("sizeof(arr)/sizeof(int) 也行，但把类型写死了，不推荐\n");
    printf("【坑】这个技巧在函数里失效：数组作参数会退化成指针，sizeof 得到 8\n");
    return 0;
}
