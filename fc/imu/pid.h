/**
 * 文件：imu/pid.h                     【角色】PID 参数与接口声明
 */

#ifndef __PID_H
#define __PID_H

#include "bsp.h"



/* PID 参数集合。 数组下标约定：0=ROLL(横滚) 1=PITCH(俯仰) 2=YAW(偏航) 【为什么用 float】 增益通常小于 1（例如 0.10），用整数会丢精度。 */
typedef struct {	 
  float PAngleToRate[3];       /* 外环（角度环）增益：角度误差 → 期望角速度 */
  float IAngleToRate[3];       /* 外环积分增益。当前 pid.c 未使用（预留） */
  float PRate[3];              /* 内环（角速度环）比例增益 */
  float IRate[3];              /* 内环积分增益 */
  float DRate[3];              /* 内环微分增益 */
  int16_t angleTrim[3];        /* 遥控器微调：补偿机械安装倾斜或重心偏移 */
} pid_conf_s; 

extern pid_conf_s pidconf;

/* 飞行模式：0=手动（自稳），非 0=定高。来源 user/deal_pack.c（遥控器下发） */
extern uint8_t altitude_mode;

/* 唯一的对外接口。deltaT = 距上次调用的秒数（约 0.001）。 由 imu/fc.c 的 flight_ctrl() 在主循环里调用（不是 TIM17 中断）。 */
void pid(float deltaT);



#endif
