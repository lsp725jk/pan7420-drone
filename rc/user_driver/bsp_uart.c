/**
 * @note      Copyright (C) Shanghai Panchip Microelectronics Co., Ltd.
 * @file      bsp_uart.c
 * @brief     uart驱动
 * @history - V1.0, 2022-02-22, yangyaming, first implementation.
 */
#include "bsp_uart.h"

UART_HandleTypeDef sUartxHandle = {0};

#define UARTx											USART2
#define BAUDRATE										115200
#define UARTx_TX_PORT									GPIOA
#define UARTx_TX_PIN									GPIO_PIN_4
#define UARTx_RX_PORT									GPIOB
#define UARTx_RX_PIN									GPIO_PIN_2
#define UARTx_TX_ALTERNATE_AFn							GPIO_AF9_USART2
// #define UARTx_RX_ALTERNATE_AFn							GPIO_AF0_USART1
#define UARTx_IRQn										USART2_IRQn

#define LENGTH												10

/* Exported macros------------------------------------------------------------- */
#define __HAL_RCC_UARTx_CLK_ENABLE()  				__HAL_RCC_USART2_CLK_ENABLE()	
#define __HAL_RCC_UARTx_RX_GPIO_CLK_ENABLE()  		__HAL_RCC_GPIOB_CLK_ENABLE()
#define __HAL_RCC_UARTx_TX_GPIO_CLK_ENABLE()  		__HAL_RCC_GPIOA_CLK_ENABLE()



static void __bsp_uart_init(uint32_t __Baud)
{
	__HAL_RCC_UARTx_CLK_ENABLE();
	sUartxHandle.Instance = UARTx;
	sUartxHandle.Init.BaudRate = __Baud;
	sUartxHandle.Init.WordLength = UART_WORDLENGTH_8B;
	sUartxHandle.Init.StopBits = UART_STOPBITS_1;
	sUartxHandle.Init.Parity = UART_PARITY_NONE;
	sUartxHandle.Init.Mode = UART_MODE_TX;
	HAL_UART_Init(&sUartxHandle);
}



static void __bsp_uart_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

        // __HAL_RCC_UARTx_RX_GPIO_CLK_ENABLE();
	__HAL_RCC_UARTx_TX_GPIO_CLK_ENABLE();
        /* if UARTx is UART0 GPIO Configuration: PB2     ------> USART1_RXD PB8     ------> USART1_TXD */
// GPIO_InitStruct.Pin = UARTx_RX_PIN;
// GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
// GPIO_InitStruct.Alternate = UARTx_RX_ALTERNATE_AFn;
// HAL_GPIO_Init(UARTx_RX_PORT, &GPIO_InitStruct);
		
    GPIO_InitStruct.Pin = UARTx_TX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Alternate = UARTx_TX_ALTERNATE_AFn;
    HAL_GPIO_Init(UARTx_TX_PORT, &GPIO_InitStruct);	
}

void bsp_uart_init(uint32_t __Baud)
{
	__bsp_uart_gpio_init();
	__bsp_uart_init(__Baud);
}

void CRSF_send(uint8_t *package, uint8_t len) // 发送CRSF协议包
{
    HAL_UART_Transmit(&sUartxHandle, package, len, 4);
}

void bsp_uart_send(uint8_t data) 
{
	HAL_UART_Transmit(&sUartxHandle, &data, 1, 2);
}

uint8_t bsp_uart_get_byte(uint8_t* p_byte)
{
	return HAL_UART_Receive(&sUartxHandle, p_byte, 1, 1);
}

int fputc(int ch, FILE *f) 
{
	bsp_uart_send(ch);
	return ch;	
}
