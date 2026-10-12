/**
 * @note Copyright (C) Shanghai Panchip Microelectronics Co., Ltd.
 * @file bsp_i2c_hw.c
 * @brief
 * @history - V0.01, 2022-02-17, yangyaming, first implementation.
 */
/**
 * 文件：drivers/bsp/bsp_i2c_hw.c          【角色】硬件 I2C 主机驱动（接气压计等
 */
#include "bsp_i2c_hw.h"



#define __bsp_i2c_hw_mutex_lock()       __disable_irq()
#define __bsp_i2c_hw_mutex_unlock()     __enable_irq()

/* I2C 外设句柄（全局）：HAL 的 I2C 接口都要拿它当参数。 */
I2C_HandleTypeDef I2C_InitStructure;

/* @description	: i2c外设使能 @param		: 无 @return		: 无 */
/* @brief  打开 I2C 外设与所用 GPIO 端口的时钟 【参数】 无 【返回】 无 【算法】外设时钟使能是使用任何外设的第一步：时钟不开，后面写它的寄存器 完全没有反应（读回来全是 0）。这里同时打开了 GPIOB（因为 SCL/SDA 在 PB4/PB5） */
static void __bsp_i2c_hw_rcc_init(void)
{
		// 外设时钟使能
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_I2C_CLK_ENABLE();
}
/* @description	: i2c gpio初始化 @param		: 无 @return		: 无 */
/* @brief  把 PB5/PB4 配成 I2C 的开漏复用功能引脚 【参数】 无 【返回】 无 【算法/逐字段说明】 Pin         = GPIO_PIN_5 / GPIO_PIN_4   先配 PB5，再配 PB4 */
static void __bsp_i2c_hw_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
				/* SDA 配置 */
	GPIO_InitStructure.Pin = GPIO_PIN_5;
	GPIO_InitStructure.Mode = GPIO_MODE_AF;
	GPIO_InitStructure.Alternate = GPIO_AF4_I2C_SCL;
	GPIO_InitStructure.OpenDrain = GPIO_OPENDRAIN;
	GPIO_InitStructure.Debounce.Enable = GPIO_DEBOUNCE_DISABLE;
	GPIO_InitStructure.SlewRate = GPIO_SLEW_RATE_LOW;
	GPIO_InitStructure.DrvStrength = GPIO_DRV_STRENGTH_LOW;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
	
				/* SCL 配置 */
	GPIO_InitStructure.Pin = GPIO_PIN_4;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
}
/* @description	: i2c外设初始化 @param		: 无 @return		: 无 */
/* @brief  初始化 I2C 外设本体（主机模式、速率、广播应答） 【参数】 无 【返回】 无 【算法/逐字段说明】 Instance    = I2C                     本芯片只有一个 I2C 外设 */
static void __bsp_i2c_hw_init(void)	
{
	I2C_InitStructure.Instance = I2C;
	I2C_InitStructure.Init.master = I2C_MASTER_MODE_ENABLE;
	I2C_InitStructure.Init.slave = I2C_SLAVE_MODE_DISABLE;
	I2C_InitStructure.Mode = HAL_I2C_MODE_MASTER;
	I2C_InitStructure.Init.broadack = I2C_BROAD_ACK_DISABLE;
	I2C_InitStructure.Init.speedclock = 500;
	I2C_InitStructure.State = HAL_I2C_STATE_RESET;

	HAL_I2C_Init(&I2C_InitStructure);
}

/* @description	: i2c初始化 @param		: 无 @return		: 无 */
/* @brief  I2C 初始化入口（时钟 → GPIO → 外设，三步走） 【参数】 无 【返回】 无 【算法】顺序必须是“先开时钟、再配引脚、最后配外设”： 引脚复用（AF）的配置依赖 GPIO 时钟，而外设一使能就会去驱动引脚， */
void bsp_i2c_hw_init(void)
{
	__bsp_i2c_hw_rcc_init();
	__bsp_i2c_hw_gpio_init();
	__bsp_i2c_hw_init();
}
/* @description	: 读取i2c单字节 @param		: 无 @return		: 读取到的字节 */
/* @brief  从从设备的某个寄存器读 1 个字节 【参数】 @param dev  从设备地址（7 位地址，HAL 会自动左移并补上读写位） @param reg  要读的寄存器地址 */
uint8_t bsp_i2c_hw_read_byte(uint8_t dev, uint8_t reg)
{
	uint8_t ret;
    __bsp_i2c_hw_mutex_lock();
	HAL_I2C_Master_Receive(&I2C_InitStructure, dev, &reg, 1, &ret, 1);
	__bsp_i2c_hw_mutex_unlock();
	return ret;
}
/* @description	: 读取i2c多字节 @param		: 无 @return		: 无 */
/* @brief  从从设备的某个寄存器开始，连续读 len 个字节 【参数】 @param dev  从设备地址 @param reg  起始寄存器地址 */
void bsp_i2c_hw_read_bytes(uint8_t dev, uint8_t reg, uint8_t* buf, uint32_t len)
{
    __bsp_i2c_hw_mutex_lock();;
	HAL_I2C_Master_Receive(&I2C_InitStructure, dev, &reg, 1, buf, len);
	__bsp_i2c_hw_mutex_unlock();
}
/* @description	: 写i2c单字节 @param		: 无 @return		: 无 */
/* @brief  向从设备的某个寄存器写 1 个字节 【参数】 @param dev   从设备地址 @param reg   寄存器地址 */
void bsp_i2c_hw_wrtie_byte(uint8_t dev, uint8_t reg, uint8_t ubyte)
{
	bsp_i2c_hw_write_bytes(dev, reg, &ubyte, 1);
}

/* @description	: 写i2c多字节 @param		: 无 @return		: 无 */
/* 【语法讲解：文件级全局数组 data[]】 大小是 1 个元素，但下面在循环里写 data[i+1]（i 可以到 len-1）—— 也就是会写到数组边界之外！这是典型的“缓冲区溢出”。 之所以当前没出事：I2C 传感器驱动的调用长度一般只有几个字节， */
uint8_t data[] = {0};
/* @brief  向从设备的某个寄存器开始，连续写 len 个字节 【参数】 @param dev  从设备地址 @param reg  起始寄存器地址 */
void bsp_i2c_hw_write_bytes(uint8_t dev, uint8_t reg, const uint8_t* buf, uint32_t len)
{
	uint8_t i;
	
    __bsp_i2c_hw_mutex_lock();
	data[0] = reg;
	for(i = 0; i < len; i++){
		data[i+1] = buf[i];
	}
    HAL_I2C_Master_Transmit(&I2C_InitStructure, dev, data, len + 1);
	__bsp_i2c_hw_mutex_unlock();
}
