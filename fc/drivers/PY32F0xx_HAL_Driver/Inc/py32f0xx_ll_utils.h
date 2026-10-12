/**
 * @file    py32f0xx_ll_utils.h
 * @author  MCU Application Team
 * @brief   Header file of UTILS LL module.
 * @verbatim
 */

/* Define to prevent recursive inclusion ------------------------------------- */
#ifndef PY32F0XX_LL_UTILS_H
#define PY32F0XX_LL_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------ */
#include "py32f0xx.h"

/* @addtogroup PY32F0xx_LL_Driver @{ */

/* @defgroup UTILS_LL UTILS @{ */

/* Private types ------------------------------------------------------------- */
/* Private variables --------------------------------------------------------- */

/* Private constants --------------------------------------------------------- */
/* @defgroup UTILS_LL_Private_Constants UTILS Private Constants @{ */

/* Max delay can be used in LL_mDelay */
#define LL_MAX_DELAY                  0xFFFFFFFFU

/* @brief Unique device ID register base address */
#define UID_BASE_ADDRESS              UID_BASE

/* @brief Flash size data register base address */
#define FLASHSIZE_BASE_ADDRESS        FLASHSIZE_BASE

/* @} */

/* Private macros ------------------------------------------------------------ */
/* @defgroup UTILS_LL_Private_Macros UTILS Private Macros @{ */
/* @} */
/* Exported types ------------------------------------------------------------ */
/* @defgroup UTILS_LL_ES_INIT UTILS Exported structures @{ */
/* @brief  UTILS System, AHB and APB1 buses clock configuration structure definition */
typedef struct
{
  uint32_t AHBCLKDivider;             /* This parameter can be a value of @ref RCC_LL_EC_SYSCLK_DIV @ref LL_RCC_SetAHBPrescaler(). */

  uint32_t APB1CLKDivider;             /* This parameter can be a value of @ref RCC_LL_EC_APB1_DIV @ref LL_RCC_SetAPB1Prescaler(). */
} LL_UTILS_ClkInitTypeDef;

/* @} */

/* Exported constants -------------------------------------------------------- */
/* @defgroup UTILS_LL_Exported_Constants UTILS Exported Constants @{ */

/* @defgroup UTILS_EC_HSE_BYPASS HSE Bypass activation @{ */
#define LL_UTILS_HSEBYPASS_OFF        0x00000000U       /* !< HSE Bypass is not enabled */
#define LL_UTILS_HSEBYPASS_ON         0x00000001U       /* !< HSE Bypass is enabled */
/* @} */

/* @} */

/* Exported macro ------------------------------------------------------------ */

/* Exported functions -------------------------------------------------------- */
/* @defgroup UTILS_LL_Exported_Functions UTILS Exported Functions @{ */

/* @defgroup UTILS_EF_DEVICE_ELECTRONIC_SIGNATURE DEVICE ELECTRONIC SIGNATURE @{ */

/* @brief  Get Word0 of the unique device identifier (UID based on 96 bits) @retval UID[31:0] */
__STATIC_INLINE uint32_t LL_GetUID_Word0(void)
{
  return (uint32_t)(READ_REG(*((uint32_t *)UID_BASE_ADDRESS)));
}



__STATIC_INLINE uint32_t LL_GetUID_Word1(void)
{
  return (uint32_t)(READ_REG(*((uint32_t *)(UID_BASE_ADDRESS + 4U))));
}



__STATIC_INLINE uint32_t LL_GetUID_Word2(void)
{
  return (uint32_t)(READ_REG(*((uint32_t *)(UID_BASE_ADDRESS + 8U))));
}

/* @brief  Get Flash memory size @retval Flash memory size(Bytes) */
__STATIC_INLINE uint32_t LL_GetFlashSize(void)
{
  return (uint32_t)(FLASH_SIZE);
}

/* @brief  Get SRAM memory size @retval SRAM memory size(Bytes) */
__STATIC_INLINE uint32_t LL_GetSramSize(void)
{
  return (uint32_t)(SRAM_END - SRAM_BASE + 1);
}

/* @} */

/* @defgroup UTILS_LL_EF_DELAY DELAY @{ */



__STATIC_INLINE void LL_InitTick(uint32_t HCLKFrequency, uint32_t Ticks)
{
        /* Configure the SysTick to have interrupt in 1ms time base */
  SysTick->LOAD  = (uint32_t)((HCLKFrequency / Ticks) - 1UL);      /* set reload register */
  SysTick->VAL   = 0UL;                                           /* Load the SysTick Counter Value */
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                   SysTick_CTRL_ENABLE_Msk;                                                         /* Enable the Systick Timer */
}

void        LL_Init1msTick(uint32_t HCLKFrequency);
void        LL_mDelay(uint32_t Delay);

/* @} */

/* @defgroup UTILS_EF_SYSTEM SYSTEM @{ */

void        LL_SetSystemCoreClock(uint32_t HCLKFrequency);
#if defined(RCC_PLL_SUPPORT)
ErrorStatus LL_PLL_ConfigSystemClock_HSI(LL_UTILS_ClkInitTypeDef *UTILS_ClkInitStruct);
ErrorStatus LL_PLL_ConfigSystemClock_HSE(uint32_t HSEFrequency, uint32_t HSEBypass,
    LL_UTILS_ClkInitTypeDef *UTILS_ClkInitStruct);
#endif
ErrorStatus LL_SetFlashLatency(uint32_t HCLKFrequency);

/* @} */

/* @} */

/* @} */

/* @} */

#ifdef __cplusplus
}
#endif

#endif /* PY32F0XX_LL_UTILS_H */

/* (C) COPYRIGHT Puya *****END OF FILE */
