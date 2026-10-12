/**
 * @note      Copyright (C) Shanghai Panchip Microelectronics Co., Ltd.
 * @file      bsp_spi_sw.h
 * @brief     gpio 模拟spi 驱动
 * @history - V1.0, 2022-08-08, yangyaming, first implementation.
 */
/**
 * 文件：drivers/bsp/bsp_rfspi_sw.h        【角色】软件模拟 SPI 的“引脚定义 +
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

/* MISO 在正常工作时是输入，这两个“把 MISO 当输出用”的宏本工程没有使用 （双向 SPI / 复用同一根线时才会需要）。 */
#define __rfspi_pan742_set_miso()           (GPIOF->BSRR = (uint32_t)GPIO_PIN_3)
#define __rfspi_pan742_clr_miso()           (GPIOF->BRR = (uint32_t)GPIO_PIN_3)

#define __rfspi_pan742_set_clk()           	(GPIOA->BSRR = (uint32_t)GPIO_PIN_1)
#define __rfspi_pan742_clr_clk()           	(GPIOA->BRR = (uint32_t)GPIO_PIN_1)

/* CS 片选：低电平有效。set_cs = 拉高 = 释放芯片（本次通信结束）。 每一次寄存器读写都必须是“拉低 CS → 发命令和数据 → 拉高 CS”这样一个完整回合。 */
#define __rfspi_pan742_set_cs()           	(GPIOF->BSRR = (uint32_t)GPIO_PIN_1)
#define __rfspi_pan742_clr_cs()           	(GPIOF->BRR = (uint32_t)GPIO_PIN_1)



/* 四个“按值翻电平”的宏函数。语法要点： (val) ? A : B   —— 三目（条件）运算符：条件为真取 A，为假取 B。 参数 val 外面必须加括号：因为宏是文本替换，若写成 val?A:B， 调用处传 1+1 会被替换成 1+1?A:B 从而算错优先级。 */
#define __rfspi_pan742_write_mosi(val)     	((val)?__rfspi_pan742_set_mosi():__rfspi_pan742_clr_mosi())
#define __rfspi_pan742_write_miso(val)     	((val)?__rfspi_pan742_set_miso():__rfspi_pan742_clr_miso())
#define __rfspi_pan742_write_clk(val)      	((val)?__rfspi_pan742_set_clk():__rfspi_pan742_clr_clk())
#define __rfspi_pan742_write_cs(val)  		((val)?__rfspi_pan742_set_cs():__rfspi_pan742_clr_cs())



#define __rfspi_pan742_read_miso()      ((GPIOF->IDR&(uint32_t)GPIO_PIN_3)?1:0)



/* 【注意】以下三个函数在本工程中只声明、没有定义，也没有任何调用点， 属于上面 swspi_pan742_t 通用化改造的半成品（保留原样未改动）。 */
void swspi_pan742_init(swspi_pan742_t* hspi, uint32_t spi_speed,
                       uint32_t mosi_port_n, uint32_t mosi_pin_n,
                       uint32_t miso_port_n, uint32_t miso_pin_n,
					   uint32_t clk_port_n, uint32_t clk_pin_n,
					   uint32_t cs_port_n, uint32_t cs_pin_n);
void swspi_pan742_write_bytes(const swspi_pan742_t* hspi, uint8_t reg, const uint8_t* buf, uint32_t len);
void swspi_pan742_read_bytes(const swspi_pan742_t* hspi, uint8_t reg, uint8_t* buf, uint32_t len);

/* 本工程真正使用的三个接口，实现在 bsp_rfspi_sw.c */
void bsp_rfspi_init(void);						/* 配置 4 个 GPIO 并置成空闲电平 */
void rfspi_pan742_write_byte(uint8_t ubyte);	/* 软件 SPI 发 1 字节（MSB first） */
uint8_t rfspi_pan742_read_byte(void);			/* 软件 SPI 收 1 字节（MSB first） */
 
#endif
/* 【注意】文件头的 @file 写的是 bsp_spi_sw.h，实际文件名是 bsp_rfspi_sw.h， 属于改名时漏改注释，不影响编译（未改动）。 */
