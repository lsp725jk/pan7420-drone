/**
 * @file    py32f0xx_ll_crc.h
 * @author  MCU Application Team
 * @brief   Header file of CRC LL module.
 * @attention
 */

/* Define to prevent recursive inclusion ------------------------------------- */
#ifndef PY32F0xx_LL_CRC_H
#define PY32F0xx_LL_CRC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------ */
#include "py32f0xx.h"

/* @addtogroup PY32F0xx_LL_Driver @{ */

#if defined(CRC)

/* @defgroup CRC_LL CRC @{ */

/* Private types ------------------------------------------------------------- */
/* Private variables --------------------------------------------------------- */
/* Private constants --------------------------------------------------------- */
/* Private macros ------------------------------------------------------------ */

/* Exported types ------------------------------------------------------------ */
/* Exported constants -------------------------------------------------------- */
/* @defgroup CRC_LL_Exported_Constants CRC Exported Constants @{ */

/* @} */

/* Exported macro ------------------------------------------------------------ */
/* @defgroup CRC_LL_Exported_Macros CRC Exported Macros @{ */

/* @defgroup CRC_LL_EM_WRITE_READ Common Write and read registers Macros @{ */



#define LL_CRC_WriteReg(__INSTANCE__, __REG__, __VALUE__) WRITE_REG(__INSTANCE__->__REG__, __VALUE__)

/* @brief  Read a value in CRC register @param  __INSTANCE__ CRC Instance @param  __REG__ Register to be read @retval Register value */
#define LL_CRC_ReadReg(__INSTANCE__, __REG__) READ_REG(__INSTANCE__->__REG__)
/* @} */

/* @} */



/* Exported functions -------------------------------------------------------- */
/* @defgroup CRC_LL_Exported_Functions CRC Exported Functions @{ */

/* @defgroup CRC_LL_EF_Configuration CRC Configuration functions @{ */

/* @brief  Reset the CRC calculation unit. @param  CRCx CRC Instance @retval None */
__STATIC_INLINE void LL_CRC_ResetCRCCalculationUnit(CRC_TypeDef *CRCx)
{
  SET_BIT(CRCx->CR, CRC_CR_RESET);
}

/* @} */

/* @defgroup CRC_LL_EF_Data_Management Data_Management @{ */



__STATIC_INLINE void LL_CRC_FeedData32(CRC_TypeDef *CRCx, uint32_t InData)
{
  WRITE_REG(CRCx->DR, InData);
}



__STATIC_INLINE uint32_t LL_CRC_ReadData32(CRC_TypeDef *CRCx)
{
  return (uint32_t)(READ_REG(CRCx->DR));
}



__STATIC_INLINE uint32_t LL_CRC_Read_IDR(CRC_TypeDef *CRCx)
{
  return (uint32_t)(READ_REG(CRCx->IDR));
}



__STATIC_INLINE void LL_CRC_Write_IDR(CRC_TypeDef *CRCx, uint32_t InData)
{
  *((uint8_t __IO *)(&CRCx->IDR)) = (uint8_t) InData;
}
/* @} */

#if defined(USE_FULL_LL_DRIVER)
/* @defgroup CRC_LL_EF_Init Initialization and de-initialization functions @{ */

ErrorStatus LL_CRC_DeInit(CRC_TypeDef *CRCx);

/* @} */
#endif /* USE_FULL_LL_DRIVER */

/* @} */

/* @} */

#endif /* defined(CRC) */

/* @} */

#ifdef __cplusplus
}
#endif

#endif /* PY32F0xx_LL_CRC_H */

/* (C) COPYRIGHT Puya *****END OF FILE */
