/**
 * ch02_ex21_volatile_opt.c —— 没有 volatile 时，优化器会一直用寄存器里的旧值
 */
#include <stdio.h>

/**
 * 普通变量版本：编译器认为"没人改它"
 */
static int g_flag_plain = 0;
/**
 * volatile 版本：编译器每次都去读内存
 */
static volatile int g_flag_vol = 0;

/**
 * 模拟硬件：把内存值推进到就绪，并把"寄存器里那份旧值"返回给调用者
 */
static int step_hw_plain(void)
{
    g_flag_plain = 1;                       /* 硬件写了内存（共 1 次） */
    return 0;                               /* 但读进来的是"还没就绪"的旧值 */
}

static int step_hw_vol(void)
{
    g_flag_vol = 1;
    return 0;
}

int main(void)
{
    printf("---- 1. 普通变量做轮询：opt 变量被缓存在寄存器里 ----\n");
    g_flag_plain = 0;
    int opt_plain = 0;
    int spins = 0;
    while (opt_plain != 1) {                /* 被判成"循环不变"，编译器可以只读一次 */
        opt_plain = step_hw_plain();                /* 返回值 0 一直是 0，于是循环永远出不来 */
        spins++;
        if (spins >= 1000) {
            printf("  轮询 %d 次后放弃（超时）！\n", spins);
            printf("  此时内存里的 g_flag_plain 其实已经是 %d\n", g_flag_plain);
            printf("  这就是看不到硬件更新的典型症状：程序卡死在等待循环里。\n");
            break;
        }
    }

    printf("---- 2. 同样的逻辑，加 volatile ----\n");
    g_flag_vol = 0;
    spins = 0;
    while (g_flag_vol != 1) {               /* 每次都真的去读内存 */
        step_hw_vol();
        spins++;
        if (spins >= 1000) {
            printf("  不应该到这里\n");
            break;
        }
    }
    printf("  轮询 %d 次后看到 g_flag_vol = %d，循环正常退出\n",
           spins, g_flag_vol);
    printf("  （严格来说，非 volatile 版本在 /O2 下的具体表现依赖编译器的优化决策；\n");
    printf("   这里是**演示**，真实项目里没加 volatile 就是随时可能踩雷。）\n");

    printf("---- 3. 只读寄存器：const volatile 两者都要 ----\n");
    const volatile int *status_reg = &g_flag_vol;
    printf("  读只读状态寄存器：%d\n", *status_reg);
    printf("  真实 MCU 里它的地址是固定的，写法类似：\n");
    printf("      #define TIM1_SR (*(const volatile uint32_t *)0x40012C10UL)\n");

    printf("---- 4. volatile 不是免费的 ----\n");
    volatile int sum_v = 0;
    int sum_n = 0;
    for (int i = 0; i < 1000; i++) {
        sum_v += i;                                 /* 每次都要读改写内存 */
        sum_n += i;                                 /* 编译器可以全程用寄存器 */
    }
    printf("  volatile 累加 = %d, 普通累加 = %d（结果一样，代价不一样）\n",
           sum_v, sum_n);
    printf("  在 Cortex-M0+ 上，volatile 版本每轮多两次内存访问，\n");
    printf("  循环上千次时差距肉眼可见。所以：只给真正需要的变量加。\n");
    return 0;
}
