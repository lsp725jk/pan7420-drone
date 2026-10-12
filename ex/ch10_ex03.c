/**
 * ch10_ex03.c —— 【本章最关键的一例】volatile：加不加，结果真的不一样
 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>

#define SPIN_LIMIT 400000000L        /**
 * 有限次自旋，防止演示时真的挂死
 */

/**
 * ==================== 版本 1：共享标志【不加】volatile ====================
 */
static int g_flag_plain = 0;                 /* "中断"会把它置 1 */

static DWORD WINAPI isr_plain(LPVOID p)
{
    (void)p;
    Sleep(100);                                      /* 模拟"100ms 后中断发生" */
    g_flag_plain = 1;                                /* 中断里置标志 */
    return 0;
}

static long spin_plain(void)
{
    long n = 0;
    while (g_flag_plain == 0 && n < SPIN_LIMIT) n++;           /* 编译器可以把读操作提到循环外 */
    return n;
}

/* ==================== 版本 2：共享标志【加了】volatile ==================== */
static volatile int g_flag_vol = 0;          /* volatile：每次都重新从内存读 */

static DWORD WINAPI isr_vol(LPVOID p)
{
    (void)p;
    Sleep(100);
    g_flag_vol = 1;
    return 0;
}

static long spin_vol(void)
{
    long n = 0;
    while (g_flag_vol == 0 && n < SPIN_LIMIT) n++;
    return n;
}

int main(void)
{
    printf("SPIN_LIMIT = %ld\n\n", SPIN_LIMIT);

                /* ---------- 版本 1 ---------- */
    HANDLE t1 = CreateThread(NULL, 0, isr_plain, NULL, 0, NULL);
    DWORD  c0 = GetTickCount();
    long   n1 = spin_plain();
    DWORD  e1 = GetTickCount() - c0;
    WaitForSingleObject(t1, 2000);
    CloseHandle(t1);
    printf("不加 volatile：自旋 %ld 次，耗时 %lu ms，最后 g_flag_plain = %d\n",
           n1, (unsigned long)e1, g_flag_plain);
    printf("  -> %s\n\n",
           (n1 >= SPIN_LIMIT) ? "【没看到】标志其实在约 100ms 时就被置 1 了，\n     但这个循环一直跑到自旋上限才停 —— 读操作被提到了循环外。"
                              : "正常看到标志变化并退出");

                /* ---------- 版本 2 ---------- */
    HANDLE t2 = CreateThread(NULL, 0, isr_vol, NULL, 0, NULL);
    DWORD  c1 = GetTickCount();
    long   n2 = spin_vol();
    DWORD  e2 = GetTickCount() - c1;
    WaitForSingleObject(t2, 2000);
    CloseHandle(t2);
    printf("加了 volatile：自旋 %ld 次，耗时 %lu ms，最后 g_flag_vol = %d\n",
           n2, (unsigned long)e2, g_flag_vol);
    printf("  -> %s\n\n",
           (n2 < SPIN_LIMIT) ? "【看到了】标志一置 1 就退出，耗时约等于那 100ms 的等待"
                             : "仍然没看到？");

    printf("结论：\n");
    printf("  · 中断（或另一个执行流）会改的变量，必须加 volatile；\n");
    printf("  · 不加 volatile，编译器认为「只有我能改它」，于是把值缓存在寄存器里，\n");
    printf("    主循环永远读不到新值 —— 这就是「程序像卡住了」的头号原因；\n");
    printf("  · 在 Cortex-M0+ 上的真实后果见 ch10_arm02_volatile.c 的反汇编：\n");
    printf("    不加 volatile 时，读取被提到了循环外，循环体变成一条无条件跳转指令。\n");
    return 0;
}
