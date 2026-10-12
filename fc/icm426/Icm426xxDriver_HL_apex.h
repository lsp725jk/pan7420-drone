/**
 * and other intellectual property rights laws.
 */

/**
 * @defgroup apex APEX（IMU 内的计步/敲击等功能集） driver @brief High-level API to drive APEX-related features. @{
 */

/**
 * @file Icm426xxDriver_HL_apex.h
 */

#ifndef _INV_ICM426xx_DRIVER_HL_APEX_H_
#define _INV_ICM426xx_DRIVER_HL_APEX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "Icm426xxDefs.h"

#include "InvError.h"

#include <stdint.h>
#include <string.h>

/* Forward declarations */
struct inv_icm426xx;

/* @brief Icm426xx TAP inputs parameters definition Icm426xx TAP输入参数定义 */
typedef struct {
	uint8_t                                  min_jerk_thr; 		/* < Minimum Jerk Threshold */
	ICM426XX_APEX_CONFIG7_TAP_MAX_PEAK_TOL_t max_peak_tol; 		/* < Maximum peak tolerance */
	ICM426XX_APEX_CONFIG8_TAP_TMAX_t         tmax; 		/* < Tap measurement window */
	ICM426XX_APEX_CONFIG8_TAP_TAVG_t         tavg; 		/* < Energy measumerement window */
	ICM426XX_APEX_CONFIG8_TAP_TMIN_t         tmin; 		/* < Single tap window */
} inv_icm426xx_tap_parameters_t;

/* @brief Icm426xx APEX inputs parameters definition Icm426xx APEX输入参数定义 */
typedef struct inv_icm426xx_apex_parameters {
				/* Peak threshold value to be considered as a valid step (mg) */
	ICM426XX_APEX_CONFIG2_PEDO_AMP_TH_t pedo_amp_th; 



	uint8_t pedo_step_cnt_th;



	uint8_t pedo_step_det_th; 



	ICM426XX_APEX_CONFIG3_PEDO_SB_TIMER_TH_t pedo_sb_timer_th;

				/* Threshold to improve run detection if not steps are counted while running */
	ICM426XX_APEX_CONFIG3_PEDO_HI_ENRGY_TH_t pedo_hi_enrgy_th; 

				/* Number of accelerometer samples to wait before triggering tilt event */
	ICM426XX_APEX_CONFIG4_TILT_WAIT_TIME_t tilt_wait_time; 



	ICM426XX_APEX_CONFIG1_DMP_POWER_SAVE_TIME_t power_save_time;



	ICM426XX_APEX_CONFIG0_DMP_POWER_SAVE_t power_save;

				/* be increased. */
	ICM426XX_APEX_CONFIG9_SENSITIVITY_MODE_t sensitivity_mode; 

#if defined(ICM_FAMILY_BPLUS)
				/* Time out for a sleep detection. */
	ICM426XX_APEX_CONFIG4_R2W_SLEEP_TIME_OUT_t r2w_sleep_time_out; 
	
				/* < Mounting matrix, chip to device frame. */
	ICM426XX_APEX_CONFIG5_R2W_MOUNTING_MATRIX_t r2w_mounting_matrix;
	
				/* Detection window for a sleep gesture detection. */
	ICM426XX_APEX_CONFIG6_R2W_SLEEP_GEST_DELAY_t r2w_gest_delay;

#elif defined(ICM_FAMILY_CPLUS)



	ICM426XX_APEX_CONFIG10_FF_DEBOUNCE_DURATION_t ff_debounce_duration;



	ICM426XX_APEX_CONFIG10_FF_MAX_DURATION_t ff_max_duration_cm;



	ICM426XX_APEX_CONFIG10_FF_MIN_DURATION_t ff_min_duration_cm;



	ICM426XX_APEX_CONFIG5_LOWG_PEAK_TH_t lowg_peak_th; 

				/* Hysteresis threshold (mg) added to the threshold after the initial threshold is met */
	ICM426XX_APEX_CONFIG4_LOWG_PEAK_TH_HYST_t lowg_peak_hyst;



	ICM426XX_APEX_CONFIG5_LOWG_TIME_TH_SAMPLES_t lowg_samples_th;



	ICM426XX_APEX_CONFIG6_HIGHG_PEAK_TH_t highg_peak_th;
	
				/* Hysteresis threshold (mg) added to the threshold after the initial threshold is met */
	ICM426XX_APEX_CONFIG4_HIGHG_PEAK_TH_HYST_t highg_peak_hyst;



	ICM426XX_APEX_CONFIG6_HIGHG_TIME_TH_SAMPLES_t highg_samples_th;
#endif

				/* Peak threshold value to be considered as a valid step (mg) in Slow walk mode */
	ICM426XX_APEX_CONFIG1_LOW_ENERGY_AMP_TH_t low_energy_amp_th;
} inv_icm426xx_apex_parameters_t;

/* @brief APEX pedometer outputs APEX计步器输出 */ 
typedef struct inv_icm426xx_apex_step_activity {
				/* Number of steps taken */
	uint16_t step_cnt; 

				/* Walk/run cadency in number of samples. is 25 samples, the register will output 100. */
	uint8_t step_cadence; 

				/* Detected activity unknown (0), walk (1) or run (2) */
	uint8_t activity_class;
} inv_icm426xx_apex_step_activity_t;

/* @brief TAP outputs */
typedef struct inv_icm426xx_tap_data {
				/* Detects single (1) or double (2) tap */
	ICM426XX_APEX_DATA4_TAP_NUM_t tap_num; 
	
				/* Axis along which tap has been detected */
	ICM426XX_APEX_DATA4_TAP_AXIS_t tap_axis; 
	
				/* Direction of the tap, either +axis (1) or -axis (0) */
	ICM426XX_APEX_DATA4_TAP_DIR_t tap_dir; 



	uint8_t double_tap_timing;
} inv_icm426xx_tap_data_t;



int inv_icm426xx_configure_smd_wom(struct inv_icm426xx *s, const uint8_t x_th, const uint8_t y_th,
                                   const uint8_t z_th, ICM426XX_SMD_CONFIG_WOM_INT_MODE_t wom_int,
                                   ICM426XX_SMD_CONFIG_WOM_MODE_t wom_mode);



int inv_icm426xx_enable_wom(struct inv_icm426xx *s);

/* @brief  Disable Wake On Motion. 禁用唤醒运动。 @param[in] s  Pointer to device. @return       0 on success, negative value on error. */
int inv_icm426xx_disable_wom(struct inv_icm426xx *s);



int inv_icm426xx_enable_smd(struct inv_icm426xx *s);



int inv_icm426xx_disable_smd(struct inv_icm426xx *s);



int inv_icm426xx_init_tap_parameters_struct(struct inv_icm426xx *          s,
                                            inv_icm426xx_tap_parameters_t *tap_inputs);



int inv_icm426xx_configure_tap_parameters(struct inv_icm426xx *                s,
                                          const inv_icm426xx_tap_parameters_t *tap_inputs);



int inv_icm426xx_get_tap_parameters(struct inv_icm426xx *          s,
                                    inv_icm426xx_tap_parameters_t *tap_params);



int inv_icm426xx_enable_tap(struct inv_icm426xx *s);

/* @brief  Disable TAP. 关闭TAP @param[in] s  Pointer to device. @return       0 on success, negative value on error. */
int inv_icm426xx_disable_tap(struct inv_icm426xx *s);



int inv_icm426xx_init_apex_parameters_struct(struct inv_icm426xx *           s,
                                             inv_icm426xx_apex_parameters_t *apex_inputs);



int inv_icm426xx_configure_apex_parameters(struct inv_icm426xx *                 s,
                                           const inv_icm426xx_apex_parameters_t *apex_inputs);



int inv_icm426xx_get_apex_parameters(struct inv_icm426xx *           s,
                                     inv_icm426xx_apex_parameters_t *apex_params);



int inv_icm426xx_set_apex_frequency(struct inv_icm426xx *                 s,
                                    const ICM426XX_APEX_CONFIG0_DMP_ODR_t frequency);

/* @brief Start DMP for APEX algorithms 为APEX算法启动DMP @param[in] s  Pointer to device. @return       0 on success, negative value on error. */
int inv_icm426xx_start_dmp(struct inv_icm426xx *s);
 
/* @brief Reset DMP memory 重置DMP内存 @param[in] s  Pointer to device. @return       0 on success, negative value on error. */
int inv_icm426xx_reset_dmp(struct inv_icm426xx *s);



int inv_icm426xx_enable_apex_pedometer(struct inv_icm426xx *s);



int inv_icm426xx_disable_apex_pedometer(struct inv_icm426xx *s);

#if defined(ICM_FAMILY_BPLUS)



int inv_icm426xx_enable_apex_r2w(struct inv_icm426xx *s);



int inv_icm426xx_disable_apex_r2w(struct inv_icm426xx *s);

#elif defined(ICM_FAMILY_CPLUS)

/* @brief  Enable Free Fall. 启用自由落体。 @param[in] s  Pointer to device. @return       0 on success, negative value on error. */
int inv_icm426xx_enable_apex_ff(struct inv_icm426xx *s);

/* @brief  Disable Free Fall. 禁用自由落体。 @param[in] s  Pointer to device. @return       0 on success, negative value on error. */
int inv_icm426xx_disable_apex_ff(struct inv_icm426xx *s);

/* @brief  Disable LowG. 禁用LowG。 @param[in] s  Pointer to device. @return       0 on success, negative value on error. */
int inv_icm426xx_disable_apex_lowg(struct inv_icm426xx *s);

/* @brief  Enable LowG.启用LowG。 @param[in] s  Pointer to device. @return       0 on success, negative value on error. */
int inv_icm426xx_enable_apex_lowg(struct inv_icm426xx *s);

#endif

/* @brief  Enable APEX algorithm Tilt. 启用APEX算法倾斜。 @param[in] s  Pointer to device. @return       0 on success, negative value on error. */
int inv_icm426xx_enable_apex_tilt(struct inv_icm426xx *s);

/* @brief  Disable APEX algorithm Tilt. 禁用APEX算法倾斜。 @param[in] s  Pointer to device. @return       0 on success, negative value on error. */
int inv_icm426xx_disable_apex_tilt(struct inv_icm426xx *s);



int inv_icm426xx_get_apex_data_activity(struct inv_icm426xx *              s,
                                        inv_icm426xx_apex_step_activity_t *apex_activity);



int inv_icm426xx_get_tap_data(struct inv_icm426xx *s, inv_icm426xx_tap_data_t *tap_data);



int inv_icm426xx_load_dmp_sram_code(struct inv_icm426xx *s, const uint8_t *dmp_prog,
                                    const uint32_t start_offset, const uint32_t size);

#ifdef __cplusplus
}
#endif

#endif /* _INV_ICM426xx_DRIVER_HL_APEX_H_ */

/* @} */
