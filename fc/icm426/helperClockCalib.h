/**
 * and other intellectual property rights laws.
 */
#ifndef _STANDALONE_HELPER_H_
#define _STANDALONE_HELPER_H_

#include <stdint.h>

#include "Icm426xxDefs.h"
#include "Icm426xxExtFunc.h"
#include "Icm426xxDriver_HL.h"

/**
 * Max value for 20 bits and 16 bits counter
 */
#define ROLLOVER_20BITS 0x100000
#define ROLLOVER_16BITS 0x10000

/* Time between 2 MCU/ICM clock calibrations and for 1 calibration */
#define TIME_US_FOR_CLOCK_CALIBRATION 1200000

/* Number of neccessary rollover occured during the TIME_US_FOR_CLOCK_CALIBRATION */
#define NB_ROLLOVER (uint8_t)(TIME_US_FOR_CLOCK_CALIBRATION / TIME_US_FOR_20BITS_TMSP_ROLLOVER)



#define NUMBER_OF_SAMPLES_FOR_ODR(odr_us)                                                          \
	TIME_US_FOR_CLOCK_CALIBRATION / (odr_us) + TIME_US_FOR_CLOCK_CALIBRATION % (odr_us);

/* Interrupt enum state for INT1, INT2, and IBI */
enum inv_icm426xx_clock_source {
	INV_ICM426XX_PLL,
	INV_ICM426XX_RC_OSC,
	INV_ICM426XX_WU_OSC,
	INV_ICM426XX_CLOCK_SOURCE_MAX
};

/* Coefficient to interpolate the linear regression between the ICM time and the MCU time */
typedef struct clk_calib {
	int      initial_recalib_after_n_samples;
	int      recalib_after_n_samples;
	uint8_t  on_going;
	float    coef[INV_ICM426XX_CLOCK_SOURCE_MAX]; 		/* < Calibration coefficient */
	uint64_t last_timestamp_sent
	    [INV_ICM426XX_SENSOR_MAX]; 	    	    /* < last timestamp sent when using the extended timestamp API（应用程序接口） */
	uint32_t last_fifo_timestamp[INV_ICM426XX_SENSOR_MAX]; 		/* < last timestamp read from FIFO（先进先出缓冲队列） */
	float    dt_error; 		/* < keep track of the error on the timestamp due to the usec resolution */
} clk_calib_t;



int inv_helper_extend_timestamp_from_fifo(struct inv_icm426xx *s, struct clk_calib *clk_cal,
                                          uint16_t timestamp_fsync, uint64_t irq_timestamp,
                                          int sensor_mask, uint64_t *timestamp);



void clock_calibration_reset(struct inv_icm426xx *s, struct clk_calib *clk_cal);



void clock_calibration_reset_sensors_stats(struct inv_icm426xx *s, struct clk_calib *clk_cal,
                                           enum inv_icm426xx_sensor);



int clock_calibration_init(struct inv_icm426xx *s, struct clk_calib *clk_cal);



int clock_calibration_update(struct inv_icm426xx *s, struct clk_calib *clk_cal,
                             uint64_t irq_timestamp, uint16_t fifo_timestamp);



int clock_calibration_restart(struct inv_icm426xx *s, struct clk_calib *clk_cal);

/* ! \brief Converter function from period (in usec) to frequency (in Hz) */
uint32_t period_us_to_frequency(const uint32_t period_us);

/* @brief Hook to disable IRQ（中断请求） in low level. */
extern void inv_helper_disable_irq(void);

/* @brief Hook to enable IRQ in low level. */
extern void inv_helper_enable_irq(void);

#endif /* !_STANDALONE_HELPER_H_ */
