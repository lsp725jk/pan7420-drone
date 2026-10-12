/**
 * ch10_ex12.c —— 【看门狗】为什么在中断里喂狗是错的
 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>

#define WD_TIMEOUT_MS 150        /**
 * 看门狗超时（真机上 IWDG 配的是 500ms）
 */
#define RUN_MS        800        /**
 * 每种场景观察多久
 */

/**
 * ---------- 模拟看门狗外设 ----------
 */
static volatile DWORD g_last_feed = 0;
static volatile long  g_wd_bites  = 0;      /* 复位次数 */
static volatile int   g_wd_stop   = 0;

static void wd_feed(void) { g_last_feed = GetTickCount(); }

static DWORD WINAPI wd_hardware(LPVOID p)
{
    (void)p;
    g_last_feed = GetTickCount();
    while (!g_wd_stop) {
        Sleep(10);
        if (GetTickCount() - g_last_feed > WD_TIMEOUT_MS) {
            g_wd_bites++;                                            /* 计数器减到 0 -> 硬件复位 */
            printf("    >>> 看门狗溢出，芯片复位！（第 %ld 次）\n", g_wd_bites);
            fflush(stdout);
            g_last_feed = GetTickCount();                            /* 复位后重新开始计时 */
        }
    }
    return 0;
}

/* ---------- 模拟"定时中断"：每 10ms 一次 ---------- */
static volatile int g_feed_in_isr = 0;       /* 1 = 把喂狗放在中断里（错误做法） */
static volatile int g_isr_stop    = 0;
static volatile long g_isr_ticks  = 0;

static DWORD WINAPI tim_isr(LPVOID p)
{
    (void)p;
    while (!g_isr_stop) {
        Sleep(10);
        g_isr_ticks++;
        if (g_feed_in_isr) wd_feed();                        /* 错：中断替主循环"证明自己活着" */
    }
    return 0;
}

/* ---------- 模拟"主循环" ---------- */
static volatile int g_hang = 0;              /* 1 = 模拟主循环卡死 */
static volatile long g_main_loops = 0;
static volatile int g_main_stop = 0;

static DWORD WINAPI main_loop(LPVOID p)
{
    (void)p;
    while (!g_main_stop) {
        g_main_loops++;
        if (g_hang) {
                                                /* 模拟"卡死在某个等待里"：不再刷灯、不再控制电机、也不再喂狗 */
            Sleep(50);
            continue;
        }
                                /* 正常情况下主循环做的活：姿态解算 + 刷灯 */
        if (!g_feed_in_isr) wd_feed();                       /* 对：喂狗点放在主循环末尾 */
        Sleep(10);
    }
    return 0;
}

static void run_scenario(const char *title, int feed_in_isr)
{
    printf("\n================ %s ================\n", title);

    g_feed_in_isr = feed_in_isr;
    g_hang = 0; g_wd_bites = 0;
    g_isr_stop = 0; g_main_stop = 0; g_wd_stop = 0;
    g_main_loops = 0; g_isr_ticks = 0;

    HANDLE hwd   = CreateThread(NULL, 0, wd_hardware, NULL, 0, NULL);
    HANDLE hisr  = CreateThread(NULL, 0, tim_isr,     NULL, 0, NULL);
    HANDLE hmain = CreateThread(NULL, 0, main_loop,   NULL, 0, NULL);

    Sleep(300);
    printf("  主循环正常跑：实时计数 = %ld ，复位次数 = %ld\n", g_main_loops, g_wd_bites);

    printf("  现在让主循环【卡死】……\n");
    g_hang = 1;
    Sleep(RUN_MS);

    printf("  结果：主循环已卡死 %d ms，看门狗咬了 %ld 次\n", RUN_MS, g_wd_bites);
    printf("  中断仍然在跑：g_isr_ticks = %ld\n", g_isr_ticks);

    g_main_stop = 1; g_isr_stop = 1; g_wd_stop = 1;
    WaitForSingleObject(hmain, 3000);
    WaitForSingleObject(hisr,  3000);
    WaitForSingleObject(hwd,   3000);
    CloseHandle(hmain); CloseHandle(hisr); CloseHandle(hwd);

    if (g_wd_bites > 0)
        printf("  -> 正确：看门狗起作用了，卡死的程序被复位。\n");
    else
        printf("  -> 【问题】主循环卡死了，看门狗却一次都没咬 —— 等于没有看门狗！\n");
}

int main(void)
{
    printf("看门狗超时 = %d ms\n", WD_TIMEOUT_MS);

                /* 场景 1：喂狗放在【定时中断】里 —— 错 */
    run_scenario("场景 1：在定时中断里喂狗（错误做法）", 1);

                /* 场景 2：喂狗放在【主循环】末尾 —— 对 */
    run_scenario("场景 2：在主循环里喂狗（正确做法）", 0);

    printf("\n结论：\n");
    printf("  · 看门狗监控的是「主循环还活着」，所以喂狗点必须在主循环里；\n");
    printf("  · 放在中断里，等于让「心跳」由另一个不受监控的时钟提供，狗永远不咬；\n");
    printf("  · 喂狗点还要放在「所有关键任务都跑完之后」：\n");
    printf("    本工程放在 main.c 的 while(1) 里，_cnt100ms >= 100 分支内、Led_Process() 之前，\n");
    printf("    这样姿态/遥控/定高任何一环卡住都会导致狗咬；\n");
    printf("  · 千万别在中断、也别在某个「只在不卡的时候才会执行到」的分支里喂狗。\n");
    return 0;
}
