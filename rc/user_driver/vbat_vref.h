#ifndef __VBAT_VREF_H__
#define __VBAT_VREF_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include "py32f0xx_hal.h"

#include "string.h"

#define bat_sum_max  10
#define TEST_VCC_MV  3300
#define CALIB_POINT_NUM 5          // 校准点数量（3~5个，这里选3个）

// 校准点结构体（存储ADC值和对应的K值）
typedef struct {
    uint32_t adc;  // 校准点的ADC采样值（滤波后）
    float k;       // 自动计算的K值
} BatCalibPoint;



void getCalibratedVrefIntVoltage(void);
void autoCalibrateBatK(void);
void battery_check(void);

#ifdef __cplusplus
}
#endif

#endif
