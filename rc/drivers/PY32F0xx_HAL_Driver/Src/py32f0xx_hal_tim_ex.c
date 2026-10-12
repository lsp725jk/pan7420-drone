/**
 * @file    py32f0xx_hal_tim_ex.c
 * @author  MCU Application Team
 * @brief   TIM HAL module driver.
 */

/* Includes ------------------------------------------------------------------ */
#include "py32f0xx_hal.h"

/**
 * @addtogroup PY32F0xx_HAL_Driver @{
 */

/* @defgroup TIMEx TIMEx @brief TIM Extended HAL module driver @{ */

#ifdef HAL_TIM_MODULE_ENABLED

/* Private typedef ----------------------------------------------------------- */
/* Private define ------------------------------------------------------------ */
/* Private macro ------------------------------------------------------------- */
/* Private variables --------------------------------------------------------- */
/* Private function prototypes ----------------------------------------------- */
static void TIM_CCxNChannelCmd(TIM_TypeDef *TIMx, uint32_t Channel, uint32_t ChannelNState);

/* Exported functions -------------------------------------------------------- */
/* @defgroup TIMEx_Exported_Functions TIM Extended Exported Functions @{ */



HAL_StatusTypeDef HAL_TIMEx_HallSensor_Init(TIM_HandleTypeDef *htim, TIM_HallSensor_InitTypeDef *sConfig)
{
  TIM_OC_InitTypeDef OC_Config;

        /* Check the TIM handle allocation */
  if (htim == NULL)
  {
    return HAL_ERROR;
  }

        /* Check the parameters */
  assert_param(IS_TIM_HALL_SENSOR_INTERFACE_INSTANCE(htim->Instance));
  assert_param(IS_TIM_COUNTER_MODE(htim->Init.CounterMode));
  assert_param(IS_TIM_CLOCKDIVISION_DIV(htim->Init.ClockDivision));
  assert_param(IS_TIM_AUTORELOAD_PRELOAD(htim->Init.AutoReloadPreload));
  assert_param(IS_TIM_IC_POLARITY(sConfig->IC1Polarity));
  assert_param(IS_TIM_IC_PRESCALER(sConfig->IC1Prescaler));
  assert_param(IS_TIM_IC_FILTER(sConfig->IC1Filter));

  if (htim->State == HAL_TIM_STATE_RESET)
  {
                /* Allocate lock resource and initialize it */
    htim->Lock = HAL_UNLOCKED;

#if (USE_HAL_TIM_REGISTER_CALLBACKS == 1)
                /* Reset interrupt callbacks to legacy week callbacks */
    TIM_ResetCallback(htim);

    if (htim->HallSensor_MspInitCallback == NULL)
    {
      htim->HallSensor_MspInitCallback = HAL_TIMEx_HallSensor_MspInit;
    }
                /* Init the low level hardware : GPIO, CLOCK, NVIC */
    htim->HallSensor_MspInitCallback(htim);
#else
                /* Init the low level hardware : GPIO, CLOCK, NVIC and DMA */
    HAL_TIMEx_HallSensor_MspInit(htim);
#endif /* USE_HAL_TIM_REGISTER_CALLBACKS */
  }

        /* Set the TIM state */
  htim->State = HAL_TIM_STATE_BUSY;

        /* Configure the Time base in the Encoder Mode */
  TIM_Base_SetConfig(htim->Instance, &htim->Init);



  TIM_TI1_SetConfig(htim->Instance, sConfig->IC1Polarity, TIM_ICSELECTION_TRC, sConfig->IC1Filter);

        /* Reset the IC1PSC Bits */
  htim->Instance->CCMR1 &= ~TIM_CCMR1_IC1PSC;
        /* Set the IC1PSC value */
  htim->Instance->CCMR1 |= sConfig->IC1Prescaler;

        /* Enable the Hall sensor interface (XOR function of the three inputs) */
  htim->Instance->CR2 |= TIM_CR2_TI1S;

        /* Select the TIM_TS_TI1F_ED signal as Input trigger for the TIM */
  htim->Instance->SMCR &= ~TIM_SMCR_TS;
  htim->Instance->SMCR |= TIM_TS_TI1F_ED;

        /* Use the TIM_TS_TI1F_ED signal to reset the TIM counter each edge detection */
  htim->Instance->SMCR &= ~TIM_SMCR_SMS;
  htim->Instance->SMCR |= TIM_SLAVEMODE_RESET;

        /* Program channel 2 in PWM 2 mode with the desired Commutation_Delay */
  OC_Config.OCFastMode = TIM_OCFAST_DISABLE;
  OC_Config.OCIdleState = TIM_OCIDLESTATE_RESET;
  OC_Config.OCMode = TIM_OCMODE_PWM2;
  OC_Config.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  OC_Config.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  OC_Config.OCPolarity = TIM_OCPOLARITY_HIGH;
  OC_Config.Pulse = sConfig->Commutation_Delay;

  TIM_OC2_SetConfig(htim->Instance, &OC_Config);



  htim->Instance->CR2 &= ~TIM_CR2_MMS;
  htim->Instance->CR2 |= TIM_TRGO_OC2REF;

        /* Initialize the TIM state */
  htim->State = HAL_TIM_STATE_READY;

  return HAL_OK;
}

/* @brief  DeInitializes the TIM Hall Sensor interface @param  htim TIM Hall Sensor Interface handle @retval HAL status */
HAL_StatusTypeDef HAL_TIMEx_HallSensor_DeInit(TIM_HandleTypeDef *htim)
{
        /* Check the parameters */
  assert_param(IS_TIM_INSTANCE(htim->Instance));

  htim->State = HAL_TIM_STATE_BUSY;

        /* Disable the TIM Peripheral Clock */
  __HAL_TIM_DISABLE(htim);

#if (USE_HAL_TIM_REGISTER_CALLBACKS == 1)
  if (htim->HallSensor_MspDeInitCallback == NULL)
  {
    htim->HallSensor_MspDeInitCallback = HAL_TIMEx_HallSensor_MspDeInit;
  }
        /* DeInit the low level hardware */
  htim->HallSensor_MspDeInitCallback(htim);
#else
        /* DeInit the low level hardware: GPIO, CLOCK, NVIC */
  HAL_TIMEx_HallSensor_MspDeInit(htim);
#endif /* USE_HAL_TIM_REGISTER_CALLBACKS */

        /* Change TIM state */
  htim->State = HAL_TIM_STATE_RESET;

        /* Release Lock */
  __HAL_UNLOCK(htim);

  return HAL_OK;
}

/* @brief  Initializes the TIM Hall Sensor MSP. @param  htim TIM Hall Sensor Interface handle @retval None */
__weak void HAL_TIMEx_HallSensor_MspInit(TIM_HandleTypeDef *htim)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(htim);



}

/* @brief  DeInitializes TIM Hall Sensor MSP. @param  htim TIM Hall Sensor Interface handle @retval None */
__weak void HAL_TIMEx_HallSensor_MspDeInit(TIM_HandleTypeDef *htim)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(htim);



}

/* @brief  Starts the TIM Hall Sensor Interface. @param  htim TIM Hall Sensor Interface handle @retval HAL status */
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Start(TIM_HandleTypeDef *htim)
{
  uint32_t tmpsmcr;

        /* Check the parameters */
  assert_param(IS_TIM_HALL_SENSOR_INTERFACE_INSTANCE(htim->Instance));



  TIM_CCxChannelCmd(htim->Instance, TIM_CHANNEL_1, TIM_CCx_ENABLE);



  tmpsmcr = htim->Instance->SMCR & TIM_SMCR_SMS;
  if (!IS_TIM_SLAVEMODE_TRIGGER_ENABLED(tmpsmcr))
  {
    __HAL_TIM_ENABLE(htim);
  }

        /* Return function status */
  return HAL_OK;
}

/* @brief  Stops the TIM Hall sensor Interface. @param  htim TIM Hall Sensor Interface handle @retval HAL status */
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Stop(TIM_HandleTypeDef *htim)
{
        /* Check the parameters */
  assert_param(IS_TIM_HALL_SENSOR_INTERFACE_INSTANCE(htim->Instance));



  TIM_CCxChannelCmd(htim->Instance, TIM_CHANNEL_1, TIM_CCx_DISABLE);

        /* Disable the Peripheral */
  __HAL_TIM_DISABLE(htim);

        /* Return function status */
  return HAL_OK;
}

/* @brief  Starts the TIM Hall Sensor Interface in interrupt mode. @param  htim TIM Hall Sensor Interface handle @retval HAL status */
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Start_IT(TIM_HandleTypeDef *htim)
{
  uint32_t tmpsmcr;

        /* Check the parameters */
  assert_param(IS_TIM_HALL_SENSOR_INTERFACE_INSTANCE(htim->Instance));

        /* Enable the capture compare Interrupts 1 event */
  __HAL_TIM_ENABLE_IT(htim, TIM_IT_CC1);



  TIM_CCxChannelCmd(htim->Instance, TIM_CHANNEL_1, TIM_CCx_ENABLE);



  tmpsmcr = htim->Instance->SMCR & TIM_SMCR_SMS;
  if (!IS_TIM_SLAVEMODE_TRIGGER_ENABLED(tmpsmcr))
  {
    __HAL_TIM_ENABLE(htim);
  }

        /* Return function status */
  return HAL_OK;
}

/* @brief  Stops the TIM Hall Sensor Interface in interrupt mode. @param  htim TIM Hall Sensor Interface handle @retval HAL status */
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Stop_IT(TIM_HandleTypeDef *htim)
{
        /* Check the parameters */
  assert_param(IS_TIM_HALL_SENSOR_INTERFACE_INSTANCE(htim->Instance));



  TIM_CCxChannelCmd(htim->Instance, TIM_CHANNEL_1, TIM_CCx_DISABLE);

        /* Disable the capture compare Interrupts event */
  __HAL_TIM_DISABLE_IT(htim, TIM_IT_CC1);

        /* Disable the Peripheral */
  __HAL_TIM_DISABLE(htim);

        /* Return function status */
  return HAL_OK;
}

#if (defined(DMA) || defined(DMA1))

HAL_StatusTypeDef HAL_TIMEx_HallSensor_Start_DMA(TIM_HandleTypeDef *htim, uint32_t *pData, uint16_t Length)
{
  uint32_t tmpsmcr;

        /* Check the parameters */
  assert_param(IS_TIM_HALL_SENSOR_INTERFACE_INSTANCE(htim->Instance));

  if (htim->State == HAL_TIM_STATE_BUSY)
  {
    return HAL_BUSY;
  }
  else if (htim->State == HAL_TIM_STATE_READY)
  {
    if (((uint32_t)pData == 0U) && (Length > 0U))
    {
      return HAL_ERROR;
    }
    else
    {
      htim->State = HAL_TIM_STATE_BUSY;
    }
  }
  else
  {
                /* nothing to do */
  }

  TIM_CCxChannelCmd(htim->Instance, TIM_CHANNEL_1, TIM_CCx_ENABLE);

        /* Set the DMA Input Capture 1 Callbacks */
  htim->hdma[TIM_DMA_ID_CC1]->XferCpltCallback = TIM_DMACaptureCplt;
  htim->hdma[TIM_DMA_ID_CC1]->XferHalfCpltCallback = TIM_DMACaptureHalfCplt;
        /* Set the DMA error callback */
  htim->hdma[TIM_DMA_ID_CC1]->XferErrorCallback = TIM_DMAError ;

        /* Enable the DMA channel for Capture 1 */
  if (HAL_DMA_Start_IT(htim->hdma[TIM_DMA_ID_CC1], (uint32_t)&htim->Instance->CCR1, (uint32_t)pData, Length) != HAL_OK)
  {
    return HAL_ERROR;
  }
        /* Enable the capture compare 1 Interrupt */
  __HAL_TIM_ENABLE_DMA(htim, TIM_DMA_CC1);



  tmpsmcr = htim->Instance->SMCR & TIM_SMCR_SMS;
  if (!IS_TIM_SLAVEMODE_TRIGGER_ENABLED(tmpsmcr))
  {
    __HAL_TIM_ENABLE(htim);
  }

        /* Return function status */
  return HAL_OK;
}

/* @brief  Stops the TIM Hall Sensor Interface in DMA mode. @param  htim TIM Hall Sensor Interface handle @retval HAL status */
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Stop_DMA(TIM_HandleTypeDef *htim)
{
        /* Check the parameters */
  assert_param(IS_TIM_HALL_SENSOR_INTERFACE_INSTANCE(htim->Instance));



  TIM_CCxChannelCmd(htim->Instance, TIM_CHANNEL_1, TIM_CCx_DISABLE);



        /* Disable the capture compare Interrupts 1 event */
  __HAL_TIM_DISABLE_DMA(htim, TIM_DMA_CC1);

  (void)HAL_DMA_Abort_IT(htim->hdma[TIM_DMA_ID_CC1]);
        /* Disable the Peripheral */
  __HAL_TIM_DISABLE(htim);

        /* Return function status */
  return HAL_OK;
}

#endif
/* @} */



HAL_StatusTypeDef HAL_TIMEx_OCN_Start(TIM_HandleTypeDef *htim, uint32_t Channel)
{
  uint32_t tmpsmcr;

        /* Check the parameters */
  assert_param(IS_TIM_CCXN_INSTANCE(htim->Instance, Channel));

        /* Enable the Capture compare channel N */
  TIM_CCxNChannelCmd(htim->Instance, Channel, TIM_CCxN_ENABLE);

        /* Enable the Main Output */
  __HAL_TIM_MOE_ENABLE(htim);



  tmpsmcr = htim->Instance->SMCR & TIM_SMCR_SMS;
  if (!IS_TIM_SLAVEMODE_TRIGGER_ENABLED(tmpsmcr))
  {
    __HAL_TIM_ENABLE(htim);
  }

        /* Return function status */
  return HAL_OK;
}



HAL_StatusTypeDef HAL_TIMEx_OCN_Stop(TIM_HandleTypeDef *htim, uint32_t Channel)
{
        /* Check the parameters */
  assert_param(IS_TIM_CCXN_INSTANCE(htim->Instance, Channel));

        /* Disable the Capture compare channel N */
  TIM_CCxNChannelCmd(htim->Instance, Channel, TIM_CCxN_DISABLE);

        /* Disable the Main Output */
  __HAL_TIM_MOE_DISABLE(htim);

        /* Disable the Peripheral */
  __HAL_TIM_DISABLE(htim);

        /* Return function status */
  return HAL_OK;
}



HAL_StatusTypeDef HAL_TIMEx_OCN_Start_IT(TIM_HandleTypeDef *htim, uint32_t Channel)
{
  uint32_t tmpsmcr;

        /* Check the parameters */
  assert_param(IS_TIM_CCXN_INSTANCE(htim->Instance, Channel));

  switch (Channel)
  {
  case TIM_CHANNEL_1:
  {
                /* Enable the TIM Output Compare interrupt */
    __HAL_TIM_ENABLE_IT(htim, TIM_IT_CC1);
    break;
  }

  case TIM_CHANNEL_2:
  {
                /* Enable the TIM Output Compare interrupt */
    __HAL_TIM_ENABLE_IT(htim, TIM_IT_CC2);
    break;
  }

  case TIM_CHANNEL_3:
  {
                /* Enable the TIM Output Compare interrupt */
    __HAL_TIM_ENABLE_IT(htim, TIM_IT_CC3);
    break;
  }



  default:
    break;
  }

        /* Enable the TIM Break interrupt */
  __HAL_TIM_ENABLE_IT(htim, TIM_IT_BREAK);

        /* Enable the Capture compare channel N */
  TIM_CCxNChannelCmd(htim->Instance, Channel, TIM_CCxN_ENABLE);

        /* Enable the Main Output */
  __HAL_TIM_MOE_ENABLE(htim);



  tmpsmcr = htim->Instance->SMCR & TIM_SMCR_SMS;
  if (!IS_TIM_SLAVEMODE_TRIGGER_ENABLED(tmpsmcr))
  {
    __HAL_TIM_ENABLE(htim);
  }

        /* Return function status */
  return HAL_OK;
}



HAL_StatusTypeDef HAL_TIMEx_OCN_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel)
{
  uint32_t tmpccer;
        /* Check the parameters */
  assert_param(IS_TIM_CCXN_INSTANCE(htim->Instance, Channel));

  switch (Channel)
  {
  case TIM_CHANNEL_1:
  {
                /* Disable the TIM Output Compare interrupt */
    __HAL_TIM_DISABLE_IT(htim, TIM_IT_CC1);
    break;
  }

  case TIM_CHANNEL_2:
  {
                /* Disable the TIM Output Compare interrupt */
    __HAL_TIM_DISABLE_IT(htim, TIM_IT_CC2);
    break;
  }

  case TIM_CHANNEL_3:
  {
                /* Disable the TIM Output Compare interrupt */
    __HAL_TIM_DISABLE_IT(htim, TIM_IT_CC3);
    break;
  }

  default:
    break;
  }

        /* Disable the Capture compare channel N */
  TIM_CCxNChannelCmd(htim->Instance, Channel, TIM_CCxN_DISABLE);

        /* Disable the TIM Break interrupt (only if no more channel is active) */
  tmpccer = htim->Instance->CCER;
  if ((tmpccer & (TIM_CCER_CC1NE | TIM_CCER_CC2NE | TIM_CCER_CC3NE)) == (uint32_t)RESET)
  {
    __HAL_TIM_DISABLE_IT(htim, TIM_IT_BREAK);
  }

        /* Disable the Main Output */
  __HAL_TIM_MOE_DISABLE(htim);

        /* Disable the Peripheral */
  __HAL_TIM_DISABLE(htim);

        /* Return function status */
  return HAL_OK;
}

#if (defined(DMA) || defined(DMA1))

HAL_StatusTypeDef HAL_TIMEx_OCN_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t *pData, uint16_t Length)
{
  uint32_t tmpsmcr;

        /* Check the parameters */
  assert_param(IS_TIM_CCXN_INSTANCE(htim->Instance, Channel));

  if (htim->State == HAL_TIM_STATE_BUSY)
  {
    return HAL_BUSY;
  }
  else if (htim->State == HAL_TIM_STATE_READY)
  {
    if (((uint32_t)pData == 0U) && (Length > 0U))
    {
      return HAL_ERROR;
    }
    else
    {
      htim->State = HAL_TIM_STATE_BUSY;
    }
  }
  else
  {
                /* nothing to do */
  }

  switch (Channel)
  {
  case TIM_CHANNEL_1:
  {
                /* Set the DMA compare callbacks */
    htim->hdma[TIM_DMA_ID_CC1]->XferCpltCallback = TIM_DMADelayPulseCplt;
    htim->hdma[TIM_DMA_ID_CC1]->XferHalfCpltCallback = TIM_DMADelayPulseHalfCplt;

                /* Set the DMA error callback */
    htim->hdma[TIM_DMA_ID_CC1]->XferErrorCallback = TIM_DMAError ;

                /* Enable the DMA channel */
    if (HAL_DMA_Start_IT(htim->hdma[TIM_DMA_ID_CC1], (uint32_t)pData, (uint32_t)&htim->Instance->CCR1, Length) != HAL_OK)
    {
      return HAL_ERROR;
    }
                /* Enable the TIM Output Compare DMA request */
    __HAL_TIM_ENABLE_DMA(htim, TIM_DMA_CC1);
    break;
  }

  case TIM_CHANNEL_2:
  {
                /* Set the DMA compare callbacks */
    htim->hdma[TIM_DMA_ID_CC2]->XferCpltCallback = TIM_DMADelayPulseCplt;
    htim->hdma[TIM_DMA_ID_CC2]->XferHalfCpltCallback = TIM_DMADelayPulseHalfCplt;

                /* Set the DMA error callback */
    htim->hdma[TIM_DMA_ID_CC2]->XferErrorCallback = TIM_DMAError ;

                /* Enable the DMA channel */
    if (HAL_DMA_Start_IT(htim->hdma[TIM_DMA_ID_CC2], (uint32_t)pData, (uint32_t)&htim->Instance->CCR2, Length) != HAL_OK)
    {
      return HAL_ERROR;
    }
                /* Enable the TIM Output Compare DMA request */
    __HAL_TIM_ENABLE_DMA(htim, TIM_DMA_CC2);
    break;
  }

  case TIM_CHANNEL_3:
  {
                /* Set the DMA compare callbacks */
    htim->hdma[TIM_DMA_ID_CC3]->XferCpltCallback = TIM_DMADelayPulseCplt;
    htim->hdma[TIM_DMA_ID_CC3]->XferHalfCpltCallback = TIM_DMADelayPulseHalfCplt;

                /* Set the DMA error callback */
    htim->hdma[TIM_DMA_ID_CC3]->XferErrorCallback = TIM_DMAError ;

                /* Enable the DMA channel */
    if (HAL_DMA_Start_IT(htim->hdma[TIM_DMA_ID_CC3], (uint32_t)pData, (uint32_t)&htim->Instance->CCR3, Length) != HAL_OK)
    {
      return HAL_ERROR;
    }
                /* Enable the TIM Output Compare DMA request */
    __HAL_TIM_ENABLE_DMA(htim, TIM_DMA_CC3);
    break;
  }

  default:
    break;
  }

        /* Enable the Capture compare channel N */
  TIM_CCxNChannelCmd(htim->Instance, Channel, TIM_CCxN_ENABLE);

        /* Enable the Main Output */
  __HAL_TIM_MOE_ENABLE(htim);



  tmpsmcr = htim->Instance->SMCR & TIM_SMCR_SMS;
  if (!IS_TIM_SLAVEMODE_TRIGGER_ENABLED(tmpsmcr))
  {
    __HAL_TIM_ENABLE(htim);
  }

        /* Return function status */
  return HAL_OK;
}



HAL_StatusTypeDef HAL_TIMEx_OCN_Stop_DMA(TIM_HandleTypeDef *htim, uint32_t Channel)
{
        /* Check the parameters */
  assert_param(IS_TIM_CCXN_INSTANCE(htim->Instance, Channel));

  switch (Channel)
  {
  case TIM_CHANNEL_1:
  {
                /* Disable the TIM Output Compare DMA request */
    __HAL_TIM_DISABLE_DMA(htim, TIM_DMA_CC1);
    (void)HAL_DMA_Abort_IT(htim->hdma[TIM_DMA_ID_CC1]);
    break;
  }

  case TIM_CHANNEL_2:
  {
                /* Disable the TIM Output Compare DMA request */
    __HAL_TIM_DISABLE_DMA(htim, TIM_DMA_CC2);
    (void)HAL_DMA_Abort_IT(htim->hdma[TIM_DMA_ID_CC2]);
    break;
  }

  case TIM_CHANNEL_3:
  {
                /* Disable the TIM Output Compare DMA request */
    __HAL_TIM_DISABLE_DMA(htim, TIM_DMA_CC3);
    (void)HAL_DMA_Abort_IT(htim->hdma[TIM_DMA_ID_CC3]);
    break;
  }

  default:
    break;
  }

        /* Disable the Capture compare channel N */
  TIM_CCxNChannelCmd(htim->Instance, Channel, TIM_CCxN_DISABLE);

        /* Disable the Main Output */
  __HAL_TIM_MOE_DISABLE(htim);

        /* Disable the Peripheral */
  __HAL_TIM_DISABLE(htim);

        /* Change the htim state */
  htim->State = HAL_TIM_STATE_READY;

        /* Return function status */
  return HAL_OK;
}
#endif

/* @} */



HAL_StatusTypeDef HAL_TIMEx_PWMN_Start(TIM_HandleTypeDef *htim, uint32_t Channel)
{
  uint32_t tmpsmcr;

        /* Check the parameters */
  assert_param(IS_TIM_CCXN_INSTANCE(htim->Instance, Channel));

        /* Enable the complementary PWM output */
  TIM_CCxNChannelCmd(htim->Instance, Channel, TIM_CCxN_ENABLE);

        /* Enable the Main Output */
  __HAL_TIM_MOE_ENABLE(htim);



  tmpsmcr = htim->Instance->SMCR & TIM_SMCR_SMS;
  if (!IS_TIM_SLAVEMODE_TRIGGER_ENABLED(tmpsmcr))
  {
    __HAL_TIM_ENABLE(htim);
  }

        /* Return function status */
  return HAL_OK;
}



HAL_StatusTypeDef HAL_TIMEx_PWMN_Stop(TIM_HandleTypeDef *htim, uint32_t Channel)
{
        /* Check the parameters */
  assert_param(IS_TIM_CCXN_INSTANCE(htim->Instance, Channel));

        /* Disable the complementary PWM output */
  TIM_CCxNChannelCmd(htim->Instance, Channel, TIM_CCxN_DISABLE);

        /* Disable the Main Output */
  __HAL_TIM_MOE_DISABLE(htim);

        /* Disable the Peripheral */
  __HAL_TIM_DISABLE(htim);

        /* Return function status */
  return HAL_OK;
}



HAL_StatusTypeDef HAL_TIMEx_PWMN_Start_IT(TIM_HandleTypeDef *htim, uint32_t Channel)
{
  uint32_t tmpsmcr;

        /* Check the parameters */
  assert_param(IS_TIM_CCXN_INSTANCE(htim->Instance, Channel));

  switch (Channel)
  {
  case TIM_CHANNEL_1:
  {
                /* Enable the TIM Capture/Compare 1 interrupt */
    __HAL_TIM_ENABLE_IT(htim, TIM_IT_CC1);
    break;
  }

  case TIM_CHANNEL_2:
  {
                /* Enable the TIM Capture/Compare 2 interrupt */
    __HAL_TIM_ENABLE_IT(htim, TIM_IT_CC2);
    break;
  }

  case TIM_CHANNEL_3:
  {
                /* Enable the TIM Capture/Compare 3 interrupt */
    __HAL_TIM_ENABLE_IT(htim, TIM_IT_CC3);
    break;
  }

  default:
    break;
  }

        /* Enable the TIM Break interrupt */
  __HAL_TIM_ENABLE_IT(htim, TIM_IT_BREAK);

        /* Enable the complementary PWM output */
  TIM_CCxNChannelCmd(htim->Instance, Channel, TIM_CCxN_ENABLE);

        /* Enable the Main Output */
  __HAL_TIM_MOE_ENABLE(htim);



  tmpsmcr = htim->Instance->SMCR & TIM_SMCR_SMS;
  if (!IS_TIM_SLAVEMODE_TRIGGER_ENABLED(tmpsmcr))
  {
    __HAL_TIM_ENABLE(htim);
  }

        /* Return function status */
  return HAL_OK;
}



HAL_StatusTypeDef HAL_TIMEx_PWMN_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel)
{
  uint32_t tmpccer;

        /* Check the parameters */
  assert_param(IS_TIM_CCXN_INSTANCE(htim->Instance, Channel));

  switch (Channel)
  {
  case TIM_CHANNEL_1:
  {
                /* Disable the TIM Capture/Compare 1 interrupt */
    __HAL_TIM_DISABLE_IT(htim, TIM_IT_CC1);
    break;
  }

  case TIM_CHANNEL_2:
  {
                /* Disable the TIM Capture/Compare 2 interrupt */
    __HAL_TIM_DISABLE_IT(htim, TIM_IT_CC2);
    break;
  }

  case TIM_CHANNEL_3:
  {
                /* Disable the TIM Capture/Compare 3 interrupt */
    __HAL_TIM_DISABLE_IT(htim, TIM_IT_CC3);
    break;
  }

  default:
    break;
  }

        /* Disable the complementary PWM output */
  TIM_CCxNChannelCmd(htim->Instance, Channel, TIM_CCxN_DISABLE);

        /* Disable the TIM Break interrupt (only if no more channel is active) */
  tmpccer = htim->Instance->CCER;
  if ((tmpccer & (TIM_CCER_CC1NE | TIM_CCER_CC2NE | TIM_CCER_CC3NE)) == (uint32_t)RESET)
  {
    __HAL_TIM_DISABLE_IT(htim, TIM_IT_BREAK);
  }

        /* Disable the Main Output */
  __HAL_TIM_MOE_DISABLE(htim);

        /* Disable the Peripheral */
  __HAL_TIM_DISABLE(htim);

        /* Return function status */
  return HAL_OK;
}

#if (defined(DMA) || defined(DMA1))

HAL_StatusTypeDef HAL_TIMEx_PWMN_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t *pData, uint16_t Length)
{
  uint32_t tmpsmcr;

        /* Check the parameters */
  assert_param(IS_TIM_CCXN_INSTANCE(htim->Instance, Channel));

  if (htim->State == HAL_TIM_STATE_BUSY)
  {
    return HAL_BUSY;
  }
  else if (htim->State == HAL_TIM_STATE_READY)
  {
    if (((uint32_t)pData == 0U) && (Length > 0U))
    {
      return HAL_ERROR;
    }
    else
    {
      htim->State = HAL_TIM_STATE_BUSY;
    }
  }
  else
  {
                /* nothing to do */
  }
  switch (Channel)
  {
  case TIM_CHANNEL_1:
  {
                /* Set the DMA compare callbacks */
    htim->hdma[TIM_DMA_ID_CC1]->XferCpltCallback = TIM_DMADelayPulseCplt;
    htim->hdma[TIM_DMA_ID_CC1]->XferHalfCpltCallback = TIM_DMADelayPulseHalfCplt;

                /* Set the DMA error callback */
    htim->hdma[TIM_DMA_ID_CC1]->XferErrorCallback = TIM_DMAError ;

                /* Enable the DMA channel */
    if (HAL_DMA_Start_IT(htim->hdma[TIM_DMA_ID_CC1], (uint32_t)pData, (uint32_t)&htim->Instance->CCR1, Length) != HAL_OK)
    {
      return HAL_ERROR;
    }
                /* Enable the TIM Capture/Compare 1 DMA request */
    __HAL_TIM_ENABLE_DMA(htim, TIM_DMA_CC1);
    break;
  }

  case TIM_CHANNEL_2:
  {
                /* Set the DMA compare callbacks */
    htim->hdma[TIM_DMA_ID_CC2]->XferCpltCallback = TIM_DMADelayPulseCplt;
    htim->hdma[TIM_DMA_ID_CC2]->XferHalfCpltCallback = TIM_DMADelayPulseHalfCplt;

                /* Set the DMA error callback */
    htim->hdma[TIM_DMA_ID_CC2]->XferErrorCallback = TIM_DMAError ;

                /* Enable the DMA channel */
    if (HAL_DMA_Start_IT(htim->hdma[TIM_DMA_ID_CC2], (uint32_t)pData, (uint32_t)&htim->Instance->CCR2, Length) != HAL_OK)
    {
      return HAL_ERROR;
    }
                /* Enable the TIM Capture/Compare 2 DMA request */
    __HAL_TIM_ENABLE_DMA(htim, TIM_DMA_CC2);
    break;
  }

  case TIM_CHANNEL_3:
  {
                /* Set the DMA compare callbacks */
    htim->hdma[TIM_DMA_ID_CC3]->XferCpltCallback = TIM_DMADelayPulseCplt;
    htim->hdma[TIM_DMA_ID_CC3]->XferHalfCpltCallback = TIM_DMADelayPulseHalfCplt;

                /* Set the DMA error callback */
    htim->hdma[TIM_DMA_ID_CC3]->XferErrorCallback = TIM_DMAError ;

                /* Enable the DMA channel */
    if (HAL_DMA_Start_IT(htim->hdma[TIM_DMA_ID_CC3], (uint32_t)pData, (uint32_t)&htim->Instance->CCR3, Length) != HAL_OK)
    {
      return HAL_ERROR;
    }
                /* Enable the TIM Capture/Compare 3 DMA request */
    __HAL_TIM_ENABLE_DMA(htim, TIM_DMA_CC3);
    break;
  }

  default:
    break;
  }

        /* Enable the complementary PWM output */
  TIM_CCxNChannelCmd(htim->Instance, Channel, TIM_CCxN_ENABLE);

        /* Enable the Main Output */
  __HAL_TIM_MOE_ENABLE(htim);



  tmpsmcr = htim->Instance->SMCR & TIM_SMCR_SMS;
  if (!IS_TIM_SLAVEMODE_TRIGGER_ENABLED(tmpsmcr))
  {
    __HAL_TIM_ENABLE(htim);
  }

        /* Return function status */
  return HAL_OK;
}



HAL_StatusTypeDef HAL_TIMEx_PWMN_Stop_DMA(TIM_HandleTypeDef *htim, uint32_t Channel)
{
        /* Check the parameters */
  assert_param(IS_TIM_CCXN_INSTANCE(htim->Instance, Channel));

  switch (Channel)
  {
  case TIM_CHANNEL_1:
  {
                /* Disable the TIM Capture/Compare 1 DMA request */
    __HAL_TIM_DISABLE_DMA(htim, TIM_DMA_CC1);
    (void)HAL_DMA_Abort_IT(htim->hdma[TIM_DMA_ID_CC1]);
    break;
  }

  case TIM_CHANNEL_2:
  {
                /* Disable the TIM Capture/Compare 2 DMA request */
    __HAL_TIM_DISABLE_DMA(htim, TIM_DMA_CC2);
    (void)HAL_DMA_Abort_IT(htim->hdma[TIM_DMA_ID_CC2]);
    break;
  }

  case TIM_CHANNEL_3:
  {
                /* Disable the TIM Capture/Compare 3 DMA request */
    __HAL_TIM_DISABLE_DMA(htim, TIM_DMA_CC3);
    (void)HAL_DMA_Abort_IT(htim->hdma[TIM_DMA_ID_CC3]);
    break;
  }

  default:
    break;
  }

        /* Disable the complementary PWM output */
  TIM_CCxNChannelCmd(htim->Instance, Channel, TIM_CCxN_DISABLE);

        /* Disable the Main Output */
  __HAL_TIM_MOE_DISABLE(htim);

        /* Disable the Peripheral */
  __HAL_TIM_DISABLE(htim);

        /* Change the htim state */
  htim->State = HAL_TIM_STATE_READY;

        /* Return function status */
  return HAL_OK;
}
#endif
/* @} */



HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Start(TIM_HandleTypeDef *htim, uint32_t OutputChannel)
{
        /* Check the parameters */
  assert_param(IS_TIM_CCXN_INSTANCE(htim->Instance, OutputChannel));

        /* Enable the complementary One Pulse output */
  TIM_CCxNChannelCmd(htim->Instance, OutputChannel, TIM_CCxN_ENABLE);

        /* Enable the Main Output */
  __HAL_TIM_MOE_ENABLE(htim);

        /* Return function status */
  return HAL_OK;
}



HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Stop(TIM_HandleTypeDef *htim, uint32_t OutputChannel)
{

        /* Check the parameters */
  assert_param(IS_TIM_CCXN_INSTANCE(htim->Instance, OutputChannel));

        /* Disable the complementary One Pulse output */
  TIM_CCxNChannelCmd(htim->Instance, OutputChannel, TIM_CCxN_DISABLE);

        /* Disable the Main Output */
  __HAL_TIM_MOE_DISABLE(htim);

        /* Disable the Peripheral */
  __HAL_TIM_DISABLE(htim);

        /* Return function status */
  return HAL_OK;
}



HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Start_IT(TIM_HandleTypeDef *htim, uint32_t OutputChannel)
{
        /* Check the parameters */
  assert_param(IS_TIM_CCXN_INSTANCE(htim->Instance, OutputChannel));

        /* Enable the TIM Capture/Compare 1 interrupt */
  __HAL_TIM_ENABLE_IT(htim, TIM_IT_CC1);

        /* Enable the TIM Capture/Compare 2 interrupt */
  __HAL_TIM_ENABLE_IT(htim, TIM_IT_CC2);

        /* Enable the complementary One Pulse output */
  TIM_CCxNChannelCmd(htim->Instance, OutputChannel, TIM_CCxN_ENABLE);

        /* Enable the Main Output */
  __HAL_TIM_MOE_ENABLE(htim);

        /* Return function status */
  return HAL_OK;
}



HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Stop_IT(TIM_HandleTypeDef *htim, uint32_t OutputChannel)
{
        /* Check the parameters */
  assert_param(IS_TIM_CCXN_INSTANCE(htim->Instance, OutputChannel));

        /* Disable the TIM Capture/Compare 1 interrupt */
  __HAL_TIM_DISABLE_IT(htim, TIM_IT_CC1);

        /* Disable the TIM Capture/Compare 2 interrupt */
  __HAL_TIM_DISABLE_IT(htim, TIM_IT_CC2);

        /* Disable the complementary One Pulse output */
  TIM_CCxNChannelCmd(htim->Instance, OutputChannel, TIM_CCxN_DISABLE);

        /* Disable the Main Output */
  __HAL_TIM_MOE_DISABLE(htim);

        /* Disable the Peripheral */
  __HAL_TIM_DISABLE(htim);

        /* Return function status */
  return HAL_OK;
}

/* @} */



HAL_StatusTypeDef HAL_TIMEx_ConfigCommutEvent(TIM_HandleTypeDef *htim, uint32_t  InputTrigger,
    uint32_t  CommutationSource)
{
        /* Check the parameters */
  assert_param(IS_TIM_COMMUTATION_EVENT_INSTANCE(htim->Instance));
  assert_param(IS_TIM_INTERNAL_TRIGGEREVENT_SELECTION(InputTrigger));

  __HAL_LOCK(htim);

  if ((InputTrigger == TIM_TS_ITR0) || (InputTrigger == TIM_TS_ITR1) ||
      (InputTrigger == TIM_TS_ITR2) || (InputTrigger == TIM_TS_ITR3))
  {
                /* Select the Input trigger */
    htim->Instance->SMCR &= ~TIM_SMCR_TS;
    htim->Instance->SMCR |= InputTrigger;
  }

        /* Select the Capture Compare preload feature */
  htim->Instance->CR2 |= TIM_CR2_CCPC;
        /* Select the Commutation event source */
  htim->Instance->CR2 &= ~TIM_CR2_CCUS;
  htim->Instance->CR2 |= CommutationSource;

        /* Disable Commutation Interrupt */
  __HAL_TIM_DISABLE_IT(htim, TIM_IT_COM);
  
#if (defined(DMA) || defined(DNA1))
        /* Disable Commutation DMA request */
  __HAL_TIM_DISABLE_DMA(htim, TIM_DMA_COM);
#endif
  
  __HAL_UNLOCK(htim);

  return HAL_OK;
}



HAL_StatusTypeDef HAL_TIMEx_ConfigCommutEvent_IT(TIM_HandleTypeDef *htim, uint32_t  InputTrigger,
    uint32_t  CommutationSource)
{
        /* Check the parameters */
  assert_param(IS_TIM_COMMUTATION_EVENT_INSTANCE(htim->Instance));
  assert_param(IS_TIM_INTERNAL_TRIGGEREVENT_SELECTION(InputTrigger));

  __HAL_LOCK(htim);

  if ((InputTrigger == TIM_TS_ITR0) || (InputTrigger == TIM_TS_ITR1) ||
      (InputTrigger == TIM_TS_ITR2) || (InputTrigger == TIM_TS_ITR3))
  {
                /* Select the Input trigger */
    htim->Instance->SMCR &= ~TIM_SMCR_TS;
    htim->Instance->SMCR |= InputTrigger;
  }

        /* Select the Capture Compare preload feature */
  htim->Instance->CR2 |= TIM_CR2_CCPC;
        /* Select the Commutation event source */
  htim->Instance->CR2 &= ~TIM_CR2_CCUS;
  htim->Instance->CR2 |= CommutationSource;
  
#if (defined(DMA) || defined(DMA1))
        /* Disable Commutation DMA request */
  __HAL_TIM_DISABLE_DMA(htim, TIM_DMA_COM);
#endif
  
        /* Enable the Commutation Interrupt */
  __HAL_TIM_ENABLE_IT(htim, TIM_IT_COM);

  __HAL_UNLOCK(htim);

  return HAL_OK;
}

#if (defined(DMA) || defined(DMA1))

HAL_StatusTypeDef HAL_TIMEx_ConfigCommutEvent_DMA(TIM_HandleTypeDef *htim, uint32_t  InputTrigger,
    uint32_t  CommutationSource)
{
        /* Check the parameters */
  assert_param(IS_TIM_COMMUTATION_EVENT_INSTANCE(htim->Instance));
  assert_param(IS_TIM_INTERNAL_TRIGGEREVENT_SELECTION(InputTrigger));

  __HAL_LOCK(htim);

  if ((InputTrigger == TIM_TS_ITR0) || (InputTrigger == TIM_TS_ITR1) ||
      (InputTrigger == TIM_TS_ITR2) || (InputTrigger == TIM_TS_ITR3))
  {
                /* Select the Input trigger */
    htim->Instance->SMCR &= ~TIM_SMCR_TS;
    htim->Instance->SMCR |= InputTrigger;
  }

        /* Select the Capture Compare preload feature */
  htim->Instance->CR2 |= TIM_CR2_CCPC;
        /* Select the Commutation event source */
  htim->Instance->CR2 &= ~TIM_CR2_CCUS;
  htim->Instance->CR2 |= CommutationSource;

        /* Enable the Commutation DMA Request */
        /* Set the DMA Commutation Callback */
  htim->hdma[TIM_DMA_ID_COMMUTATION]->XferCpltCallback = TIMEx_DMACommutationCplt;
  htim->hdma[TIM_DMA_ID_COMMUTATION]->XferHalfCpltCallback = TIMEx_DMACommutationHalfCplt;
        /* Set the DMA error callback */
  htim->hdma[TIM_DMA_ID_COMMUTATION]->XferErrorCallback = TIM_DMAError;

        /* Disable Commutation Interrupt */
  __HAL_TIM_DISABLE_IT(htim, TIM_IT_COM);

        /* Enable the Commutation DMA Request */
  __HAL_TIM_ENABLE_DMA(htim, TIM_DMA_COM);

  __HAL_UNLOCK(htim);

  return HAL_OK;
}

#endif

HAL_StatusTypeDef HAL_TIMEx_MasterConfigSynchronization(TIM_HandleTypeDef *htim,
    TIM_MasterConfigTypeDef *sMasterConfig)
{
  uint32_t tmpcr2;
  uint32_t tmpsmcr;

        /* Check the parameters */
  assert_param(IS_TIM_MASTER_INSTANCE(htim->Instance));
  assert_param(IS_TIM_TRGO_SOURCE(sMasterConfig->MasterOutputTrigger));
  assert_param(IS_TIM_MSM_STATE(sMasterConfig->MasterSlaveMode));

        /* Check input state */
  __HAL_LOCK(htim);

        /* Change the handler state */
  htim->State = HAL_TIM_STATE_BUSY;

        /* Get the TIMx CR2 register value */
  tmpcr2 = htim->Instance->CR2;

        /* Get the TIMx SMCR register value */
  tmpsmcr = htim->Instance->SMCR;

        /* Reset the MMS Bits */
  tmpcr2 &= ~TIM_CR2_MMS;
        /* Select the TRGO source */
  tmpcr2 |=  sMasterConfig->MasterOutputTrigger;

        /* Update TIMx CR2 */
  htim->Instance->CR2 = tmpcr2;

  if (IS_TIM_SLAVE_INSTANCE(htim->Instance))
  {
                /* Reset the MSM Bit */
    tmpsmcr &= ~TIM_SMCR_MSM;
                /* Set master mode */
    tmpsmcr |= sMasterConfig->MasterSlaveMode;

                /* Update TIMx SMCR */
    htim->Instance->SMCR = tmpsmcr;
  }

        /* Change the htim state */
  htim->State = HAL_TIM_STATE_READY;

  __HAL_UNLOCK(htim);

  return HAL_OK;
}



HAL_StatusTypeDef HAL_TIMEx_ConfigBreakDeadTime(TIM_HandleTypeDef *htim,
    TIM_BreakDeadTimeConfigTypeDef *sBreakDeadTimeConfig)
{
        /* Keep this variable initialized to 0 as it is used to configure BDTR register */
  uint32_t tmpbdtr = 0U;

        /* Check the parameters */
  assert_param(IS_TIM_BREAK_INSTANCE(htim->Instance));
  assert_param(IS_TIM_OSSR_STATE(sBreakDeadTimeConfig->OffStateRunMode));
  assert_param(IS_TIM_OSSI_STATE(sBreakDeadTimeConfig->OffStateIDLEMode));
  assert_param(IS_TIM_LOCK_LEVEL(sBreakDeadTimeConfig->LockLevel));
  assert_param(IS_TIM_DEADTIME(sBreakDeadTimeConfig->DeadTime));
  assert_param(IS_TIM_BREAK_STATE(sBreakDeadTimeConfig->BreakState));
  assert_param(IS_TIM_BREAK_POLARITY(sBreakDeadTimeConfig->BreakPolarity));
  assert_param(IS_TIM_AUTOMATIC_OUTPUT_STATE(sBreakDeadTimeConfig->AutomaticOutput));

        /* Check input state */
  __HAL_LOCK(htim);



        /* Set the BDTR bits */
  MODIFY_REG(tmpbdtr, TIM_BDTR_DTG, sBreakDeadTimeConfig->DeadTime);
  MODIFY_REG(tmpbdtr, TIM_BDTR_LOCK, sBreakDeadTimeConfig->LockLevel);
  MODIFY_REG(tmpbdtr, TIM_BDTR_OSSI, sBreakDeadTimeConfig->OffStateIDLEMode);
  MODIFY_REG(tmpbdtr, TIM_BDTR_OSSR, sBreakDeadTimeConfig->OffStateRunMode);
  MODIFY_REG(tmpbdtr, TIM_BDTR_BKE, sBreakDeadTimeConfig->BreakState);
  MODIFY_REG(tmpbdtr, TIM_BDTR_BKP, sBreakDeadTimeConfig->BreakPolarity);
  MODIFY_REG(tmpbdtr, TIM_BDTR_AOE, sBreakDeadTimeConfig->AutomaticOutput);



        /* Set TIMx_BDTR */
  htim->Instance->BDTR = tmpbdtr;

  __HAL_UNLOCK(htim);

  return HAL_OK;
}

#if defined(TIM14)

HAL_StatusTypeDef HAL_TIMEx_RemapConfig(TIM_HandleTypeDef *htim, uint32_t Remap)
{
  __HAL_LOCK(htim);

        /* Check parameters */
  assert_param(IS_TIM_REMAP(htim->Instance, Remap));

        /* Set the Timer remapping configuration */
  WRITE_REG(htim->Instance->OR, Remap);

  __HAL_UNLOCK(htim);

  return HAL_OK;
}
#endif

/* @} */



/* @brief  Hall commutation changed callback in non-blocking mode @param  htim TIM handle @retval None */
__weak void HAL_TIMEx_CommutCallback(TIM_HandleTypeDef *htim)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(htim);



}
/* @brief  Hall commutation changed half complete callback in non-blocking mode @param  htim TIM handle @retval None */
__weak void HAL_TIMEx_CommutHalfCpltCallback(TIM_HandleTypeDef *htim)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(htim);



}

/* @brief  Hall Break detection callback in non-blocking mode @param  htim TIM handle @retval None */
__weak void HAL_TIMEx_BreakCallback(TIM_HandleTypeDef *htim)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(htim);



}
/* @} */



/* @brief  Return the TIM Hall Sensor interface handle state. @param  htim TIM Hall Sensor handle @retval HAL state */
HAL_TIM_StateTypeDef HAL_TIMEx_HallSensor_GetState(TIM_HandleTypeDef *htim)
{
  return htim->State;
}

/* @} */

/* @} */

/* Private functions --------------------------------------------------------- */
/* @defgroup TIMEx_Private_Functions TIMEx Private Functions @{ */

#if (defined(DMA) || defined(DMA1))
/* @brief  TIM DMA Commutation callback. @param  hdma pointer to DMA handle. @retval None */
void TIMEx_DMACommutationCplt(DMA_HandleTypeDef *hdma)
{
  TIM_HandleTypeDef *htim = (TIM_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

        /* Change the htim state */
  htim->State = HAL_TIM_STATE_READY;

#if (USE_HAL_TIM_REGISTER_CALLBACKS == 1)
  htim->CommutationCallback(htim);
#else
  HAL_TIMEx_CommutCallback(htim);
#endif /* USE_HAL_TIM_REGISTER_CALLBACKS */
}

/* @brief  TIM DMA Commutation half complete callback. @param  hdma pointer to DMA handle. @retval None */
void TIMEx_DMACommutationHalfCplt(DMA_HandleTypeDef *hdma)
{
  TIM_HandleTypeDef *htim = (TIM_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

        /* Change the htim state */
  htim->State = HAL_TIM_STATE_READY;

#if (USE_HAL_TIM_REGISTER_CALLBACKS == 1)
  htim->CommutationHalfCpltCallback(htim);
#else
  HAL_TIMEx_CommutHalfCpltCallback(htim);
#endif /* USE_HAL_TIM_REGISTER_CALLBACKS */
}



#endif

static void TIM_CCxNChannelCmd(TIM_TypeDef *TIMx, uint32_t Channel, uint32_t ChannelNState)
{
  uint32_t tmp;

  tmp = TIM_CCER_CC1NE << (Channel & 0x1FU);     /* 0x1FU = 31 bits max shift */

        /* Reset the CCxNE Bit */
  TIMx->CCER &=  ~tmp;

        /* Set or reset the CCxNE Bit */
  TIMx->CCER |= (uint32_t)(ChannelNState << (Channel & 0x1FU));     /* 0x1FU = 31 bits max shift */
}
/* @} */

#endif /* HAL_TIM_MODULE_ENABLED */
/* @} */

/* @} */

/* (C) COPYRIGHT Puya *****END OF FILE */
