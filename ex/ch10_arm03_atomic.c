/**
 * ch10_arm03_atomic.c —— 【x++ 不是一条指令】：Cortex-M0+ 上的读-改-写
 */
#include <stdint.h>

/**
 * 中断与主循环共享的计数器
 */
volatile uint32_t g_tick = 0;

/**
 * 模拟中断服务函数：里面做一次自增
 */
void TIM_IRQHandler_sim(void)
{
    g_tick++;                         /**
     * 编译成 ldr / adds / str 三条指令
     */
}

/**
 * 主循环里读走并清零（这也是读-改-写，同样不原子）
 */
uint32_t take_tick(void)
{
    uint32_t v = g_tick;
    g_tick = 0;
    return v;
}



void TIM_IRQHandler_safe(void)
{
    __asm volatile ("cpsid i" ::: "memory");           /* 关中断 */
    g_tick++;
    __asm volatile ("cpsie i" ::: "memory");           /* 开中断 */
}
