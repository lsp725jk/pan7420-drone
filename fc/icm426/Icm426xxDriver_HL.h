/**
 * and other intellectual property rights laws.
 */

/**
 * @defgroup Driver Driver @brief High-level API to drive the device. @{
 */

/**
 * @file Icm426xxDriver_HL.h
 */

#ifndef _INV_ICM426xx_DRIVER_HL_H_
#define _INV_ICM426xx_DRIVER_HL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "Icm426xxDefs.h"
#include "Icm426xxTransport.h"

#include "InvError.h"

#include <stdint.h>
#include <string.h>



#ifndef INV_ICM426XX_LIGHTWEIGHT_DRIVER
#define INV_ICM426XX_LIGHTWEIGHT_DRIVER 1
#endif

/* @brief Scale factor and max ODR  比例因子和最大ODR Dependant of chip */
#if (defined(ICM42600) || defined(ICM42622) || defined(ICM42633) || defined(ICM42686P) ||          \
     defined(ICM42688P) || defined(ICM42688V) || defined(ICM42686V))

#define PLL_SCALE_FACTOR_Q24      ((32UL << 24) / 30)
#define ICM_PART_DEFAULT_OIS_MODE ICM426XX_SENSOR_CONFIG2_OIS_MODE_32KHZ
#else
#define PLL_SCALE_FACTOR_Q24      (1UL << 24)
#define ICM_PART_DEFAULT_OIS_MODE ICM426XX_SENSOR_CONFIG2_OIS_MODE_8KHZ
#endif

/* @brief Max FSR（满量程） values for accel and gyro Dependant of chip */
#if defined(ICM42686P) || defined(ICM42686V)
#define ACCEL_CONFIG0_FS_SEL_MAX ICM426XX_ACCEL_CONFIG0_FS_SEL_32g
#define GYRO_CONFIG0_FS_SEL_MAX  ICM426XX_GYRO_CONFIG0_FS_SEL_4000dps

#define ACCEL_OFFUSER_MAX_MG 2000
#define GYRO_OFFUSER_MAX_DPS 128
#else
#define ACCEL_CONFIG0_FS_SEL_MAX ICM426XX_ACCEL_CONFIG0_FS_SEL_16g
#define GYRO_CONFIG0_FS_SEL_MAX  ICM426XX_GYRO_CONFIG0_FS_SEL_2000dps

#define ACCEL_OFFUSER_MAX_MG 1000
#define GYRO_OFFUSER_MAX_DPS 64
#endif

/* @brief RTC Support flag Define whether the RTC mode is supported Dependant of chip */
#if (defined(ICM42600) || defined(ICM42622) || defined(ICM42633) || defined(ICM42686P) ||          \
     defined(ICM42688P) || defined(ICM42688V) || defined(ICM42686V))
#define RTC_SUPPORTED 1
#else
#define RTC_SUPPORTED 0
#endif



#define ICM426XX_FIFO_MIRRORING_SIZE 16 * 129 // packet size * max_count = 2064



#define ICM426XX_DEFAULT_WOM_THS_MG 52 >> 2 /* = 52mg/4 */



#define ICM426XX_ACC_STARTUP_TIME_US 20000U

/* @brief Icm426xx Gyroscope start-up time before having correct data */
#define ICM426XX_GYR_STARTUP_TIME_US 60000U

/* @brief Sensor identifier for UI control and OIS function */
enum inv_icm426xx_sensor {
	INV_ICM426XX_SENSOR_ACCEL, 		/* < Accelerometer (UI control path) */
	INV_ICM426XX_SENSOR_GYRO, 		/* < Gyroscope (UI control path) */
	INV_ICM426XX_SENSOR_FSYNC_EVENT, 		/* < Used by OIS and UI control layers */
	INV_ICM426XX_SENSOR_OIS, 		/* < Only used by OIS layer */
	INV_ICM426XX_SENSOR_TEMPERATURE, 
	INV_ICM426XX_SENSOR_TAP, 		/* < Tap and Double tap */
	INV_ICM426XX_SENSOR_DMP_PEDOMETER_EVENT, 		/* < Pedometer: step is detected */
	INV_ICM426XX_SENSOR_DMP_PEDOMETER_COUNT, 		/* < Pedometer: step counter */
	INV_ICM426XX_SENSOR_DMP_TILT, 		/* < Tilt */
#if defined(ICM_FAMILY_BPLUS)
	INV_ICM426XX_SENSOR_DMP_R2W, 		/* < Raise to wake */
#elif defined(ICM_FAMILY_CPLUS)
	INV_ICM426XX_SENSOR_DMP_FF, 		/* < Free Fall */
	INV_ICM426XX_SENSOR_DMP_LOWG, 		/* < Low G */
#endif
	INV_ICM426XX_SENSOR_MAX
};

/* @brief Configure Fifo usage */
typedef enum {
	INV_ICM426XX_FIFO_DISABLED = 0, 		/* < Fifo is disabled and data source is sensors registers */
	INV_ICM426XX_FIFO_ENABLED  = 1, 		/* < Fifo is used as data source */
} INV_ICM426XX_FIFO_CONFIG_t;

/* @brief Sensor event structure definition */
typedef struct {
	int      sensor_mask;
	uint16_t timestamp_fsync;
	int16_t  accel[3];
	int16_t  gyro[3];
	int16_t  temperature;
	int8_t   accel_high_res[3];
	int8_t   gyro_high_res[3];
} inv_icm426xx_sensor_event_t;

/* @brief Icm426xx driver states definition */
struct inv_icm426xx {
				/* @brief Transport structure */
	struct inv_icm426xx_transport transport; 

				/* @brief Callback executed when retrieving data from FIFO or registers. 当从FIFO或寄存器检索数据时执行回调。 */
	void (*sensor_event_cb)(inv_icm426xx_sensor_event_t *event); 

				/* @brief Gyro bias values (lsb) collected during self test */
	int gyro_st_bias[3];

				/* @brief Accel bias values (lsb) collected during self test */
	int accel_st_bias[3];

				/* @brief Flag to keep track if self-test has already been executed */
	int st_result;

				/* @brief FIFO mirroring memory area */
	uint8_t fifo_data[ICM426XX_FIFO_MIRRORING_SIZE]; 

				/* @brief Internal counter to keep track of the timestamp to register access availability */
	uint8_t tmst_to_reg_en_cnt; 

				/* @brief DMP（数字运动处理器） started status */
	uint8_t dmp_is_on;
	
				/* @brief DMP executes from SRAM（静态随机存储器） */
	uint8_t dmp_from_sram;

				/* @brief Internal state needed to discard first gyro samples */
	uint64_t gyro_start_time_us;
	
				/* @brief Internal state needed to discard first accel samples */
	uint64_t accel_start_time_us;

				/* @brief Internal status of data endianness mode to report correctly data */
	uint8_t endianess_data;

				/* @brief Indicates if highres mode is used for FIFO */
	uint8_t fifo_highres_enabled;

				/* @brief Indicates if data are retrieved from FIFO or registers */
	INV_ICM426XX_FIFO_CONFIG_t fifo_is_used;

#if (!INV_ICM426XX_LIGHTWEIGHT_DRIVER)
		/* @brief Indicates if the next FSYNC（帧同步） should be ignored. FSYNC value can be generated on the next first ODR. */
	uint8_t fsync_to_be_ignored;
#endif

		/* @brief Accel Low Power could report with wrong ODR（输出数据速率） if internal counter dummy ODR changes when re-enabling WU after 8*n ODR changes. */
	uint32_t wu_off_acc_odr_changes;

				/* @brief Keeps track if wom or smd is enabled */
	uint8_t wom_smd_mask;

				/* @brief Keeps track if wom is enabled */
	uint8_t wom_enable;



	struct {
		uint8_t acc_lp_avg; 				/* < Low-power averaging setting for accelerometer */
		uint8_t reserved; 				/* < reserved field */
		uint8_t acc_ln_bw; 				/* < Low-noise filter bandwidth setting for accelerometer */
		uint8_t gyr_ln_bw; 				/* < Low-noise filter bandwidth setting for gyroscope */
	} avg_bw_setting;

				/* @brief Keeps track of timestamp when gyro is power off */
	uint64_t gyro_power_off_tmst;
};

/* Interrupt enum state for INT1, INT2, and IBI */
typedef enum { INV_ICM426XX_DISABLE = 0, INV_ICM426XX_ENABLE } inv_icm426xx_interrupt_value;

/* @brief Icm426xx set of interrupt enable flag */
typedef struct {
	inv_icm426xx_interrupt_value INV_ICM426XX_UI_FSYNC;
	inv_icm426xx_interrupt_value INV_ICM426XX_UI_DRDY;
	inv_icm426xx_interrupt_value INV_ICM426XX_FIFO_THS;
	inv_icm426xx_interrupt_value INV_ICM426XX_FIFO_FULL;
	inv_icm426xx_interrupt_value INV_ICM426XX_SMD;
	inv_icm426xx_interrupt_value INV_ICM426XX_WOM_X;
	inv_icm426xx_interrupt_value INV_ICM426XX_WOM_Y;
	inv_icm426xx_interrupt_value INV_ICM426XX_WOM_Z;
	inv_icm426xx_interrupt_value INV_ICM426XX_STEP_DET;
	inv_icm426xx_interrupt_value INV_ICM426XX_STEP_CNT_OVFL;
	inv_icm426xx_interrupt_value INV_ICM426XX_TILT_DET;
#if defined(ICM_FAMILY_BPLUS)
	inv_icm426xx_interrupt_value INV_ICM426XX_SLEEP_DET;
	inv_icm426xx_interrupt_value INV_ICM426XX_WAKE_DET;
#elif defined(ICM_FAMILY_CPLUS)
	inv_icm426xx_interrupt_value INV_ICM426XX_FF_DET;
	inv_icm426xx_interrupt_value INV_ICM426XX_LOWG_DET;
#endif
	inv_icm426xx_interrupt_value INV_ICM426XX_TAP_DET;
} inv_icm426xx_interrupt_parameter_t;

#if defined(ICM_FAMILY_CPLUS)
/* Possible interface configurations mode */
typedef enum {
	INV_ICM426XX_SINGLE_INTERFACE    = 0x01,
	INV_ICM426XX_DUAL_INTERFACE      = 0x02,
	INV_ICM426XX_DUAL_INTERFACE_SPI4 = 0x06,
	INV_ICM426XX_TRIPLE_INTERFACE    = 0x08,
} inv_icm426xx_interface_mode_t;
#endif



int inv_icm426xx_set_reg_bank(struct inv_icm426xx *s, uint8_t bank);



int inv_icm426xx_init(struct inv_icm426xx *s, struct inv_icm426xx_serif *serif,
                      void (*sensor_event_cb)(inv_icm426xx_sensor_event_t *event));

/* @brief Perform a soft reset of the device. 对设备进行软复位。 @param[in] s  Pointer to device. @return       0 on success, negative value on error. */
int inv_icm426xx_device_reset(struct inv_icm426xx *s);



int inv_icm426xx_get_who_am_i(struct inv_icm426xx *s, uint8_t *who_am_i);



int inv_icm426xx_force_clock_source(struct inv_icm426xx *                s,
                                    ICM426XX_INTF_CONFIG1_ACCEL_LP_CLK_t clk_src);

/* @brief Enable accel in low power mode. 在低功耗模式下启用加速。 @param[in] s  Pointer to device. @return       0 on success, negative value on error. */
int inv_icm426xx_enable_accel_low_power_mode(struct inv_icm426xx *s);

/* @brief Enable accel in low noise mode. 在低噪音模式下启用加速。 @param[in] s  Pointer to device. @return       0 on success, negative value on error. */
int inv_icm426xx_enable_accel_low_noise_mode(struct inv_icm426xx *s);

/* @brief Disable accel. 禁用accel。 @param[in] s  Pointer to device. @return       0 on success, negative value on error. */
int inv_icm426xx_disable_accel(struct inv_icm426xx *s);

/* @brief Enable gyro in low noise mode. 使陀螺处于低噪声模式。 @param[in] s  Pointer to device. @return       0 on success, negative value on error. */
int inv_icm426xx_enable_gyro_low_noise_mode(struct inv_icm426xx *s);

/* @brief Disable gyro 禁用陀螺 @param[in] s  Pointer to device. @return       0 on success, negative value on error. */
int inv_icm426xx_disable_gyro(struct inv_icm426xx *s);

/* @brief Enable fsync tagging functionality. 启用fsync标记功能。 - enables fsync this increase power consumption. - enables fsync related interrupt */
int inv_icm426xx_enable_fsync(struct inv_icm426xx *s);

/* @brief Disable fsync tagging functionality. 禁用fsync标记功能。 - disables fsync more available in registers. - disables fsync related interrupt */
int inv_icm426xx_disable_fsync(struct inv_icm426xx *s);



int inv_icm426xx_configure_timestamp_resolution(struct inv_icm426xx *        s,
                                                ICM426XX_TMST_CONFIG_RESOL_t resol);



int inv_icm426xx_set_config_ibi(struct inv_icm426xx *               s,
                                inv_icm426xx_interrupt_parameter_t *interrupt_to_configure);



int inv_icm426xx_get_config_ibi(struct inv_icm426xx *               s,
                                inv_icm426xx_interrupt_parameter_t *interrupt_to_configure);



int inv_icm426xx_set_config_int1(struct inv_icm426xx *               s,
                                 inv_icm426xx_interrupt_parameter_t *interrupt_to_configure);



int inv_icm426xx_get_config_int1(struct inv_icm426xx *               s,
                                 inv_icm426xx_interrupt_parameter_t *interrupt_to_configure);



int inv_icm426xx_set_config_int2(struct inv_icm426xx *               s,
                                 inv_icm426xx_interrupt_parameter_t *interrupt_to_configure);



int inv_icm426xx_get_config_int2(struct inv_icm426xx *               s,
                                 inv_icm426xx_interrupt_parameter_t *interrupt_to_configure);



int inv_icm426xx_get_data_from_registers(struct inv_icm426xx *s);



int inv_icm426xx_get_data_from_fifo(struct inv_icm426xx *s);



uint32_t inv_icm426xx_convert_odr_bitfield_to_us(uint32_t odr_bitfield);



int inv_icm426xx_set_accel_frequency(struct inv_icm426xx *              s,
                                     const ICM426XX_ACCEL_CONFIG0_ODR_t frequency);



int inv_icm426xx_set_gyro_frequency(struct inv_icm426xx *             s,
                                    const ICM426XX_GYRO_CONFIG0_ODR_t frequency);



int inv_icm426xx_set_accel_fsr(struct inv_icm426xx *s, ICM426XX_ACCEL_CONFIG0_FS_SEL_t accel_fsr_g);



int inv_icm426xx_get_accel_fsr(struct inv_icm426xx *            s,
                               ICM426XX_ACCEL_CONFIG0_FS_SEL_t *accel_fsr_g);



int inv_icm426xx_set_gyro_fsr(struct inv_icm426xx *s, ICM426XX_GYRO_CONFIG0_FS_SEL_t gyro_fsr_dps);



int inv_icm426xx_get_gyro_fsr(struct inv_icm426xx *s, ICM426XX_GYRO_CONFIG0_FS_SEL_t *gyro_fsr_dps);



int inv_icm426xx_set_accel_lp_avg(struct inv_icm426xx *                        s,
                                  ICM426XX_GYRO_ACCEL_CONFIG0_ACCEL_FILT_AVG_t acc_avg);



int inv_icm426xx_set_accel_ln_bw(struct inv_icm426xx *                       s,
                                 ICM426XX_GYRO_ACCEL_CONFIG0_ACCEL_FILT_BW_t acc_bw);



int inv_icm426xx_set_gyro_ln_bw(struct inv_icm426xx *                      s,
                                ICM426XX_GYRO_ACCEL_CONFIG0_GYRO_FILT_BW_t gyr_bw);

/* @brief reset ICM426XX fifo 复位ICM426XX fifo @param[in] s  Pointer to device. @return       0 on success, negative value on error. */
int inv_icm426xx_reset_fifo(struct inv_icm426xx *s);



int inv_icm426xx_enable_timestamp_to_register(struct inv_icm426xx *s);



int inv_icm426xx_disable_timestamp_to_register(struct inv_icm426xx *s);



int inv_icm426xx_get_current_timestamp(struct inv_icm426xx *s, uint32_t *icm_time);



int inv_icm426xx_enable_clkin_rtc(struct inv_icm426xx *s, uint8_t enable);



int inv_icm426xx_get_clkin_rtc_status(struct inv_icm426xx *s);



int inv_icm426xx_enable_high_resolution_fifo(struct inv_icm426xx *s);



int inv_icm426xx_disable_high_resolution_fifo(struct inv_icm426xx *s);



int inv_icm426xx_configure_fifo(struct inv_icm426xx *s, INV_ICM426XX_FIFO_CONFIG_t fifo_config);



int inv_icm426xx_configure_fifo_wm(struct inv_icm426xx *s, uint16_t wm);



uint32_t inv_icm426xx_get_fifo_timestamp_resolution_us_q24(struct inv_icm426xx *s);



uint32_t inv_icm426xx_get_reg_timestamp_resolution_us_q24(struct inv_icm426xx *s);

#if defined(ICM_FAMILY_CPLUS)

int inv_icm426xx_set_fifo_dec_rate(struct inv_icm426xx *s, uint8_t dec_rate);



int inv_icm426xx_interface_change_procedure(struct inv_icm426xx *         s,
                                            inv_icm426xx_interface_mode_t interface_mode);
#endif



const char *inv_icm426xx_get_version(void);

#ifdef __cplusplus
}
#endif

#endif /* _INV_ICM426xx_DRIVER_HL_H_ */

/* @} */
