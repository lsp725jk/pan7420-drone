/**
 * @file    py32f0xx_hal_rtc_ex.c
 * @author  MCU Application Team
 * @brief   Extended RTC HAL module driver.
 */

/* Includes ------------------------------------------------------------------ */
#include "py32f0xx_hal.h"

/**
 * @addtogroup PY32F0xx_HAL_Driver @{
 */

#ifdef HAL_RTC_MODULE_ENABLED

/* @defgroup RTCEx RTCEx @brief RTC Extended HAL module driver @{ */

/* Private typedef ----------------------------------------------------------- */
/* Private define ------------------------------------------------------------ */
/* Private macro ------------------------------------------------------------- */
/* @defgroup RTCEx_Private_Macros RTCEx Private Macros @{ */
/* @} */

/* Private variables --------------------------------------------------------- */
/* Private function prototypes ----------------------------------------------- */
/* Private functions --------------------------------------------------------- */

/* @defgroup RTCEx_Exported_Functions RTCEx Exported Functions @{ */

/* @defgroup RTCEx_Exported_Functions_Group2 RTC Second functions @brief    RTC Second functions @verbatim ##### RTC Second functions ##### */



HAL_StatusTypeDef HAL_RTCEx_SetSecond_IT(RTC_HandleTypeDef *hrtc)
{
        /* Check input parameters */
  if (hrtc == NULL)
  {
    return HAL_ERROR;
  }

        /* Process Locked */
  __HAL_LOCK(hrtc);

  hrtc->State = HAL_RTC_STATE_BUSY;

        /* Enable Second interuption */
  __HAL_RTC_SECOND_ENABLE_IT(hrtc, RTC_IT_SEC);

  hrtc->State = HAL_RTC_STATE_READY;

        /* Process Unlocked */
  __HAL_UNLOCK(hrtc);

  return HAL_OK;
}



HAL_StatusTypeDef HAL_RTCEx_DeactivateSecond(RTC_HandleTypeDef *hrtc)
{
        /* Check input parameters */
  if (hrtc == NULL)
  {
    return HAL_ERROR;
  }

        /* Process Locked */
  __HAL_LOCK(hrtc);

  hrtc->State = HAL_RTC_STATE_BUSY;

        /* Deactivate Second interuption */
  __HAL_RTC_SECOND_DISABLE_IT(hrtc, RTC_IT_SEC);

  hrtc->State = HAL_RTC_STATE_READY;

        /* Process Unlocked */
  __HAL_UNLOCK(hrtc);

  return HAL_OK;
}



void HAL_RTCEx_RTCIRQHandler(RTC_HandleTypeDef *hrtc)
{

  if (__HAL_RTC_SECOND_GET_IT_SOURCE(hrtc, RTC_IT_SEC))
  {
                /* Get the status of the Interrupt */
    if (__HAL_RTC_SECOND_GET_FLAG(hrtc, RTC_FLAG_SEC))
    {
                        /* Check if Overrun occurred */
      if (__HAL_RTC_SECOND_GET_FLAG(hrtc, RTC_FLAG_OW))
      {
                                /* Second error callback */
        HAL_RTCEx_RTCEventErrorCallback(hrtc);

                                /* Clear flag Second */
        __HAL_RTC_OVERFLOW_CLEAR_FLAG(hrtc, RTC_FLAG_OW);

                                /* Change RTC state */
        hrtc->State = HAL_RTC_STATE_ERROR;
      }
      else
      {
                                /* Second callback */
        HAL_RTCEx_RTCEventCallback(hrtc);

                                /* Change RTC state */
        hrtc->State = HAL_RTC_STATE_READY;
      }

                        /* Clear flag Second */
      __HAL_RTC_SECOND_CLEAR_FLAG(hrtc, RTC_FLAG_SEC);
    }
  }
  if (__HAL_RTC_ALARM_GET_IT_SOURCE(hrtc, RTC_IT_ALRA))
  {
                /* Get the status of the Interrupt */
    if (__HAL_RTC_ALARM_GET_FLAG(hrtc, RTC_FLAG_ALRAF) != (uint32_t)RESET)
    {
                        /* AlarmA callback */
#if (USE_HAL_RTC_REGISTER_CALLBACKS == 1)
      hrtc->AlarmAEventCallback(hrtc);
#else
      HAL_RTC_AlarmAEventCallback(hrtc);
#endif /* USE_HAL_RTC_REGISTER_CALLBACKS */

                        /* Clear the Alarm interrupt pending bit */
      __HAL_RTC_ALARM_CLEAR_FLAG(hrtc, RTC_FLAG_ALRAF);
    }
  }

}



__weak void HAL_RTCEx_RTCEventCallback(RTC_HandleTypeDef *hrtc)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(hrtc);

}



__weak void HAL_RTCEx_RTCEventErrorCallback(RTC_HandleTypeDef *hrtc)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(hrtc);

}

/* @} */



HAL_StatusTypeDef HAL_RTCEx_SetSmoothCalib(RTC_HandleTypeDef *hrtc, uint32_t SmoothCalibPeriod, uint32_t SmoothCalibPlusPulses, uint32_t SmouthCalibMinusPulsesValue)
{
        /* Check input parameters */
  if (hrtc == NULL)
  {
    return HAL_ERROR;
  }
        /* Prevent unused argument(s) compilation warning */
  UNUSED(SmoothCalibPeriod);
  UNUSED(SmoothCalibPlusPulses);

        /* Check the parameters */
  assert_param(IS_RTC_SMOOTH_CALIB_MINUS(SmouthCalibMinusPulsesValue));

        /* Process Locked */
  __HAL_LOCK(hrtc);

  hrtc->State = HAL_RTC_STATE_BUSY;

        /* Sets RTC Clock Calibration value. */
  MODIFY_REG(RTC->BKP_RTCCR, BKP_RTCCR_CAL, SmouthCalibMinusPulsesValue);

        /* Change RTC state */
  hrtc->State = HAL_RTC_STATE_READY;

        /* Process Unlocked */
  __HAL_UNLOCK(hrtc);

  return HAL_OK;
}

/* @} */

/* @} */

/* @} */

#endif /* HAL_RTC_MODULE_ENABLED */

/* @} */

/* (C) COPYRIGHT Puya *****END OF FILE */

