/**
 * and other intellectual property rights laws.
 */

/**
 * @defgroup Transport Transport @brief    Abstraction layer to communicate with device. @{
 */

/**
 * @file Icm426xxTransport.h
 */

#ifndef _INV_ICM426XX_TRANSPORT_H_
#define _INV_ICM426XX_TRANSPORT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * forward declaration
 */
struct inv_icm426xx;

/* Available serial interface type. 可用的串口类型。 */
#define ICM426XX_UI_I2C    0 /* < identifies I2C（两线串行总线） interface. */
#define ICM426XX_UI_SPI4   1 /* < identifies 4-wire SPI（串行外设接口） interface. */
#define ICM426XX_UI_I3C    2 /* < identifies I3C interface. */
#define ICM426XX_AUX1_SPI3 3 /* < identifies 3-wire SPI interface for AUX1. */
#define ICM426XX_AUX2_SPI3 4 /* < identifies 3-wire SPI interface for AUX2. */
#define ICM426XX_AUX1_SPI4 5 /* < identifies 4-wire SPI interface for AUX1. */

/* @brief Serif type definition.衬线字体定义。 in `struct inv_icm426xx_serif` struct. */
typedef uint32_t ICM426XX_SERIAL_IF_TYPE_t;

/* @brief base sensor serial interface 基本传感器串行接口 */
struct inv_icm426xx_serif {
	void *context;
	int (*read_reg)(struct inv_icm426xx_serif *serif, uint8_t reg, uint8_t *buf, uint32_t len);
	int (*write_reg)(struct inv_icm426xx_serif *serif, uint8_t reg, const uint8_t *buf,
	                 uint32_t len);
	int (*configure)(struct inv_icm426xx_serif *serif);
	uint32_t max_read;
	uint32_t max_write;
	uint32_t serif_type;
};

/* @brief transport interface 串行数据传输接口 */
struct inv_icm426xx_transport {
	struct inv_icm426xx_serif
	    serif; 	    	    /* < Warning : this field MUST be the first one of struct inv_icm426xx_transport */

				/* @brief Contains mirrored values of some IP registers */
	struct register_cache {
		uint8_t intf_cfg_1_reg; 				/* < INTF_CONFIG1, Bank: 0, Address: 0x4D */
		uint8_t pwr_mngt_0_reg; 				/* < PWR_MGMT_0, Bank: 0, Address: 0x4E */
		uint8_t gyro_cfg_0_reg; 				/* < GYRO_CONFIG0, Bank: 0, Address: 0x4F */
		uint8_t accel_cfg_0_reg; 				/* < ACCEL_CONFIG0, Bank: 0, Address: 0x50 */
		uint8_t tmst_cfg_reg; 				/* < TMST_CONFIG, Bank: 0, Address: 0x54 */
		uint8_t bank_sel_reg; 				/* < MPUREG_REG_BANK_SEL, All banks, Address 0x76 */
	} register_cache; 
};



int inv_icm426xx_init_transport(struct inv_icm426xx *s);



int inv_icm426xx_read_reg(struct inv_icm426xx *s, uint8_t reg, uint32_t len, uint8_t *buf);



int inv_icm426xx_write_reg(struct inv_icm426xx *s, uint8_t reg, uint32_t len, const uint8_t *buf);

#ifdef __cplusplus
}
#endif

#endif /* _INV_ICM426XX_TRANSPORT_H_ */

/* @} */
