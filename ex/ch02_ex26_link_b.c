/**
 * ch02_ex26_link_src.c —— 定义放在 .c 里（配对示例，和 main 一起编译）
 */
#include <stdio.h>
#include <stdint.h>
#include "ch02_ex26_link.h"

/**
 * ---- 定义（分配空间，全程序只有这一份）----
 */
uint32_t g_tick = 0;
const char *g_fw_version = "ch02_ex26 / C17";

/**
 * ---- 用 static 做"本文件私有"的辅助变量和函数 ----
 */
static uint32_t s_local_calls = 0;      /* 内部链接：别的 .c 看不到，也不会重名 */

static void note_call(void)
{
    s_local_calls++;
}

/* ---- 对外接口 ---- */
uint32_t tick_now(void)
{
    note_call();
    return g_tick;
}

void tick_advance(uint32_t n)
{
    note_call();
    g_tick += n;                                /* 无符号加法，回绕是标准保证的 */
}

/* 计算"从 since 到现在过了多少 tick"。 这是嵌入式里最经典的用法：即使 g_tick 回绕了，差值仍然正确 —— 因为无符号减法的结果与回绕无关（模 2^32 运算）。 */
uint32_t tick_elapsed(uint32_t since)
{
    note_call();
    return g_tick - since;
}
