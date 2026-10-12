/**
 * ch02_ex28_storage_cmp.c —— 四种存储期的地址与生命期对照
 */
#include <stdio.h>
#include <stdlib.h>

/**
 * 文件作用域：静态存储期 + 外部链接
 */
int g_data = 0x1111;
/**
 * 文件作用域未初始化：静态存储期，放 BSS 段，保证为 0
 */
int g_bss;
/**
 * 字符串字面量：静态存储期，放只读数据段
 */
const char *g_literal = "literal in rodata";

/* 自动 vs 静态：同一个函数里放一起对照 */
static void compare_auto_and_static(int round)
{
    int auto_var = 0;
    static int static_var = 0;
    auto_var++;
    static_var++;
    printf("  第 %d 轮: auto_var 地址=%p 值=%d | static_var 地址=%p 值=%d\n",
           round, (void *)&auto_var, auto_var,
           (void *)&static_var, static_var);
}

int main(void)
{
    printf("---- 1. 静态存储期：地址固定、值跨调用保留 ----\n");
    compare_auto_and_static(1);
    compare_auto_and_static(2);
    compare_auto_and_static(3);
    printf("  auto_var 的地址每轮都一样（同一个栈帧位置），但值每次都是 1；\n");
    printf("  static_var 的地址也一样，但值在累加（1、2、3）。\n");

    printf("---- 2. 四类对象落在哪一段内存 ----\n");
    printf("  文件作用域已初始化变量 g_data     = %p  (数据段 .data)\n",
           (void *)&g_data);
    printf("  文件作用域未初始化变量 g_bss      = %p  (BSS 段，保证为 0，实测 %d)\n",
           (void *)&g_bss, g_bss);
    printf("  字符串字面量 g_literal 指向       = %p  (只读数据段 .rdata)\n",
           (void *)g_literal);
    printf("  函数里 malloc 出来的堆内存        = ");
    int *heap = (int *)malloc(sizeof(int));
    if (heap != NULL) {
        printf("%p  (堆)\n", (void *)heap);
        *heap = 0x2222;
        printf("    堆内容 = 0x%X\n", (unsigned)*heap);
        free(heap);
        heap = NULL;
    }
    printf("  函数内普通局部变量                = ");
    {
        int on_stack = 0;
        printf("%p  (栈)\n", (void *)&on_stack);
    }

    printf("---- 3. 生命期 vs 地址：最容易搞混的一点 ----\n");
    printf("  上面 auto_var 的地址在三次调用里完全一样，\n");
    printf("  但它的生命期在**每次函数返回时就结束**，下次是重新创建的对象。\n");
    printf("  \"地址相同\"只是栈帧位置重合，不代表\"还是同一个对象\"。\n");

    printf("---- 4. 各种存储期的初始化规则 ----\n");
    static int local_static;                    /* 静态存储期：没写初值就是 0 */
    int local_auto = 0;                         /* 自动存储期：必须自己初始化 */
    printf("  static 未初始化 = %d （保证为 0）\n", local_static);
    printf("  auto  显式初始化 = %d （不初始化就是不确定值）\n", local_auto);
    printf("  堆：malloc 出来的内存**不保证为 0**（calloc 才保证）。\n");
    return 0;
}
