/**
 * @file    py32f0xx_hal_rtc.c
 * @author  MCU Application Team
 * @brief   RTC HAL module driver.
 */

/* Includes ------------------------------------------------------------------ */
#include "py32f0xx_hal.h"

/**
 * @addtogroup PY32F0xx_HAL_Driver @{
 */

/* @defgroup RTC RTC @brief RTC HAL module driver @{ */

#ifdef HAL_RTC_MODULE_ENABLED

/* Private typedef ----------------------------------------------------------- */
/* Private define ------------------------------------------------------------ */
/* @defgroup RTC_Private_Constants RTC Private Constants @{ */
#define RTC_ALARM_RESETVALUE_REGISTER    (uint16_t)0xFFFF
#define RTC_ALARM_RESETVALUE             0xFFFFFFFFU

/* @} */

/* Private macro ------------------------------------------------------------- */
/* @defgroup RTC_Private_Macros RTC Private Macros @{ */
/* @} */

/* Private variables --------------------------------------------------------- */
static __IO uint32_t      gCounter_Alarm = RTC_ALARM_RESETVALUE;

/* Private function prototypes ----------------------------------------------- */
/* @defgroup RTC_Private_Functions RTC Private Functions @{ */
static uint32_t           RTC_ReadTimeCounter(RTC_HandleTypeDef *hrtc);
static HAL_StatusTypeDef  RTC_WriteTimeCounter(RTC_HandleTypeDef *hrtc, uint32_t TimeCounter);
static uint32_t           RTC_ReadAlarmCounter(RTC_HandleTypeDef *hrtc);
static HAL_StatusTypeDef  RTC_WriteAlarmCounter(RTC_HandleTypeDef *hrtc, uint32_t AlarmCounter);
static HAL_StatusTypeDef  RTC_EnterInitMode(RTC_HandleTypeDef *hrtc);
static HAL_StatusTypeDef  RTC_ExitInitMode(RTC_HandleTypeDef *hrtc);
static uint8_t            RTC_ByteToBcd2(uint8_t Value);
static uint8_t            RTC_Bcd2ToByte(uint8_t Value);
static uint8_t            RTC_IsLeapYear(uint16_t nYear);
static void               RTC_DateUpdate(RTC_HandleTypeDef *hrtc, uint32_t DayElapsed);
static uint8_t            RTC_WeekDayNum(uint32_t nYear, uint8_t nMonth, uint8_t nDay);

/* @} */

/* Private functions --------------------------------------------------------- */
/* @defgroup RTC_Exported_Functions RTC Exported Functions @{ */



HAL_StatusTypeDef HAL_RTC_Init(RTC_HandleTypeDef *hrtc)
{
  uint32_t prescaler = 0U;
        /* Check input parameters */
  if (hrtc == NULL)
  {
    return HAL_ERROR;
  }

        /* Check the parameters */
  assert_param(IS_RTC_ALL_INSTANCE(hrtc->Instance));
  assert_param(IS_RTC_CALIB_OUTPUT(hrtc->Init.OutPut));
  assert_param(IS_RTC_ASYNCH_PREDIV(hrtc->Init.AsynchPrediv));

#if (USE_HAL_RTC_REGISTER_CALLBACKS == 1)
  if (hrtc->State == HAL_RTC_STATE_RESET)
  {
                /* Allocate lock resource and initialize it */
    hrtc->Lock = HAL_UNLOCKED;

    hrtc->AlarmAEventCallback          =  HAL_RTC_AlarmAEventCallback;                /* Legacy weak AlarmAEventCallback */

    if (hrtc->MspInitCallback == NULL)
    {
      hrtc->MspInitCallback = HAL_RTC_MspInit;
    }
                /* Init the low level hardware */
    hrtc->MspInitCallback(hrtc);

    if (hrtc->MspDeInitCallback == NULL)
    {
      hrtc->MspDeInitCallback = HAL_RTC_MspDeInit;
    }
  }
#else
  if (hrtc->State == HAL_RTC_STATE_RESET)
  {
                /* Allocate lock resource and initialize it */
    hrtc->Lock = HAL_UNLOCKED;

                /* Initialize RTC MSP */
    HAL_RTC_MspInit(hrtc);
  }
#endif /* (USE_HAL_RTC_REGISTER_CALLBACKS) */

        /* Set RTC state */
  hrtc->State = HAL_RTC_STATE_BUSY;

        /* Waiting for synchro */
  if (HAL_RTC_WaitForSynchro(hrtc) != HAL_OK)
  {
                /* Set RTC state */
    hrtc->State = HAL_RTC_STATE_ERROR;

    return HAL_ERROR;
  }

        /* Set Initialization mode */
  if (RTC_EnterInitMode(hrtc) != HAL_OK)
  {
                /* Set RTC state */
    hrtc->State = HAL_RTC_STATE_ERROR;

    return HAL_ERROR;
  }
  else
  {
                /* Clear Flags Bits */
    CLEAR_BIT(hrtc->Instance->CRL, (RTC_FLAG_OW | RTC_FLAG_ALRAF | RTC_FLAG_SEC));

                /* Set the signal which will be routed to RTC Tamper pin */
    MODIFY_REG(hrtc->Instance->BKP_RTCCR, (BKP_RTCCR_CCO | BKP_RTCCR_ASOE | BKP_RTCCR_ASOS), hrtc->Init.OutPut);

    if (hrtc->Init.AsynchPrediv != RTC_AUTO_1_SECOND)
    {
                        /* RTC Prescaler provided directly by end-user */
      prescaler = hrtc->Init.AsynchPrediv;
    }
    else
    {
                        /* RTC Prescaler will be automatically calculated to get 1 second timebase */
                        /* Get the RTCCLK frequency */
      prescaler = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_RTC);

                        /* Check that RTC clock is enabled */
      if (prescaler == 0U)
      {
                                /* Should not happen. Frequency is not available */
        hrtc->State = HAL_RTC_STATE_ERROR;
        return HAL_ERROR;
      }
      else
      {
                                /* RTC period = RTCCLK/(RTC_PR + 1) */
        prescaler = prescaler - 1U;
      }
    }

                /* Configure the RTC_PRLH / RTC_PRLL */
    MODIFY_REG(hrtc->Instance->PRLH, RTC_PRLH_PRL, (prescaler >> 16U));
    MODIFY_REG(hrtc->Instance->PRLL, RTC_PRLL_PRL, (prescaler & RTC_PRLL_PRL));

                /* Wait for synchro */
    if (RTC_ExitInitMode(hrtc) != HAL_OK)
    {
      hrtc->State = HAL_RTC_STATE_ERROR;

      return HAL_ERROR;
    }

                /* Initialize date to 1st of January 2000 */
    hrtc->DateToUpdate.Year = 0x00U;
    hrtc->DateToUpdate.Month = RTC_MONTH_JANUARY;
    hrtc->DateToUpdate.Date = 0x01U;

                /* Set RTC state */
    hrtc->State = HAL_RTC_STATE_READY;

    return HAL_OK;
  }
}



HAL_StatusTypeDef HAL_RTC_DeInit(RTC_HandleTypeDef *hrtc)
{
        /* Check input parameters */
  if (hrtc == NULL)
  {
    return HAL_ERROR;
  }

        /* Check the parameters */
  assert_param(IS_RTC_ALL_INSTANCE(hrtc->Instance));

        /* Set RTC state */
  hrtc->State = HAL_RTC_STATE_BUSY;

        /* Set Initialization mode */
  if (RTC_EnterInitMode(hrtc) != HAL_OK)
  {
                /* Set RTC state */
    hrtc->State = HAL_RTC_STATE_ERROR;

                /* Release Lock */
    __HAL_UNLOCK(hrtc);

    return HAL_ERROR;
  }
  else
  {
    CLEAR_REG(hrtc->Instance->CNTL);
    CLEAR_REG(hrtc->Instance->CNTH);
    WRITE_REG(hrtc->Instance->PRLL, 0x00008000U);
    CLEAR_REG(hrtc->Instance->PRLH);

                /* Reset All CRH/CRL bits */
    CLEAR_REG(hrtc->Instance->CRH);
    CLEAR_REG(hrtc->Instance->CRL);

    if (RTC_ExitInitMode(hrtc) != HAL_OK)
    {
      hrtc->State = HAL_RTC_STATE_ERROR;

                        /* Process Unlocked */
      __HAL_UNLOCK(hrtc);

      return HAL_ERROR;
    }
  }

        /* Wait for synchro */
  HAL_RTC_WaitForSynchro(hrtc);

        /* Clear RSF flag */
  CLEAR_BIT(hrtc->Instance->CRL, RTC_FLAG_RSF);

#if (USE_HAL_RTC_REGISTER_CALLBACKS == 1)
  if (hrtc->MspDeInitCallback == NULL)
  {
    hrtc->MspDeInitCallback = HAL_RTC_MspDeInit;
  }

        /* DeInit the low level hardware: CLOCK, NVIC. */
  hrtc->MspDeInitCallback(hrtc);

#else
        /* De-Initialize RTC MSP */
  HAL_RTC_MspDeInit(hrtc);
#endif /* (USE_HAL_RTC_REGISTER_CALLBACKS) */

  hrtc->State = HAL_RTC_STATE_RESET;

        /* Release Lock */
  __HAL_UNLOCK(hrtc);

  return HAL_OK;
}

#if (USE_HAL_RTC_REGISTER_CALLBACKS == 1)

HAL_StatusTypeDef HAL_RTC_RegisterCallback(RTC_HandleTypeDef *hrtc, HAL_RTC_CallbackIDTypeDef CallbackID, pRTC_CallbackTypeDef pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (pCallback == NULL)
  {
    return HAL_ERROR;
  }

        /* Process locked */
  __HAL_LOCK(hrtc);

  if (HAL_RTC_STATE_READY == hrtc->State)
  {
    switch (CallbackID)
    {
    case HAL_RTC_ALARM_A_EVENT_CB_ID :
      hrtc->AlarmAEventCallback = pCallback;
      break;

    case HAL_RTC_TAMPER1_EVENT_CB_ID :
      hrtc->Tamper1EventCallback = pCallback;
      break;

    case HAL_RTC_MSPINIT_CB_ID :
      hrtc->MspInitCallback = pCallback;
      break;

    case HAL_RTC_MSPDEINIT_CB_ID :
      hrtc->MspDeInitCallback = pCallback;
      break;

    default :
                        /* Return error status */
      status =  HAL_ERROR;
      break;
    }
  }
  else if (HAL_RTC_STATE_RESET == hrtc->State)
  {
    switch (CallbackID)
    {
    case HAL_RTC_MSPINIT_CB_ID :
      hrtc->MspInitCallback = pCallback;
      break;

    case HAL_RTC_MSPDEINIT_CB_ID :
      hrtc->MspDeInitCallback = pCallback;
      break;

    default :
                        /* Return error status */
      status =  HAL_ERROR;
      break;
    }
  }
  else
  {
                /* Return error status */
    status =  HAL_ERROR;
  }

        /* Release Lock */
  __HAL_UNLOCK(hrtc);

  return status;
}



HAL_StatusTypeDef HAL_RTC_UnRegisterCallback(RTC_HandleTypeDef *hrtc, HAL_RTC_CallbackIDTypeDef CallbackID)
{
  HAL_StatusTypeDef status = HAL_OK;

        /* Process locked */
  __HAL_LOCK(hrtc);

  if (HAL_RTC_STATE_READY == hrtc->State)
  {
    switch (CallbackID)
    {
    case HAL_RTC_ALARM_A_EVENT_CB_ID :
      hrtc->AlarmAEventCallback = HAL_RTC_AlarmAEventCallback;                     /* Legacy weak AlarmAEventCallback */
      break;

    case HAL_RTC_MSPINIT_CB_ID :
      hrtc->MspInitCallback = HAL_RTC_MspInit;
      break;

    case HAL_RTC_MSPDEINIT_CB_ID :
      hrtc->MspDeInitCallback = HAL_RTC_MspDeInit;
      break;

    default :
                        /* Return error status */
      status =  HAL_ERROR;
      break;
    }
  }
  else if (HAL_RTC_STATE_RESET == hrtc->State)
  {
    switch (CallbackID)
    {
    case HAL_RTC_MSPINIT_CB_ID :
      hrtc->MspInitCallback = HAL_RTC_MspInit;
      break;

    case HAL_RTC_MSPDEINIT_CB_ID :
      hrtc->MspDeInitCallback = HAL_RTC_MspDeInit;
      break;

    default :
                        /* Return error status */
      status =  HAL_ERROR;
      break;
    }
  }
  else
  {
                /* Return error status */
    status =  HAL_ERROR;
  }

        /* Release Lock */
  __HAL_UNLOCK(hrtc);

  return status;
}
#endif /* USE_HAL_RTC_REGISTER_CALLBACKS */



__weak void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(hrtc);

}



__weak void HAL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(hrtc);

}

/* @} */



HAL_StatusTypeDef HAL_RTC_SetTime(RTC_HandleTypeDef *hrtc, RTC_TimeTypeDef *sTime, uint32_t Format)
{
  uint32_t counter_time = 0U, counter_alarm = 0U;

        /* Check input parameters */
  if ((hrtc == NULL) || (sTime == NULL))
  {
    return HAL_ERROR;
  }

        /* Check the parameters */
  assert_param(IS_RTC_FORMAT(Format));

        /* Process Locked */
  __HAL_LOCK(hrtc);

  hrtc->State = HAL_RTC_STATE_BUSY;

  if (Format == RTC_FORMAT_BIN)
  {
    assert_param(IS_RTC_HOUR24(sTime->Hours));
    assert_param(IS_RTC_MINUTES(sTime->Minutes));
    assert_param(IS_RTC_SECONDS(sTime->Seconds));

    counter_time = (uint32_t)(((uint32_t)sTime->Hours * 3600U) + \
                              ((uint32_t)sTime->Minutes * 60U) + \
                              ((uint32_t)sTime->Seconds));
  }
  else
  {
    assert_param(IS_RTC_HOUR24(RTC_Bcd2ToByte(sTime->Hours)));
    assert_param(IS_RTC_MINUTES(RTC_Bcd2ToByte(sTime->Minutes)));
    assert_param(IS_RTC_SECONDS(RTC_Bcd2ToByte(sTime->Seconds)));

    counter_time = (((uint32_t)(RTC_Bcd2ToByte(sTime->Hours)) * 3600U) + \
                    ((uint32_t)(RTC_Bcd2ToByte(sTime->Minutes)) * 60U) + \
                    ((uint32_t)(RTC_Bcd2ToByte(sTime->Seconds))));
  }

        /* Write time counter in RTC registers */
  if (RTC_WriteTimeCounter(hrtc, counter_time) != HAL_OK)
  {
                /* Set RTC state */
    hrtc->State = HAL_RTC_STATE_ERROR;

                /* Process Unlocked */
    __HAL_UNLOCK(hrtc);

    return HAL_ERROR;
  }
  else
  {
                /* Clear Second and overflow flags */
    CLEAR_BIT(hrtc->Instance->CRL, (RTC_FLAG_SEC | RTC_FLAG_OW));

                /* Read current Alarm counter in RTC registers */
    counter_alarm = RTC_ReadAlarmCounter(hrtc);

                /* Set again alarm to match with new time if enabled */
    if (counter_alarm != RTC_ALARM_RESETVALUE)
    {
      if (counter_alarm < counter_time)
      {
                                /* Add 1 day to alarm counter */
        counter_alarm += (uint32_t)(24U * 3600U);

                                /* Write new Alarm counter in RTC registers */
        if (RTC_WriteAlarmCounter(hrtc, counter_alarm) != HAL_OK)
        {
                                        /* Set RTC state */
          hrtc->State = HAL_RTC_STATE_ERROR;

                                        /* Process Unlocked */
          __HAL_UNLOCK(hrtc);

          return HAL_ERROR;
        }
      }
    }

    hrtc->State = HAL_RTC_STATE_READY;

    __HAL_UNLOCK(hrtc);

    return HAL_OK;
  }
}



HAL_StatusTypeDef HAL_RTC_GetTime(RTC_HandleTypeDef *hrtc, RTC_TimeTypeDef *sTime, uint32_t Format)
{
  uint32_t counter_time = 0U, counter_alarm = 0U, days_elapsed = 0U, hours = 0U;

        /* Check input parameters */
  if ((hrtc == NULL) || (sTime == NULL))
  {
    return HAL_ERROR;
  }

        /* Check the parameters */
  assert_param(IS_RTC_FORMAT(Format));

        /* Check if counter overflow occurred */
  if (__HAL_RTC_OVERFLOW_GET_FLAG(hrtc, RTC_FLAG_OW))
  {
    return HAL_ERROR;
  }

        /* Read the time counter */
  counter_time = RTC_ReadTimeCounter(hrtc);

        /* Fill the structure fields with the read parameters */
  hours = counter_time / 3600U;
  sTime->Minutes  = (uint8_t)((counter_time % 3600U) / 60U);
  sTime->Seconds  = (uint8_t)((counter_time % 3600U) % 60U);

  if (hours >= 24U)
  {
                /* Get number of days elapsed from last calculation */
    days_elapsed = (hours / 24U);

                /* Set Hours in RTC_TimeTypeDef structure */
    sTime->Hours = (hours % 24U);

                /* Read Alarm counter in RTC registers */
    counter_alarm = RTC_ReadAlarmCounter(hrtc);

                /* Calculate remaining time to reach alarm (only if set and not yet expired) */
    if ((counter_alarm != RTC_ALARM_RESETVALUE) && (counter_alarm > counter_time))
    {
      counter_alarm -= counter_time;
    }
    else
    {
                        /* In case of counter_alarm < counter_time */
                        /* Alarm expiration already occurred but alarm not deactivated */
      counter_alarm = RTC_ALARM_RESETVALUE;
    }

                /* Set updated time in decreasing counter by number of days elapsed */
    counter_time -= (days_elapsed * 24U * 3600U);

                /* Write time counter in RTC registers */
    if (RTC_WriteTimeCounter(hrtc, counter_time) != HAL_OK)
    {
      return HAL_ERROR;
    }

                /* Set updated alarm to be set */
    if (counter_alarm != RTC_ALARM_RESETVALUE)
    {
      counter_alarm += counter_time;

                        /* Write time counter in RTC registers */
      if (RTC_WriteAlarmCounter(hrtc, counter_alarm) != HAL_OK)
      {
        return HAL_ERROR;
      }
    }
    else
    {
                        /* Alarm already occurred. Set it to reset values to avoid unexpected expiration */
      if (RTC_WriteAlarmCounter(hrtc, counter_alarm) != HAL_OK)
      {
        return HAL_ERROR;
      }
    }

                /* Update date */
    RTC_DateUpdate(hrtc, days_elapsed);
  }
  else
  {
    sTime->Hours = hours;
  }

        /* Check the input parameters format */
  if (Format != RTC_FORMAT_BIN)
  {
                /* Convert the time structure parameters to BCD format */
    sTime->Hours    = (uint8_t)RTC_ByteToBcd2(sTime->Hours);
    sTime->Minutes  = (uint8_t)RTC_ByteToBcd2(sTime->Minutes);
    sTime->Seconds  = (uint8_t)RTC_ByteToBcd2(sTime->Seconds);
  }

  return HAL_OK;
}



HAL_StatusTypeDef HAL_RTC_SetDate(RTC_HandleTypeDef *hrtc, RTC_DateTypeDef *sDate, uint32_t Format)
{
  uint32_t counter_time = 0U, counter_alarm = 0U, hours = 0U;

        /* Check input parameters */
  if ((hrtc == NULL) || (sDate == NULL))
  {
    return HAL_ERROR;
  }

        /* Check the parameters */
  assert_param(IS_RTC_FORMAT(Format));

        /* Process Locked */
  __HAL_LOCK(hrtc);

  hrtc->State = HAL_RTC_STATE_BUSY;

  if (Format == RTC_FORMAT_BIN)
  {
    assert_param(IS_RTC_YEAR(sDate->Year));
    assert_param(IS_RTC_MONTH(sDate->Month));
    assert_param(IS_RTC_DATE(sDate->Date));

                /* Change the current date */
    hrtc->DateToUpdate.Year  = sDate->Year;
    hrtc->DateToUpdate.Month = sDate->Month;
    hrtc->DateToUpdate.Date  = sDate->Date;
  }
  else
  {
    assert_param(IS_RTC_YEAR(RTC_Bcd2ToByte(sDate->Year)));
    assert_param(IS_RTC_MONTH(RTC_Bcd2ToByte(sDate->Month)));
    assert_param(IS_RTC_DATE(RTC_Bcd2ToByte(sDate->Date)));

                /* Change the current date */
    hrtc->DateToUpdate.Year  = RTC_Bcd2ToByte(sDate->Year);
    hrtc->DateToUpdate.Month = RTC_Bcd2ToByte(sDate->Month);
    hrtc->DateToUpdate.Date  = RTC_Bcd2ToByte(sDate->Date);
  }

        /* WeekDay set by user can be ignored because automatically calculated */
  hrtc->DateToUpdate.WeekDay = RTC_WeekDayNum(hrtc->DateToUpdate.Year, hrtc->DateToUpdate.Month, hrtc->DateToUpdate.Date);
  sDate->WeekDay = hrtc->DateToUpdate.WeekDay;

        /* Reset time to be aligned on the same day */
        /* Read the time counter */
  counter_time = RTC_ReadTimeCounter(hrtc);

        /* Fill the structure fields with the read parameters */
  hours = counter_time / 3600U;
  if (hours > 24U)
  {
                /* Set updated time in decreasing counter by number of days elapsed */
    counter_time -= ((hours / 24U) * 24U * 3600U);
                /* Write time counter in RTC registers */
    if (RTC_WriteTimeCounter(hrtc, counter_time) != HAL_OK)
    {
                        /* Set RTC state */
      hrtc->State = HAL_RTC_STATE_ERROR;

                        /* Process Unlocked */
      __HAL_UNLOCK(hrtc);

      return HAL_ERROR;
    }

                /* Read current Alarm counter in RTC registers */
    counter_alarm = RTC_ReadAlarmCounter(hrtc);

                /* Set again alarm to match with new time if enabled */
    if (counter_alarm != RTC_ALARM_RESETVALUE)
    {
      if (counter_alarm < counter_time)
      {
                                /* Add 1 day to alarm counter */
        counter_alarm += (uint32_t)(24U * 3600U);

                                /* Write new Alarm counter in RTC registers */
        if (RTC_WriteAlarmCounter(hrtc, counter_alarm) != HAL_OK)
        {
                                        /* Set RTC state */
          hrtc->State = HAL_RTC_STATE_ERROR;

                                        /* Process Unlocked */
          __HAL_UNLOCK(hrtc);

          return HAL_ERROR;
        }
      }
    }



  }

  hrtc->State = HAL_RTC_STATE_READY ;

        /* Process Unlocked */
  __HAL_UNLOCK(hrtc);

  return HAL_OK;
}



HAL_StatusTypeDef HAL_RTC_GetDate(RTC_HandleTypeDef *hrtc, RTC_DateTypeDef *sDate, uint32_t Format)
{
  RTC_TimeTypeDef stime = {0U};

        /* Check input parameters */
  if ((hrtc == NULL) || (sDate == NULL))
  {
    return HAL_ERROR;
  }

        /* Check the parameters */
  assert_param(IS_RTC_FORMAT(Format));

        /* Call HAL_RTC_GetTime function to update date if counter higher than 24 hours */
  if (HAL_RTC_GetTime(hrtc, &stime, RTC_FORMAT_BIN) != HAL_OK)
  {
    return HAL_ERROR;
  }

        /* Fill the structure fields with the read parameters */
  sDate->WeekDay  = hrtc->DateToUpdate.WeekDay;
  sDate->Year     = hrtc->DateToUpdate.Year;
  sDate->Month    = hrtc->DateToUpdate.Month;
  sDate->Date     = hrtc->DateToUpdate.Date;

        /* Check the input parameters format */
  if (Format != RTC_FORMAT_BIN)
  {
                /* Convert the date structure parameters to BCD format */
    sDate->Year   = (uint8_t)RTC_ByteToBcd2(sDate->Year);
    sDate->Month  = (uint8_t)RTC_ByteToBcd2(sDate->Month);
    sDate->Date   = (uint8_t)RTC_ByteToBcd2(sDate->Date);
  }
  return HAL_OK;
}

/* @} */

/* @defgroup RTC_Exported_Functions_Group3 Alarm functions @brief   RTC Alarm functions @verbatim ##### RTC Alarm functions ##### */



HAL_StatusTypeDef HAL_RTC_SetAlarm(RTC_HandleTypeDef *hrtc, RTC_AlarmTypeDef *sAlarm, uint32_t Format)
{
  uint32_t counter_alarm = 0U, counter_time;
  RTC_TimeTypeDef stime = {0U};

        /* Check input parameters */
  if ((hrtc == NULL) || (sAlarm == NULL))
  {
    return HAL_ERROR;
  }

        /* Check the parameters */
  assert_param(IS_RTC_FORMAT(Format));

        /* Process Locked */
  __HAL_LOCK(hrtc);

  hrtc->State = HAL_RTC_STATE_BUSY;

        /* Call HAL_RTC_GetTime function to update date if counter higher than 24 hours */
  if (HAL_RTC_GetTime(hrtc, &stime, RTC_FORMAT_BIN) != HAL_OK)
  {
    return HAL_ERROR;
  }

        /* Convert time in seconds */
  counter_time = (uint32_t)(((uint32_t)stime.Hours * 3600U) + \
                            ((uint32_t)stime.Minutes * 60U) + \
                            ((uint32_t)stime.Seconds));

  if (Format == RTC_FORMAT_BIN)
  {
    assert_param(IS_RTC_HOUR24(sAlarm->AlarmTime.Hours));
    assert_param(IS_RTC_MINUTES(sAlarm->AlarmTime.Minutes));
    assert_param(IS_RTC_SECONDS(sAlarm->AlarmTime.Seconds));

    counter_alarm = (uint32_t)(((uint32_t)sAlarm->AlarmTime.Hours * 3600U) + \
                               ((uint32_t)sAlarm->AlarmTime.Minutes * 60U) + \
                               ((uint32_t)sAlarm->AlarmTime.Seconds));
  }
  else
  {
    assert_param(IS_RTC_HOUR24(RTC_Bcd2ToByte(sAlarm->AlarmTime.Hours)));
    assert_param(IS_RTC_MINUTES(RTC_Bcd2ToByte(sAlarm->AlarmTime.Minutes)));
    assert_param(IS_RTC_SECONDS(RTC_Bcd2ToByte(sAlarm->AlarmTime.Seconds)));

    counter_alarm = (((uint32_t)(RTC_Bcd2ToByte(sAlarm->AlarmTime.Hours)) * 3600U) + \
                     ((uint32_t)(RTC_Bcd2ToByte(sAlarm->AlarmTime.Minutes)) * 60U) + \
                     ((uint32_t)RTC_Bcd2ToByte(sAlarm->AlarmTime.Seconds)));
  }

        /* Check that requested alarm should expire in the same day (otherwise add 1 day) */
  if (counter_alarm < counter_time)
  {
                /* Add 1 day to alarm counter */
    counter_alarm += (uint32_t)(24U * 3600U);
  }

        /* Write Alarm counter in RTC registers */
  if (RTC_WriteAlarmCounter(hrtc, counter_alarm) != HAL_OK)
  {
                /* Set RTC state */
    hrtc->State = HAL_RTC_STATE_ERROR;

                /* Process Unlocked */
    __HAL_UNLOCK(hrtc);

    return HAL_ERROR;
  }
  else
  {
    hrtc->State = HAL_RTC_STATE_READY;

    __HAL_UNLOCK(hrtc);

    return HAL_OK;
  }
}



HAL_StatusTypeDef HAL_RTC_SetAlarm_IT(RTC_HandleTypeDef *hrtc, RTC_AlarmTypeDef *sAlarm, uint32_t Format)
{
  uint32_t counter_alarm = 0U, counter_time;
  RTC_TimeTypeDef stime = {0U};

        /* Check input parameters */
  if ((hrtc == NULL) || (sAlarm == NULL))
  {
    return HAL_ERROR;
  }

        /* Check the parameters */
  assert_param(IS_RTC_FORMAT(Format));

        /* Process Locked */
  __HAL_LOCK(hrtc);

  hrtc->State = HAL_RTC_STATE_BUSY;

        /* Call HAL_RTC_GetTime function to update date if counter higher than 24 hours */
  if (HAL_RTC_GetTime(hrtc, &stime, RTC_FORMAT_BIN) != HAL_OK)
  {
    return HAL_ERROR;
  }

        /* Convert time in seconds */
  counter_time = (uint32_t)(((uint32_t)stime.Hours * 3600U) + \
                            ((uint32_t)stime.Minutes * 60U) + \
                            ((uint32_t)stime.Seconds));

  if (Format == RTC_FORMAT_BIN)
  {
    assert_param(IS_RTC_HOUR24(sAlarm->AlarmTime.Hours));
    assert_param(IS_RTC_MINUTES(sAlarm->AlarmTime.Minutes));
    assert_param(IS_RTC_SECONDS(sAlarm->AlarmTime.Seconds));

    counter_alarm = (uint32_t)(((uint32_t)sAlarm->AlarmTime.Hours * 3600U) + \
                               ((uint32_t)sAlarm->AlarmTime.Minutes * 60U) + \
                               ((uint32_t)sAlarm->AlarmTime.Seconds));
  }
  else
  {
    assert_param(IS_RTC_HOUR24(RTC_Bcd2ToByte(sAlarm->AlarmTime.Hours)));
    assert_param(IS_RTC_MINUTES(RTC_Bcd2ToByte(sAlarm->AlarmTime.Minutes)));
    assert_param(IS_RTC_SECONDS(RTC_Bcd2ToByte(sAlarm->AlarmTime.Seconds)));

    counter_alarm = (((uint32_t)(RTC_Bcd2ToByte(sAlarm->AlarmTime.Hours)) * 3600U) + \
                     ((uint32_t)(RTC_Bcd2ToByte(sAlarm->AlarmTime.Minutes)) * 60U) + \
                     ((uint32_t)RTC_Bcd2ToByte(sAlarm->AlarmTime.Seconds)));
  }

        /* Check that requested alarm should expire in the same day (otherwise add 1 day) */
  if (counter_alarm < counter_time)
  {
                /* Add 1 day to alarm counter */
    counter_alarm += (uint32_t)(24U * 3600U);
  }
  
        /* Write alarm counter in RTC registers */
  if (RTC_WriteAlarmCounter(hrtc, counter_alarm) != HAL_OK)
  {
                /* Set RTC state */
    hrtc->State = HAL_RTC_STATE_ERROR;

                /* Process Unlocked */
    __HAL_UNLOCK(hrtc);

    return HAL_ERROR;
  }
  else
  {
                /* Clear flag alarm A */
    __HAL_RTC_ALARM_CLEAR_FLAG(hrtc, RTC_FLAG_ALRAF);

                /* Configure the Alarm interrupt */
    __HAL_RTC_ALARM_ENABLE_IT(hrtc, RTC_IT_ALRA);

                /* RTC Alarm Interrupt Configuration: EXTI configuration */
    __HAL_RTC_ALARM_EXTI_ENABLE_IT();

    __HAL_RTC_ALARM_EXTI_ENABLE_RISING_EDGE();

    hrtc->State = HAL_RTC_STATE_READY;

    __HAL_UNLOCK(hrtc);

    return HAL_OK;
  }
}



HAL_StatusTypeDef HAL_RTC_GetAlarm(RTC_HandleTypeDef *hrtc, RTC_AlarmTypeDef *sAlarm, uint32_t Format)
{
  uint32_t counter_alarm = 0U;

        /* Check input parameters */
  if ((hrtc == NULL) || (sAlarm == NULL))
  {
    return HAL_ERROR;
  }

        /* Check the parameters */
  assert_param(IS_RTC_FORMAT(Format));

        /* Read Alarm counter in RTC registers */
  counter_alarm = RTC_ReadAlarmCounter(hrtc);

        /* Fill the structure with the read parameters */
        /* Set hours in a day range (between 0 to 24) */
  sAlarm->AlarmTime.Hours   = (uint32_t)((counter_alarm / 3600U) % 24U);
  sAlarm->AlarmTime.Minutes = (uint32_t)((counter_alarm % 3600U) / 60U);
  sAlarm->AlarmTime.Seconds = (uint32_t)((counter_alarm % 3600U) % 60U);

  if (Format != RTC_FORMAT_BIN)
  {
    sAlarm->AlarmTime.Hours   = RTC_ByteToBcd2(sAlarm->AlarmTime.Hours);
    sAlarm->AlarmTime.Minutes = RTC_ByteToBcd2(sAlarm->AlarmTime.Minutes);
    sAlarm->AlarmTime.Seconds = RTC_ByteToBcd2(sAlarm->AlarmTime.Seconds);
  }

  return HAL_OK;
}



HAL_StatusTypeDef HAL_RTC_DeactivateAlarm(RTC_HandleTypeDef *hrtc)
{
        /* Check input parameters */
  if (hrtc == NULL)
  {
    return HAL_ERROR;
  }

        /* Process Locked */
  __HAL_LOCK(hrtc);

  hrtc->State = HAL_RTC_STATE_BUSY;

        /* In case of interrupt mode is used, the interrupt source must disabled */
  __HAL_RTC_ALARM_DISABLE_IT(hrtc, RTC_IT_ALRA);

        /* Set Initialization mode */
  if (RTC_EnterInitMode(hrtc) != HAL_OK)
  {
                /* Set RTC state */
    hrtc->State = HAL_RTC_STATE_ERROR;

                /* Process Unlocked */
    __HAL_UNLOCK(hrtc);

    return HAL_ERROR;
  }
  else
  {
                /* Clear flag alarm A */
    __HAL_RTC_ALARM_CLEAR_FLAG(hrtc, RTC_FLAG_ALRAF);

                /* Set to default values ALRH & ALRL registers */
    WRITE_REG(hrtc->Instance->ALRH, RTC_ALARM_RESETVALUE_REGISTER);
    WRITE_REG(hrtc->Instance->ALRL, RTC_ALARM_RESETVALUE_REGISTER);

                /* RTC Alarm Interrupt Configuration: Disable EXTI configuration */
    __HAL_RTC_ALARM_EXTI_DISABLE_IT();

                /* Wait for synchro */
    if (RTC_ExitInitMode(hrtc) != HAL_OK)
    {
      hrtc->State = HAL_RTC_STATE_ERROR;

                        /* Process Unlocked */
      __HAL_UNLOCK(hrtc);

      return HAL_ERROR;
    }
  }
  hrtc->State = HAL_RTC_STATE_READY;

        /* Process Unlocked */
  __HAL_UNLOCK(hrtc);

  return HAL_OK;
}



void HAL_RTC_AlarmIRQHandler(RTC_HandleTypeDef *hrtc)
{
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

        /* Clear the EXTI's line Flag for RTC Alarm */
  __HAL_RTC_ALARM_EXTI_CLEAR_FLAG();

        /* Change RTC state */
  hrtc->State = HAL_RTC_STATE_READY;
}



__weak void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(hrtc);

}



HAL_StatusTypeDef HAL_RTC_PollForAlarmAEvent(RTC_HandleTypeDef *hrtc, uint32_t Timeout)
{
  uint32_t tickstart = HAL_GetTick();

        /* Check input parameters */
  if (hrtc == NULL)
  {
    return HAL_ERROR;
  }

  while (__HAL_RTC_ALARM_GET_FLAG(hrtc, RTC_FLAG_ALRAF) == RESET)
  {
    if (Timeout != HAL_MAX_DELAY)
    {
      if ((Timeout == 0) || ((HAL_GetTick() - tickstart) > Timeout))
      {
        hrtc->State = HAL_RTC_STATE_TIMEOUT;
        return HAL_TIMEOUT;
      }
    }
  }

        /* Clear the Alarm interrupt pending bit */
  __HAL_RTC_ALARM_CLEAR_FLAG(hrtc, RTC_FLAG_ALRAF);

        /* Change RTC state */
  hrtc->State = HAL_RTC_STATE_READY;

  return HAL_OK;
}

/* @} */



HAL_RTCStateTypeDef HAL_RTC_GetState(RTC_HandleTypeDef *hrtc)
{
  return hrtc->State;
}

/* @} */



HAL_StatusTypeDef HAL_RTC_WaitForSynchro(RTC_HandleTypeDef *hrtc)
{
  uint32_t tickstart = 0U;

        /* Check input parameters */
  if (hrtc == NULL)
  {
    return HAL_ERROR;
  }

        /* Clear RSF flag */
  CLEAR_BIT(hrtc->Instance->CRL, RTC_FLAG_RSF);

  tickstart = HAL_GetTick();

        /* Wait the registers to be synchronised */
  while ((hrtc->Instance->CRL & RTC_FLAG_RSF) == (uint32_t)RESET)
  {
    if ((HAL_GetTick() - tickstart) >  RTC_TIMEOUT_VALUE)
    {
      return HAL_TIMEOUT;
    }
  }

  return HAL_OK;
}

/* @} */



/* @} */

/* @addtogroup RTC_Private_Functions @{ */



static uint32_t RTC_ReadTimeCounter(RTC_HandleTypeDef *hrtc)
{
  uint16_t high1 = 0U, high2 = 0U, low = 0U;
  uint32_t timecounter = 0U;

  high1 = READ_REG(hrtc->Instance->CNTH & RTC_CNTH_RTC_CNT);
  low   = READ_REG(hrtc->Instance->CNTL & RTC_CNTL_RTC_CNT);
  high2 = READ_REG(hrtc->Instance->CNTH & RTC_CNTH_RTC_CNT);

  if (high1 != high2)
  {

    timecounter = (((uint32_t) high2 << 16U) | READ_REG(hrtc->Instance->CNTL & RTC_CNTL_RTC_CNT));
  }
  else
  {

    timecounter = (((uint32_t) high1 << 16U) | low);
  }

  return timecounter;
}



static HAL_StatusTypeDef RTC_WriteTimeCounter(RTC_HandleTypeDef *hrtc, uint32_t TimeCounter)
{
  HAL_StatusTypeDef status = HAL_OK;

        /* Set Initialization mode */
  if (RTC_EnterInitMode(hrtc) != HAL_OK)
  {
    status = HAL_ERROR;
  }
  else
  {
                /* Set RTC COUNTER MSB word */
    WRITE_REG(hrtc->Instance->CNTH, (TimeCounter >> 16U));
                /* Set RTC COUNTER LSB word */
    WRITE_REG(hrtc->Instance->CNTL, (TimeCounter & RTC_CNTL_RTC_CNT));

                /* Wait for synchro */
    if (RTC_ExitInitMode(hrtc) != HAL_OK)
    {
      status = HAL_ERROR;
    }
  }

  return status;
}



static uint32_t RTC_ReadAlarmCounter(RTC_HandleTypeDef *hrtc)
{
  (void)hrtc;
  return gCounter_Alarm;
}



static HAL_StatusTypeDef RTC_WriteAlarmCounter(RTC_HandleTypeDef *hrtc, uint32_t AlarmCounter)
{
  HAL_StatusTypeDef status = HAL_OK;

        /* Set Initialization mode */
  if (RTC_EnterInitMode(hrtc) != HAL_OK)
  {
    status = HAL_ERROR;
  }
  else
  {
                /* Set RTC COUNTER MSB word */
    WRITE_REG(hrtc->Instance->ALRH, (AlarmCounter >> 16U));
                /* Set RTC COUNTER LSB word */
    WRITE_REG(hrtc->Instance->ALRL, (AlarmCounter & RTC_ALRL_RTC_ALR));
                /* Update the global variable gCounter_Alarm */
    gCounter_Alarm = AlarmCounter;
                /* Wait for synchro */
    if (RTC_ExitInitMode(hrtc) != HAL_OK)
    {
      status = HAL_ERROR;
    }
  }

  return status;
}



static HAL_StatusTypeDef RTC_EnterInitMode(RTC_HandleTypeDef *hrtc)
{
  uint32_t tickstart = 0U;

  tickstart = HAL_GetTick();
        /* Wait till RTC is in INIT state and if Time out is reached exit */
  while ((hrtc->Instance->CRL & RTC_CRL_RTOFF) == (uint32_t)RESET)
  {
    if ((HAL_GetTick() - tickstart) >  RTC_TIMEOUT_VALUE)
    {
      return HAL_TIMEOUT;
    }
  }

        /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);



  return HAL_OK;
}



static HAL_StatusTypeDef RTC_ExitInitMode(RTC_HandleTypeDef *hrtc)
{
  uint32_t tickstart = 0U;

        /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);
  
  tickstart = HAL_GetTick();
        /* Wait till RTC is in NOINIT state and if Time out is reached exit */
  while ((hrtc->Instance->CRL & RTC_CRL_RTOFF) == RTC_CRL_RTOFF)
  {
    if ((HAL_GetTick() - tickstart) >  RTC_RTOFF_RESET_TIMEOUT_VALUE)
    {
      break;
    }
  }

  tickstart = HAL_GetTick();
        /* Wait till RTC is in INIT state and if Time out is reached exit */
  while ((hrtc->Instance->CRL & RTC_CRL_RTOFF) == (uint32_t)RESET)
  {
    if ((HAL_GetTick() - tickstart) >  RTC_TIMEOUT_VALUE)
    {
      return HAL_TIMEOUT;
    }
  }

  return HAL_OK;
}

/* @brief  Converts a 2 digit decimal to BCD format. @param  Value: Byte to be converted @retval Converted byte */
static uint8_t RTC_ByteToBcd2(uint8_t Value)
{
  uint32_t bcdhigh = 0U;

  while (Value >= 10U)
  {
    bcdhigh++;
    Value -= 10U;
  }

  return ((uint8_t)(bcdhigh << 4U) | Value);
}

/* @brief  Converts from 2 digit BCD to Binary. @param  Value: BCD value to be converted @retval Converted word */
static uint8_t RTC_Bcd2ToByte(uint8_t Value)
{
  uint32_t tmp = 0U;
  tmp = ((uint8_t)(Value & (uint8_t)0xF0) >> (uint8_t)0x4) * 10U;
  return (tmp + (Value & (uint8_t)0x0F));
}



static void RTC_DateUpdate(RTC_HandleTypeDef *hrtc, uint32_t DayElapsed)
{
  uint32_t year = 0U, month = 0U, day = 0U;
  uint32_t loop = 0U;

        /* Get the current year */
  year = hrtc->DateToUpdate.Year;

        /* Get the current month and day */
  month = hrtc->DateToUpdate.Month;
  day = hrtc->DateToUpdate.Date;

  for (loop = 0U; loop < DayElapsed; loop++)
  {
    if ((month == 1U) || (month == 3U) || (month == 5U) || (month == 7U) || \
        (month == 8U) || (month == 10U) || (month == 12U))
    {
      if (day < 31U)
      {
        day++;
      }
                        /* Date structure member: day = 31 */
      else
      {
        if (month != 12U)
        {
          month++;
          day = 1U;
        }
                                /* Date structure member: day = 31 & month =12 */
        else
        {
          month = 1U;
          day = 1U;
          year++;
        }
      }
    }
    else if ((month == 4U) || (month == 6U) || (month == 9U) || (month == 11U))
    {
      if (day < 30U)
      {
        day++;
      }
                        /* Date structure member: day = 30 */
      else
      {
        month++;
        day = 1U;
      }
    }
    else if (month == 2U)
    {
      if (day < 28U)
      {
        day++;
      }
      else if (day == 28U)
      {
                                /* Leap year */
        if (RTC_IsLeapYear(year))
        {
          day++;
        }
        else
        {
          month++;
          day = 1U;
        }
      }
      else if (day == 29U)
      {
        month++;
        day = 1U;
      }
    }
  }

        /* Update year */
  hrtc->DateToUpdate.Year = year;

        /* Update day and month */
  hrtc->DateToUpdate.Month = month;
  hrtc->DateToUpdate.Date = day;

        /* Update day of the week */
  hrtc->DateToUpdate.WeekDay = RTC_WeekDayNum(year, month, day);
}

/* @brief  Check whether the passed year is Leap or not. @param  nYear  year to check @retval 1: leap year 0: not leap year */
static uint8_t RTC_IsLeapYear(uint16_t nYear)
{
  if ((nYear % 4U) != 0U)
  {
    return 0U;
  }

  if ((nYear % 100U) != 0U)
  {
    return 1U;
  }

  if ((nYear % 400U) == 0U)
  {
    return 1U;
  }
  else
  {
    return 0U;
  }
}



static uint8_t RTC_WeekDayNum(uint32_t nYear, uint8_t nMonth, uint8_t nDay)
{
  uint32_t year = 0U, weekday = 0U;

  year = 2000U + nYear;

  if (nMonth < 3U)
  {

    weekday = (((23U * nMonth) / 9U) + nDay + 4U + year + ((year - 1U) / 4U) - ((year - 1U) / 100U) + ((year - 1U) / 400U)) % 7U;
  }
  else
  {
                /* D = { [(23 x month)/9] + day + 4 + year + [year/4] - [year/100] + [year/400] - 2 } mod 7 */
    weekday = (((23U * nMonth) / 9U) + nDay + 4U + year + (year / 4U) - (year / 100U) + (year / 400U) - 2U) % 7U;
  }

  return (uint8_t)weekday;
}

/* @} */

#endif /* HAL_RTC_MODULE_ENABLED */
/* @} */

/* @} */

/* (C) COPYRIGHT Puya *****END OF FILE */
