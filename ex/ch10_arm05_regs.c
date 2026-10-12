/**
 * ch10_arm05_regs.c —— 寄存器读-改-写的代码生成
 */
#include <stdint.h>

/**
 * 用普通变量模拟一个 32 位外设寄存器（真机上它是某个固定地址）
 */
volatile uint32_t REG = 0;

/**
 * 错：整字覆盖，把别的位一起清掉了
 */
void set_bit_wrong(void)
{
    REG = (1u << 3);
}

/**
 * 对：读-改-写，只动第 3 位
 */
void set_bit_right(void)
{
    REG |= (1u << 3);
}

/**
 * 清某一位：&= ~掩码
 */
void clear_bit(void)
{
    REG &= ~(1u << 3);
}

/* 翻转某一位：^= */
void toggle_bit(void)
{
    REG ^= (1u << 3);
}

/* 改"多位域"（先清掉那几位，再或进新值）—— 配置外设时最常用 */
void set_field(uint32_t v)
{
    const uint32_t MASK = 0x7u << 4;                  /* 第 4~6 位，3 位宽 */
    REG = (REG & ~MASK) | ((v & 0x7u) << 4);
}
