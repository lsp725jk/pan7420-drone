/**
 * @note      Copyright (C) Shanghai Panchip Microelectronics Co., Ltd.
 * @file      bsp_uart.h
 * @brief     uart驱动
 * @history - V1.0, 2022-02-22, yangyaming, first implementation.
 */

#ifndef __BSP_UART_H
#define __BSP_UART_H

#include "py32f0xx_hal.h"
#include "stdint.h"
// #include "bsp.h"

void bsp_uart_init(uint32_t __Baud);
uint8_t bsp_uart_get_byte(uint8_t* p_byte);
void CRSF_send(uint8_t *package, uint8_t len);
#endif
