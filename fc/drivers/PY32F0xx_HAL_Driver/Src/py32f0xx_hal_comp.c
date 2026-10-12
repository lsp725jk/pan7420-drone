/**
 * @file    py32f0xx_hal_comp.c
 * @author  MCU Application Team
 * @brief   COMP HAL module driver.
 */

/* Includes ------------------------------------------------------------------ */
#include "py32f0xx_hal.h"

/**
 * @addtogroup PY32F0xx_HAL_Driver @{
 */

#ifdef HAL_COMP_MODULE_ENABLED



/* @defgroup COMP COMP @brief COMP HAL module driver @{ */

/* Private typedef ----------------------------------------------------------- */
/* Private define ------------------------------------------------------------ */
/* @addtogroup COMP_Private_Constants @{ */

/* Delay for COMP startup time. */
/* Note: Delay required to reach propagation delay specification. */
/* Literal set to maximum value (refer to device datasheet, */
/* parameter "tSTART"). */
/* Unit: us */
#define COMP_DELAY_STARTUP_US          (80UL) /* !< Delay for COMP startup time */

/* Delay for COMP voltage scaler stabilization time. */
/* Literal set to maximum value (refer to device datasheet, */
/* parameter "tSTART_SCALER"). */
/* Unit: us */
#define COMP_DELAY_VOLTAGE_SCALER_STAB_US (200UL)  /* !< Delay for COMP voltage scaler stabilization time */

#define COMP_OUTPUT_LEVEL_BITOFFSET_POS   (COMP_CSR_COMP_OUT_Pos)

/* @} */

/* Private macro ------------------------------------------------------------- */
/* Private variables --------------------------------------------------------- */
/* Private function prototypes ----------------------------------------------- */
/* Exported functions -------------------------------------------------------- */

/* @defgroup COMP_Exported_Functions COMP Exported Functions @{ */



HAL_StatusTypeDef HAL_COMP_Init(COMP_HandleTypeDef *hcomp)
{
  uint32_t tmp_csr;
  uint32_t exti_line;
  uint32_t comp_voltage_scaler_initialized;     /* Value "0" if comparator voltage scaler is not initialized */
  __IO uint32_t wait_loop_index = 0UL;
  HAL_StatusTypeDef status = HAL_OK;

        /* Check the COMP handle allocation and lock status */
  if(hcomp == NULL)
  {
    status = HAL_ERROR;
  }
  else if(__HAL_COMP_IS_LOCKED(hcomp))
  {
    status = HAL_ERROR;
  }
  else
  {
                /* Check the parameters */
    assert_param(IS_COMP_ALL_INSTANCE(hcomp->Instance));
    assert_param(IS_COMP_INPUT_PLUS(hcomp->Instance, hcomp->Init.InputPlus));
    assert_param(IS_COMP_INPUT_MINUS(hcomp->Instance, hcomp->Init.InputMinus));
    assert_param(IS_COMP_OUTPUTPOL(hcomp->Init.OutputPol));
    assert_param(IS_COMP_POWERMODE(hcomp->Init.Mode));
    assert_param(IS_COMP_HYSTERESIS(hcomp->Init.Hysteresis));
    assert_param(IS_COMP_TRIGGERMODE(hcomp->Init.TriggerMode));
    assert_param(IS_COMP_WINDOWMODE(hcomp->Init.WindowMode));

    if(hcomp->State == HAL_COMP_STATE_RESET)
    {
                        /* Allocate lock resource and initialize it */
      hcomp->Lock = HAL_UNLOCKED;

                        /* Set COMP error code to none */
      COMP_CLEAR_ERRORCODE(hcomp);



#if (USE_HAL_COMP_REGISTER_CALLBACKS == 1)
                        /* Init the COMP Callback settings */
      hcomp->TriggerCallback = HAL_COMP_TriggerCallback;             /* Legacy weak callback */

      if (hcomp->MspInitCallback == NULL)
      {
        hcomp->MspInitCallback = HAL_COMP_MspInit;                 /* Legacy weak MspInit */
      }

                        /* Init the low level hardware */
                        /* Note: Internal control clock of the comparators must */
                        /* be enabled in "HAL_COMP_MspInit()" */
                        /* using "__HAL_RCC_SYSCFG_CLK_ENABLE()". */
      hcomp->MspInitCallback(hcomp);
#else
                        /* Init the low level hardware */
                        /* Note: Internal control clock of the comparators must */
                        /* be enabled in "HAL_COMP_MspInit()" */
                        /* using "__HAL_RCC_SYSCFG_CLK_ENABLE()". */
      HAL_COMP_MspInit(hcomp);
#endif /* USE_HAL_COMP_REGISTER_CALLBACKS */
    }

                /* Memorize voltage scaler state before initialization */
    comp_voltage_scaler_initialized = READ_BIT(hcomp->Instance->CSR, (COMP_CSR_INMSEL_1 | COMP_CSR_INMSEL_0));

                /* Set COMP parameters */
    tmp_csr = (  hcomp->Init.InputMinus
                 | hcomp->Init.InputPlus
                 | hcomp->Init.OutputPol
                 | hcomp->Init.Mode
              );

                /* Set parameters in COMP register */
                /* Note: Update all bits except read-only, lock and enable bits */
    MODIFY_REG(hcomp->Instance->CSR,
               COMP_CSR_LOCK  | COMP_CSR_COMP_OUT | COMP_CSR_PWRMODE |
               COMP_CSR_POLARITY | COMP_CSR_WINMODE | COMP_CSR_INPSEL |
               COMP_CSR_INMSEL | COMP_CSR_EN,
               tmp_csr
              );

                /* Set digital filter */
    if (hcomp->Init.DigitalFilter == 0)
    {
                        /* Disable digital filter */
      CLEAR_BIT(hcomp->Instance->FR, COMP_FR_FLTEN);
    }else
    {
      WRITE_REG(hcomp->Instance->FR, (COMP_FR_FLTEN | ((hcomp->Init.DigitalFilter) << COMP_FR_FLTCNT_Pos)));
    }

                /* Set Vrefint Scaler Enable */
    if ((hcomp->Init.InputMinus == COMP_INPUT_MINUS_1_4VREFINT) ||  (hcomp->Init.InputMinus == COMP_INPUT_MINUS_1_2VREFINT) || \
        (hcomp->Init.InputMinus == COMP_INPUT_MINUS_3_4VREFINT) ||  (hcomp->Init.InputMinus == COMP_INPUT_MINUS_VREFINT))
    {
      SET_BIT(COMP12_COMMON->CSR_ODD, COMP_CSR_SCALER_EN);
    }

                /* Set comparator hysteresis mode */
    MODIFY_REG(COMP12_COMMON->CSR_ODD, COMP_CSR_HYST, hcomp->Init.Hysteresis);

                /* Set window mode */
                /* Note: Window mode bit is located into 1 out of the 2 pairs of COMP */
                /* instances. Therefore, this function can update another COMP */
                /* instance that the one currently selected. */
    if(hcomp->Init.WindowMode == COMP_WINDOWMODE_COMP1_INPUT_PLUS_COMMON)
    {
      CLEAR_BIT(COMP12_COMMON->CSR_ODD, COMP_CSR_WINMODE);
      SET_BIT(COMP12_COMMON->CSR_EVEN, COMP_CSR_WINMODE);
    }
    else if(hcomp->Init.WindowMode == COMP_WINDOWMODE_COMP2_INPUT_PLUS_COMMON)
    {
      SET_BIT(COMP12_COMMON->CSR_ODD, COMP_CSR_WINMODE);
      CLEAR_BIT(COMP12_COMMON->CSR_EVEN, COMP_CSR_WINMODE);
    }
    else
    {
      CLEAR_BIT(COMP12_COMMON->CSR_ODD, COMP_CSR_WINMODE);
      CLEAR_BIT(COMP12_COMMON->CSR_EVEN, COMP_CSR_WINMODE);
    }

                /* Delay for COMP scaler bridge voltage stabilization */
                /* Apply the delay if voltage scaler bridge is required and not already enabled */
    if ((READ_BIT(hcomp->Instance->CSR, (COMP_CSR_INMSEL_1 | COMP_CSR_INMSEL_0)) != 0UL) &&
        (comp_voltage_scaler_initialized == 0UL)               )
    {
                        /* Wait loop initialization and execution */
                        /* Note: Variable divided by 2 to compensate partially */
                        /* CPU processing cycles, scaling in us split to not */
                        /* exceed 32 bits register capacity and handle low frequency. */
      wait_loop_index = ((COMP_DELAY_VOLTAGE_SCALER_STAB_US / 10UL) * (SystemCoreClock / (100000UL * 2UL)));
      while(wait_loop_index != 0UL)
      {
        wait_loop_index--;
      }
    }

                /* Get the EXTI line corresponding to the selected COMP instance */
    exti_line = COMP_GET_EXTI_LINE(hcomp->Instance);

                /* Manage EXTI settings */
    if((hcomp->Init.TriggerMode & (COMP_EXTI_IT | COMP_EXTI_EVENT)) != 0UL)
    {
                        /* Configure EXTI rising edge */
      if((hcomp->Init.TriggerMode & COMP_EXTI_RISING) != 0UL)
      {
        LL_EXTI_EnableRisingTrig(exti_line);
      }
      else
      {
        LL_EXTI_DisableRisingTrig(exti_line);
      }

                        /* Configure EXTI falling edge */
      if((hcomp->Init.TriggerMode & COMP_EXTI_FALLING) != 0UL)
      {
        LL_EXTI_EnableFallingTrig(exti_line);
      }
      else
      {
        LL_EXTI_DisableFallingTrig(exti_line);
      }

                        /* Clear COMP EXTI pending bit (if any) */
      LL_EXTI_ClearFlag(exti_line);

                        /* Configure EXTI event mode */
      if((hcomp->Init.TriggerMode & COMP_EXTI_EVENT) != 0UL)
      {
        LL_EXTI_EnableEvent(exti_line);
      }
      else
      {
        LL_EXTI_DisableEvent(exti_line);
      }

                        /* Configure EXTI interrupt mode */
      if((hcomp->Init.TriggerMode & COMP_EXTI_IT) != 0UL)
      {
        LL_EXTI_EnableIT(exti_line);
      }
      else
      {
        LL_EXTI_DisableIT(exti_line);
      }
    }
    else
    {
                        /* Disable EXTI event mode */
      LL_EXTI_DisableEvent(exti_line);

                        /* Disable EXTI interrupt mode */
      LL_EXTI_DisableIT(exti_line);
    }

                /* Set HAL COMP handle state */
                /* Note: Transition from state reset to state ready, */
                /* otherwise (coming from state ready or busy) no state update. */
    if (hcomp->State == HAL_COMP_STATE_RESET)
    {
      hcomp->State = HAL_COMP_STATE_READY;
    }
  }

  return status;
}



HAL_StatusTypeDef HAL_COMP_DeInit(COMP_HandleTypeDef *hcomp)
{
  HAL_StatusTypeDef status = HAL_OK;

        /* Check the COMP handle allocation and lock status */
  if(hcomp == NULL)
  {
    status = HAL_ERROR;
  }
  else if(__HAL_COMP_IS_LOCKED(hcomp))
  {
    status = HAL_ERROR;
  }
  else
  {
                /* Check the parameter */
    assert_param(IS_COMP_ALL_INSTANCE(hcomp->Instance));

                /* Set COMP_CSR register to reset value */
    WRITE_REG(hcomp->Instance->CSR, 0x00000000UL);

#if (USE_HAL_COMP_REGISTER_CALLBACKS == 1)
    if (hcomp->MspDeInitCallback == NULL)
    {
      hcomp->MspDeInitCallback = HAL_COMP_MspDeInit;             /* Legacy weak MspDeInit */
    }

                /* DeInit the low level hardware: GPIO, RCC clock, NVIC */
    hcomp->MspDeInitCallback(hcomp);
#else
                /* DeInit the low level hardware: GPIO, RCC clock, NVIC */
    HAL_COMP_MspDeInit(hcomp);
#endif /* USE_HAL_COMP_REGISTER_CALLBACKS */

                /* Set HAL COMP handle state */
    hcomp->State = HAL_COMP_STATE_RESET;

                /* Release Lock */
    __HAL_UNLOCK(hcomp);
  }

  return status;
}

/* @brief  Initialize the COMP MSP. @param  hcomp  COMP handle @retval None */
__weak void HAL_COMP_MspInit(COMP_HandleTypeDef *hcomp)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(hcomp);



}

/* @brief  DeInitialize the COMP MSP. @param  hcomp  COMP handle @retval None */
__weak void HAL_COMP_MspDeInit(COMP_HandleTypeDef *hcomp)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(hcomp);



}

#if (USE_HAL_COMP_REGISTER_CALLBACKS == 1)

HAL_StatusTypeDef HAL_COMP_RegisterCallback(COMP_HandleTypeDef *hcomp, HAL_COMP_CallbackIDTypeDef CallbackID, pCOMP_CallbackTypeDef pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (pCallback == NULL)
  {
                /* Update the error code */
    hcomp->ErrorCode |= HAL_COMP_ERROR_INVALID_CALLBACK;

    return HAL_ERROR;
  }

  if (HAL_COMP_STATE_READY == hcomp->State)
  {
    switch (CallbackID)
    {
    case HAL_COMP_TRIGGER_CB_ID :
      hcomp->TriggerCallback = pCallback;
      break;

    case HAL_COMP_MSPINIT_CB_ID :
      hcomp->MspInitCallback = pCallback;
      break;

    case HAL_COMP_MSPDEINIT_CB_ID :
      hcomp->MspDeInitCallback = pCallback;
      break;

    default :
                        /* Update the error code */
      hcomp->ErrorCode |= HAL_COMP_ERROR_INVALID_CALLBACK;

                        /* Return error status */
      status = HAL_ERROR;
      break;
    }
  }
  else if (HAL_COMP_STATE_RESET == hcomp->State)
  {
    switch (CallbackID)
    {
    case HAL_COMP_MSPINIT_CB_ID :
      hcomp->MspInitCallback = pCallback;
      break;

    case HAL_COMP_MSPDEINIT_CB_ID :
      hcomp->MspDeInitCallback = pCallback;
      break;

    default :
                        /* Update the error code */
      hcomp->ErrorCode |= HAL_COMP_ERROR_INVALID_CALLBACK;

                        /* Return error status */
      status = HAL_ERROR;
      break;
    }
  }
  else
  {
                /* Update the error code */
    hcomp->ErrorCode |= HAL_COMP_ERROR_INVALID_CALLBACK;

                /* Return error status */
    status =  HAL_ERROR;
  }

  return status;
}



HAL_StatusTypeDef HAL_COMP_UnRegisterCallback(COMP_HandleTypeDef *hcomp, HAL_COMP_CallbackIDTypeDef CallbackID)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (HAL_COMP_STATE_READY == hcomp->State)
  {
    switch (CallbackID)
    {
    case HAL_COMP_TRIGGER_CB_ID :
      hcomp->TriggerCallback = HAL_COMP_TriggerCallback;                     /* Legacy weak callback */
      break;

    case HAL_COMP_MSPINIT_CB_ID :
      hcomp->MspInitCallback = HAL_COMP_MspInit;                             /* Legacy weak MspInit */
      break;

    case HAL_COMP_MSPDEINIT_CB_ID :
      hcomp->MspDeInitCallback = HAL_COMP_MspDeInit;                         /* Legacy weak MspDeInit */
      break;

    default :
                        /* Update the error code */
      hcomp->ErrorCode |= HAL_COMP_ERROR_INVALID_CALLBACK;

                        /* Return error status */
      status =  HAL_ERROR;
      break;
    }
  }
  else if (HAL_COMP_STATE_RESET == hcomp->State)
  {
    switch (CallbackID)
    {
    case HAL_COMP_MSPINIT_CB_ID :
      hcomp->MspInitCallback = HAL_COMP_MspInit;                             /* Legacy weak MspInit */
      break;

    case HAL_COMP_MSPDEINIT_CB_ID :
      hcomp->MspDeInitCallback = HAL_COMP_MspDeInit;                         /* Legacy weak MspDeInit */
      break;

    default :
                        /* Update the error code */
      hcomp->ErrorCode |= HAL_COMP_ERROR_INVALID_CALLBACK;

                        /* Return error status */
      status =  HAL_ERROR;
      break;
    }
  }
  else
  {
                /* Update the error code */
    hcomp->ErrorCode |= HAL_COMP_ERROR_INVALID_CALLBACK;

                /* Return error status */
    status =  HAL_ERROR;
  }

  return status;
}

#endif /* USE_HAL_COMP_REGISTER_CALLBACKS */

/* @} */



/* @brief  Start the comparator. @param  hcomp  COMP handle @retval HAL status */
HAL_StatusTypeDef HAL_COMP_Start(COMP_HandleTypeDef *hcomp)
{
  __IO uint32_t wait_loop_index = 0UL;
  HAL_StatusTypeDef status = HAL_OK;

        /* Check the COMP handle allocation and lock status */
  if(hcomp == NULL)
  {
    status = HAL_ERROR;
  }
  else if(__HAL_COMP_IS_LOCKED(hcomp))
  {
    status = HAL_ERROR;
  }
  else
  {
                /* Check the parameter */
    assert_param(IS_COMP_ALL_INSTANCE(hcomp->Instance));

    if(hcomp->State == HAL_COMP_STATE_READY)
    {
                        /* Enable the selected comparator */
      SET_BIT(hcomp->Instance->CSR, COMP_CSR_EN);

                        /* Set HAL COMP handle state */
      hcomp->State = HAL_COMP_STATE_BUSY;

                        /* Delay for COMP startup time */
                        /* Wait loop initialization and execution */
                        /* Note: Variable divided by 2 to compensate partially */
                        /* CPU processing cycles, scaling in us split to not */
                        /* exceed 32 bits register capacity and handle low frequency. */
      wait_loop_index = ((COMP_DELAY_STARTUP_US / 10UL) * (SystemCoreClock / (100000UL * 2UL)));
      while(wait_loop_index != 0UL)
      {
        wait_loop_index--;
      }
    }
    else
    {
      status = HAL_ERROR;
    }
  }

  return status;
}

/* @brief  Stop the comparator. @param  hcomp  COMP handle @retval HAL status */
HAL_StatusTypeDef HAL_COMP_Stop(COMP_HandleTypeDef *hcomp)
{
  HAL_StatusTypeDef status = HAL_OK;

        /* Check the COMP handle allocation and lock status */
  if(hcomp == NULL)
  {
    status = HAL_ERROR;
  }
  else if(__HAL_COMP_IS_LOCKED(hcomp))
  {
    status = HAL_ERROR;
  }
  else
  {
                /* Check the parameter */
    assert_param(IS_COMP_ALL_INSTANCE(hcomp->Instance));

                /* Check compliant states: HAL_COMP_STATE_READY or HAL_COMP_STATE_BUSY */
                /* (all states except HAL_COMP_STATE_RESET and except locked status. */
    if(hcomp->State != HAL_COMP_STATE_RESET)
    {
                        /* Disable the selected comparator */
      CLEAR_BIT(hcomp->Instance->CSR, COMP_CSR_EN);

                        /* Set HAL COMP handle state */
      hcomp->State = HAL_COMP_STATE_READY;
    }
    else
    {
      status = HAL_ERROR;
    }
  }

  return status;
}

/* @brief  Comparator IRQ handler. @param  hcomp  COMP handle @retval None */
void HAL_COMP_IRQHandler(COMP_HandleTypeDef *hcomp)
{
        /* Get the EXTI line corresponding to the selected COMP instance */
  uint32_t exti_line = COMP_GET_EXTI_LINE(hcomp->Instance);
  uint32_t comparator_window_mode_odd = READ_BIT(COMP12_COMMON->CSR_ODD, COMP_CSR_WINMODE);
  uint32_t comparator_window_mode_even = READ_BIT(COMP12_COMMON->CSR_EVEN, COMP_CSR_WINMODE);

        /* Check COMP EXTI flag */
  if(LL_EXTI_IsActiveFlag(exti_line) != 0UL)
  {
                /* Check whether comparator is in independent or window mode */
    if(   (comparator_window_mode_odd != 0UL)
          || (comparator_window_mode_even != 0UL))
    {
                        /* Clear COMP EXTI line pending bit of the pair of comparators */
                        /* in window mode. */
                        /* Note: Pair of comparators in window mode can both trig IRQ when */
                        /* input voltage is changing from "out of window" area */
                        /* (low or high ) to the other "out of window" area (high or low). */
                        /* Both flags must be cleared to call comparator trigger */
                        /* callback is called once. */
      LL_EXTI_ClearFlag((COMP_EXTI_LINE_COMP1 | COMP_EXTI_LINE_COMP2));
    }
    else
    {
                        /* Clear COMP EXTI line pending bit */
      LL_EXTI_ClearFlag(exti_line);
    }

                /* COMP trigger user callback */
#if (USE_HAL_COMP_REGISTER_CALLBACKS == 1)
    hcomp->TriggerCallback(hcomp);
#else
    HAL_COMP_TriggerCallback(hcomp);
#endif /* USE_HAL_COMP_REGISTER_CALLBACKS */
  }
  else
  {
                /* nothing to do */
  }
}

/* @} */



HAL_StatusTypeDef HAL_COMP_Lock(COMP_HandleTypeDef *hcomp)
{
  HAL_StatusTypeDef status = HAL_OK;

        /* Check the COMP handle allocation and lock status */
  if(hcomp == NULL)
  {
    status = HAL_ERROR;
  }
  else if(__HAL_COMP_IS_LOCKED(hcomp))
  {
    status = HAL_ERROR;
  }
  else
  {
                /* Check the parameter */
    assert_param(IS_COMP_ALL_INSTANCE(hcomp->Instance));

                /* Set HAL COMP handle state */
    switch(hcomp->State)
    {
    case HAL_COMP_STATE_RESET:
      hcomp->State = HAL_COMP_STATE_RESET_LOCKED;
      break;
    case HAL_COMP_STATE_READY:
      hcomp->State = HAL_COMP_STATE_READY_LOCKED;
      break;
    default:         /* HAL_COMP_STATE_BUSY */
      hcomp->State = HAL_COMP_STATE_BUSY_LOCKED;
      break;
    }
  }

  if(status == HAL_OK)
  {
                /* Set the lock bit corresponding to selected comparator */
    __HAL_COMP_LOCK(hcomp);
  }

  return status;
}



uint32_t HAL_COMP_GetOutputLevel(COMP_HandleTypeDef *hcomp)
{
        /* Check the parameter */
  assert_param(IS_COMP_ALL_INSTANCE(hcomp->Instance));

  return (uint32_t)(READ_BIT(hcomp->Instance->CSR, COMP_CSR_COMP_OUT)
                    >> COMP_OUTPUT_LEVEL_BITOFFSET_POS);
}

/* @brief  Comparator trigger callback. @param  hcomp  COMP handle @retval None */
__weak void HAL_COMP_TriggerCallback(COMP_HandleTypeDef *hcomp)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(hcomp);



}



/* @} */



/* @brief  Return the COMP handle state. @param  hcomp  COMP handle @retval HAL state */
HAL_COMP_StateTypeDef HAL_COMP_GetState(COMP_HandleTypeDef *hcomp)
{
        /* Check the COMP handle allocation */
  if(hcomp == NULL)
  {
    return HAL_COMP_STATE_RESET;
  }

        /* Check the parameter */
  assert_param(IS_COMP_ALL_INSTANCE(hcomp->Instance));

        /* Return HAL COMP handle state */
  return hcomp->State;
}

/* @brief  Return the COMP error code. @param hcomp COMP handle @retval COMP error code */
uint32_t HAL_COMP_GetError(COMP_HandleTypeDef *hcomp)
{
        /* Check the parameters */
  assert_param(IS_COMP_ALL_INSTANCE(hcomp->Instance));

  return hcomp->ErrorCode;
}

/* @} */

/* @} */

/* @} */



#endif /* HAL_COMP_MODULE_ENABLED */

/* @} */

/* (C) COPYRIGHT Puya *****END OF FILE */
