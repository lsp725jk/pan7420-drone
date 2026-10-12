/**
 * @note Copyright (C) Shanghai Panchip Microelectronics Co., Ltd.
 * @file bsp_i2c_hw.h
 * @brief
 * @history - V0.01, 2022-02-17, yangyaming, first implementation.
 */
/**
 * 文件：drivers/bsp/bsp_i2c_hw.h          【角色】硬件 I2C 主机驱动的接口声明
 */
#ifndef __BSP_I2C_HW_H
#define __BSP_I2C_HW_H



#ifdef __cplusplus
extern "C" {
#endif



#include "cx32l003.h"
#include "cx32l003_hal_conf.h"



/* 初始化：配置时钟、GPIO（开漏复用）、I2C 外设 */
void bsp_i2c_hw_init(void);

/* 读 1 个寄存器（单字节） */
uint8_t bsp_i2c_hw_read_byte(uint8_t dev, uint8_t reg);
/* 从某个寄存器起连读多字节（I2C 传感器最常用，保证同一批数据来自同一时刻） */
void bsp_i2c_hw_read_bytes(uint8_t dev, uint8_t reg, uint8_t* buf, uint32_t len);

void bsp_i2c_hw_write_byte(uint8_t dev, uint8_t reg, uint8_t ubyte);
/* 从某个寄存器起连写多字节（实现里会先把寄存器地址和数据拼成一个缓冲区再发） */
void bsp_i2c_hw_write_bytes(uint8_t dev, uint8_t reg, const uint8_t* buf, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif // __LIB_DRIVER_RFIIC_PAN308_H
/* 【注意】#endif 行尾注释写的是 __LIB_DRIVER_RFIIC_PAN308_H， 与真正的宏名 __BSP_I2C_HW_H 不一致，属于改名时漏改注释（未改动）。 */
