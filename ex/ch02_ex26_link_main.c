/**
 * ch02_ex26_link_main.c —— 使用另一个 .c 里定义的东西（配对示例）
 */
#include <stdio.h>
#include "ch02_ex26_link.h"

int main(void)
{
    printf("---- 1. 读另一个文件里的变量 ----\n");
    printf("固件版本 = %s\n", g_fw_version);
    printf("初始 tick = %u\n", g_tick);
    printf("tick_now() = %u\n", tick_now());

    printf("---- 2. 调另一个文件里的函数 ----\n");
    tick_advance(100);
    printf("tick_advance(100) 之后 tick_now() = %u\n", tick_now());

    printf("---- 3. 用无符号减法算时间差（回绕也不怕）----\n");
    g_tick = 4294967200u;                       /* 离回绕只差 96 */
    uint32_t mark = tick_now();
    printf("把 g_tick 设成 %u（接近 2^32），mark = %u\n", g_tick, mark);
    tick_advance(200);                          /* 这一步会回绕 */
    printf("再走 200 个 tick 之后 g_tick = %u（已经回绕）\n", g_tick);
    printf("tick_elapsed(mark) = %u   <-- 仍然是 200，正确！\n",
           tick_elapsed(mark));
    printf("这就是嵌入式里用无符号做计时的原因。\n");

    printf("---- 4. 声明 vs 定义 ----\n");
    printf("  头文件里的 extern uint32_t g_tick; 只是声明（不占空间）；\n");
    printf("  .c 里的     uint32_t g_tick = 0;  才是定义（占空间）。\n");
    printf("  定义全程序只能一份，声明可以重复无数次 ——\n");
    printf("  这就是声明放 .h、定义放 .c 这条规矩的来源。\n");
    printf("  如果两份 .c 都定义，链接报 LNK2005；\n");
    printf("  如果没有任何一份定义，链接报 LNK2019。\n");
    return 0;
}
