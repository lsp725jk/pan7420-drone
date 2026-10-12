/**
 * ch03_ex10_bitfield.c —— 位段的读改写与原子性提醒
 */
#include <stdio.h>
#include <stdint.h>

/**
 * 32 位控制字：把一整个寄存器按位拆开
 */
typedef union {
    uint32_t raw;
    struct {
        uint32_t enable    : 1;
        uint32_t dir       : 1;
        uint32_t microstep : 3;                     /**
         * 0..7
         */
        uint32_t reserved1 : 3;
        uint32_t current   : 8;                     /* 0..255 */
        uint32_t reserved2 : 16;
    } f;
} ctrl_word_t;

/* 打印 32 位二进制，方便观察位的位置 */
static void print_bits32(uint32_t v)
{
    for (int i = 31; i >= 0; i--) {
        putchar((v & (1u << i)) ? '1' : '0');
        if (i % 8 == 0 && i != 0) {
            putchar('\'');
        }
    }
}

int main(void)
{
    ctrl_word_t w;
    w.raw = 0;

    printf("---- 1. 逐字段赋值：每次都是一次读-改-写 ----\n");
    print_bits32(w.raw);
    printf("   <- raw = 0x%08X\n", w.raw);
    w.f.enable = 1;                         /* 读 raw → 改 bit0 → 写 raw */
    print_bits32(w.raw);
    printf("   <- enable = 1（bit0）\n");
    w.f.dir = 1;
    print_bits32(w.raw);
    printf("   <- dir = 1（bit1）\n");
    w.f.microstep = 5;
    print_bits32(w.raw);
    printf("   <- microstep = 5（bit2..4 = 101）\n");
    w.f.current = 0xA5;
    print_bits32(w.raw);
    printf("   <- current = 0xA5（bit8..15）\n");
    printf("  一共改了 4 次，也就是 4 次\"读-改-写\"。\n");

    printf("---- 2. 为什么这在中断场景下危险 ----\n");
    printf("  假设这个 raw 就是硬件寄存器，主循环在改 microstep，\n");
    printf("  中断也在改 current。两条\"读-改-写\"序列可能交错：\n");
    printf("    主循环读 raw -> 中断读 raw -> 主循环写回 -> 中断写回\n");
    printf("  结果主循环那次修改被整个覆盖掉了。\n");
    printf("  症状：某个字段偶尔自己变回旧值，且只在中断频繁时出现。\n");

    printf("---- 3. 正确做法：shadow 变量 + 一次性写回 ----\n");
    uint32_t shadow = 0;
    shadow |= (1u << 0);                            /* enable = 1 */
    shadow |= (1u << 1);                            /* dir = 1 */
    shadow &= ~(0x7u << 2);                         /* 先清 microstep 字段 */
    shadow |= (5u << 2);                            /* 再写 5 */
    shadow &= ~(0xFFu << 8);                        /* 先清 current 字段 */
    shadow |= (0xA5u << 8);                         /* 再写 0xA5 */
    printf("  shadow = 0x%08X -> ", shadow);
    print_bits32(shadow);
    printf("\n");
    printf("  在真实硬件上，最后一句 REG = shadow; 才是一次真正的写操作，\n");
    printf("  中间过程只动内存里的副本，避免出现\"改到一半\"的寄存器状态。\n");
    printf("  （如果硬件要求原子，还需要在读-改-写期间关中断。）\n");

    printf("---- 4. 位段的位序是实现定义的 ----\n");
    printf("  enable 是第一个字段，在本机占 bit0 —— 但标准没规定顺序。\n");
    printf("  证据：把 raw 打印出来对照就能确认本机的布局：\n");
    printf("    refresh 后的 raw = 0x%08X\n", w.raw);
    printf("  【结论】解析外部协议（别人的设备发来的数据）时，\n");
    printf("         一定要手工移位掩码，不要用位段；\n");
    printf("         只在\"本编译器 + 本芯片\"的内部寄存器映射里才用位段。\n");
    return 0;
}
