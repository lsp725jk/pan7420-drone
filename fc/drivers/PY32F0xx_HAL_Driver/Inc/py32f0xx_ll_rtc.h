/**
 * @file    py32f0xx_ll_rtc.h
 * @author  MCU Application Team
 * @brief   Header file of RTC LL module.
 * @attention
 */
/* Define to prevent recursive inclusion ------------------------------------- */
#ifndef __PY32F0xx_LL_RTC_H
#define __PY32F0xx_LL_RTC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------ */
#include "py32f0xx.h"

/* @addtogroup PY32F0xx_LL_Driver @{ */

#if defined(RTC)

/* @defgroup RTC_LL RTC @{ */

/* Private types ------------------------------------------------------------- */
/* Private variables --------------------------------------------------------- */
/* Private constants --------------------------------------------------------- */

/* Private macros ------------------------------------------------------------ */
#if defined(USE_FULL_LL_DRIVER)
/* @defgroup RTC_LL_Private_Macros RTC Private Macros @{ */
/* @} */
#endif /* USE_FULL_LL_DRIVER */

/* Exported types ------------------------------------------------------------ */
#if defined(USE_FULL_LL_DRIVER)
/* @defgroup RTC_LL_ES_INIT RTC Exported Init structure @{ */

/* @brief  RTC Init structures definition */
typedef struct
{
  uint32_t AsynchPrescaler;     /* !< Specifies the RTC Asynchronous Predivider value. @ref LL_RTC_SetAsynchPrescaler(). */

  uint32_t OutPutSource;        /* This parameter can be a value of @ref LL_RTC_Output_Source @ref LL_RTC_SetOutputSource(). */

} LL_RTC_InitTypeDef;

/* @brief  RTC Time structure definition */
typedef struct
{
  uint8_t Hours;       

  uint8_t Minutes;     

  uint8_t Seconds;     
} LL_RTC_TimeTypeDef;



/* @brief  RTC Alarm structure definition */
typedef struct
{
  LL_RTC_TimeTypeDef AlarmTime;      /* !< Specifies the RTC Alarm Time members. */

} LL_RTC_AlarmTypeDef;

/* @} */
#endif /* USE_FULL_LL_DRIVER */

/* Exported constants -------------------------------------------------------- */
/* @defgroup RTC_LL_Exported_Constants RTC Exported Constants @{ */

#if defined(USE_FULL_LL_DRIVER)
/* @defgroup RTC_LL_EC_FORMAT FORMAT @{ */
#define LL_RTC_FORMAT_BIN                  (0x000000000U) /* !< Binary data format */
#define LL_RTC_FORMAT_BCD                  (0x000000001U) /* !< BCD data format */
/* @} */
#endif /* USE_FULL_LL_DRIVER */

/* @defgroup LL_RTC_Output_Source         Clock Source to output on the Tamper Pin @{ */
#define LL_RTC_CALIB_OUTPUT_NONE           (0x00000000U)                       /* !< Calibration output disabled */
#define LL_RTC_CALIB_OUTPUT_RTCCLOCK       BKP_RTCCR_CCO                       /* !< Calibration output is RTC Clock with a frequency divided by 64 on the TAMPER Pin */
#define LL_RTC_CALIB_OUTPUT_ALARM          BKP_RTCCR_ASOE                      /* !< Calibration output is Alarm pulse signal on the TAMPER pin */
#define LL_RTC_CALIB_OUTPUT_SECOND        (BKP_RTCCR_ASOS | BKP_RTCCR_ASOE)    /* !< Calibration output is Second pulse signal on the TAMPER pin */
/* @} */

/* @} */

/* Exported macro ------------------------------------------------------------ */
/* @defgroup RTC_LL_Exported_Macros RTC Exported Macros @{ */

/* @defgroup RTC_LL_EM_WRITE_READ Common Write and read registers Macros @{ */



#define LL_RTC_WriteReg(__INSTANCE__, __REG__, __VALUE__) WRITE_REG(__INSTANCE__->__REG__, (__VALUE__))

/* @brief  Read a value in RTC register @param  __INSTANCE__ RTC Instance @param  __REG__ Register to be read @retval Register value */
#define LL_RTC_ReadReg(__INSTANCE__, __REG__) READ_REG(__INSTANCE__->__REG__)
/* @} */

/* @defgroup RTC_LL_EM_Convert Convert helper Macros @{ */



#define __LL_RTC_CONVERT_BIN2BCD(__VALUE__) (uint8_t)((((__VALUE__) / 10U) << 4U) | ((__VALUE__) % 10U))



#define __LL_RTC_CONVERT_BCD2BIN(__VALUE__) (uint8_t)(((uint8_t)((__VALUE__) & (uint8_t)0xF0U) >> (uint8_t)0x4U) * 10U + ((__VALUE__) & (uint8_t)0x0FU))

/* @} */

/* @} */

/* Exported functions -------------------------------------------------------- */
/* @defgroup RTC_LL_Exported_Functions RTC Exported Functions @{ */

/* @defgroup RTC_LL_EF_Configuration Configuration @{ */



__STATIC_INLINE void LL_RTC_SetAsynchPrescaler(RTC_TypeDef *RTCx, uint32_t AsynchPrescaler)
{
  MODIFY_REG(RTCx->PRLH, RTC_PRLH_PRL, (AsynchPrescaler >> 16));
  MODIFY_REG(RTCx->PRLL, RTC_PRLL_PRL, (AsynchPrescaler & RTC_PRLL_PRL));
}



__STATIC_INLINE uint32_t LL_RTC_GetDivider(RTC_TypeDef *RTCx)
{
  register uint16_t Highprescaler = 0, Lowprescaler = 0;
  Highprescaler = READ_REG(RTCx->DIVH & RTC_DIVH_RTC_DIV);
  Lowprescaler  = READ_REG(RTCx->DIVL & RTC_DIVL_RTC_DIV);

  return (((uint32_t) Highprescaler << 16U) | Lowprescaler);
}



__STATIC_INLINE void LL_RTC_SetOutputSource(RTC_TypeDef *RTCx, uint32_t OutputSource)
{
  MODIFY_REG(RTCx->BKP_RTCCR, (BKP_RTCCR_CCO | BKP_RTCCR_ASOE | BKP_RTCCR_ASOS), OutputSource);
}



__STATIC_INLINE uint32_t LL_RTC_GetOutPutSource(RTC_TypeDef *RTCx)
{
  return (uint32_t)(READ_BIT(RTCx->BKP_RTCCR, (BKP_RTCCR_CCO | BKP_RTCCR_ASOE | BKP_RTCCR_ASOS)));
}

/* @brief  Enable the write protection for RTC registers. @param  RTCx RTC Instance @retval None */
__STATIC_INLINE void LL_RTC_EnableWriteProtection(RTC_TypeDef *RTCx)
{
  CLEAR_BIT(RTCx->CRL, RTC_CRL_CNF);
}

/* @brief  Disable the write protection for RTC registers. @param  RTCx RTC Instance @retval None */
__STATIC_INLINE void LL_RTC_DisableWriteProtection(RTC_TypeDef *RTCx)
{
  SET_BIT(RTCx->CRL, RTC_CRL_CNF);
}

/* @} */

/* @defgroup RTC_LL_EF_Time Time @{ */



__STATIC_INLINE void LL_RTC_TIME_Set(RTC_TypeDef *RTCx, uint32_t TimeCounter)
{
        /* Set RTC COUNTER MSB word */
  WRITE_REG(RTCx->CNTH, (TimeCounter >> 16U));
        /* Set RTC COUNTER LSB word */
  WRITE_REG(RTCx->CNTL, (TimeCounter & RTC_CNTL_RTC_CNT));
}



__STATIC_INLINE uint32_t LL_RTC_TIME_Get(RTC_TypeDef *RTCx)
{
  register uint16_t high = 0, low = 0;

  high = READ_REG(RTCx->CNTH & RTC_CNTH_RTC_CNT);
  low  = READ_REG(RTCx->CNTL & RTC_CNTL_RTC_CNT);
  return ((uint32_t)(((uint32_t) high << 16U) | low));
}

/* @} */

/* @defgroup RTC_LL_EF_ALARM  ALARM @{ */



__STATIC_INLINE void LL_RTC_ALARM_Set(RTC_TypeDef *RTCx, uint32_t AlarmCounter)
{
        /* Set RTC COUNTER MSB word */
  WRITE_REG(RTCx->ALRH, (AlarmCounter >> 16));
        /* Set RTC COUNTER LSB word */
  WRITE_REG(RTCx->ALRL, (AlarmCounter & RTC_ALRL_RTC_ALR));
}



/* @defgroup RTC_LL_EF_Calibration Calibration @{ */



__STATIC_INLINE void LL_RTC_CAL_SetCoarseDigital(RTC_TypeDef *RTCx, uint32_t Value)
{
  MODIFY_REG(RTCx->BKP_RTCCR, BKP_RTCCR_CAL, Value);
}

/* @brief  Get the coarse digital calibration value @param  RTCx RTC Instance @retval value of coarse calibration expressed in ppm */
__STATIC_INLINE uint32_t LL_RTC_CAL_GetCoarseDigital(RTC_TypeDef *RTCx)
{
  return (uint32_t)(READ_BIT(RTCx->BKP_RTCCR, BKP_RTCCR_CAL));
}
/* @} */

/* @defgroup RTC_LL_EF_FLAG_Management FLAG Management @{ */

/* @brief  Get Alarm  flag @rmtoll CRL          ALRF         LL_RTC_IsActiveFlag_ALR @param  RTCx RTC Instance @retval State of bit (1 or 0). */
__STATIC_INLINE uint32_t LL_RTC_IsActiveFlag_ALR(RTC_TypeDef *RTCx)
{
  return (READ_BIT(RTCx->CRL, RTC_CRL_ALRF) == (RTC_CRL_ALRF));
}

/* @brief  Clear Alarm flag @rmtoll CRL          ALRF         LL_RTC_ClearFlag_ALR @param  RTCx RTC Instance @retval None */
__STATIC_INLINE void LL_RTC_ClearFlag_ALR(RTC_TypeDef *RTCx)
{
  CLEAR_BIT(RTCx->CRL, RTC_CRL_ALRF);
}



__STATIC_INLINE uint32_t LL_RTC_IsActiveFlag_RS(RTC_TypeDef *RTCx)
{
  return (READ_BIT(RTCx->CRL, RTC_CRL_RSF) == (RTC_CRL_RSF));
}

/* @brief  Clear Registers synchronization flag @rmtoll CRL          RSF           LL_RTC_ClearFlag_RS @param  RTCx RTC Instance @retval None */
__STATIC_INLINE void LL_RTC_ClearFlag_RS(RTC_TypeDef *RTCx)
{
  CLEAR_BIT(RTCx->CRL, RTC_CRL_RSF);
}



__STATIC_INLINE uint32_t LL_RTC_IsActiveFlag_OW(RTC_TypeDef *RTCx)
{
  return (READ_BIT(RTCx->CRL, RTC_CRL_OWF) == (RTC_CRL_OWF));
}

/* @brief  Clear Registers OverFlow flag @rmtoll CRL          OWF           LL_RTC_ClearFlag_OW @param  RTCx RTC Instance @retval None */
__STATIC_INLINE void LL_RTC_ClearFlag_OW(RTC_TypeDef *RTCx)
{
  CLEAR_BIT(RTCx->CRL, RTC_CRL_OWF);
}



__STATIC_INLINE uint32_t LL_RTC_IsActiveFlag_SEC(RTC_TypeDef *RTCx)
{
  return (READ_BIT(RTCx->CRL, RTC_CRL_SECF) == (RTC_CRL_SECF));
}

/* @brief  Clear Registers synchronization flag @rmtoll CRL          SECF           LL_RTC_ClearFlag_SEC @param  RTCx RTC Instance @retval None */
__STATIC_INLINE void LL_RTC_ClearFlag_SEC(RTC_TypeDef *RTCx)
{
  CLEAR_BIT(RTCx->CRL, RTC_CRL_SECF);
}



__STATIC_INLINE uint32_t LL_RTC_IsActiveFlag_RTOF(RTC_TypeDef *RTCx)
{
  return (READ_BIT(RTCx->CRL, RTC_CRL_RTOFF) == (RTC_CRL_RTOFF));
}

/* @} */

/* @defgroup RTC_LL_EF_IT_Management IT Management @{ */



__STATIC_INLINE void LL_RTC_EnableIT_ALR(RTC_TypeDef *RTCx)
{
  SET_BIT(RTCx->CRH, RTC_CRH_ALRIE);
}



__STATIC_INLINE void LL_RTC_DisableIT_ALR(RTC_TypeDef *RTCx)
{
  CLEAR_BIT(RTCx->CRH, RTC_CRH_ALRIE);
}



__STATIC_INLINE uint32_t LL_RTC_IsEnabledIT_ALR(RTC_TypeDef *RTCx)
{
  return (READ_BIT(RTCx->CRH, RTC_CRH_ALRIE) == (RTC_CRH_ALRIE));
}



__STATIC_INLINE void LL_RTC_EnableIT_SEC(RTC_TypeDef *RTCx)
{
  SET_BIT(RTCx->CRH, RTC_CRH_SECIE);
}



__STATIC_INLINE void LL_RTC_DisableIT_SEC(RTC_TypeDef *RTCx)
{
  CLEAR_BIT(RTCx->CRH, RTC_CRH_SECIE);
}



__STATIC_INLINE uint32_t LL_RTC_IsEnabledIT_SEC(RTC_TypeDef *RTCx)
{
  return (READ_BIT(RTCx->CRH, RTC_CRH_SECIE) == (RTC_CRH_SECIE));
}



__STATIC_INLINE void LL_RTC_EnableIT_OW(RTC_TypeDef *RTCx)
{
  SET_BIT(RTCx->CRH, RTC_CRH_OWIE);
}



__STATIC_INLINE void LL_RTC_DisableIT_OW(RTC_TypeDef *RTCx)
{
  CLEAR_BIT(RTCx->CRH, RTC_CRH_OWIE);
}



__STATIC_INLINE uint32_t LL_RTC_IsEnabledIT_OW(RTC_TypeDef *RTCx)
{
  return (READ_BIT(RTCx->CRH, RTC_CRH_OWIE) == (RTC_CRH_OWIE));
}
/* @} */

#if defined(USE_FULL_LL_DRIVER)
/* @defgroup RTC_LL_EF_Init Initialization and de-initialization functions @{ */

ErrorStatus LL_RTC_DeInit(RTC_TypeDef *RTCx);
ErrorStatus LL_RTC_Init(RTC_TypeDef *RTCx, LL_RTC_InitTypeDef *RTC_InitStruct);
void        LL_RTC_StructInit(LL_RTC_InitTypeDef *RTC_InitStruct);
ErrorStatus LL_RTC_TIME_Init(RTC_TypeDef *RTCx, uint32_t RTC_Format, LL_RTC_TimeTypeDef *RTC_TimeStruct);
void        LL_RTC_TIME_StructInit(LL_RTC_TimeTypeDef *RTC_TimeStruct);
ErrorStatus LL_RTC_ALARM_Init(RTC_TypeDef *RTCx, uint32_t RTC_Format, LL_RTC_AlarmTypeDef *RTC_AlarmStruct);
void        LL_RTC_ALARM_StructInit(LL_RTC_AlarmTypeDef *RTC_AlarmStruct);
ErrorStatus LL_RTC_EnterInitMode(RTC_TypeDef *RTCx);
ErrorStatus LL_RTC_ExitInitMode(RTC_TypeDef *RTCx);
ErrorStatus LL_RTC_WaitForSynchro(RTC_TypeDef *RTCx);
ErrorStatus LL_RTC_TIME_SetCounter(RTC_TypeDef *RTCx, uint32_t TimeCounter);
ErrorStatus LL_RTC_ALARM_SetCounter(RTC_TypeDef *RTCx, uint32_t AlarmCounter);

/* @} */
#endif /* USE_FULL_LL_DRIVER */

/* @} */

/* @} */

/* @} */

#endif /* defined(RTC) */

/* @} */

#ifdef __cplusplus
}
#endif

#endif /* __PY32F0xx_LL_RTC_H */

/* (C) COPYRIGHT Puya *****END OF FILE */
