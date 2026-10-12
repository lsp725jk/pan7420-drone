/**
 * @note Copyright (C) Shanghai Panchip Microelectronics Co., Ltd.
 * @file bsp.h
 * @brief
 * @history - V0.01, 2022-02-17, yangyaming, first implementation.
 */
/**
 * 文件：drivers/bsp/bsp.h                 【角色】板级驱动的“总头文件”——
 */

#ifndef __BSP_H
#define __BSP_H



#include <stdbool.h>
#include "py32f0xx.h"
#include "py32f0xx_hal_conf.h"
#include "py32f0xx_hal_tim.h"
#include "py32f0xx_hal_tim_ex.h"
#include "math.h"

/* ---- 第 2 组：本工程实现的外设驱动 ---- main.h     ：工程级公共宏与类型（来自 user/ 目录） config_rf.h：射频参数（频率/地址/速率/功率/模式），被射频驱动当实参用 bsp_delay.h：微秒/毫秒延时宏（几乎所有驱动都要用） */
#include "main.h"
#include "config_rf.h"
#include "bsp_delay.h"
// #include "bsp_timer.h"
#include "bsp_rfspi_sw.h"
#include "bsp_pn298.h"
#include "bsp_uart.h"
#include "spi.h"
// #include "driver_bmp390.h"
#include "bsp_PWM.h"
#include "adc.h"



#include "Icm426xxDefs.h"
#include "Icm426xxExtFunc.h"
#include "Icm426xxDriver_HL.h"
#include "Icm426xxTransport.h"
#include "Icm426xxVersion.h"
#include "Icm426xxDriver_HL_apex.h"
#include "Icm426xxSelfTest.h"
#include "InvError.h"
#include "ErrorHelper.h"
#include "icm42688.h"
/* TIM16.h：TIM16 定时器（微秒计时）与 TIM17 中断初始化 */
#include "TIM16.h"

/* ---- 第 4 组：上层业务模块 ---- 注意这一组是“驱动层反过来包含应用层”，说明 bsp.h 事实上承担了 “全工程公共头文件”的角色（见上面第三节的取舍说明）。 */
#include "accgyro.h"
#include "fc.h"
#include "imu.h"
#include "pid.h"
#include "filter.h"
#include "mlib.h"
#include "altitude.h"
#include "decode.h"
#include "flash.h"
#include "deal_pack.h"
#include "led.h"
#include "bmp3.h"
#include "common.h"
#include "bmp3_defs.h"
#include "flip.h"
#include "adc.h"
#include "WDG.h"



extern uint8_t powerupTimer;

extern double baroPressure, baroTemperature;		//气压计压力、温度]
/* 【注意】上面这行的行尾注释多了一个右方括号“]”，是笔误，仅注释无影响（未改动）。 */



extern int16_t setVelocity;   //控制无人机上升下降速度，油门在中间范围时无人机悬停
/* setVelocity 定义在 imu/decode.c；由 annexCode() 根据油门杆位置算出， 再被 imu/altitude.c 的定高环当作“目标垂直速度”使用。 */



void bsp_init(void);

/* 气压计接口声明。注意：get_pressure_altitude 与 baro_update 的实现在 bsp.c 里被整段注释掉了，当前工程没有它们的定义 —— 只声明不调用不会报错，一旦有人调用就会在链接阶段报“未定义引用”（未改动）。 */
void bmp390_init_spi(void);
void get_pressure_altitude(float *pressure, float *altitude);
bool baro_update(void);

/* 以下三个是 ICM42688 官方驱动要求的回调，实现在 bsp.c。 */
void inv_icm426xx_sleep_us(uint32_t us);
uint64_t inv_icm426xx_get_time_us(void);
void SetupMCUHardware(struct inv_icm426xx_serif *icm_serif);
#endif
