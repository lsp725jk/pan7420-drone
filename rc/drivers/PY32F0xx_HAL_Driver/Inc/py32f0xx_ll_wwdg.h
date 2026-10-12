/**
 * @file    py32f0xx_ll_wwdg.h
 * @author  MCU Application Team
 * @brief   Header file of WWDG LL module.
 * @attention
 */

/* Define to prevent recursive inclusion ------------------------------------- */
#ifndef PY32F0XX_LL_WWDG_H
#define PY32F0XX_LL_WWDG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------ */
#include "py32f0xx.h"

/* @addtogroup PY32F0xx_LL_Driver @{ */

#if defined (WWDG)
/* @defgroup WWDG_LL WWDG @{ */

/* Private types ------------------------------------------------------------- */
/* Private variables --------------------------------------------------------- */
/* Private constants --------------------------------------------------------- */
/* Private macros ------------------------------------------------------------ */
/* Exported types ------------------------------------------------------------ */
/* Exported constants -------------------------------------------------------- */
/* @defgroup WWDG_LL_Exported_Constants WWDG Exported Constants @{ */

/* @defgroup WWDG_LL_EC_IT IT Defines @brief    IT defines which can be used with LL_WWDG_ReadReg and  LL_WWDG_WriteReg functions @{ */
#define LL_WWDG_CFR_EWI                    WWDG_CFR_EWI
/* @} */

/* @defgroup WWDG_LL_EC_PRESCALER  PRESCALER @{ */
#define LL_WWDG_PRESCALER_1                0x00000000U                                             /* !< WWDG counter clock = (PCLK1/4096)/1 */
#define LL_WWDG_PRESCALER_2                (                   WWDG_CFR_WDGTB_0)                   /* !< WWDG counter clock = (PCLK1/4096)/2 */
#define LL_WWDG_PRESCALER_4                (WWDG_CFR_WDGTB_1                   )                   /* !< WWDG counter clock = (PCLK1/4096)/4 */
#define LL_WWDG_PRESCALER_8                (WWDG_CFR_WDGTB_0 | WWDG_CFR_WDGTB_1)                   /* !< WWDG counter clock = (PCLK1/4096)/8 */
/* @} */

/* @} */

/* Exported macro ------------------------------------------------------------ */
/* @defgroup WWDG_LL_Exported_Macros WWDG Exported Macros @{ */
/* @defgroup WWDG_LL_EM_WRITE_READ Common Write and read registers macros @{ */

#define LL_WWDG_WriteReg(__INSTANCE__, __REG__, __VALUE__) WRITE_REG(__INSTANCE__->__REG__, (__VALUE__))

/* @brief  Read a value in WWDG register @param  __INSTANCE__ WWDG Instance @param  __REG__ Register to be read @retval Register value */
#define LL_WWDG_ReadReg(__INSTANCE__, __REG__) READ_REG(__INSTANCE__->__REG__)
/* @} */

/* @} */

/* Exported functions -------------------------------------------------------- */
/* @defgroup WWDG_LL_Exported_Functions WWDG Exported Functions @{ */

/* @defgroup WWDG_LL_EF_Configuration Configuration @{ */

__STATIC_INLINE void LL_WWDG_Enable(WWDG_TypeDef *WWDGx)
{
  SET_BIT(WWDGx->CR, WWDG_CR_WDGA);
}



__STATIC_INLINE uint32_t LL_WWDG_IsEnabled(WWDG_TypeDef *WWDGx)
{
  return (READ_BIT(WWDGx->CR, WWDG_CR_WDGA) == (WWDG_CR_WDGA));
}



__STATIC_INLINE void LL_WWDG_SetCounter(WWDG_TypeDef *WWDGx, uint32_t Counter)
{
  MODIFY_REG(WWDGx->CR, WWDG_CR_T, Counter);
}



__STATIC_INLINE uint32_t LL_WWDG_GetCounter(WWDG_TypeDef *WWDGx)
{
  return (READ_BIT(WWDGx->CR, WWDG_CR_T));
}



__STATIC_INLINE void LL_WWDG_SetPrescaler(WWDG_TypeDef *WWDGx, uint32_t Prescaler)
{
  MODIFY_REG(WWDGx->CFR, WWDG_CFR_WDGTB, Prescaler);
}



__STATIC_INLINE uint32_t LL_WWDG_GetPrescaler(WWDG_TypeDef *WWDGx)
{
  return (READ_BIT(WWDGx->CFR, WWDG_CFR_WDGTB));
}



__STATIC_INLINE void LL_WWDG_SetWindow(WWDG_TypeDef *WWDGx, uint32_t Window)
{
  MODIFY_REG(WWDGx->CFR, WWDG_CFR_W, Window);
}



__STATIC_INLINE uint32_t LL_WWDG_GetWindow(WWDG_TypeDef *WWDGx)
{
  return (READ_BIT(WWDGx->CFR, WWDG_CFR_W));
}

/* @} */

/* @defgroup WWDG_LL_EF_FLAG_Management FLAG Management @{ */

__STATIC_INLINE uint32_t LL_WWDG_IsActiveFlag_EWKUP(WWDG_TypeDef *WWDGx)
{
  return (READ_BIT(WWDGx->SR, WWDG_SR_EWIF) == (WWDG_SR_EWIF));
}



__STATIC_INLINE void LL_WWDG_ClearFlag_EWKUP(WWDG_TypeDef *WWDGx)
{
  WRITE_REG(WWDGx->SR, ~WWDG_SR_EWIF);
}

/* @} */

/* @defgroup WWDG_LL_EF_IT_Management IT Management @{ */

__STATIC_INLINE void LL_WWDG_EnableIT_EWKUP(WWDG_TypeDef *WWDGx)
{
  SET_BIT(WWDGx->CFR, WWDG_CFR_EWI);
}



__STATIC_INLINE uint32_t LL_WWDG_IsEnabledIT_EWKUP(WWDG_TypeDef *WWDGx)
{
  return (READ_BIT(WWDGx->CFR, WWDG_CFR_EWI) == (WWDG_CFR_EWI));
}

/* @} */

/* @} */

/* @} */

#endif /* WWDG */

/* @} */

#ifdef __cplusplus
}
#endif

#endif /* PY32F0XX_LL_WWDG_H */

/* (C) COPYRIGHT Puya *****END OF FILE */
