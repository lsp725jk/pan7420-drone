/**
 * ch02_ex10_storage_scope.c —— 存储期、作用域、链接：三个容易混的概念
 */
#include <stdio.h>
#include <stdlib.h>

int g_external = 1;                 /**
 * 文件作用域 + 静态存储期 + 外部链接
 */
static int g_internal = 2;          /**
 * 文件作用域 + 静态存储期 + 内部链接
 */

/**
 * ---- 1. 块作用域与"名字遮蔽"（shadowing）----
 */
static void shadow_demo(void)
{
    int x = 1;                              /* 外层 x */
    printf("  外层 x = %d\n", x);
    {
        int x = 99;                                 /* 内层 x，遮蔽了外层 */
        printf("  内层 x = %d  (遮蔽了外层那个)\n", x);
    }
    printf("  回到外层 x = %d  (内层那个已经销毁)\n", x);
}

/* ---- 2. 自动存储期 vs 静态存储期：地址就能看出来 ---- */
static void address_demo(void)
{
    int auto_var = 1;                               /* 自动存储期：栈 */
    static int static_var = 1;                      /* 静态存储期：数据段 */
    printf("  auto_var   地址 = %p\n", (void *)&auto_var);
    printf("  static_var 地址 = %p\n", (void *)&static_var);
                /* 同一次运行里多调几次，auto_var 地址通常不变（同一个栈帧位置）， 但它的**生命周期**在函数返回时就结束了，这是关键区别。 */
}

/* ---- 3. 动态存储期：malloc/free ---- */
static void heap_demo(void)
{
    int *p = (int *)malloc(4 * sizeof(int));            /* 在堆上要 16 字节 */
    if (p == NULL) {
        printf("  分配失败\n");
        return;
    }
    for (int i = 0; i < 4; i++) {
        p[i] = i * i;
    }
    printf("  堆上数组 = %d %d %d %d  地址 = %p\n",
           p[0], p[1], p[2], p[3], (void *)p);

                /* 【坑 1】不 free 就是内存泄漏。 【坑 2】用 malloc 的返回值前必须判 NULL。 */
    free(p);
    p = NULL;                               /* 好习惯：free 之后立刻置 NULL */

                /* 【坑 3】嵌入式里 malloc 常常是禁用的： 堆碎片+不确定性，飞控这种要求实时性的场合一般用静态数组代替。 本机（Windows）有完整的堆，所以这里可以演示。 */
}

/* ---- 4. 返回指向局部变量的指针：经典错误 ---- */
static int *dangling_pointer(void)
{
    int local = 123;
    return &local;                          /* 错！函数返回后 local 的存储期就结束了 */
}

int main(void)
{
    printf("---- 1. 块作用域 ----\n");
    shadow_demo();

    printf("---- 2. 存储期：地址与生命周期 ----\n");
    address_demo();
    address_demo();

    printf("---- 3. 动态存储期 ----\n");
    heap_demo();

    printf("---- 4. 文件作用域与链接 ----\n");
    printf("  g_external = %d （外部链接：别的 .c 用 extern 就能访问）\n",
           g_external);
    printf("  g_internal = %d （内部链接：只有本文件能访问）\n", g_internal);

    printf("---- 5. 悬空指针（危险示范）----\n");
    int *bad = dangling_pointer();
    printf("  dangling_pointer() 返回了局部变量的地址 = %p\n", (void *)bad);
    printf("  它的内容现在是未定义的，读到什么都不奇怪。\n");
    fflush(stdout);
        /* 故意读一下看会发生什么（结果不可预测，所以注释掉了）： printf("  *bad = %d\n", *bad); 取消注释后本机可能仍然打印 123（栈还没被覆盖）， 但那只是巧合 —— 这就是"幽灵 bug"的来源。 */

    printf("---- 6. 各种存储期的初始化规则 ----\n");
    static int static_zeroed;               /* 静态存储期：没写初值就是 0 */
    int auto_uninitialized;                 /* 自动存储期：没写初值就是不确定值 */
    printf("  static 未初始化 = %d （保证为 0）\n", static_zeroed);
    printf("  auto 未初始化   = ？（不打印，值不确定，读它是 UB 风险）\n");
    auto_uninitialized = 0;                 /* 想用就先赋值 */
    printf("  赋值后再用      = %d\n", auto_uninitialized);
    return 0;
}
