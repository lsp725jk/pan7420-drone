/**
 * ch02_ex34_volatile_bad.c —— volatile 的三种典型误用（对比正确做法）
 */
#include <stdio.h>
#include <stdint.h>

/**
 * 场景：主循环累加，中断里也累加（用函数模拟中断）
 */
static volatile uint32_t g_counter_v = 0;
static uint32_t          g_counter_plain = 0;

/**
 * 模拟"中断服务程序"做一次累加
 */
static void fake_isr_vol(void)
{
    g_counter_v += 1;                       /* 读-改-写，中间可能被主循环打断 */
}

static void fake_isr_plain(void)
{
    g_counter_plain += 1;
}

int main(void)
{
    printf("---- 1. volatile 不保证原子性 ----\n");
    printf("  g_counter_v += 1 编译出来是三条指令：LDR / ADD / STR。\n");
    printf("  如果主循环刚 LDR 完，中断就来了一次完整的 +=，\n");
    printf("  主循环再 STR 回去时就把中断那次的结果**覆盖**了。\n");
    printf("  症状：计数器偶尔少加（丢更新），且只在中断频繁时出现。\n");
    fake_isr_vol();
    g_counter_v += 1;
    printf("  正常顺序下 g_counter_v = %u（两次累加都对）\n",
           (unsigned)g_counter_v);
    printf("  但正常顺序不是你能保证的 —— 这才是问题所在。\n");

    printf("---- 2. volatile 管不了多核/多线程 ----\n");
    printf("  volatile 只告诉编译器每次都要真的读写内存，\n");
    printf("  它**不保证**：\n");
    printf("    a) 其它核/线程什么时候能看到你的写（内存序、缓存一致性）；\n");
    printf("    b) 两步操作之间不被打断（原子性）；\n");
    printf("    c) 编译器和 CPU 不会重排你的读写顺序（内存屏障）。\n");
    printf("  所以 volatile 不是线程安全的同义词。\n");

    printf("---- 3. 正确做法对照 ----\n");
    printf("  场景 A：单核 + 中断共享的计数器\n");
    printf("    做法 1（推荐）：关中断 -> 累加 -> 开中断，变量用普通 uint32_t；\n");
    printf("    做法 2：用 C11 的 atomic_uint 和 atomic_fetch_add；\n");
    printf("    做法 3：把一个 32 位计数器拆成「只让中断写、主循环读并清零」的单向模式。\n");
    printf("  场景 B：轮询硬件标志（单核、只有主循环读）\n");
    printf("    做法：volatile 足够 —— 因为只需要每次都真的读，不需要原子。\n");
    printf("  场景 C：DMA 缓冲区\n");
    printf("    做法：缓冲区加 volatile，但**描述符的读写顺序**要配内存屏障；\n");
    printf("          并且要在 DMA 完成中断里再读，不要盲等。\n");

    printf("---- 4. 判断口诀 ----\n");
    printf("  · 只需要看到别人的修改        -> volatile 够用；\n");
    printf("  · 需要读-改-写不被打断         -> volatile 不够，要关中断或原子；\n");
    printf("  · 需要多核之间顺序一致        -> volatile 完全不够，要内存屏障/原子。\n");
    fake_isr_plain();
    g_counter_plain += 1;
    printf("  对照：普通变量 g_counter_plain = %u（单线程下当然也对，\n",
           (unsigned)g_counter_plain);
    printf("        它的问题是编译器可能把循环里的读优化掉，见 02.9 的实测）。\n");
    return 0;
}
