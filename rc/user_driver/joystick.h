#ifndef __JOYSTICK_H__
#define __JOYSTICK_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include "board.h"



#define CRSF_DIGITAL_CHANNEL_MIN 172
#define CRSF_DIGITAL_CHANNEL_MID 992
#define CRSF_DIGITAL_CHANNEL_MAX 1811

// typedef struct {
// uint16_t min;             // 最小值
// uint16_t center;          // 中心点
// uint16_t max;             // 最大值
// uint8_t reverse;          // 反转变量
// int8_t offest;            //偏移量
// } AxisRange;

// // 航模通道结构体
// typedef struct {
// AxisRange roll;           // 横滚通道
// AxisRange pitch;          // 俯仰通道
// AxisRange throttle;       // 油门通道
// AxisRange yaw;            // 偏航通道
// } Joystick_Channel;



// void auto_calibrate_joystick_center(void);
void joystick_init(void);
// void calibrate_joystick_max(void);

void startCalibration(void) ;



#ifdef __cplusplus
}
#endif

#endif
