/**
 * @note      Copyright (C) Shanghai Panchip Microelectronics Co., Ltd.
 * @file      bsp_uart.h
 * @brief     uart驱动
 * @history - V1.0, 2022-02-22, yangyaming, first implementation.
 */

/**
 * 文件：user/bsp_usart.h                  【角色】USART1 串口驱动接口声明
 */
#ifndef __BSP_UART_H
#define __BSP_UART_H

#include "bsp.h"

/* 初始化 USART1（引脚 PB2/PB8，115200-8-N-1） */
void bsp_uart_init(void);
/* 阻塞收 1 字节，返回 HAL 状态（HAL_OK=0 成功）；本工程未使用 */
uint8_t bsp_uart_get_byte(uint8_t* p_byte);
/* 阻塞发 1 字节（printf 的底层出口） */
void bsp_uart_send(uint8_t data);
#endif

