/**
 * and other intellectual property rights laws.
 */
#ifndef _ICM42688_H_
#define _ICM42688_H_

#include <stdint.h>
#include "bsp.h"
#include "Icm426xxTransport.h"
#include "Icm426xxDefs.h"
#include "Icm426xxDriver_HL.h"
#define ICM_USE_HARD_SPI



extern struct inv_icm426xx icm_driver;

typedef struct {
  int16_t x;     /**
   * < Raw int16_t value from the x axis
   */
  int16_t y;     /* < Raw int16_t value from the y axis */
  int16_t z;     /* < Raw int16_t value from the z axis */
} icm42688RawData_t;

typedef struct {
  float x;     /* < value from the x axis */
  float y;     /* < value from the y axis */
  float z;     /* < value from the z axis */
} icm42688RealData_t;

#define INV_MSG(level,msg, ...) 	      printf("%d," msg "\r\n", __LINE__, ##__VA_ARGS__)
/* \return 0 on success, negative value on error. */
int SetupInvDevice(struct inv_icm426xx_serif *icm_serif);

/* timestamp produced by Icm426xx to a 64 bits timestamp). starts them in the requested power mode. low-power mode. for possible values. */
int ConfigureInvDevice(uint8_t is_low_noise_mode, ICM426XX_ACCEL_CONFIG0_FS_SEL_t acc_fsr_g,
                       ICM426XX_GYRO_CONFIG0_FS_SEL_t gyr_fsr_dps,
                       ICM426XX_ACCEL_CONFIG0_ODR_t acc_freq, ICM426XX_GYRO_CONFIG0_ODR_t gyr_freq,
                       uint8_t is_rtc_mode);

/* is HandleInvDeviceDataRegisters. \return 0 on success, negative value on error. */
int GetDataFromInvDevice(void);



void HandleInvDeviceDataRegisters(inv_icm426xx_sensor_event_t *event);

uint8_t bsp_IcmGetRawData(float* accData, float* GyroData);



void check_rc(int rc, const char *msg_context);

#endif /* !_EXAMPLE_RAW_DATA_REGISTER_H_ */
