/**
 * @note Copyright (C) Shanghai Panchip Microelectronics Co., Ltd.
 * @file lib_driver_timer_pan308.c
 * @brief
 * @history - V0.01, 2022-02-22, yangyaming, first implementation.
 */
/**
 * 文件：drivers/bsp/bsp_timer.c           【角色】1ms 定时中断的“中断服务内容”：
 */

#include "bsp_timer.h"
#include "bsp.h"

#include "main.h"
/* @description	: 系统嘀嗒定时器中断服务程序 @param 		: 无 @return		: 无 */
/* 以下 4 个计数器都是“文件级全局变量”，定义在这里、声明（extern）在 user/main.c 里： extern uint8_t  _cnt10ms; extern uint8_t  _cnt4ms; extern uint8_t  _cnt100ms; */
uint8_t _cnt10ms;
uint8_t _cnt4ms;
uint8_t _cnt100ms;
uint16_t _cnt1s;

/* @brief  定时器溢出（周期到）回调 —— TIM17 每 1ms 调用一次 【参数】 @param htim  HAL 的定时器句柄指针，htim->Instance 告诉你“是哪个定时器 触发的”（TIM16 还是 TIM17 …）。 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{	
		// BSP_LED_Toggle(LED_GREEN);
// if (g_uc_enable_systick_isr == 0)
// {
// return;
// }
	if (htim->Instance == TIM17) {
		static uint8_t _cnt20ms;
		
		baroUpdateTimer++;
		altitudeCtrlTimer++;
		_cnt10ms++;
		_cnt4ms++;
		_cnt100ms++;
		_cnt1s++;

		if(++_cnt20ms >= 20) {
			_cnt20ms = 0;
			calibrationEnbTimer++;		//校准时间
// fc.takeofflandTimer++;
			powerupTimer++;
		}

  }
		// SysTick_ISR();
}

/* 磐启微电子 www.panchip.com */
