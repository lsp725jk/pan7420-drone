/**
 * ch02_ex35_extern_static.c —— 内部链接 vs 外部链接：同名符号会不会冲突
 */
#include <stdio.h>

/**
 * ---- 本文件的私有工具：加了 static，别的文件就算也有同名函数也不冲突 ----
 */
static int helper(int x)
{
    return x + 1;
}

static int g_local_state = 0;           /**
 * 本文件私有变量
 */

/**
 * ---- 别的文件定义的东西（只声明）----
 */
int other_helper(int x);                /* ch02_ex35_other.c 里的 helper 包装 */
int other_state_value(void);

int main(void)
{
    printf("---- 1. 两个文件各有自己的 helper ----\n");
    printf("  本文件的 helper(10) = %d\n", helper(10));
    printf("  另一个文件的 helper 包装 other_helper(10) = %d\n", other_helper(10));
    printf("  两者互不干扰 —— 因为两个 helper 都是 static（内部链接）。\n");

    printf("---- 2. 两个文件各有自己的 g_local_state ----\n");
    g_local_state = 111;
    printf("  本文件 g_local_state = %d\n", g_local_state);
    printf("  另一个文件的同名变量通过 other_state_value() 读出来 = %d\n",
           other_state_value());
    printf("  地址必然不同、值互不影响。\n");

    printf("---- 3. 如果把 static 去掉会怎样 ----\n");
    printf("  两个文件都定义 int helper(int x) 且都不加 static 时，链接会报：\n");
    printf("    error LNK2005: helper 已经在 ch02_ex35_extern_static.obj 中定义\n");
    printf("  即使两个函数**实现完全一样**，也是重复定义。\n");
    printf("  【对策】只在本文件用的函数/变量一律加 static。\n");

    printf("---- 4. 为什么工程上依赖这个特性 ----\n");
    printf("  a) 常见的函数名（init / helper / parse / dump）到处都在用，\n");
    printf("     靠 static 才能让每个文件独立命名而不撞车；\n");
    printf("  b) 暴露的符号越少，链接越快、二进制越小、越不容易误用；\n");
    printf("  c) 编译器可以据此做更好的优化（内部链接 = 调用点可见）。\n");
    return 0;
}
