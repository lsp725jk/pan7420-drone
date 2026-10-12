/**
 * ch10_ex18.c —— 【对照实验】中断体本身的执行时间：慢活 vs 打标记
 */
#include <stdio.h>
#include <time.h>

#define ISR_CALLS   2000            /**
 * 模拟 2000 次中断
 */

/**
 * ---------- 模式 A：中断体里干慢活 ----------
 */
static volatile double g_sink = 0;
static void isr_body_slow(void)
{
    for (int i = 0; i < 2000; i++) g_sink += i * 0.5;           /* 模拟中断里做滤波/解算 */
}

/* ---------- 模式 B：中断体只打标记 ---------- */
static volatile unsigned int g_tick = 0;
static void isr_body_mark(void)
{
    g_tick++;                               /* 相当于工程里的 _cnt10ms++ */
}

/* 主循环里才做的活（和上面那个慢活一样重） */
static void work_in_main(void) { isr_body_slow(); }

static double ms_since(clock_t c0)
{
    return 1000.0 * (double)(clock() - c0) / CLOCKS_PER_SEC;
}

int main(void)
{
    printf("模拟 %d 次中断\n\n", ISR_CALLS);

    clock_t c0 = clock();
    for (int i = 0; i < ISR_CALLS; i++) isr_body_slow();
    double tA = ms_since(c0);

    clock_t c1 = clock();
    for (int i = 0; i < ISR_CALLS; i++) isr_body_mark();
    double tB = ms_since(c1);

    clock_t c2 = clock();
    for (int i = 0; i < ISR_CALLS; i++) work_in_main();
    double tC = ms_since(c2);

    printf("模式 A：中断体里干慢活\n");
    printf("  %d 次中断共耗时 %.2f ms ，平均每次中断 = %.6f ms (%.3f us)\n",
           ISR_CALLS, tA, tA / ISR_CALLS, tA * 1000.0 / ISR_CALLS);
    printf("  -> 这 %.3f us 里，主循环一行都跑不了，别的中断也全部被阻塞。\n\n",
           tA * 1000.0 / ISR_CALLS);

    printf("模式 B：中断体只打标记\n");
    printf("  %d 次中断共耗时 %.4f ms ，平均每次中断 = %.8f ms (%.6f us)\n",
           ISR_CALLS, tB, tB / ISR_CALLS, tB * 1000.0 / ISR_CALLS);
    printf("  同样的活放到主循环里做：%.2f ms\n", tC);
    printf("  -> 中断体的时间只有模式 A 的 %.6f%%\n\n",
           tA > 0 ? 100.0 * tB / tA : 0.0);

    printf("换到真机上怎么算这笔账（原理示意）：\n");
    printf("  · 假设姿态解算一次要 300us，中断每 1ms 来一次：\n");
    printf("    模式 A 的 CPU 占用 = 300/1000 = 30%%；\n");
    printf("    而中断执行期间射频/串口中断全被阻塞，1ms 内到达的数据会丢。\n");
    printf("  · 模式 B 的中断体只有几个时钟周期（M0+ 48MHz 时约 0.1us），\n");
    printf("    CPU 占用可以忽略，而且主循环随时可以被中断。\n\n");

    printf("结论：\n");
    printf("  1) 中断里做业务，代价不只是「慢」，而是「这段时间整个系统停摆」；\n");
    printf("  2) 真机上还要额外算上：中断压栈 32 字节、软件浮点库调用、\n");
    printf("     以及被打断的指令重新取指的流水线开销；\n");
    printf("  3) 判断标准：中断体应该只有几条到几十条指令。\n");
    printf("     如果你在中断里看到 for 循环 / printf / 浮点 / HAL 阻塞调用 —— 就是错的。\n");
    printf("\n注：上面的绝对时间是本机 x86 的实测值，M0+ 上会慢 10~100 倍；\n");
    printf("    但「模式 A 比模式 B 慢几个数量级」这个结论与平台无关。\n");
    return 0;
}