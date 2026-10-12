/**
 * @file    py32f0xx_it.c
 * @author  MCU Application Team
 * @brief   Interrupt Service Routines.
 * @attention
 */



/* Includes ------------------------------------------------------------------ */
#include "main.h"
#include "py32f0xx_it.h"
#include "bsp.h"

extern TIM_HandleTypeDef TimHandle_17;
/* Private includes ---------------------------------------------------------- */
/* Private typedef ----------------------------------------------------------- */
/* Private define ------------------------------------------------------------ */
/* Private macro ------------------------------------------------------------- */
/* Private variables --------------------------------------------------------- */
/* Private function prototypes ----------------------------------------------- */
/* Private user code --------------------------------------------------------- */
/* External variables -------------------------------------------------------- */



/* Cortex-M0+ Processor Interruption and Exception Handlers */

/* @brief  不可屏蔽中断（NMI, Non-Maskable Interrupt） 【为什么留空】本工程不使用 NMI（PY32 上 NMI 主要与时钟安全系统/硬件错误相关）。 留空即可：它优先级最高，一旦进来说明有严重硬件异常，但目前不做处理。 */
void NMI_Handler(void)
{
}



void HardFault_Handler(void)
{
  while (1)
  {
  }
}

/* @brief  系统服务调用（SVC）：一般是 RTOS 用来从任务态切到特权态 【为什么留空】本工程是裸机（无 RTOS），不会主动发 SVC 指令。 */
void SVC_Handler(void)
{
}

/* @brief  可挂起系统调用（PendSV）：RTOS 用它做任务上下文切换 【为什么留空】同上，本工程裸机运行，不涉及任务切换。 */
void PendSV_Handler(void)
{
}

/* @brief  系统滴答定时器中断（SysTick，HAL 配成 1ms 一次） 【参数/返回】无 【算法/为什么这么写】只调用 HAL_IncTick()：它把 HAL 内部的毫秒计数 uwTick 加 1。 全工程的“时间感”都来自这里： */
void SysTick_Handler(void)
{
	HAL_IncTick();
}
/* @brief  TIM17 中断（本工程配成 1ms 周期）：飞控的“心跳” 【参数/返回】无 【为什么只有一行】把中断处理交给 HAL_TIM_IRQHandler()： 它负责判断中断来源（更新/比较/捕获）、清中断标志，然后回调 */
void TIM17_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&TimHandle_17);
}



/* (C) COPYRIGHT Puya *****END OF FILE */
