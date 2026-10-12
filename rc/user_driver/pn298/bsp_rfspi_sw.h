/**
 * @note      Copyright (C) Shanghai Panchip Microelectronics Co., Ltd.
 * @file      bsp_spi_sw.h
 * @brief     gpio 模拟spi 驱动
 * @history - V1.0, 2022-08-08, yangyaming, first implementation.
 */
#ifndef __BSP_RFSPI_SW_H
#define __BSP_RFSPI_SW_H
 
#include "bsp.h"



typedef struct{
	struct{
		GPIO_TypeDef* port;
		uint32_t pin;
	}clk;
	
	struct{
		GPIO_TypeDef* port;
		uint32_t pin;
	}cs;
	
	struct{
		GPIO_TypeDef* port;
		uint32_t pin;
	}mosi;
	
	struct{
		GPIO_TypeDef* port;
		uint32_t pin;
	}miso;
	
	union{
        uint32_t code;
        struct{
            uint16_t l;
            uint16_t h;
        }clk_tim_code;
    }speed;
}swspi_pan742_t;

#define __rfspi_pan742_delay                  __delay_pan742
#define __rfspi_pan742_set_mosi()           (GPIOA->BSRR = (uint32_t)GPIO_PIN_0)
#define __rfspi_pan742_clr_mosi()           (GPIOA->BRR = (uint32_t)GPIO_PIN_0)

#define __rfspi_pan742_set_miso()           (GPIOF->BSRR = (uint32_t)GPIO_PIN_3)
#define __rfspi_pan742_clr_miso()           (GPIOF->BRR = (uint32_t)GPIO_PIN_3)

#define __rfspi_pan742_set_clk()           	(GPIOA->BSRR = (uint32_t)GPIO_PIN_1)
#define __rfspi_pan742_clr_clk()           	(GPIOA->BRR = (uint32_t)GPIO_PIN_1)

#define __rfspi_pan742_set_cs()           	(GPIOF->BSRR = (uint32_t)GPIO_PIN_1)
#define __rfspi_pan742_clr_cs()           	(GPIOF->BRR = (uint32_t)GPIO_PIN_1)



#define __rfspi_pan742_write_mosi(val)     	((val)?__rfspi_pan742_set_mosi():__rfspi_pan742_clr_mosi())
#define __rfspi_pan742_write_miso(val)     	((val)?__rfspi_pan742_set_miso():__rfspi_pan742_clr_miso())
#define __rfspi_pan742_write_clk(val)      	((val)?__rfspi_pan742_set_clk():__rfspi_pan742_clr_clk())
#define __rfspi_pan742_write_cs(val)  		((val)?__rfspi_pan742_set_cs():__rfspi_pan742_clr_cs())

#define __rfspi_pan742_read_miso()      ((GPIOF->IDR&(uint32_t)GPIO_PIN_3)?1:0)



void swspi_pan742_init(swspi_pan742_t* hspi, uint32_t spi_speed,
                       uint32_t mosi_port_n, uint32_t mosi_pin_n,
                       uint32_t miso_port_n, uint32_t miso_pin_n,
					   uint32_t clk_port_n, uint32_t clk_pin_n,
					   uint32_t cs_port_n, uint32_t cs_pin_n);
void swspi_pan742_write_bytes(const swspi_pan742_t* hspi, uint8_t reg, const uint8_t* buf, uint32_t len);
void swspi_pan742_read_bytes(const swspi_pan742_t* hspi, uint8_t reg, uint8_t* buf, uint32_t len);

void bsp_rfspi_init(void);
void rfspi_pan742_write_byte(uint8_t ubyte);
uint8_t rfspi_pan742_read_byte(void);
 
#endif
