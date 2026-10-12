/**
 * @note Copyright (C) Shanghai Panchip Microelectronics Co., Ltd.
 * @file bsp.c
 * @brief
 * @history - V0.01, 2022-02-17, yangyaming, first implementation.
 */
/**
 * 文件：drivers/bsp/bsp.c                 【角色】板级支持包（BSP）初始化汇总 +
 */

#include "bsp.h"
#include "spi.h"



double baroPressure, baroTemperature;
/* @description	: bsp外设初始化 @param		: 无 @return		: 无 */
/* @brief  板级外设初始化总入口（上电后按顺序把要用的外设都点一遍） 【参数】 无 【返回】 无 【算法/为什么是这个顺序】 1) bsp_rfspi_init()  —— 先把软件 SPI 的 4 个 GPIO 配好并置成空闲电平。 */
void bsp_init(void)
{
	bsp_rfspi_init();

	bsp_pn298_init();

	bsp_uart_init();

// bsp_timer_init();
	
	spi_init();

// bmp390_init_spi();
}



// // 读取气压和海拔
// void get_pressure_altitude(float *pressure, float *altitude) {
// uint32_t press_raw, temp_raw;
// float press_pa, temp_c;

// // 读取温度和气压
// if (bmp390_read_temperature_pressure(&bmp, &temp_raw, &temp_c,
// &press_raw, &press_pa) == 0) {
// *pressure = press_pa;  // 单位：Pa
// //				printf("temperature: %.2f ",temp_c);
// // 计算海拔高度（国际标准公式）
// const float sea_level_pressure = 101325.0f; // 海平面标准气压
// *altitude = 44330.0f * (1.0f - powf(press_pa / sea_level_pressure, 1/5.255f));
// }
// }

// bool baro_update(void)
// {
// uint32_t press_raw, temp_raw;
// float press_pa, temp_c;

// // 读取温度和气压
// if (bmp390_read_temperature_pressure(&bmp, &temp_raw, &temp_c, &press_raw, &press_pa) == 0)
// {
// baroPressure=lrintf(press_pa);
// baroTemperature=lrintf(temp_c);
// return true;
// }
// return false;
// }



/* @brief  给 ICM42688 官方驱动使用的“微秒级延时”回调 【参数】 @param us 要延时的微秒数 【返回】 无 【为什么要这么一个函数】 */
void inv_icm426xx_sleep_us(uint32_t us)
{
	delay_us(us);
}

/* @brief  给 ICM42688 官方驱动使用的“取当前时间戳”回调 【参数】 无 【返回】 固定的 0 【为什么返回 0 也可以】 */
uint64_t inv_icm426xx_get_time_us(void)
{
	return 0;
}



/* @brief  把 ICM42688 官方驱动的“串行接口描述结构体”接到本平台的读写函数上 【参数】 @param icm_serif  InvenSense 官方定义的“串行接口描述”结构体指针， 由调用者（icm426/icm42688.c）分配好传进来，本函数负责填字段。 */
void SetupMCUHardware(struct inv_icm426xx_serif *icm_serif)
{
    
				/* Initialize serial inteface between MCU and Icm426xx */
	icm_serif->context    = 0; 		/* no need */
	icm_serif->read_reg   = inv_io_hal_read_reg;
	icm_serif->write_reg  = inv_io_hal_write_reg;
	icm_serif->max_read   = 1024 * 3; 		/* maximum number of bytes allowed per serial read */
	icm_serif->max_write  = 1024 * 3; 		/* maximum number of bytes allowed per serial write */
	icm_serif->serif_type = 0;
		// inv_io_hal_init(icm_serif);
}

