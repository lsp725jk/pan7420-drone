/**
 * @file    py32f0xx_hal_exti.c
 * @author  MCU Application Team
 * @brief   EXTI HAL module driver.
 */

/* Includes ------------------------------------------------------------------ */
#include "py32f0xx_hal.h"

/* @addtogroup PY32F0xx_HAL_Driver @{ */

/* @addtogroup EXTI @{ */
/* of bounds [0,3] in following API : HAL_EXTI_SetConfigLine HAL_EXTI_GetConfigLine HAL_EXTI_ClearConfigLine */

#ifdef HAL_EXTI_MODULE_ENABLED

/* Private typedef ----------------------------------------------------------- */
/* Private defines ------------------------------------------------------------ */
/* @defgroup EXTI_Private_Constants EXTI Private Constants @{ */
#define EXTI_MODE_OFFSET                    0x04u   /* 0x10: offset between CPU IMR/EMR registers */
#define EXTI_CONFIG_OFFSET                  0x08u   /* 0x20: offset between CPU Rising/Falling configuration registers */
/* @} */

/* Private macros ------------------------------------------------------------ */
/* Private variables --------------------------------------------------------- */
/* Private function prototypes ----------------------------------------------- */
/* Exported functions -------------------------------------------------------- */

/* @addtogroup EXTI_Exported_Functions @{ */

/* @addtogroup EXTI_Exported_Functions_Group1 @brief    Configuration functions @verbatim ##### Configuration functions ##### */



HAL_StatusTypeDef HAL_EXTI_SetConfigLine(EXTI_HandleTypeDef *hexti, EXTI_ConfigTypeDef *pExtiConfig)
{
  uint32_t regval;
  uint32_t linepos;
  uint32_t maskline;

        /* Check null pointer */
  if ((hexti == NULL) || (pExtiConfig == NULL))
  {
    return HAL_ERROR;
  }

        /* Check parameters */
  assert_param(IS_EXTI_LINE(pExtiConfig->Line));
  assert_param(IS_EXTI_MODE(pExtiConfig->Mode));

        /* Assign line number to handle */
  hexti->Line = pExtiConfig->Line;

        /* Compute line mask */
  linepos = (pExtiConfig->Line & EXTI_PIN_MASK);
  maskline = (1uL << linepos);

        /* Configure triggers for configurable lines */
  if ((pExtiConfig->Line & EXTI_CONFIG) != 0x00u)
  {
    assert_param(IS_EXTI_TRIGGER(pExtiConfig->Trigger));

                /* Configure rising trigger */
                /* Mask or set line */
    if ((pExtiConfig->Trigger & EXTI_TRIGGER_RISING) != 0x00u)
    {
      EXTI->RTSR |= maskline;
    }
    else
    {
      EXTI->RTSR &= ~maskline;
    }

                /* Configure falling trigger */
                /* Mask or set line */
    if ((pExtiConfig->Trigger & EXTI_TRIGGER_FALLING) != 0x00u)
    {
      EXTI->FTSR |= maskline;
    }
    else
    {
      EXTI->FTSR &= ~maskline;
    }



                /* Configure gpio port selection in case of gpio exti line */
    if ((pExtiConfig->Line & EXTI_GPIO) == EXTI_GPIO)
    {
      assert_param(IS_EXTI_GPIO_PORT(pExtiConfig->GPIOSel));
      assert_param(IS_EXTI_GPIO_PIN(linepos));

      regval = EXTI->EXTICR[linepos >> 2u];
      regval &= ~(EXTI_EXTICR1_EXTI0 << (EXTI_EXTICR1_EXTI1_Pos * (linepos & 0x03u)));
      regval |= (pExtiConfig->GPIOSel << (EXTI_EXTICR1_EXTI1_Pos * (linepos & 0x03u)));
      EXTI->EXTICR[linepos >> 2u] = regval;
    }
  }

        /* Configure interrupt mode : read current mode */
        /* Mask or set line */
  if ((pExtiConfig->Mode & EXTI_MODE_INTERRUPT) != 0x00u)
  {
    EXTI->IMR |= maskline;
  }
  else
  {
    EXTI->IMR &= ~maskline;
  }

        /* Configure event mode : read current mode */
        /* Mask or set line */
  if ((pExtiConfig->Mode & EXTI_MODE_EVENT) != 0x00u)
  {
    EXTI->EMR |= maskline;
  }
  else
  {
    EXTI->EMR &= ~maskline;
  }

  return HAL_OK;
}



HAL_StatusTypeDef HAL_EXTI_GetConfigLine(EXTI_HandleTypeDef *hexti, EXTI_ConfigTypeDef *pExtiConfig)
{
  uint32_t regval;
  uint32_t linepos;
  uint32_t maskline;

        /* Check null pointer */
  if ((hexti == NULL) || (pExtiConfig == NULL))
  {
    return HAL_ERROR;
  }

        /* Check the parameter */
  assert_param(IS_EXTI_LINE(hexti->Line));

        /* Store handle line number to configuration structure */
  pExtiConfig->Line = hexti->Line;

        /* Compute line mask */
  linepos = (pExtiConfig->Line & EXTI_PIN_MASK);
  maskline = (1uL << linepos);

        /* 1] Get core mode : interrupt */

        /* Check if selected line is enable */
  if ((EXTI->IMR & maskline) != 0x00u)
  {
    pExtiConfig->Mode = EXTI_MODE_INTERRUPT;
  }
  else
  {
    pExtiConfig->Mode = EXTI_MODE_NONE;
  }

        /* Get event mode */
        /* Check if selected line is enable */
  if ((EXTI->EMR & maskline) != 0x00u)
  {
    pExtiConfig->Mode |= EXTI_MODE_EVENT;
  }

        /* 2] Get trigger for configurable lines : rising */
  if ((pExtiConfig->Line & EXTI_CONFIG) != 0x00u)
  {
                /* Check if configuration of selected line is enable */
    if ((EXTI->RTSR & maskline) != 0x00u)
    {
      pExtiConfig->Trigger = EXTI_TRIGGER_RISING;
    }
    else
    {
      pExtiConfig->Trigger = EXTI_TRIGGER_NONE;
    }

                /* Get falling configuration */
                /* Check if configuration of selected line is enable */
    if ((EXTI->FTSR & maskline) != 0x00u)
    {
      pExtiConfig->Trigger |= EXTI_TRIGGER_FALLING;
    }

                /* Get Gpio port selection for gpio lines */
    if ((pExtiConfig->Line & EXTI_GPIO) == EXTI_GPIO)
    {
      assert_param(IS_EXTI_GPIO_PIN(linepos));

      regval = EXTI->EXTICR[linepos >> 2u];
      pExtiConfig->GPIOSel = ((regval << (EXTI_EXTICR1_EXTI1_Pos * (3uL - (linepos & 0x03u)))) >> 24);
    }
    else
    {
      pExtiConfig->GPIOSel = 0x00u;
    }
  }
  else
  {
                /* No Trigger selected */
    pExtiConfig->Trigger = EXTI_TRIGGER_NONE;
    pExtiConfig->GPIOSel = 0x00u;
  }

  return HAL_OK;
}

/* @brief  Clear whole configuration of a dedicated Exti line. @param  hexti Exti handle. @retval HAL Status. */
HAL_StatusTypeDef HAL_EXTI_ClearConfigLine(EXTI_HandleTypeDef *hexti)
{
  uint32_t regval;
  uint32_t linepos;
  uint32_t maskline;

        /* Check null pointer */
  if (hexti == NULL)
  {
    return HAL_ERROR;
  }

        /* Check the parameter */
  assert_param(IS_EXTI_LINE(hexti->Line));

        /* compute line mask */
  linepos = (hexti->Line & EXTI_PIN_MASK);
  maskline = (1uL << linepos);

        /* 1] Clear interrupt mode */
  EXTI->IMR = (EXTI->IMR & ~maskline);

        /* 2] Clear event mode */
  EXTI->EMR = (EXTI->EMR & ~maskline);

        /* 3] Clear triggers in case of configurable lines */
  if ((hexti->Line & EXTI_CONFIG) != 0x00u)
  {
    EXTI->RTSR = (EXTI->RTSR & ~maskline);
    EXTI->FTSR = (EXTI->FTSR & ~maskline);

                /* Get Gpio port selection for gpio lines */
    if ((hexti->Line & EXTI_GPIO) == EXTI_GPIO)
    {
      assert_param(IS_EXTI_GPIO_PIN(linepos));

      regval = EXTI->EXTICR[linepos >> 2u];
      regval &= ~(EXTI_EXTICR1_EXTI0 << (EXTI_EXTICR1_EXTI1_Pos * (linepos & 0x03u)));
      EXTI->EXTICR[linepos >> 2u] = regval;
    }
  }

  return HAL_OK;
}



HAL_StatusTypeDef HAL_EXTI_RegisterCallback(EXTI_HandleTypeDef *hexti, EXTI_CallbackIDTypeDef CallbackID, void (*pPendingCbfn)(void))
{
  HAL_StatusTypeDef status = HAL_OK;

  switch (CallbackID)
  {
  case  HAL_EXTI_COMMON_CB_ID:
    hexti->PendingCallback = pPendingCbfn;
    break;

  default:
    status = HAL_ERROR;
    break;
  }

  return status;
}



HAL_StatusTypeDef HAL_EXTI_GetHandle(EXTI_HandleTypeDef *hexti, uint32_t ExtiLine)
{
        /* Check the parameters */
  assert_param(IS_EXTI_LINE(ExtiLine));

        /* Check null pointer */
  if (hexti == NULL)
  {
    return HAL_ERROR;
  }
  else
  {
                /* Store line number as handle private field */
    hexti->Line = ExtiLine;

    return HAL_OK;
  }
}

/* @} */

/* @addtogroup EXTI_Exported_Functions_Group2 @brief EXTI IO functions. @verbatim ##### IO operation functions ##### */

/* @brief  Handle EXTI interrupt request. @param  hexti Exti handle. @retval none. */
void HAL_EXTI_IRQHandler(EXTI_HandleTypeDef *hexti)
{
  uint32_t regval;
  uint32_t maskline;

        /* Compute line mask */
  maskline = (1uL << (hexti->Line & EXTI_PIN_MASK));

        /* Get pending bit */
  regval = (EXTI->PR & maskline);
  if (regval != 0x00u)
  {
                /* Clear pending bit */
    EXTI->PR = maskline;

                /* Call callback */
    if (hexti->PendingCallback != NULL)
    {
      hexti->PendingCallback();
    }
  }
}



uint32_t HAL_EXTI_GetPending(EXTI_HandleTypeDef *hexti, uint32_t Edge)
{
  uint32_t regval;
  uint32_t linepos;
  uint32_t maskline;

        /* Check parameters */
  assert_param(IS_EXTI_LINE(hexti->Line));
  assert_param(IS_EXTI_CONFIG_LINE(hexti->Line));
  assert_param(IS_EXTI_PENDING_EDGE(Edge));

        /* Compute line mask */
  linepos = (hexti->Line & EXTI_PIN_MASK);
  maskline = (1uL << linepos);

        /* return 1 if bit is set else 0 */
  regval = ((EXTI->PR & maskline) >> linepos);
  return regval;
}

/* @brief  Clear interrupt pending bit of a dedicated line. @param  hexti Exti handle. @retval None. */
void HAL_EXTI_ClearPending(EXTI_HandleTypeDef *hexti)
{
  uint32_t maskline;

        /* Check parameters */
  assert_param(IS_EXTI_LINE(hexti->Line));
  assert_param(IS_EXTI_CONFIG_LINE(hexti->Line));

        /* Compute line mask */
  maskline = (1uL << (hexti->Line & EXTI_PIN_MASK));

        /* Clear Pending bit */
  EXTI->PR =  maskline;
}

/* @brief  Generate a software interrupt for a dedicated line. @param  hexti Exti handle. @retval None. */
void HAL_EXTI_GenerateSWI(EXTI_HandleTypeDef *hexti)
{
  uint32_t maskline;

        /* Check parameters */
  assert_param(IS_EXTI_LINE(hexti->Line));
  assert_param(IS_EXTI_CONFIG_LINE(hexti->Line));

        /* Compute line mask */
  maskline = (1uL << (hexti->Line & EXTI_PIN_MASK));

        /* Generate Software interrupt */
  EXTI->SWIER = maskline;
}



/* @} */

/* @} */

#endif /* HAL_EXTI_MODULE_ENABLED */
/* @} */

/* @} */

/* (C) COPYRIGHT Puya *****END OF FILE */
