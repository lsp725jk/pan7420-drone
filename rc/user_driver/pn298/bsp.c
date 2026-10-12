/**
 * @note Copyright (C) Shanghai Panchip Microelectronics Co., Ltd.
 * @file bsp.c
 * @brief
 * @history - V0.01, 2022-02-17, yangyaming, first implementation.
 */

#include "bsp.h"

/**
 * @description	: bsp外设初始化
 * @param		: 无
 * @return		: 无
 */
void bsp_init(void)
{
	bsp_rfspi_init();
	bsp_pn298_init();
		// bsp_uart_init();
		// bsp_timer_init();
}

