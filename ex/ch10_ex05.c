/**
 * ch10_ex05.c —— 【中断打标记、主循环干活】这个模式长什么样
 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>

/**
 * ================== 中断与主循环之间只传这几样东西 ==================
 */
/**
 * 全部是 volatile：中断写、主循环读
 */
static volatile long g_tick10ms = 0;      /* 10ms 节拍计数（对应工程里的 _cnt10ms） */
static volatile long g_rx_flag  = 0;      /* 收到一帧遥控数据的标记 */
static volatile int  g_rx_byte  = 0;      /* 中断"暂存"的那一个字节 */

/* 【约定】这两个变量只有中断写，主循环只读，所以不需要临界区 */
static volatile int  g_stop = 0;          /* 演示用：通知模拟中断线程退出 */

/* ================== 模拟 TIM 中断（1ms 一次，10ms 一次节拍） ================== */
static DWORD WINAPI fake_tim_isr(LPVOID p)
{
    (void)p;
    long ms = 0;
    while (!g_stop) {
        Sleep(1);                                          /* 模拟 1ms 到一次 */
        if (++ms % 10 == 0) {
            g_tick10ms++;                                          /* 只加计数，几纳秒的事 */
        }
        if (ms == 55) {                                    /* 模拟第 55ms 收到一个字节 */
            g_rx_byte = 0x5A;                                      /* 不做解析，只存起来 + 打标记 */
            g_rx_flag = 1;
        }
    }
    return 0;
}

/* ================== 主循环里"干活"的部分 ================== */
static void do_attitude_solve(void)  { /* 假装要 2ms */
    volatile double s = 0; for (int i = 0; i < 200000; i++) s += i * 0.5; (void)s;
}
static void do_rx_parse(int byte)    { printf("    [主循环] 解析遥控字节 0x%02X\n", byte); }
static void do_flash_write(void)     { printf("    [主循环] 写 Flash（慢操作，中断里绝不能做）\n"); }

int main(void)
{
    HANDLE isr = CreateThread(NULL, 0, fake_tim_isr, NULL, 0, NULL);
    long seen_tick = 0;
    long loops = 0;

    printf("主循环开始（模拟飞控 main 里的 while(1)）\n");
    while (loops < 12) {
        loops++;

                                /* 1) 读节拍：因为 g_tick10ms 是 volatile 且只有一个写者，直接读就行 */
        long t = g_tick10ms;
        if (t - seen_tick >= 3) {                          /* 每约 30ms 做一次姿态解算 */
            seen_tick = t;
            printf("  [主循环] tick = %ld ，跑一次姿态解算\n", t);
            do_attitude_solve();
        }

                                /* 2) 读"标记"：先看有没有事，再决定干不干 —— 这就是"打标记"的价值 */
        if (g_rx_flag) {
            int b = g_rx_byte;                                     /* 先取走数据 */
            g_rx_flag = 0;                                         /* 再清标记（主循环清自己这一侧的标记） */
            do_rx_parse(b);
            if (b == 0x5A) do_flash_write();
        }

        Sleep(20);
    }

    g_stop = 1;
    WaitForSingleObject(isr, 2000);
    CloseHandle(isr);

    printf("\n最终 g_tick10ms = %ld\n", g_tick10ms);
    printf("\n对照工程代码：\n");
    printf("  drivers/bsp/bsp_timer.c 的 HAL_TIM_PeriodElapsedCallback 里只有 _cnt10ms++ 之类；\n");
    printf("  user/main.c 的 while(1) 里才做解析遥控、姿态控制、刷灯、喂狗。\n");
    printf("  这正是本模式：中断打标记（加计数），主循环干活。\n");
    return 0;
}
