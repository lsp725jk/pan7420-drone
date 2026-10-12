/**
 * ch09_ex20.c —— setjmp.h：C 里唯一的"跨函数跳转"
 */
#include <stdio.h>
#include <setjmp.h>

static jmp_buf g_env;
static int     g_depth = 0;

static void level3(void)
{
    printf("    level3: 发现致命错误，longjmp 回 main\n");
    longjmp(g_env, 99);                               /* 第二参数是"错误码"，会变成 setjmp 的返回值 */
                /* longjmp 不会返回，所以这后面的代码编译器会标成"无法访问" */
}

static void level2(void) { g_depth++; level3(); g_depth--; printf("level2 之后\n"); }
static void level1(void) { g_depth++; level2(); g_depth--; printf("level1 之后\n"); }

int main(void)
{
                /* 1) 基本用法：setjmp 返回两次 */
    int code = setjmp(g_env);
    if (code == 0) {
        printf("1) setjmp 第一次返回 0，进入正常流程（g_depth = %d）\n", g_depth);
        level1();
        printf("   正常情况下这里会执行\n");
    } else {
        printf("1) 从 longjmp 跳回来了，返回值 = %d ，g_depth = %d\n", code, g_depth);
    }

                /* 2) 变式：把 longjmp 当"异常"用 —— 深层错误直接跳到统一处理点 */
    printf("\n2) 用 setjmp/longjmp 做统一错误出口：\n");
    if (setjmp(g_env) == 0) {
        printf("   进入解析流程\n");
        level3();                                              /* 深层出错 */
    } else {
        printf("   回到统一错误处理点，释放资源、复位状态，然后继续跑\n");
    }

                /* 3) 【坑】非 volatile 的局部变量，在 longjmp 之后的值是不确定的 */
    printf("\n3) 局部变量的坑：\n");
    {
        int          plain = 1;                             /* 不是 volatile，标准说值"不确定" */
        volatile int safe  = 1;                             /* volatile，标准保证保留 */
        if (setjmp(g_env) == 0) {
            plain = 2;
            safe  = 2;
            longjmp(g_env, 1);
        }
        printf("   plain = %d , safe = %d\n", plain, safe);
        printf("   标准规定 plain 的值不确定（本机实测恰好是 2，但换编译器/开优化就可能变回 1）\n");
    }

                /* 4) 【坑】不能 longjmp 到一个"已经返回的函数"里的 jmp_buf， 也不能靠它跳过带清理逻辑的作用域（C++ 里更是会跳过析构函数）。 */
    printf("\n4) 限制：setjmp 只能在同一函数的一次激活内使用，函数返回后再 longjmp 就是 UB\n");

        /* 5) 嵌入式里的典型用法（原理示意）：在 main 里 setjmp， HardFault / 断言失败时 longjmp 回来做"软复位"， ——但要注意：longjmp 不会恢复外设寄存器，真正靠谱的做法还是 "看门狗复位"，见第 10 章。 */
    printf("\n5) 嵌入式提醒：longjmp 只恢复 CPU 的寄存器上下文，不恢复外设状态；\n");
    printf("   所以工程里更常用看门狗复位，而不是 longjmp 兜底\n");
    return 0;
}
