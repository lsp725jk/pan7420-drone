/**
 * 文件：user/spi.h                        【角色】SPI1 总线接口与两个片选宏
 */
#ifndef SPI_H
#define SPI_H

/**
 * 包含其他头文件
 */
#include "py32f0xx_hal.h"
#include "bsp.h"

/* BMP390 片选：PB7（低有效）。与 py32f0xx_hal_msp.c 里 HAL_SPI_MspInit() 配的 PB7 输出脚对应 */
#define SPI_BMP390_CS_LOW()      HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_RESET)
#define SPI_BMP390_CS_HIGH()     HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_SET)

/* ICM42688 片选：PA8（低有效）。用前必须先把 BMP390 的 CS 拉高，防止两个从机抢 MISO 线 */
#define SPI_ICM42688_CS_LOW()      HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_RESET)
#define SPI_ICM42688_CS_HIGH()     HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_SET)



/* 前置声明：icm426 驱动内部的串行接口结构体，这里只用它的名字，不关心内容 */
struct inv_icm426xx_serif;

/* 全局变量声明 */
/* SPI1 句柄：定义在 common.c（不在 spi.c）。MSP 初始化、收发都依赖它 */
extern SPI_HandleTypeDef Spi1Handle;

/* 初始化 SPI1（主机、模式0、750kHz）；被 bsp_init() 调用 */
uint8_t spi_init(void);
/* 注意：spi_deinit() 只有声明，本工程没有实现也没有调用（历史遗留），未改动 */
uint8_t spi_deinit(void);



// 修改后的函数声明
int ICM42688_read_reg(struct inv_icm426xx_serif *serif, uint8_t reg, uint8_t *buf, uint32_t len);
int ICM42688_write_reg(struct inv_icm426xx_serif *serif, uint8_t reg, const uint8_t *buf, uint32_t len);

int inv_io_hal_read_reg(struct inv_icm426xx_serif *serif, uint8_t reg, uint8_t *rbuffer,
                        uint32_t rlen);
int inv_io_hal_write_reg(struct inv_icm426xx_serif *serif, uint8_t reg, const uint8_t *wbuffer,
                         uint32_t wlen);

uint8_t icm42688_read_reg(uint8_t reg);
uint8_t icm42688_read_regs(uint8_t reg, uint8_t* buf, uint16_t len);
uint8_t icm42688_write_reg(uint8_t reg, uint8_t value);

bool bmp3_spi_Write(uint8_t reg, uint8_t data);
bool bmp3_spi_ReadBuf(uint8_t reg, uint8_t * data, uint8_t length);

#endif /* __BSP_SPI_W25Q64_H */

