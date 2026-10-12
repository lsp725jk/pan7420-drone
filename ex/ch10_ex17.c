/**
 * ch10_ex17.c —— 【对照实验】同一段代码在 /Od 与 /O2 下的行为差异
 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>

#define SPIN_LIMIT 400000000L

/**
 * 加 volatile 的版本 —— 无论什么优化等级都必须看到
 */
static volatile int g_flag_vol = 0;
static DWORD WINAPI isr_vol(LPVOID p) { (void)p; Sleep(80); g_flag_vol = 1; return 0; }
static long spin_vol(void)
{
    long n = 0;
    while (g_flag_vol == 0 && n < SPIN_LIMIT) n++;
    return n;
}

/* 不加 volatile 的版本 —— 优化等级决定它能不能看到 */
static int g_flag_plain = 0;
static DWORD WINAPI isr_plain(LPVOID p) { (void)p; Sleep(80); g_flag_plain = 1; return 0; }
static long spin_plain(void)
{
    long n = 0;
    while (g_flag_plain == 0 && n < SPIN_LIMIT) n++;
    return n;
}

int main(void)
{
    printf("SPIN_LIMIT = %ld ，中断在第 80ms 时置标志\n\n", SPIN_LIMIT);

    HANDLE t1 = CreateThread(NULL, 0, isr_vol, NULL, 0, NULL);
    DWORD  c0 = GetTickCount();
    long   n1 = spin_vol();
    DWORD  e1 = GetTickCount() - c0;
    WaitForSingleObject(t1, 2000); CloseHandle(t1);
    printf("[加 volatile]   自旋 %10ld 次，耗时 %4lu ms  -> %s\n",
           n1, (unsigned long)e1,
           (n1 < SPIN_LIMIT) ? "看到标志，正常退出" : "没看到（不该发生！）");

    HANDLE t2 = CreateThread(NULL, 0, isr_plain, NULL, 0, NULL);
    DWORD  c1 = GetTickCount();
    long   n2 = spin_plain();
    DWORD  e2 = GetTickCount() - c1;
    WaitForSingleObject(t2, 2000); CloseHandle(t2);
    printf("[不加 volatile] 自旋 %10ld 次，耗时 %4lu ms  -> %s\n",
           n2, (unsigned long)e2,
           (n2 < SPIN_LIMIT) ? "看到标志，正常退出（本次优化没把它提走）"
                             : "【没看到】标志在 80ms 时就被置 1 了，循环却跑满上限");

    printf("\n怎么读这个结果：\n");
    printf("  · /Od 编译时：不加 volatile 的那一行也是「看到标志」。\n");
    printf("    这就是为什么「调试版好好的」。\n");
    printf("  · /O2 编译时：不加 volatile 的那一行变成「没看到」。\n");
    printf("    这就是「Release 版卡死」。\n");
    printf("  · 加了 volatile 的那一行，两个等级都是「看到」。\n");
    printf("\n结论：不要把「调试版能跑」当成「代码是对的」。\n");
    printf("      发布前一定要用最终优化等级再测一遍；\n");
    printf("      而「中断会改的变量加 volatile」是唯一能根治这一类的办法。\n");
    return 0;
}