/**
 * @note      Copyright (C) Shanghai Panchip Microelectronics Co., Ltd.
 * @file      bsp_rfspi_sw.c
 * @brief     gpio 模拟spi 驱动
 * @history - V1.0, 2022-08-08, yangyaming, first implementation.
 */
/**
 * 文件：drivers/bsp/bsp_rfspi_sw.c        【角色】软件模拟 SPI（bit-banging）的
 */
#include "bsp_rfspi_sw.h"
/* PAN7420  PAN1026 PF1     CSN PF3	   MISO PA0	   MOSI */
/* 【上面这段是作者原有的接线表，读代码时以此为准】 CSN  (片选) = PF1 —— 低电平有效：CS=0 表示“我要跟芯片说话了” MISO (收)   = PF3 —— 接芯片的输出，MCU 侧配成输入 MOSI (发)   = PA0 —— MCU 侧配成推挽输出 */
 
/* @brief  软件 SPI：向 PN298 发送 1 个字节（高位在前，MSB first） 【参数】 @param ubyte  要发送的字节 【返回】 无 */
 void rfspi_pan742_write_byte(uint8_t ubyte)
{
	uint8_t i;  
    for(i = 0; i < 8; i++){
		__rfspi_pan742_write_clk(0); 
// __rfspi_pan742_delay(1);
		if(ubyte & 0x80){
			__rfspi_pan742_write_mosi(1);
		}else{
			__rfspi_pan742_write_mosi(0);
		}
		ubyte <<= 1;
		__rfspi_pan742_write_clk(1);  
		__rfspi_pan742_delay(1);	
    } 
	__rfspi_pan742_write_clk(0);
	__rfspi_pan742_delay(5);
}

/* @brief  软件 SPI：从 PN298 接收 1 个字节（高位在前，MSB first） 【参数】 无 【返回】 读到的字节 【算法：8 个比特是怎么一位一位采回来的】 */
uint8_t rfspi_pan742_read_byte(void)
{
	uint8_t i;  
	uint8_t ret = 0;  
	for(i = 0; i < 8; i++) {  
		__rfspi_pan742_write_clk(0);  
		__rfspi_pan742_delay(1);   	
		__rfspi_pan742_write_clk(1);
		ret <<= 1;
		if(__rfspi_pan742_read_miso()){
			ret |= 0x01;
		} 
	}
	__rfspi_pan742_write_clk(0);
	__rfspi_pan742_delay(5);
	return ret;
}

/* @brief  初始化软件 SPI 用的 4 个 GPIO（MOSI/CLK/CS 输出、MISO/IRQ 输入） 【参数】 无 【返回】 无 【算法/逐段说明】 1) 开时钟：任何 GPIO 在使用前必须先打开它所在端口的时钟 */
void bsp_rfspi_init(void)
{
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	
		// MOSI（SPI 主出从入数据线）
  GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		// CLK
	GPIO_InitStruct.Pin = GPIO_PIN_1;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		// CS.

	GPIO_InitStruct.Pin = GPIO_PIN_1;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
		// MISO（SPI 主入从出数据线）
	GPIO_InitStruct.Pin = GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
	
		// IRQ（中断请求）
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);


	
	__rfspi_pan742_write_mosi(0);
	__rfspi_pan742_write_clk(0);
	__rfspi_pan742_write_cs(1);
	
}
