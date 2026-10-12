/**
 * @note Copyright (C) Shanghai Panchip Microelectronics Co., Ltd.
 * @file lib_driver_delay_pan742.c
 * @brief
 * @history - V0.01, 2022-08-08, yangyaming, first implementation.
 */
/**
 * 文件：drivers/bsp/bsp_delay.c           【角色】最底层的“忙等”延时循环
 */

#include "bsp_delay.h"

/* @description	: delay函数 @param		: delay的值 @return		: 无 */



#if defined(__CC_ARM)
/* 【ARMCC 分支】__ASM 是 ARMCC 的关键字，表示“这个函数的函数体是汇编， 不要按 C 语法解析”。__delay_pan742_loop 是一个汇编标号（label）， 不是函数，只在汇编范围内可见。 */
__ASM void __delay_pan742(uint32_t cycle)
{
__delay_pan742_loop
    SUBS    r0,r0,#1                    /* r0 = r0 - 1，并更新标志位（Z/C 等） */
    BCS     __delay_pan742_loop     /* BCS = Branch if Carry Set：无符号减法 未借位（即 r0 减完仍 >= 0，没变成负数） 就跳回去继续减。r0 减到 0 时 SUBS 会产生借位（C=0），循环结束。 */
    BX      lr                          /* 返回调用者（lr = link register） */
}
#elif defined(__GNUC__)
/* 【GCC 分支】 __attribute__((naked))：告诉 GCC“不要给这个函数生成任何函数序言/尾声” （normal 函数会自己压栈、保存寄存器、在结尾恢复）， 因为下面那段汇编要自己控制一切。 */
__attribute__((naked)) void __delay_pan742(uint32_t cycle)
{
    (void)cycle;
    __asm__ volatile(
        ".short 0x3801\n\t"                   /* subs r0, #1 */
        ".short 0xD8FC\n\t"                   /* bcs  .-4 */
        ".short 0x4770\n\t");                  /* bx   lr */
}
#else
#error "__delay_pan742: unsupported compiler (need ARMCC or GCC)"
#endif



