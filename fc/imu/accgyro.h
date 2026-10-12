#ifndef __ACCGYRO_H
#define __ACCGYRO_H

// ------------------------------------------------------------------------------------
// #include "platform.h"
#include "bsp.h"

// ------------------------------------------------------------------------------------
typedef struct {
  void (*accgyro_init)(void);
  uint8_t (*accgyro_read)(float* acc, float* gyro);
  float accRaw[3];
  float gyroRaw[3];
  int16_t accRaw_offset[3];
  int16_t gyroRaw_offset[3];
  
  uint8_t accCalibrating;
  uint8_t gyroCalibrating;
  
  int32_t accValue[3];        // 传感器读取减offset
  int32_t gyroValue[3];
  
  int32_t accData[3];         // 滤波后
  int32_t gyroData[3];
  
} accgyro_s;



extern accgyro_s accgyro;
extern int16_t acc_1G;
extern float gyroScale;    // gyro output scaled to rad per second

// ------------------------------------------------------------------------------------
void accgyro_init(void);
uint8_t accgyro_update(void);
int imu_hardware_calibration(void);

typedef enum {
    ALIGN_DEFAULT = 0,                                      // driver-provided alignment
    CW0_DEG = 1,
    CW90_DEG = 2,
    CW180_DEG = 3,
    CW270_DEG = 4,
    CW0_DEG_FLIP = 5,
    CW90_DEG_FLIP = 6,
    CW180_DEG_FLIP = 7,
    CW270_DEG_FLIP = 8
} sensor_align_e;



void apply_sensor_alignment(float * dest, float * src, sensor_align_e rotation);

int icm42688_calibrate_horizon(void);
// void accgyro_test(void);



#endif 



