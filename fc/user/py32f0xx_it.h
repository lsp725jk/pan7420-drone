/**
 * @file    py32f0xx_it.h
 * @author  MCU Application Team
 * @brief   This file contains the headers of the interrupt handlers.
 * @attention
 */

/* Define to prevent recursive inclusion ------------------------------------- */
#ifndef __PY32F0XX_IT_H
#define __PY32F0XX_IT_H



#ifdef __cplusplus
 extern "C" {
#endif 

/* Private includes ---------------------------------------------------------- */
/* Exported types ------------------------------------------------------------ */
/* Exported constants -------------------------------------------------------- */
/* Exported macro ------------------------------------------------------------ */
/* Exported functions prototypes --------------------------------------------- */

void NMI_Handler(void);        /* 不可屏蔽中断：本工程留空 */
void HardFault_Handler(void);  /* 硬件错误：死循环 + 看门狗复位 */
void SVC_Handler(void);        /* 系统服务调用：裸机不用 */
void PendSV_Handler(void);     /* 可挂起系统调用：裸机不用 */
void SysTick_Handler(void);    /* 1ms 系统滴答：HAL 时基（HAL_GetTick） */

#ifdef __cplusplus
}
#endif

#endif /* __PY32F0XX_IT_H */

/* (C) COPYRIGHT Puya *****END OF FILE */
