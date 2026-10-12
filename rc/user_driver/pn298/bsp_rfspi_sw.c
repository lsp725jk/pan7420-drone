/**
 * @note      Copyright (C) Shanghai Panchip Microelectronics Co., Ltd.
 * @file      bsp_rfspi_sw.c
 * @brief     gpio 模拟spi 驱动
 * @history - V1.0, 2022-08-08, yangyaming, first implementation.
 */
#include "bsp_rfspi_sw.h"
/**
 * PAN7420  PAN1026 PF1     CSN PF3	   MISO PA0	   MOSI
 */
 
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
		// CS
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
