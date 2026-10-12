/**
 * @note Copyright (C) Shanghai Panchip Microelectronics Co., Ltd.
 * @file lib_driver_delay_pan308.h
 * @brief
 * @history - V0.01, 2022-02-17, yangyaming, first implementation.
 */

#ifndef __BSP_DELAY_H
#define __BSP_DELAY_H

#ifdef __cplusplus
extern "C"{
#endif //__cplusplus

#include "bsp.h"



void  __delay_pan742(uint32_t cycle);
			  
#define delay_us(us)    do{if((us)){__delay_pan742((((int32_t)(us))<<4) + 270);}}while(0)
#define delay_ms(ms)    do{uint32_t cnt=(ms);while(cnt--){__delay_pan742(7980);}}while(0)



#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__LIB_DRIVER_DELAY_PAN308_H



