/**
 * @file    system_py32f0xx.h
 * @brief   CMSIS Cortex-M0+ Device Peripheral Access Layer Header File for
 * @version v1.0.0
 * @attention
 */

/* This file refers the CMSIS standard, some adjustments are made according to Puya chips */
#ifndef SYSTEM_PY32F0XX_H
#define SYSTEM_PY32F0XX_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------ */
#include <stdint.h>

/* Exported variables -------------------------------------------------------- */
extern uint32_t SystemCoreClock;     /* !< System Clock Frequency (Core Clock) */

/* Exported constants -------------------------------------------------------- */
extern const uint32_t AHBPrescTable[16];  /* !< AHB prescalers table values */
extern const uint32_t APBPrescTable[8];   /* !< APB prescalers table values */
extern const uint32_t HSIFreqTable[8];    /* !< HSI frequency table values */

/* Initialize the system @param  none @return none @brief  Setup the microcontroller system. */
extern void SystemInit(void);

/* Update SystemCoreClock variable @param  none @return none @brief  Updates the SystemCoreClock with current core Clock */
extern void SystemCoreClockUpdate(void);

#ifdef __cplusplus
}
#endif

#endif /* SYSTEM_PY32F0XX_H */
