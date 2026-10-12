/**
 * @note      Copyright (C) Shanghai Panchip Microelectronics Co., Ltd.
 * @file      bsp_uart.c
 * @brief     uart驱动
 * @history - V1.0, 2022-02-22, yangyaming, first implementation.
 */
/**
 * 文件：user/bsp_usart.c                  【角色】USART1 串口收发封装（带 printf 重定向）
 */
#include "bsp_uart.h"

/* UART 句柄：{0} 把结构体全部清零，避免 HAL 用到未初始化成员 */
UART_HandleTypeDef sUartxHandle = {0};

#define UARTx											USART1
#define BAUDRATE										115200
#define UARTx_TX_PORT									GPIOB
#define UARTx_TX_PIN									GPIO_PIN_8
#define UARTx_RX_PORT									GPIOB
#define UARTx_RX_PIN									GPIO_PIN_2
#define UARTx_TX_ALTERNATE_AFn							GPIO_AF8_USART1
#define UARTx_RX_ALTERNATE_AFn							GPIO_AF0_USART1
#define UARTx_IRQn										USART1_IRQn

#define LENGTH												10

/* Exported macros------------------------------------------------------------- */
#define __HAL_RCC_UARTx_CLK_ENABLE()  				__HAL_RCC_USART1_CLK_ENABLE()	
#define __HAL_RCC_UARTx_RX_GPIO_CLK_ENABLE()  		__HAL_RCC_GPIOB_CLK_ENABLE()
#define __HAL_RCC_UARTx_TX_GPIO_CLK_ENABLE()  		__HAL_RCC_GPIOB_CLK_ENABLE()



static void __bsp_uart_init(void)
{
	__HAL_RCC_UARTx_CLK_ENABLE();
	sUartxHandle.Instance = UARTx;
	sUartxHandle.Init.BaudRate = BAUDRATE;
	sUartxHandle.Init.WordLength = UART_WORDLENGTH_8B;
	sUartxHandle.Init.StopBits = UART_STOPBITS_1;
	sUartxHandle.Init.Parity = UART_PARITY_NONE;
	sUartxHandle.Init.Mode = UART_MODE_TX_RX;
				/* 1 停止位 + 无校验 + 8 数据位 = 常说的 “8-N-1”，与电脑串口助手的默认设置一致 */
	HAL_UART_Init(&sUartxHandle);
}



static void __bsp_uart_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_UARTx_RX_GPIO_CLK_ENABLE();
	__HAL_RCC_UARTx_TX_GPIO_CLK_ENABLE();
        /* if UARTx is UART0 GPIO Configuration: PB2     ------> USART1_RXD PB8     ------> USART1_TXD */
    GPIO_InitStruct.Pin = UARTx_RX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = UARTx_RX_ALTERNATE_AFn;
    HAL_GPIO_Init(UARTx_RX_PORT, &GPIO_InitStruct);
		
    GPIO_InitStruct.Pin = UARTx_TX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Alternate = UARTx_TX_ALTERNATE_AFn;
    HAL_GPIO_Init(UARTx_TX_PORT, &GPIO_InitStruct);	
}

/* @brief  USART1 总初始化（先配引脚，再配外设） 【参数】无 【返回】无 【为什么是这个顺序】引脚必须先配成复用推挽，否则 USART 一使能就会在普通 IO 状态上 产生一段无意义的电平（对端会收到乱码字节）。 */
void bsp_uart_init(void)
{
	__bsp_uart_gpio_init();
	__bsp_uart_init();
}

/* 发送一个字节：超时 2ms。超时参数只是“防止死等”，正常情况一个字节 87us 就发完了 */
void bsp_uart_send(uint8_t data) 
{
	HAL_UART_Transmit(&sUartxHandle, &data, 1, 2);
}

/* 接收一个字节：超时 1ms，返回 HAL 状态（HAL_OK=0）。 本工程没有调用者（遥控走射频），保留是为了通用性 */
uint8_t bsp_uart_get_byte(uint8_t* p_byte)
{
	return HAL_UART_Receive(&sUartxHandle, p_byte, 1, 1);
}



int fputc(int ch, FILE *f) 
{
	bsp_uart_send(ch);
	return ch;	
}



