/**
 * @note Copyright (C) Shanghai Panchip Microelectronics Co., Ltd.
 * @file lib_driver_delay_pan742.c
 * @brief
 * @history - V0.01, 2022-08-08, yangyaming, first implementation.
 */

#include "bsp_delay.h"

/**
 * @description	: delay函数
 * @param		: delay的值
 * @return		: 无
 */

#if defined(__CC_ARM)
__ASM void __delay_pan742(uint32_t cycle)
{
__delay_pan742_loop
    SUBS    r0,r0,#1
    BCS     __delay_pan742_loop
    BX      lr
}
#elif defined(__GNUC__)
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
