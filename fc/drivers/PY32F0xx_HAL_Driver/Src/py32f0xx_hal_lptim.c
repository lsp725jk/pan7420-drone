/**
 * @file    py32f0xx_hal_lptim.c
 * @author  MCU Application Team
 * @brief   LPTIM HAL module driver.
 */

/* Includes ------------------------------------------------------------------ */
#include "py32f0xx_hal.h"

/* @addtogroup PY32F0xx_HAL_Driver @{ */

/* @defgroup LPTIM LPTIM @brief LPTIM HAL module driver. @{ */

#ifdef HAL_LPTIM_MODULE_ENABLED



/* Private typedef ----------------------------------------------------------- */
/* Private define ------------------------------------------------------------ */
/* Private macro ------------------------------------------------------------- */
/* Private variables --------------------------------------------------------- */
/* Private function prototypes ----------------------------------------------- */
#if (USE_HAL_LPTIM_REGISTER_CALLBACKS == 1)
static void LPTIM_ResetCallback(LPTIM_HandleTypeDef *lptim);
#endif /* USE_HAL_LPTIM_REGISTER_CALLBACKS */

/* Exported functions -------------------------------------------------------- */

/* @defgroup LPTIM_Exported_Functions LPTIM Exported Functions @{ */



HAL_StatusTypeDef HAL_LPTIM_Init(LPTIM_HandleTypeDef *hlptim)
{
  uint32_t tmpcfgr;

        /* Check the LPTIM handle allocation */
  if(hlptim == NULL)
  {
    return HAL_ERROR;
  }

        /* Check the parameters */
  assert_param(IS_LPTIM_INSTANCE(hlptim->Instance));

  assert_param(IS_LPTIM_CLOCK_PRESCALER(hlptim->Init.Prescaler));

  if(hlptim->State == HAL_LPTIM_STATE_RESET)
  {
                /* Allocate lock resource and initialize it */
    hlptim->Lock = HAL_UNLOCKED;

#if (USE_HAL_LPTIM_REGISTER_CALLBACKS == 1)
                /* Reset interrupt callbacks to legacy weak callbacks */
    LPTIM_ResetCallback(hlptim);

    if(hlptim->MspInitCallback == NULL)
    {
      hlptim->MspInitCallback = HAL_LPTIM_MspInit;
    }

                /* Init the low level hardware : GPIO, CLOCK, NVIC */
    hlptim->MspInitCallback(hlptim);
#else
                /* Init the low level hardware : GPIO, CLOCK, NVIC */
    HAL_LPTIM_MspInit(hlptim);
#endif /* USE_HAL_LPTIM_REGISTER_CALLBACKS */
  }

        /* Change the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_BUSY;

        /* Get the LPTIMx CFGR value */
  tmpcfgr = hlptim->Instance->CFGR;

        /* Clear PRESC, PRELOAD */
  tmpcfgr &= (uint32_t)(~(LPTIM_CFGR_PRELOAD | LPTIM_CFGR_PRESC));

        /* Set initialization parameters */
  tmpcfgr |= (hlptim->Init.Prescaler|hlptim->Init.UpdateMode);

        /* Write to LPTIMx CFGR */
  hlptim->Instance->CFGR = tmpcfgr;

        /* Change the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_READY;

        /* Return function status */
  return HAL_OK;
}

/* @brief  DeInitialize the LPTIM peripheral. @param  hlptim LPTIM handle @retval HAL status */
HAL_StatusTypeDef HAL_LPTIM_DeInit(LPTIM_HandleTypeDef *hlptim)
{
        /* Check the LPTIM handle allocation */
  if(hlptim == NULL)
  {
    return HAL_ERROR;
  }

        /* Change the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_BUSY;

        /* Disable the LPTIM Peripheral Clock */
  __HAL_LPTIM_DISABLE(hlptim);

#if (USE_HAL_LPTIM_REGISTER_CALLBACKS == 1)
  if(hlptim->MspDeInitCallback == NULL)
  {
    hlptim->MspDeInitCallback = HAL_LPTIM_MspDeInit;
  }

        /* DeInit the low level hardware: CLOCK, NVIC. */
  hlptim->MspDeInitCallback(hlptim);
#else
        /* DeInit the low level hardware: CLOCK, NVIC. */
  HAL_LPTIM_MspDeInit(hlptim);
#endif /* USE_HAL_LPTIM_REGISTER_CALLBACKS */

        /* Change the LPTIM state */
  hlptim->State = HAL_LPTIM_STATE_RESET;

        /* Release Lock */
  __HAL_UNLOCK(hlptim);

        /* Return function status */
  return HAL_OK;
}

/* @brief  Initialize the LPTIM MSP. @param  hlptim LPTIM handle @retval None */
__weak void HAL_LPTIM_MspInit(LPTIM_HandleTypeDef *hlptim)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(hlptim);



}

/* @brief  DeInitialize LPTIM MSP. @param  hlptim LPTIM handle @retval None */
__weak void HAL_LPTIM_MspDeInit(LPTIM_HandleTypeDef *hlptim)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(hlptim);



}

/* @} */



HAL_StatusTypeDef HAL_LPTIM_SetOnce_Start(LPTIM_HandleTypeDef *hlptim, uint32_t Period)
{
        /* Check the parameters */
  assert_param(IS_LPTIM_INSTANCE(hlptim->Instance));
  assert_param(IS_LPTIM_PERIOD(Period));

        /* Set the LPTIM state */
  hlptim->State= HAL_LPTIM_STATE_BUSY;

        /* Enable the Peripheral */
  __HAL_LPTIM_ENABLE(hlptim);

        /* Load the period value in the autoreload register */
  __HAL_LPTIM_AUTORELOAD_SET(hlptim, Period);

        /* Start timer in single mode */
  __HAL_LPTIM_START_SINGLE(hlptim);

        /* Change the TIM state */
  hlptim->State= HAL_LPTIM_STATE_READY;

        /* Return function status */
  return HAL_OK;
}

/* @brief  Stop the LPTIM Set once mode. @param  hlptim LPTIM handle @retval HAL status */
HAL_StatusTypeDef HAL_LPTIM_SetOnce_Stop(LPTIM_HandleTypeDef *hlptim)
{
        /* Check the parameters */
  assert_param(IS_LPTIM_INSTANCE(hlptim->Instance));

        /* Set the LPTIM state */
  hlptim->State= HAL_LPTIM_STATE_BUSY;

        /* Disable the Peripheral */
  __HAL_LPTIM_DISABLE(hlptim);

        /* Change the TIM state */
  hlptim->State= HAL_LPTIM_STATE_READY;

        /* Return function status */
  return HAL_OK;
}



HAL_StatusTypeDef HAL_LPTIM_SetOnce_Start_IT(LPTIM_HandleTypeDef *hlptim, uint32_t Period)
{
        /* Check the parameters */
  assert_param(IS_LPTIM_INSTANCE(hlptim->Instance));
  assert_param(IS_LPTIM_PERIOD(Period));

        /* Set the LPTIM state */
  hlptim->State= HAL_LPTIM_STATE_BUSY;

        /* Enable Autoreload match interrupt */
  __HAL_LPTIM_ENABLE_IT(hlptim, LPTIM_IT_ARRM);

        /* Enable the Peripheral */
  __HAL_LPTIM_ENABLE(hlptim);

        /* Load the period value in the autoreload register */
  __HAL_LPTIM_AUTORELOAD_SET(hlptim, Period);

        /* Start timer in single mode */
  __HAL_LPTIM_START_SINGLE(hlptim);

        /* Change the TIM state */
  hlptim->State= HAL_LPTIM_STATE_READY;

        /* Return function status */
  return HAL_OK;
}

/* @brief  Stop the LPTIM Set once mode in interrupt mode. @param  hlptim LPTIM handle @retval HAL status */
HAL_StatusTypeDef HAL_LPTIM_SetOnce_Stop_IT(LPTIM_HandleTypeDef *hlptim)
{
        /* Check the parameters */
  assert_param(IS_LPTIM_INSTANCE(hlptim->Instance));

        /* Set the LPTIM state */
  hlptim->State= HAL_LPTIM_STATE_BUSY;

        /* Disable the Peripheral */
  __HAL_LPTIM_DISABLE(hlptim);

        /* Disable Autoreload match interrupt */
  __HAL_LPTIM_DISABLE_IT(hlptim, LPTIM_IT_ARRM);

        /* Change the TIM state */
  hlptim->State= HAL_LPTIM_STATE_READY;

        /* Return function status */
  return HAL_OK;
}

/* @} */



/* @brief  Return the current counter value. @param  hlptim LPTIM handle @retval Counter value. */
uint32_t HAL_LPTIM_ReadCounter(LPTIM_HandleTypeDef *hlptim)
{
        /* Check the parameters */
  assert_param(IS_LPTIM_INSTANCE(hlptim->Instance));

  return (hlptim->Instance->CNT);
}

/* @brief  Return the current Autoreload (Period) value. @param  hlptim LPTIM handle @retval Autoreload value. */
uint32_t HAL_LPTIM_ReadAutoReload(LPTIM_HandleTypeDef *hlptim)
{
        /* Check the parameters */
  assert_param(IS_LPTIM_INSTANCE(hlptim->Instance));

  return (hlptim->Instance->ARR);
}



/* @} */



/* @brief  Handle LPTIM interrupt request. @param  hlptim LPTIM handle @retval None */
void HAL_LPTIM_IRQHandler(LPTIM_HandleTypeDef *hlptim)
{
        /* Autoreload match interrupt */
  if(__HAL_LPTIM_GET_FLAG(hlptim, LPTIM_FLAG_ARRM) != RESET)
  {
    if(__HAL_LPTIM_GET_IT_SOURCE(hlptim, LPTIM_IT_ARRM) != RESET)
    {
                        /* Clear Autoreload match flag */
      __HAL_LPTIM_CLEAR_FLAG(hlptim, LPTIM_FLAG_ARRM);

                        /* Autoreload match Callback */
#if (USE_HAL_LPTIM_REGISTER_CALLBACKS == 1)
      hlptim->AutoReloadMatchCallback(hlptim);
#else
      HAL_LPTIM_AutoReloadMatchCallback(hlptim);
#endif /* USE_HAL_LPTIM_REGISTER_CALLBACKS */
    }
  }
}

/* @brief  Autoreload match callback in non-blocking mode. @param  hlptim LPTIM handle @retval None */
__weak void HAL_LPTIM_AutoReloadMatchCallback(LPTIM_HandleTypeDef *hlptim)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(hlptim);



}



#if (USE_HAL_LPTIM_REGISTER_CALLBACKS == 1)

HAL_StatusTypeDef HAL_LPTIM_RegisterCallback(LPTIM_HandleTypeDef *       hlptim,
    HAL_LPTIM_CallbackIDTypeDef CallbackID,
    pLPTIM_CallbackTypeDef      pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  if(pCallback == NULL)
  {
    return HAL_ERROR;
  }

        /* Process locked */
  __HAL_LOCK(hlptim);

  if(hlptim->State == HAL_LPTIM_STATE_READY)
  {
    switch (CallbackID)
    {
    case HAL_LPTIM_MSPINIT_CB_ID :
      hlptim->MspInitCallback = pCallback;
      break;

    case HAL_LPTIM_MSPDEINIT_CB_ID :
      hlptim->MspDeInitCallback = pCallback;
      break;

    case HAL_LPTIM_AUTORELOAD_MATCH_CB_ID :
      hlptim->AutoReloadMatchCallback = pCallback;
      break;

    default :
                        /* Return error status */
      status =  HAL_ERROR;
      break;
    }
  }
  else if(hlptim->State == HAL_LPTIM_STATE_RESET)
  {
    switch (CallbackID)
    {
    case HAL_LPTIM_MSPINIT_CB_ID :
      hlptim->MspInitCallback = pCallback;
      break;

    case HAL_LPTIM_MSPDEINIT_CB_ID :
      hlptim->MspDeInitCallback = pCallback;
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
  __HAL_UNLOCK(hlptim);

  return status;
}



HAL_StatusTypeDef HAL_LPTIM_UnRegisterCallback(LPTIM_HandleTypeDef *       hlptim,
    HAL_LPTIM_CallbackIDTypeDef CallbackID)
{
  HAL_StatusTypeDef status = HAL_OK;

        /* Process locked */
  __HAL_LOCK(hlptim);

  if(hlptim->State == HAL_LPTIM_STATE_READY)
  {
    switch (CallbackID)
    {
    case HAL_LPTIM_MSPINIT_CB_ID :
      hlptim->MspInitCallback = HAL_LPTIM_MspInit;                                      /* Legacy weak MspInit Callback */
      break;

    case HAL_LPTIM_MSPDEINIT_CB_ID :
      hlptim->MspDeInitCallback = HAL_LPTIM_MspDeInit;                                   /* Legacy weak Msp DeInit Callback */
      break;

    case HAL_LPTIM_AUTORELOAD_MATCH_CB_ID :
      hlptim->AutoReloadMatchCallback = HAL_LPTIM_AutoReloadMatchCallback;               /* Legacy weak IC Msp DeInit Callback */
      break;

    default :
                        /* Return error status */
      status =  HAL_ERROR;
      break;
    }
  }
  else if(hlptim->State == HAL_LPTIM_STATE_RESET)
  {
    switch (CallbackID)
    {
    case HAL_LPTIM_MSPINIT_CB_ID :
      hlptim->MspInitCallback = HAL_LPTIM_MspInit;                                       /* Legacy weak MspInit Callback */
      break;

    case HAL_LPTIM_MSPDEINIT_CB_ID :
      hlptim->MspDeInitCallback = HAL_LPTIM_MspDeInit;                                    /* Legacy weak Msp DeInit Callback */
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
  __HAL_UNLOCK(hlptim);

  return status;
}
#endif /* USE_HAL_LPTIM_REGISTER_CALLBACKS */

/* @} */

/* @defgroup LPTIM_Group5 Peripheral State functions @brief   Peripheral State functions. @verbatim ##### Peripheral State functions ##### */

/* @brief  Return the LPTIM handle state. @param  hlptim LPTIM handle @retval HAL state */
HAL_LPTIM_StateTypeDef HAL_LPTIM_GetState(LPTIM_HandleTypeDef *hlptim)
{
        /* Return LPTIM handle state */
  return hlptim->State;
}

/* @} */



/* @} */

/* Private functions --------------------------------------------------------- */

/* @defgroup LPTIM_Private_Functions LPTIM Private Functions @{ */
#if (USE_HAL_LPTIM_REGISTER_CALLBACKS == 1)

static void LPTIM_ResetCallback(LPTIM_HandleTypeDef *lptim)
{
        /* Reset the LPTIM callback to the legacy weak callbacks */
  lptim->AutoReloadMatchCallback = HAL_LPTIM_AutoReloadMatchCallback;     /* Auto-reload match Callback */
}
#endif /* USE_HAL_LPTIM_REGISTER_CALLBACKS */

/* @} */



#endif /* HAL_LPTIM_MODULE_ENABLED */
/* @} */

/* @} */

/* (C) COPYRIGHT Puya *****END OF FILE */
