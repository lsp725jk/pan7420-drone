/**
 * 文件：imu/fc.h                    【角色】飞行状态中枢：全局 fc 结构体与调度入口
 */

#ifndef __FC_H
#define __FC_H

#include "bsp.h"



/**
 * RC alias
 */
/* 遥控通道索引别名。这四个宏看着简单，但作用很大： 代码里写 axisPID[ROLL] 比 axisPID[0] 可读得多，而且通道顺序若调整， 只改这里即可，不用满工程找数字。 对应 imu/decode.c 里填充的 rcCommand[] 数组下标。 */
#define ROLL       0
#define PITCH      1
#define YAW        2
#define THROTTLE   3



typedef struct {
		uint8_t uartCmdNew:1;           				/* 收到新的串口指令（置 1 表示有待处理指令） */
		uint8_t motorArmed:1;							/* 电机已解锁：1=可以转，0=强制停转 */
		uint8_t motorIdle:1;			//电机空转（怠速，起飞前/降落后维持低速旋转）
		uint8_t bVelocityControl:1;     				/* 速度控制模式（本版本未使用，预留） */
		uint8_t bAccGyroCalibrationEnb:1; 				/* 允许进行加速度计/陀螺仪校准 */
	
    uint8_t bTakeoff:1;                     /* 正在起飞流程中 */
    uint8_t bLanding:1;                     /* 正在降落流程中 */
	
} fc_ctrl_flags_s;

/* 飞行控制总状态。全局只有一份（fc.c 里定义 fc 实体）。 */
typedef struct {
		fc_ctrl_flags_s flags;          				/* 上面那组标志位 */
	
    uint8_t takeofflandTimer;               /* 起飞/降落计时（单位见使用的定时器节拍） */
    int32_t takeoffAlt;                     /* 起飞点高度基准（毫米），用于相对高度计算 */
  
} fc_ctrl_s;

/* 全局飞行状态实体。定义在 fc.c，其他文件通过这一行 extern 访问。 */
extern fc_ctrl_s fc;



void flight_ctrl(void);

/* 上电初始化：把 fc 的各标志位清零、PID 微调清零。 由 user/main.c 在进入主循环前调用一次。 */
void fc_init(void);



#endif

