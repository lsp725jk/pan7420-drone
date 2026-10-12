/**
 * @note      Copyright (C) Shanghai Panchip Microelectronics Co., Ltd.
 * @file      bsp_uart.c
 * @brief     uart驱动
 * @history - V1.0, 2022-02-22, yangyaming, first implementation.
 */
/**
 * 文件：drivers/bsp/bsp_uart.c            【角色】串口（USART1）驱动 + printf 重定向
 */
#include "bsp_uart.h"

/* 全局的串口句柄。HAL 库的所有串口函数都要拿它当第一个参数； 用 {0} 显式初始化，保证上电时各字段是确定的 0，而不是栈/内存里的随机值。 */
UART_HandleTypeDef sUartxHandle = {0};

/* ---- 串口硬件参数集中定义（换板子只改这一段） ---- */
#define UARTx											USART1
#define BAUDRATE										115200
																												/* 115200 是调试串口的通用速率： 太快则普通 USB-TTL 容易出错， 太慢则打印一句话要等很久 */
#define UARTx_TX_PORT									GPIOB
#define UARTx_TX_PIN									GPIO_PIN_8
#define UARTx_RX_PORT									GPIOB
#define UARTx_RX_PIN									GPIO_PIN_2
#define UARTx_TX_ALTERNATE_AFn							GPIO_AF8_USART1
#define UARTx_RX_ALTERNATE_AFn							GPIO_AF0_USART1
																																																								/* 复用功能编号（AF, Alternate Function）： 一个 GPIO 引脚内部可以接到多个外设， 靠 AF 编号选“接到哪一个”。 【注意】同一路 USART1 的 TX 用 AF8、 */
#define UARTx_IRQn										USART1_IRQn
																																																								/* 【注意】定义了中断号但本文件没用它， 本驱动是纯查询/阻塞式，不开串口中断 （未改动） */

#define LENGTH												10
																																																								/* 【注意】宏 LENGTH 在本文件里没有任何引用， 是历史残留（未改动） */

/* Exported macros------------------------------------------------------------- */
/* 下面三个宏把“某个具体外设的时钟使能”抽象成“本串口的时钟使能”， 换 UARTx 时这几行的目标会跟着变 —— 宏嵌套宏是 C 里做“参数化驱动”的常用手法。 */
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
	HAL_UART_Init(&sUartxHandle);
}



/* @brief  把 PB2/PB8 两个引脚配成 USART1 的复用功能 【参数】 无 【返回】 无 【算法】 1) 打开 GPIOB 的时钟（和串口外设一样，GPIO 也要先开时钟）。 */
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

/* @brief  串口初始化入口（先配 GPIO，再配外设） 【参数】 无 【返回】 无 【调用关系】bsp.c: bsp_init() 调用（排在射频初始化之后）。 顺序“先 GPIO 后外设”不是硬性的（HAL 两种顺序都能跑）， */
void bsp_uart_init(void)
{
	__bsp_uart_gpio_init();
	__bsp_uart_init();
}

/* @brief  阻塞发送 1 个字节 【参数】 @param data 要发送的字节 【返回】 无 【算法】HAL_UART_Transmit(&句柄, &数据, 长度1, 超时2ms)： */
void bsp_uart_send(uint8_t data) 
{
	HAL_UART_Transmit(&sUartxHandle, &data, 1, 2);
}

/* @brief  查询方式接收 1 个字节（带 1ms 超时） 【参数】 @param p_byte 收到的字节放到这里 【返回】 HAL 状态码：HAL_OK(0) 表示收到了，HAL_TIMEOUT(3) 之类表示超时/出错 【返回值语义提醒（容易搞反）】 */
uint8_t bsp_uart_get_byte(uint8_t* p_byte)
{
	return HAL_UART_Receive(&sUartxHandle, p_byte, 1, 1);
}

/* @brief  printf 重定向钩子：C 库每输出一个字符就会调用它一次 【参数】 @param ch  要输出的字符（其实是 int，因为 EOF 要用 -1 表示） @param f   文件流指针（printf 时是 stdout），本函数不需要它 */
int fputc(int ch, FILE *f) 
{
	bsp_uart_send(ch);
	return ch;	
}
