/**
 * @file    py32f0xx_ll_cortex.h
 * @author  MCU Application Team
 * @brief   Header file of CORTEX LL module.
 * @attention
 */

/* Define to prevent recursive inclusion ------------------------------------- */
#ifndef __PY32F0XX_LL_CORTEX_H
#define __PY32F0XX_LL_CORTEX_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------ */
#include "py32f0xx.h"

/* @addtogroup PY32F0xx_LL_Driver @{ */

/* @defgroup CORTEX_LL CORTEX @{ */

/* Private types ------------------------------------------------------------- */
/* Private variables --------------------------------------------------------- */

/* Private constants --------------------------------------------------------- */

/* Private macros ------------------------------------------------------------ */

/* Exported types ------------------------------------------------------------ */
/* Exported constants -------------------------------------------------------- */
/* @defgroup CORTEX_LL_Exported_Constants CORTEX Exported Constants @{ */

/* @defgroup CORTEX_LL_EC_CLKSOURCE_HCLK SYSTICK Clock Source @{ */
#define LL_SYSTICK_CLKSOURCE_HCLK_DIV8     0x00000000U                 /* !< AHB clock divided by 8 selected as SysTick clock source. */
#define LL_SYSTICK_CLKSOURCE_HCLK          SysTick_CTRL_CLKSOURCE_Msk  /* !< AHB clock selected as SysTick clock source. */
/* @} */

/* @} */

/* Exported macro ------------------------------------------------------------ */

/* Exported functions -------------------------------------------------------- */
/* @defgroup CORTEX_LL_Exported_Functions CORTEX Exported Functions @{ */

/* @defgroup CORTEX_LL_EF_SYSTICK SYSTICK @{ */



__STATIC_INLINE uint32_t LL_SYSTICK_IsActiveCounterFlag(void)
{
  return (((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == (SysTick_CTRL_COUNTFLAG_Msk)) ? 1UL : 0UL);
}



__STATIC_INLINE void LL_SYSTICK_SetClkSource(uint32_t Source)
{
  if (Source == LL_SYSTICK_CLKSOURCE_HCLK)
  {
    SET_BIT(SysTick->CTRL, LL_SYSTICK_CLKSOURCE_HCLK);
  }
  else
  {
    CLEAR_BIT(SysTick->CTRL, LL_SYSTICK_CLKSOURCE_HCLK);
  }
}



__STATIC_INLINE uint32_t LL_SYSTICK_GetClkSource(void)
{
  return READ_BIT(SysTick->CTRL, LL_SYSTICK_CLKSOURCE_HCLK);
}

/* @brief  Enable SysTick exception request @rmtoll STK_CTRL     TICKINT       LL_SYSTICK_EnableIT @retval None */
__STATIC_INLINE void LL_SYSTICK_EnableIT(void)
{
  SET_BIT(SysTick->CTRL, SysTick_CTRL_TICKINT_Msk);
}

/* @brief  Disable SysTick exception request @rmtoll STK_CTRL     TICKINT       LL_SYSTICK_DisableIT @retval None */
__STATIC_INLINE void LL_SYSTICK_DisableIT(void)
{
  CLEAR_BIT(SysTick->CTRL, SysTick_CTRL_TICKINT_Msk);
}



__STATIC_INLINE uint32_t LL_SYSTICK_IsEnabledIT(void)
{
  return ((READ_BIT(SysTick->CTRL, SysTick_CTRL_TICKINT_Msk) == (SysTick_CTRL_TICKINT_Msk)) ? 1UL : 0UL);
}

/* @} */

/* @defgroup CORTEX_LL_EF_LOW_POWER_MODE LOW POWER MODE @{ */

/* @brief  Processor uses sleep as its low power mode @rmtoll SCB_SCR      SLEEPDEEP     LL_LPM_EnableSleep @retval None */
__STATIC_INLINE void LL_LPM_EnableSleep(void)
{
        /* Clear SLEEPDEEP bit of Cortex System Control Register */
  CLEAR_BIT(SCB->SCR, ((uint32_t)SCB_SCR_SLEEPDEEP_Msk));
}

/* @brief  Processor uses deep sleep as its low power mode @rmtoll SCB_SCR      SLEEPDEEP     LL_LPM_EnableDeepSleep @retval None */
__STATIC_INLINE void LL_LPM_EnableDeepSleep(void)
{
        /* Set SLEEPDEEP bit of Cortex System Control Register */
  SET_BIT(SCB->SCR, ((uint32_t)SCB_SCR_SLEEPDEEP_Msk));
}



__STATIC_INLINE void LL_LPM_EnableSleepOnExit(void)
{
        /* Set SLEEPONEXIT bit of Cortex System Control Register */
  SET_BIT(SCB->SCR, ((uint32_t)SCB_SCR_SLEEPONEXIT_Msk));
}

/* @brief  Do not sleep when returning to Thread mode. @rmtoll SCB_SCR      SLEEPONEXIT   LL_LPM_DisableSleepOnExit @retval None */
__STATIC_INLINE void LL_LPM_DisableSleepOnExit(void)
{
        /* Clear SLEEPONEXIT bit of Cortex System Control Register */
  CLEAR_BIT(SCB->SCR, ((uint32_t)SCB_SCR_SLEEPONEXIT_Msk));
}



__STATIC_INLINE void LL_LPM_EnableEventOnPend(void)
{
        /* Set SEVEONPEND bit of Cortex System Control Register */
  SET_BIT(SCB->SCR, ((uint32_t)SCB_SCR_SEVONPEND_Msk));
}



__STATIC_INLINE void LL_LPM_DisableEventOnPend(void)
{
        /* Clear SEVEONPEND bit of Cortex System Control Register */
  CLEAR_BIT(SCB->SCR, ((uint32_t)SCB_SCR_SEVONPEND_Msk));
}

/* @} */

/* @defgroup CORTEX_LL_EF_MCU_INFO MCU INFO @{ */

/* @brief  Get Implementer code @rmtoll SCB_CPUID    IMPLEMENTER   LL_CPUID_GetImplementer @retval Value should be equal to 0x41 for ARM */
__STATIC_INLINE uint32_t LL_CPUID_GetImplementer(void)
{
  return (uint32_t)(READ_BIT(SCB->CPUID, SCB_CPUID_IMPLEMENTER_Msk) >> SCB_CPUID_IMPLEMENTER_Pos);
}



__STATIC_INLINE uint32_t LL_CPUID_GetVariant(void)
{
  return (uint32_t)(READ_BIT(SCB->CPUID, SCB_CPUID_VARIANT_Msk) >> SCB_CPUID_VARIANT_Pos);
}



__STATIC_INLINE uint32_t LL_CPUID_GetArchitecture(void)
{
  return (uint32_t)(READ_BIT(SCB->CPUID, SCB_CPUID_ARCHITECTURE_Msk) >> SCB_CPUID_ARCHITECTURE_Pos);
}

/* @brief  Get Part number @rmtoll SCB_CPUID    PARTNO        LL_CPUID_GetParNo @retval Value should be equal to 0xC60 for Cortex-M0+ */
__STATIC_INLINE uint32_t LL_CPUID_GetParNo(void)
{
  return (uint32_t)(READ_BIT(SCB->CPUID, SCB_CPUID_PARTNO_Msk) >> SCB_CPUID_PARTNO_Pos);
}



__STATIC_INLINE uint32_t LL_CPUID_GetRevision(void)
{
  return (uint32_t)(READ_BIT(SCB->CPUID, SCB_CPUID_REVISION_Msk) >> SCB_CPUID_REVISION_Pos);
}

/* @} */

/* @} */

/* @} */

/* @} */

#ifdef __cplusplus
}
#endif

#endif /* __PY32F0XX_LL_CORTEX_H */

/* (C) COPYRIGHT Puya *****END OF FILE */
