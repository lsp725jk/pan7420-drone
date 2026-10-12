/**
 * ch10_ex21.c —— 【对照实验】喂狗太频繁 vs 太慢，都救不了你
 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>

#define HANG_MS 350                 /**
 * 模拟"某次 Flash 擦写卡了 350ms"
 */

static volatile DWORD g_last_feed = 0;
static volatile long  g_bites = 0;
static volatile int   g_wd_stop = 0;
static int            g_timeout_ms = 500;

static void wd_feed(void) { g_last_feed = GetTickCount(); }

static DWORD WINAPI wd_hardware(LPVOID p)
{
    (void)p;
    g_last_feed = GetTickCount();
    while (!g_wd_stop) {
        Sleep(5);
        if (GetTickCount() - g_last_feed > (DWORD)g_timeout_ms) {
            g_bites++;
            g_last_feed = GetTickCount();
        }
    }
    return 0;
}

static volatile int g_main_stop = 0;
static int          g_feed_period_ms = 20;
static int          g_hang_once = 0;

static DWORD WINAPI main_loop(LPVOID p)
{
    (void)p;
    int hung = 0;
    while (!g_main_stop) {
                                /* 模拟主循环的一次长卡顿（只发生一次） */
        if (g_hang_once && !hung) {
            hung = 1;
            g_last_feed = GetTickCount();                              /* 卡顿开始前刚喂过 */
                                                /* 卡顿期间：不喂狗、不刷灯、不控制电机 */
            for (int t = 0; t < HANG_MS && !g_main_stop; t += 5) Sleep(5);
            g_hang_once = 0;
            continue;
        }
        wd_feed();                                              /* 主循环末尾喂狗 */
        Sleep(g_feed_period_ms);
    }
    return 0;
}

static void scenario(const char *title, int timeout_ms, int feed_ms)
{
    printf("\n--- %s ---\n", title);
    printf("    超时 = %d ms ，喂狗周期 = %d ms ，余量 = %.1f 倍\n",
           timeout_ms, feed_ms, (double)timeout_ms / feed_ms);

    g_timeout_ms = timeout_ms;
    g_feed_period_ms = feed_ms;
    g_bites = 0; g_wd_stop = 0; g_main_stop = 0; g_hang_once = 1;

    HANDLE hwd  = CreateThread(NULL, 0, wd_hardware, NULL, 0, NULL);
    HANDLE hman = CreateThread(NULL, 0, main_loop,   NULL, 0, NULL);

    Sleep(150);                                         /* 先让它正常跑一会儿 */
    printf("    正常运行时咬的次数 = %ld\n", g_bites);

    g_hang_once = 1;                                    /* 触发卡顿 */
    Sleep(HANG_MS + 300);
    printf("    主循环卡了 %d ms 之后：看门狗咬了 %ld 次\n", HANG_MS, g_bites);

    g_main_stop = 1; g_wd_stop = 1;
    WaitForSingleObject(hman, 3000); WaitForSingleObject(hwd, 3000);
    CloseHandle(hman); CloseHandle(hwd);

    if (g_bites == 0)
        printf("    -> 狗没咬：这次卡顿被「余量」吞掉了（问题被掩盖）\n");
    else
        printf("    -> 狗咬了：卡顿被发现\n");
}

int main(void)
{
    printf("场景：主循环里发生一次 %d ms 的长卡顿\n", HANG_MS);

    scenario("A) 超时 500ms，每 20ms 喂（经典配置）", 500, 20);
    scenario("B) 超时 500ms，每 5ms 喂（余量过大）", 500, 5);
    scenario("C) 超时 100ms，每 20ms 喂（余量不足）", 100, 20);

    printf("\n=== 怎么读这三个结果 ===\n");
    printf("A) 500/20 = 25 倍余量：%d ms 的卡顿不会误触发。\n", HANG_MS);
    printf("   你工程里是 500ms 超时、约 100ms 喂一次（5 倍余量），属于同一档，\n");
    printf("   而且「4ms 姿态周期抖动、偶尔一次 Flash 擦写」都不会误复位。\n");
    printf("B) 余量越大，越能容忍卡顿 —— 但也越晚发现问题。\n");
    printf("   如果卡顿本身就是要抓的故障，余量太大就是「看门狗失效」。\n");
    printf("C) 余量小于 1 就必然误复位；即使大于 1，只要系统里有比余量更长的\n");
    printf("   正常操作（比如 Flash 页擦除几十毫秒），也会周期性重启。\n");
    printf("\n=== 选参数的实用规则 ===\n");
    printf("  · 超时 = 最长正常单次操作时间 × 3~5；\n");
    printf("  · 喂狗周期 < 超时 / 5；\n");
    printf("  · 喂狗点放在「所有关键任务都跑完之后」（见 ch10_ex12.c）；\n");
    printf("  · 调试时冻结看门狗可以，但发布前必须取消冻结。\n");
    printf("\n注：本实验用线程模拟，具体「咬」的次数受 Windows 调度粒度（约 15.6ms）影响，\n");
    printf("    但三种配置的相对关系是稳定的。\n");
    return 0;
}
