/**
 * @file    py32f0xx_hal_rtc_ex.h
 * @author  MCU Application Team
 * @brief   Header file of RTC HAL Extension module.
 * @attention
 */

/* Define to prevent recursive inclusion ------------------------------------- */
#ifndef __PY32F0xxHAL_RTC_EX_H
#define __PY32F0xxHAL_RTC_EX_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------ */
#include "py32f0xx_hal_def.h"

/* @addtogroup PY32F0xx_HAL_Driver @{ */

/* @addtogroup RTCEx @{ */

/* @addtogroup RTCEx_Private_Macros @{ */



/* @defgroup RTCEx_IS_RTC_Definitions Private macros to check input parameters @{ */
#define IS_RTC_SMOOTH_CALIB_MINUS(__VALUE__) ((__VALUE__) <= 0x0000007FU)

/* @} */

/* @} */



/* Exported macro ------------------------------------------------------------ */
/* @defgroup RTCEx_Exported_Macros RTCEx Exported Macros @{ */



#define __HAL_RTC_SECOND_ENABLE_IT(__HANDLE__, __INTERRUPT__)  SET_BIT((__HANDLE__)->Instance->CRH, (__INTERRUPT__))



#define __HAL_RTC_SECOND_DISABLE_IT(__HANDLE__, __INTERRUPT__) CLEAR_BIT((__HANDLE__)->Instance->CRH, (__INTERRUPT__))



#define __HAL_RTC_SECOND_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__)      ((((((__HANDLE__)->Instance->CRH)& ((__INTERRUPT__)))) != RESET)? SET : RESET)



#define __HAL_RTC_SECOND_GET_FLAG(__HANDLE__, __FLAG__)        (((((__HANDLE__)->Instance->CRL) & (__FLAG__)) != RESET)? SET : RESET)



#define __HAL_RTC_SECOND_CLEAR_FLAG(__HANDLE__, __FLAG__)      CLEAR_BIT((__HANDLE__)->Instance->CRL,(__FLAG__))



#define __HAL_RTC_OVERFLOW_ENABLE_IT(__HANDLE__, __INTERRUPT__)  SET_BIT((__HANDLE__)->Instance->CRH, (__INTERRUPT__))



#define __HAL_RTC_OVERFLOW_DISABLE_IT(__HANDLE__, __INTERRUPT__) CLEAR_BIT((__HANDLE__)->Instance->CRH, (__INTERRUPT__))



#define __HAL_RTC_OVERFLOW_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__)    ((((((__HANDLE__)->Instance->CRH)& ((__INTERRUPT__))) ) != RESET)? SET : RESET)



#define __HAL_RTC_OVERFLOW_GET_FLAG(__HANDLE__, __FLAG__)        (((((__HANDLE__)->Instance->CRL) & (__FLAG__)) != RESET)? SET : RESET)



#define __HAL_RTC_OVERFLOW_CLEAR_FLAG(__HANDLE__, __FLAG__)      CLEAR_BIT((__HANDLE__)->Instance->CRL,(__FLAG__))

/* @} */

/* Exported functions -------------------------------------------------------- */
/* @addtogroup RTCEx_Exported_Functions @{ */

/* RTC Second functions */
/* @addtogroup RTCEx_Exported_Functions_Group2 @{ */
HAL_StatusTypeDef HAL_RTCEx_SetSecond_IT(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_DeactivateSecond(RTC_HandleTypeDef *hrtc);
void              HAL_RTCEx_RTCIRQHandler(RTC_HandleTypeDef *hrtc);
void              HAL_RTCEx_RTCEventCallback(RTC_HandleTypeDef *hrtc);
void              HAL_RTCEx_RTCEventErrorCallback(RTC_HandleTypeDef *hrtc);

/* @} */

/* Extension Control functions */
/* @addtogroup RTCEx_Exported_Functions_Group3 @{ */
HAL_StatusTypeDef HAL_RTCEx_SetSmoothCalib(RTC_HandleTypeDef *hrtc, uint32_t SmoothCalibPeriod, uint32_t SmoothCalibPlusPulses, uint32_t SmouthCalibMinusPulsesValue);
/* @} */

/* @} */

/* @} */

/* @} */

#ifdef __cplusplus
}
#endif

#endif /* __PY32F0xxHAL_RTC_EX_H */

/* (C) COPYRIGHT Puya *****END OF FILE */
