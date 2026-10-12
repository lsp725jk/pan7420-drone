/**
 * 附录C 例 5：交叉编译目标检查（PY32F030 = Cortex-M0+）
 */
#include <stdint.h>
#include <stddef.h>

/**
 * 用一个 volatile 指针模拟寄存器，避免被优化掉
 */
#define REG32(addr) (*(volatile uint32_t*)(uintptr_t)(addr))

/* PY32F030 常见外设基址（示例值，具体以芯片手册为准，这里只用于演示编译） */
#define RCC_BASE      0x40021000u
#define RCC_IOPENR    REG32(RCC_BASE + 0x34u)

static volatile uint32_t g_counter = 0;

/* 简单的忙等延时：循环次数靠 NOP 撑，别指望精确 */
static void delay_loop(volatile uint32_t n) {
    while (n-- > 0u) {
        __asm volatile ("nop");
    }
}

/* 累加，给优化器一点活干；返回值必须被用到，否则整个函数会被删掉 */
uint32_t accumulate(const uint32_t* data, size_t len) {
    uint32_t sum = 0u;
    for (size_t i = 0u; i < len; ++i) {
        sum += data[i];
    }
    return sum;
}

/* 让某个 GPIO 端口时钟使能（只看能否生成正确代码，不上板） */
void enable_gpio_clock(void) {
    RCC_IOPENR |= (1u << 0);
}

int main(void) {
    static const uint32_t table[4] = {1u, 2u, 3u, 4u};
    g_counter = accumulate(table, 4u);
    enable_gpio_clock();
    for (;;) {
        delay_loop(1000u);
        g_counter++;
    }
                /* 裸机上 main 通常不返回 */
}
