/**
 * @file    py32f0xx_ll_iwdg.h
 * @author  MCU Application Team
 * @brief   Header file of IWDG LL module.
 * @attention
 */

/* Define to prevent recursive inclusion ------------------------------------- */
#ifndef PY32F0XX_LL_IWDG_H
#define PY32F0XX_LL_IWDG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------ */
#include "py32f0xx.h"

/* @addtogroup PY32F0xx_LL_Driver @{ */

#if defined(IWDG)

/* @defgroup IWDG_LL IWDG @{ */

/* Private types ------------------------------------------------------------- */
/* Private variables --------------------------------------------------------- */

/* Private constants --------------------------------------------------------- */
/* @defgroup IWDG_LL_Private_Constants IWDG Private Constants @{ */

#define LL_IWDG_KEY_RELOAD                 0x0000AAAAU               /* !< IWDG Reload Counter Enable */
#define LL_IWDG_KEY_ENABLE                 0x0000CCCCU               /* !< IWDG Peripheral Enable */
#define LL_IWDG_KEY_WR_ACCESS_ENABLE       0x00005555U               /* !< IWDG KR Write Access Enable */
#define LL_IWDG_KEY_WR_ACCESS_DISABLE      0x00000000U               /* !< IWDG KR Write Access Disable */

/* @} */

/* Private macros ------------------------------------------------------------ */

/* Exported types ------------------------------------------------------------ */
/* Exported constants -------------------------------------------------------- */
/* @defgroup IWDG_LL_Exported_Constants IWDG Exported Constants @{ */

/* @defgroup IWDG_LL_EC_GET_FLAG Get Flags Defines @brief    Flags defines which can be used with LL_IWDG_ReadReg function @{ */
#define LL_IWDG_SR_PVU                     IWDG_SR_PVU                           /* !< Watchdog prescaler value update */
#define LL_IWDG_SR_RVU                     IWDG_SR_RVU                           /* !< Watchdog counter reload value update */

/* @} */

/* @defgroup IWDG_LL_EC_PRESCALER  Prescaler Divider @{ */
#define LL_IWDG_PRESCALER_4                0x00000000U                                          /* !< Divider by 4 */
#define LL_IWDG_PRESCALER_8                (                              IWDG_PR_PR_0)         /* !< Divider by 8 */
#define LL_IWDG_PRESCALER_16               (               IWDG_PR_PR_1               )         /* !< Divider by 16 */
#define LL_IWDG_PRESCALER_32               (               IWDG_PR_PR_1 | IWDG_PR_PR_0)         /* !< Divider by 32 */
#define LL_IWDG_PRESCALER_64               (IWDG_PR_PR_2                              )         /* !< Divider by 64 */
#define LL_IWDG_PRESCALER_128              (IWDG_PR_PR_2 |                IWDG_PR_PR_0)         /* !< Divider by 128 */
#define LL_IWDG_PRESCALER_256              (IWDG_PR_PR_2 | IWDG_PR_PR_1               )         /* !< Divider by 256 */
/* @} */

/* @} */

/* Exported macro ------------------------------------------------------------ */
/* @defgroup IWDG_LL_Exported_Macros IWDG Exported Macros @{ */

/* @defgroup IWDG_LL_EM_WRITE_READ Common Write and read registers Macros @{ */



#define LL_IWDG_WriteReg(__INSTANCE__, __REG__, __VALUE__) WRITE_REG(__INSTANCE__->__REG__, (__VALUE__))

/* @brief  Read a value in IWDG register @param  __INSTANCE__ IWDG Instance @param  __REG__ Register to be read @retval Register value */
#define LL_IWDG_ReadReg(__INSTANCE__, __REG__) READ_REG(__INSTANCE__->__REG__)
/* @} */

/* @} */



/* Exported functions -------------------------------------------------------- */
/* @defgroup IWDG_LL_Exported_Functions IWDG Exported Functions @{ */
/* @defgroup IWDG_LL_EF_Configuration Configuration @{ */



__STATIC_INLINE void LL_IWDG_Enable(IWDG_TypeDef *IWDGx)
{
  WRITE_REG(IWDGx->KR, LL_IWDG_KEY_ENABLE);
}



__STATIC_INLINE void LL_IWDG_ReloadCounter(IWDG_TypeDef *IWDGx)
{
  WRITE_REG(IWDGx->KR, LL_IWDG_KEY_RELOAD);
}



__STATIC_INLINE void LL_IWDG_EnableWriteAccess(IWDG_TypeDef *IWDGx)
{
  WRITE_REG(IWDGx->KR, LL_IWDG_KEY_WR_ACCESS_ENABLE);
}

/* @brief  Disable write access to IWDG_PR, IWDG_RLR and IWDG_WINR registers @param  IWDGx IWDG Instance @retval None */
__STATIC_INLINE void LL_IWDG_DisableWriteAccess(IWDG_TypeDef *IWDGx)
{
  WRITE_REG(IWDGx->KR, LL_IWDG_KEY_WR_ACCESS_DISABLE);
}



__STATIC_INLINE void LL_IWDG_SetPrescaler(IWDG_TypeDef *IWDGx, uint32_t Prescaler)
{
  WRITE_REG(IWDGx->PR, IWDG_PR_PR & Prescaler);
}



__STATIC_INLINE uint32_t LL_IWDG_GetPrescaler(IWDG_TypeDef *IWDGx)
{
  return (uint32_t)(READ_REG(IWDGx->PR));
}



__STATIC_INLINE void LL_IWDG_SetReloadCounter(IWDG_TypeDef *IWDGx, uint32_t Counter)
{
  WRITE_REG(IWDGx->RLR, IWDG_RLR_RL & Counter);
}



__STATIC_INLINE uint32_t LL_IWDG_GetReloadCounter(IWDG_TypeDef *IWDGx)
{
  return (uint32_t)(READ_REG(IWDGx->RLR));
}



/* @} */

/* @defgroup IWDG_LL_EF_FLAG_Management FLAG Management @{ */



__STATIC_INLINE uint32_t LL_IWDG_IsActiveFlag_PVU(IWDG_TypeDef *IWDGx)
{
  return (READ_BIT(IWDGx->SR, IWDG_SR_PVU) == (IWDG_SR_PVU));
}



__STATIC_INLINE uint32_t LL_IWDG_IsActiveFlag_RVU(IWDG_TypeDef *IWDGx)
{
  return (READ_BIT(IWDGx->SR, IWDG_SR_RVU) == (IWDG_SR_RVU));
}



__STATIC_INLINE uint32_t LL_IWDG_IsReady(IWDG_TypeDef *IWDGx)
{
  return (READ_BIT(IWDGx->SR, IWDG_SR_PVU | IWDG_SR_RVU) == 0U);
}

/* @} */



/* @} */

/* @} */

#endif /* IWDG */

/* @} */

#ifdef __cplusplus
}
#endif

#endif /* PY32F0XX_LL_IWDG_H */

/* (C) COPYRIGHT Puya *****END OF FILE */
