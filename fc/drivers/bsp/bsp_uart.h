/**
 * @note      Copyright (C) Shanghai Panchip Microelectronics Co., Ltd.
 * @file      bsp_uart.h
 * @brief     uart驱动
 * @history - V1.0, 2022-02-22, yangyaming, first implementation.
 */
/**
 * 文件：drivers/bsp/bsp_uart.h            【角色】串口驱动的对外接口声明
 */

#ifndef __BSP_UART_H
#define __BSP_UART_H

#include "bsp.h"

void bsp_uart_init(void);
uint8_t bsp_uart_get_byte(uint8_t* p_byte);

#endif
