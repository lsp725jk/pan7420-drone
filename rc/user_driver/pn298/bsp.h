/**
 * @note Copyright (C) Shanghai Panchip Microelectronics Co., Ltd.
 * @file bsp.h
 * @brief
 * @history - V0.01, 2022-02-17, yangyaming, first implementation.
 */

#ifndef __BSP_H
#define __BSP_H



#include "py32f0xx.h"
#include "py32f0xx_hal_conf.h"
#include "py32f0xx_hal_tim.h"
#include "py32f0xx_hal_tim_ex.h"

#include "config_rf.h"
#include "bsp_delay.h"

#include "bsp_rfspi_sw.h"
#include "bsp_pn298.h"
#include "bsp_uart.h"

void bsp_init(void);
#endif
