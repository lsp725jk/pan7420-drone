/**
 * ch10_ex04.c —— volatile【不】保证原子性：x++ 会丢更新
 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>

#define ROUNDS 2000000L

/**
 * ---------- 版本 1：只加 volatile（很多人的第一反应） ----------
 */
static volatile long g_counter_vol = 0;

static DWORD WINAPI inc_vol(LPVOID p)
{
    (void)p;
    for (long i = 0; i < ROUNDS; i++) g_counter_vol++;           /* 读-改-写，三步 */
    return 0;
}

/* ---------- 版本 2：用临界区把它变成不可打断的一整段（相当于关中断） ---------- */
static volatile long g_counter_cs = 0;
static CRITICAL_SECTION g_cs;

static DWORD WINAPI inc_cs(LPVOID p)
{
    (void)p;
    for (long i = 0; i < ROUNDS; i++) {
        EnterCriticalSection(&g_cs);                      /* 真机上的对应物：__disable_irq() */
        g_counter_cs++;
        LeaveCriticalSection(&g_cs);                      /* __enable_irq() */
    }
    return 0;
}

static long run_two_threads(LPTHREAD_START_ROUTINE fn)
{
    HANDLE t1 = CreateThread(NULL, 0, fn, NULL, 0, NULL);
    HANDLE t2 = CreateThread(NULL, 0, fn, NULL, 0, NULL);
    WaitForSingleObject(t1, INFINITE);
    WaitForSingleObject(t2, INFINITE);
    CloseHandle(t1);
    CloseHandle(t2);
    return 0;
}

int main(void)
{
    InitializeCriticalSection(&g_cs);

    printf("期望值 = 2 * %ld = %ld\n\n", ROUNDS, 2 * ROUNDS);

                /* ---------- 只用 volatile ---------- */
    g_counter_vol = 0;
    run_two_threads(inc_vol);
    printf("只加 volatile      ：结果 = %ld ，丢了 %ld 次\n",
           g_counter_vol, 2 * ROUNDS - g_counter_vol);
    printf("  -> volatile 只保证「每次都去内存读/写」，不保证「读-改-写」不被打断。\n\n");

                /* ---------- 加临界区 ---------- */
    g_counter_cs = 0;
    DWORD c0 = GetTickCount();
    run_two_threads(inc_cs);
    DWORD e0 = GetTickCount() - c0;
    printf("加临界区（关中断）：结果 = %ld ，丢了 %ld 次，耗时 %lu ms\n",
           g_counter_cs, 2 * ROUNDS - g_counter_cs, (unsigned long)e0);
    printf("  -> 正确，但代价是每次自增都要进出临界区（真机上是几十个周期）。\n\n");

    printf("三条实用结论：\n");
    printf("  1) 共享变量：加 volatile 解决「看不见」，加临界区解决「丢更新」，两件事都要做；\n");
    printf("  2) 更好的办法是【单写者约定】：只让中断写、主循环只读；\n");
    printf("     或者反过来，主循环写、中断只读 —— 只有一方写就不会丢更新；\n");
    printf("  3) 位数大于 CPU 字长的量（比如 64 位计数器）即使只读也会被「撕裂」，\n");
    printf("     读的时候同样要关中断。\n");
    DeleteCriticalSection(&g_cs);
    return 0;
}
