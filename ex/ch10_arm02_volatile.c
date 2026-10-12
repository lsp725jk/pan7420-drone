/**
 * ch10_arm02_volatile.c —— 【本章最重要的一段汇编证据】
 */

/**
 * 版本 1：普通指针。编译器认为"这块内存只有我能改"，于是把读取提到循环外
 */
unsigned int * const REG_PLAIN = (unsigned int *)0x50000000u;

/**
 * 版本 2：volatile 指针。编译器每次使用都必须重新从内存读
 */
volatile unsigned int * const REG_VOL = (volatile unsigned int *)0x50000000u;

#define FLAG 0x1u

/* 坏版本：如果没有硬件/中断真的去改那个地址，这个函数永远不会返回 */
void wait_plain(void)
{
    while (!(*REG_PLAIN & FLAG)) {
                                /* 空转 */
    }
}

/* 好版本：每次循环都重新读寄存器，标志一变就能看到 */
void wait_volatile(void)
{
    while (!(*REG_VOL & FLAG)) {
                                /* 空转 */
    }
}
