/**
 * @note Copyright (C) Shanghai Panchip Microelectronics Co., Ltd.
 * @file lib_driver_delay_pan308.h
 * @brief
 * @history - V0.01, 2022-02-17, yangyaming, first implementation.
 */
/**
 * 文件：drivers/bsp/bsp_delay.h           【角色】把“循环次数”换算成微秒/毫秒的
 */

#ifndef __BSP_DELAY_H
#define __BSP_DELAY_H

#ifdef __cplusplus
extern "C"{
#endif //__cplusplus

#include "bsp.h"



/* 汇编写的空转循环：参数是“循环次数”，不是时间单位。实现在 bsp_delay.c。 */
void  __delay_pan742(uint32_t cycle);
			  
/* 【宏函数语法讲解】 1) 为什么外面要套 do{ ... }while(0)： 宏是文本替换。如果不套，写成 if(x) delay_us(10); else ... 时， 展开后会变成 if(x) 语句1; 语句2;  —— 多出来的语句就跑出了 if 的范围。 */
#define delay_us(us)    do{if((us)){__delay_pan742((((int32_t)(us))<<4) + 270);}}while(0)

#define delay_ms(ms)    do{uint32_t cnt=(ms);while(cnt--){__delay_pan742(7980);}}while(0)



#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__LIB_DRIVER_DELAY_PAN308_H
/* 【注意（疑似问题，未改动）】 1) 文件头注释写的是 lib_driver_delay_pan308.h，实际文件名是 bsp_delay.h， 属于改名时漏改注释。 2) delay_us 与 delay_ms 的换算系数不是按同一个内核频率标定的 */
