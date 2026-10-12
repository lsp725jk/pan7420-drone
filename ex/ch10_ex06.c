/**
 * ch10_ex06.c —— 【另一个经典坑】while (_cnt < 10); 空转等待为什么出不来
 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>

#define LIMIT 600000000L
#define TARGET 3                                  /**
 * 等到第 3 个节拍就够说明问题了
 */

/**
 * 模拟工程里 drivers/bsp/bsp_timer.c 的 _cnt10ms
 */
static unsigned char cnt_plain = 0;              /* 工程现状：没加 volatile */
static volatile unsigned char cnt_vol = 0;       /* 建议写法：加 volatile */

static volatile int g_stop = 0;

static DWORD WINAPI fake_isr(LPVOID p)
{
    (void)p;
    while (!g_stop) {
        Sleep(1);                                                /* 每次约 15.6ms（Windows 的定时器精度） */
        cnt_plain++;                                             /* 中断里加计数 */
        cnt_vol++;
    }
    return 0;
}

/* 写法 A（错）：空转等待 */
static long wait_plain(void)
{
    long guard = 0;
    while (cnt_plain < TARGET && guard < LIMIT) guard++;           /* 等几个节拍 */
    return guard;
}
/* 写法 B（对）：volatile + 超时保护 */
static long wait_vol(void)
{
    long guard = 0;
    while (cnt_vol < TARGET && guard < LIMIT) guard++;
    return guard;
}

int main(void)
{
    HANDLE isr = CreateThread(NULL, 0, fake_isr, NULL, 0, NULL);

    DWORD c0 = GetTickCount();
    long  g1 = wait_plain();
    DWORD e1 = GetTickCount() - c0;
    printf("写法 A（无 volatile 空转）：guard = %ld ，耗时 %lu ms ，cnt_plain = %u\n",
           g1, (unsigned long)e1, cnt_plain);
    printf("  -> 中断确实把 cnt_plain 加到了 %u，但这个循环【没看见】，"
           "一直转到达上限\n\n", cnt_plain);

    cnt_vol = 0;
    DWORD c1 = GetTickCount();
    long  g2 = wait_vol();
    DWORD e2 = GetTickCount() - c1;
    printf("写法 B（volatile 空转）  ：guard = %ld ，耗时 %lu ms ，cnt_vol = %u\n",
           g2, (unsigned long)e2, cnt_vol);
    printf("  -> %s\n\n",
           (g2 < LIMIT) ? "看到第 3 个节拍就退出了" : "仍然没等到，请把 LIMIT 调大再试");

    g_stop = 1;
    WaitForSingleObject(isr, 2000);
    CloseHandle(isr);

    printf("为什么是「可能永远出不来」而不是「一定」：\n");
    printf("  · 优化等级、编译器、_cnt10ms 是不是 static、循环体里有没有函数调用，\n");
    printf("    都会影响编译器敢不敢把读取提出去；\n");
    printf("  · 所以这类 bug 的典型症状是「调试版好好的，Release 版卡死」、\n");
    printf("    「改了不相干的一行代码就好了」—— 最难查的一类问题；\n");
    printf("  · 唯一靠谱的做法：共享变量一律 volatile，并且不要用空转等待，\n");
    printf("    改成「状态机到点才执行」（见 ch10_ex05.c）。\n");
    return 0;
}
