/**
 * ch10_ex19.c —— 【变式】关中断临界区：简单关开 vs 保存/恢复
 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>

/**
 * ---------- 模拟中断使能状态（1 = 开，0 = 关） ----------
 */
static volatile int g_irq_enabled = 1;
static volatile long g_irq_off_count = 0;      /* 统计」实际关了几次「 */

/* 简单关开 */
static void irq_disable_simple(void) { g_irq_enabled = 0; }
static void irq_enable_simple(void)  { g_irq_enabled = 1; }

/* 保存 / 恢复（模拟 PRIMASK） */
static int  irq_save_and_disable(void)
{
    int old = g_irq_enabled;
    g_irq_enabled = 0;
    if (!old) g_irq_off_count++;
    return old;
}
static void irq_restore(int old) { g_irq_enabled = old; }

/* ---------- 会被」嵌套调用「的底层操作 ---------- */
static long g_counter_simple = 0;
static long g_counter_saved  = 0;

/* 内层函数：自己也开了临界区 */
static void inner_simple(void)
{
    irq_disable_simple();
    g_counter_simple++;
    irq_enable_simple();                  /* ← 它无条件把中断打开了 */
}
static void outer_simple(void)
{
    irq_disable_simple();
    g_counter_simple++;
    inner_simple();                       /* 内层跑完时中断已经被打开 */
                /* 这里本应还在临界区里！但中断其实已经开了 */
    g_counter_simple++;
    irq_enable_simple();
}

static void inner_saved(void)
{
    int old = irq_save_and_disable();
    g_counter_saved++;
    irq_restore(old);                     /* ← 恢复成」进来时的状态「 */
}
static void outer_saved(void)
{
    int old = irq_save_and_disable();
    g_counter_saved++;
    inner_saved();                        /* 内层恢复成」关"，外层仍然在临界区里 */
    g_counter_saved++;
    irq_restore(old);
}

int main(void)
{
    printf("=== 1) 简单关开 + 嵌套：临界区被提前打开 ===\n");
    g_irq_enabled = 1;
    irq_disable_simple();
    printf("  外层关中断后            : g_irq_enabled = %d （应为 0）\n", g_irq_enabled);
    inner_simple();
    printf("  调用内层函数之后        : g_irq_enabled = %d ", g_irq_enabled);
    printf(g_irq_enabled ? "<-- 【错】中断已经被内层打开了！\n" : "（正确）\n");
    printf("  外层剩下的那两行代码，其实已经不在临界区里了 —— 丢更新又回来了。\n\n");

    printf("=== 2) 保存/恢复 + 嵌套：临界区保持到最外层退出 ===\n");
    g_irq_enabled = 1;
    g_irq_off_count = 0;
    {
        int old = irq_save_and_disable();
        printf("  外层关中断后            : g_irq_enabled = %d （应为 0）\n", g_irq_enabled);
        inner_saved();
        printf("  调用内层函数之后        : g_irq_enabled = %d ", g_irq_enabled);
        printf(g_irq_enabled ? "（错）\n" : "<-- 【对】仍然是关着的\n");
        irq_restore(old);
    }
    printf("  外层恢复之后            : g_irq_enabled = %d （应为 1）\n",
           g_irq_enabled);
    printf("\n");

                /* ---------- 3) 量化代价：临界区长度和"关中断总时间" ---------- */
    printf("=== 3) 临界区的代价 ===\n");
    const long N = 2000000;
    DWORD c0 = GetTickCount();
    for (long i = 0; i < N; i++) { int o = irq_save_and_disable(); g_counter_saved++; irq_restore(o); }
    DWORD e0 = GetTickCount() - c0;
    DWORD c1 = GetTickCount();
    for (long i = 0; i < N; i++) { g_counter_saved++; }
    DWORD e1 = GetTickCount() - c1;
    printf("  加临界区  : %ld 次自增耗时 %lu ms\n", N, (unsigned long)e0);
    printf("  不加临界区: %ld 次自增耗时 %lu ms\n", N, (unsigned long)e1);
    printf("  -> 每次进出临界区的开销是真机上的「写 PRIMASK + 读 PRIMASK」，\n");
    printf("     在 Cortex-M0+ 上是几条指令（约几个时钟周期）。\n\n");

    printf("=== 结论 ===\n");
    printf("  1) 嵌套临界区必须用「保存/恢复」（__get_PRIMASK / __set_PRIMASK）；\n");
    printf("  2) 简单 __disable_irq/__enable_irq 只适合「绝对不会嵌套」的场合；\n");
    printf("  3) 临界区越短越好 —— 关中断期间别的中断会被拖延甚至丢失；\n");
    printf("  4) 能靠「单写者约定」解决的就别用临界区，因为那是零开销的。\n");
    return 0;
}
