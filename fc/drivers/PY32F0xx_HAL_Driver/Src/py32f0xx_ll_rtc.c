/**
 * @file    py32f0xx_ll_rtc.c
 * @author  MCU Application Team
 * @brief   RTC LL module driver.
 * @attention
 */

#if defined(USE_FULL_LL_DRIVER)

/* Includes ------------------------------------------------------------------ */
#include "py32f0xx_ll_rtc.h"
#include "py32f0xx_ll_cortex.h"
#ifdef  USE_FULL_ASSERT
#include "py32_assert.h"
#else
#define assert_param(expr) ((void)0U)
#endif

/* @addtogroup PY32F0xx_LL_Driver @{ */

#if defined(RTC)

/* @addtogroup RTC_LL @{ */

/* Private types ------------------------------------------------------------- */
/* Private variables --------------------------------------------------------- */
/* Private constants --------------------------------------------------------- */
/* @defgroup RTC_LL_Private_Constants RTC Private Constants @{ */
/* Default values used for prescaler */
#define RTC_ASYNCH_PRESC_DEFAULT     0x00007FFFU

/* Values used for timeout */
#define RTC_INITMODE_TIMEOUT         2000U /* 2s when tick set to 1ms */
#define RTC_SYNCHRO_TIMEOUT          2000U /* 2s when tick set to 1ms */
#define RTC_WAIT_RTOFF_ZERO          4U    /* 1ms when tick set to 1ms */
/* @} */

/* Private macros ------------------------------------------------------------ */
/* @addtogroup RTC_LL_Private_Macros @{ */

#define IS_LL_RTC_ASYNCH_PREDIV(__VALUE__)   ((__VALUE__) <= 0xFFFFFU)

#define IS_LL_RTC_FORMAT(__VALUE__) (((__VALUE__) == LL_RTC_FORMAT_BIN) \
                                  || ((__VALUE__) == LL_RTC_FORMAT_BCD))

#define IS_LL_RTC_HOUR24(__HOUR__)            ((__HOUR__) <= 23U)
#define IS_LL_RTC_MINUTES(__MINUTES__)        ((__MINUTES__) <= 59U)
#define IS_LL_RTC_SECONDS(__SECONDS__)        ((__SECONDS__) <= 59U)
#define IS_LL_RTC_CALIB_OUTPUT(__OUTPUT__) (((__OUTPUT__) == LL_RTC_CALIB_OUTPUT_NONE) || \
                                            ((__OUTPUT__) == LL_RTC_CALIB_OUTPUT_RTCCLOCK) || \
                                            ((__OUTPUT__) == LL_RTC_CALIB_OUTPUT_ALARM) || \
                                            ((__OUTPUT__) == LL_RTC_CALIB_OUTPUT_SECOND))
/* @} */
/* Private function prototypes ----------------------------------------------- */
/* Exported functions -------------------------------------------------------- */
/* @addtogroup RTC_LL_Exported_Functions @{ */

/* @addtogroup RTC_LL_EF_Init @{ */



ErrorStatus LL_RTC_DeInit(RTC_TypeDef *RTCx)
{
  ErrorStatus status = ERROR;

        /* Check the parameter */
  assert_param(IS_RTC_ALL_INSTANCE(RTCx));

        /* Disable the write protection for RTC registers */
  LL_RTC_DisableWriteProtection(RTCx);

        /* Set Initialization mode */
  if (LL_RTC_EnterInitMode(RTCx) != ERROR)
  {
    LL_RTC_WriteReg(RTCx, CNTL, 0x0000);
    LL_RTC_WriteReg(RTCx, CNTH, 0x0000);
    LL_RTC_WriteReg(RTCx, PRLH, 0x0000);
    LL_RTC_WriteReg(RTCx, PRLL, 0x8000);
    LL_RTC_WriteReg(RTCx, CRH,  0x0000);
    LL_RTC_WriteReg(RTCx, CRL,  0x0020);

                /* Reset Tamper and alternate functions configuration register */
    LL_RTC_WriteReg(RTCx, BKP_RTCCR,  0x0000);

                /* Exit Initialization Mode */
    if (LL_RTC_ExitInitMode(RTCx) != ERROR)
    {
                        /* Wait till the RTC RSF flag is set */
      status = LL_RTC_WaitForSynchro(RTCx);

                        /* Clear RSF Flag */
      LL_RTC_ClearFlag_RS(RTCx);

                        /* Enable the write protection for RTC registers */
      LL_RTC_EnableWriteProtection(RTCx);
    }
  }
  else
  {
                /* Enable the write protection for RTC registers */
    LL_RTC_EnableWriteProtection(RTCx);
  }

  return status;
}



ErrorStatus LL_RTC_Init(RTC_TypeDef *RTCx, LL_RTC_InitTypeDef *RTC_InitStruct)
{
  ErrorStatus status = ERROR;

        /* Check the parameters */
  assert_param(IS_RTC_ALL_INSTANCE(RTCx));
  assert_param(IS_LL_RTC_ASYNCH_PREDIV(RTC_InitStruct->AsynchPrescaler));
  assert_param(IS_LL_RTC_CALIB_OUTPUT(RTC_InitStruct->OutPutSource));
        /* Waiting for synchro */
  if (LL_RTC_WaitForSynchro(RTCx) != ERROR)
  {
                /* Set Initialization mode */
    if (LL_RTC_EnterInitMode(RTCx) != ERROR)
    {
                        /* Clear Flag Bits */
      LL_RTC_ClearFlag_ALR(RTCx);
      LL_RTC_ClearFlag_OW(RTCx);
      LL_RTC_ClearFlag_SEC(RTCx);

                        /* Set the signal which will be routed to RTC Tamper Pin */
      LL_RTC_SetOutputSource(RTCx, RTC_InitStruct->OutPutSource);

                        /* Configure Synchronous and Asynchronous prescaler factor */
      LL_RTC_SetAsynchPrescaler(RTCx, RTC_InitStruct->AsynchPrescaler);

                        /* Exit Initialization Mode */
      LL_RTC_ExitInitMode(RTCx);

      status = SUCCESS;
    }
  }
  return status;
}



void LL_RTC_StructInit(LL_RTC_InitTypeDef *RTC_InitStruct)
{
        /* Set RTC_InitStruct fields to default values */
  RTC_InitStruct->AsynchPrescaler = RTC_ASYNCH_PRESC_DEFAULT;
  RTC_InitStruct->OutPutSource    = LL_RTC_CALIB_OUTPUT_NONE;
}



ErrorStatus LL_RTC_TIME_Init(RTC_TypeDef *RTCx, uint32_t RTC_Format, LL_RTC_TimeTypeDef *RTC_TimeStruct)
{
  ErrorStatus status = ERROR;
  uint32_t counter_time = 0U;

        /* Check the parameters */
  assert_param(IS_RTC_ALL_INSTANCE(RTCx));
  assert_param(IS_LL_RTC_FORMAT(RTC_Format));

  if (RTC_Format == LL_RTC_FORMAT_BIN)
  {
    assert_param(IS_LL_RTC_HOUR24(RTC_TimeStruct->Hours));
    assert_param(IS_LL_RTC_MINUTES(RTC_TimeStruct->Minutes));
    assert_param(IS_LL_RTC_SECONDS(RTC_TimeStruct->Seconds));
  }
  else
  {
    assert_param(IS_LL_RTC_HOUR24(__LL_RTC_CONVERT_BCD2BIN(RTC_TimeStruct->Hours)));
    assert_param(IS_LL_RTC_MINUTES(__LL_RTC_CONVERT_BCD2BIN(RTC_TimeStruct->Minutes)));
    assert_param(IS_LL_RTC_SECONDS(__LL_RTC_CONVERT_BCD2BIN(RTC_TimeStruct->Seconds)));
  }

        /* Enter Initialization mode */
  if (LL_RTC_EnterInitMode(RTCx) != ERROR)
  {
                /* Check the input parameters format */
    if (RTC_Format == LL_RTC_FORMAT_BIN)
    {
      counter_time = (uint32_t)(((uint32_t)RTC_TimeStruct->Hours * 3600U) + \
                                ((uint32_t)RTC_TimeStruct->Minutes * 60U) + \
                                ((uint32_t)RTC_TimeStruct->Seconds));
      LL_RTC_TIME_Set(RTCx, counter_time);
    }
    else
    {
      counter_time = (((uint32_t)(__LL_RTC_CONVERT_BCD2BIN(RTC_TimeStruct->Hours)) * 3600U) + \
                      ((uint32_t)(__LL_RTC_CONVERT_BCD2BIN(RTC_TimeStruct->Minutes)) * 60U) + \
                      ((uint32_t)(__LL_RTC_CONVERT_BCD2BIN(RTC_TimeStruct->Seconds))));
      LL_RTC_TIME_Set(RTCx, counter_time);
    }
    status = SUCCESS;
  }
        /* Exit Initialization mode */
  LL_RTC_ExitInitMode(RTCx);

  return status;
}



void LL_RTC_TIME_StructInit(LL_RTC_TimeTypeDef *RTC_TimeStruct)
{
        /* Time = 00h:00min:00sec */
  RTC_TimeStruct->Hours      = 0U;
  RTC_TimeStruct->Minutes    = 0U;
  RTC_TimeStruct->Seconds    = 0U;
}



ErrorStatus LL_RTC_ALARM_Init(RTC_TypeDef *RTCx, uint32_t RTC_Format, LL_RTC_AlarmTypeDef *RTC_AlarmStruct)
{
  ErrorStatus status = ERROR;
  uint32_t counter_alarm = 0U;
        /* Check the parameters */
  assert_param(IS_RTC_ALL_INSTANCE(RTCx));
  assert_param(IS_LL_RTC_FORMAT(RTC_Format));

  if (RTC_Format == LL_RTC_FORMAT_BIN)
  {
    assert_param(IS_LL_RTC_HOUR24(RTC_AlarmStruct->AlarmTime.Hours));
    assert_param(IS_LL_RTC_MINUTES(RTC_AlarmStruct->AlarmTime.Minutes));
    assert_param(IS_LL_RTC_SECONDS(RTC_AlarmStruct->AlarmTime.Seconds));
  }
  else
  {
    assert_param(IS_LL_RTC_HOUR24(__LL_RTC_CONVERT_BCD2BIN(RTC_AlarmStruct->AlarmTime.Hours)));
    assert_param(IS_LL_RTC_MINUTES(__LL_RTC_CONVERT_BCD2BIN(RTC_AlarmStruct->AlarmTime.Minutes)));
    assert_param(IS_LL_RTC_SECONDS(__LL_RTC_CONVERT_BCD2BIN(RTC_AlarmStruct->AlarmTime.Seconds)));
  }

        /* Enter Initialization mode */
  if (LL_RTC_EnterInitMode(RTCx) != ERROR)
  {
                /* Check the input parameters format */
    if (RTC_Format == LL_RTC_FORMAT_BIN)
    {
      counter_alarm = (uint32_t)(((uint32_t)RTC_AlarmStruct->AlarmTime.Hours * 3600U) + \
                                 ((uint32_t)RTC_AlarmStruct->AlarmTime.Minutes * 60U) + \
                                 ((uint32_t)RTC_AlarmStruct->AlarmTime.Seconds));
      LL_RTC_ALARM_Set(RTCx, counter_alarm);
    }
    else
    {
      counter_alarm = (((uint32_t)(__LL_RTC_CONVERT_BCD2BIN(RTC_AlarmStruct->AlarmTime.Hours)) * 3600U) + \
                       ((uint32_t)(__LL_RTC_CONVERT_BCD2BIN(RTC_AlarmStruct->AlarmTime.Minutes)) * 60U) + \
                       ((uint32_t)(__LL_RTC_CONVERT_BCD2BIN(RTC_AlarmStruct->AlarmTime.Seconds))));
      LL_RTC_ALARM_Set(RTCx, counter_alarm);
    }
    status = SUCCESS;
  }
        /* Exit Initialization mode */
  LL_RTC_ExitInitMode(RTCx);

  return status;
}



void LL_RTC_ALARM_StructInit(LL_RTC_AlarmTypeDef *RTC_AlarmStruct)
{
        /* Alarm Time Settings : Time = 00h:00mn:00sec */
  RTC_AlarmStruct->AlarmTime.Hours      = 0U;
  RTC_AlarmStruct->AlarmTime.Minutes    = 0U;
  RTC_AlarmStruct->AlarmTime.Seconds    = 0U;
}



ErrorStatus LL_RTC_EnterInitMode(RTC_TypeDef *RTCx)
{
  __IO uint32_t timeout = RTC_INITMODE_TIMEOUT;
  ErrorStatus status = SUCCESS;
  uint32_t tmp = 0U;

        /* Check the parameter */
  assert_param(IS_RTC_ALL_INSTANCE(RTCx));

        /* Wait till RTC is in INIT state and if Time out is reached exit */
  tmp = LL_RTC_IsActiveFlag_RTOF(RTCx);
  while ((timeout != 0U) && (tmp == 0U))
  {
    if (LL_SYSTICK_IsActiveCounterFlag() == 1U)
    {
      timeout --;
    }
    tmp = LL_RTC_IsActiveFlag_RTOF(RTCx);
    if (timeout == 0U)
    {
      status = ERROR;
    }
  }

        /* Disable the write protection for RTC registers */
  LL_RTC_DisableWriteProtection(RTCx);

  return status;
}



ErrorStatus LL_RTC_ExitInitMode(RTC_TypeDef *RTCx)
{
  __IO uint32_t timeout = RTC_INITMODE_TIMEOUT;
  __IO uint32_t timeout_waitRtoffZer0 = RTC_WAIT_RTOFF_ZERO;
  ErrorStatus status = SUCCESS;
  uint32_t tmp = 0U;

        /* Check the parameter */
  assert_param(IS_RTC_ALL_INSTANCE(RTCx));

        /* Disable initialization mode */
  LL_RTC_EnableWriteProtection(RTCx);

        /* Wait till RTC is in INIT state and if Time out is reached exit */
  tmp = LL_RTC_IsActiveFlag_RTOF(RTCx);
  while ((timeout_waitRtoffZer0 != 0U) && (tmp !=0U))
  {
    if (LL_SYSTICK_IsActiveCounterFlag() == 1U)
    {
      timeout_waitRtoffZer0 --;
    }
    tmp = LL_RTC_IsActiveFlag_RTOF(RTCx);
  }
  while ((timeout != 0U) && (tmp ==0U))
  {
    if (LL_SYSTICK_IsActiveCounterFlag() == 1U)
    {
      timeout --;
    }
    tmp = LL_RTC_IsActiveFlag_RTOF(RTCx);
    if (timeout == 0U)
    {
      status = ERROR;
    }
  }
  return status;
}



ErrorStatus LL_RTC_TIME_SetCounter(RTC_TypeDef *RTCx, uint32_t TimeCounter)
{
  ErrorStatus status = ERROR;
        /* Check the parameter */
  assert_param(IS_RTC_ALL_INSTANCE(RTCx));

        /* Enter Initialization mode */
  if (LL_RTC_EnterInitMode(RTCx) != ERROR)
  {
    LL_RTC_TIME_Set(RTCx, TimeCounter);
    status = SUCCESS;
  }
        /* Exit Initialization mode */
  LL_RTC_ExitInitMode(RTCx);

  return status;
}



ErrorStatus LL_RTC_ALARM_SetCounter(RTC_TypeDef *RTCx, uint32_t AlarmCounter)
{
  ErrorStatus status = ERROR;
        /* Check the parameter */
  assert_param(IS_RTC_ALL_INSTANCE(RTCx));

        /* Enter Initialization mode */
  if (LL_RTC_EnterInitMode(RTCx) != ERROR)
  {
    LL_RTC_ALARM_Set(RTCx, AlarmCounter);
    status = SUCCESS;
  }
        /* Exit Initialization mode */
  LL_RTC_ExitInitMode(RTCx);

  return status;
}



ErrorStatus LL_RTC_WaitForSynchro(RTC_TypeDef *RTCx)
{
  __IO uint32_t timeout = RTC_SYNCHRO_TIMEOUT;
  ErrorStatus status = SUCCESS;
  uint32_t tmp = 0U;

        /* Check the parameter */
  assert_param(IS_RTC_ALL_INSTANCE(RTCx));

        /* Clear RSF flag */
  LL_RTC_ClearFlag_RS(RTCx);

        /* Wait the registers to be synchronised */
  tmp = LL_RTC_IsActiveFlag_RS(RTCx);
  while ((timeout != 0U) && (tmp == 0U))
  {
    if (LL_SYSTICK_IsActiveCounterFlag() == 1U)
    {
      timeout--;
    }
    tmp = LL_RTC_IsActiveFlag_RS(RTCx);
    if (timeout == 0U)
    {
      status = ERROR;
    }
  }

  return (status);
}

/* @} */

/* @} */

/* @} */

#endif /* defined(RTC) */

/* @} */

#endif /* USE_FULL_LL_DRIVER */

/* (C) COPYRIGHT Puya *****END OF FILE */
