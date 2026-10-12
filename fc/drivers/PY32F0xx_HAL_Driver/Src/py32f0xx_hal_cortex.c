/**
 * @file    py32f0xx_hal_cortex.c
 * @author  MCU Application Team
 * @brief   CORTEX HAL module driver.
 */

/* Includes ------------------------------------------------------------------ */
#include "py32f0xx_hal.h"

/**
 * @addtogroup PY32F0xx_HAL_Driver @{
 */

/* @addtogroup CORTEX @{ */

#ifdef HAL_CORTEX_MODULE_ENABLED

/* Private types ------------------------------------------------------------- */
/* Private variables --------------------------------------------------------- */
/* Private constants --------------------------------------------------------- */
/* Private macros ------------------------------------------------------------ */
/* Private function prototypes ----------------------------------------------- */
/* Exported functions -------------------------------------------------------- */

/* @addtogroup CORTEX_Exported_Functions @{ */



void HAL_NVIC_SetPriority(IRQn_Type IRQn, uint32_t PreemptPriority, uint32_t SubPriority)
{
        /* Check the parameters */
  assert_param(IS_NVIC_PREEMPTION_PRIORITY(PreemptPriority));
  NVIC_SetPriority(IRQn,PreemptPriority);
}



void HAL_NVIC_EnableIRQ(IRQn_Type IRQn)
{
        /* Check the parameters */
  assert_param(IS_NVIC_DEVICE_IRQ(IRQn));

        /* Enable interrupt */
  NVIC_EnableIRQ(IRQn);
}



void HAL_NVIC_DisableIRQ(IRQn_Type IRQn)
{
        /* Check the parameters */
  assert_param(IS_NVIC_DEVICE_IRQ(IRQn));

        /* Disable interrupt */
  NVIC_DisableIRQ(IRQn);
}

/* @brief  Initiate a system reset request to reset the MCU. @retval None */
void HAL_NVIC_SystemReset(void)
{
        /* System Reset */
  NVIC_SystemReset();
}



uint32_t HAL_SYSTICK_Config(uint32_t TicksNumb)
{
  return SysTick_Config(TicksNumb);
}
/* @} */

/* @addtogroup CORTEX_Exported_Functions_Group2 @brief   Cortex control functions @verbatim ##### Peripheral Control functions ##### */



uint32_t HAL_NVIC_GetPriority(IRQn_Type IRQn)
{
        /* Get priority for Cortex-M system or device specific interrupts */
  return NVIC_GetPriority(IRQn);
}



void HAL_NVIC_SetPendingIRQ(IRQn_Type IRQn)
{
        /* Check the parameters */
  assert_param(IS_NVIC_DEVICE_IRQ(IRQn));

        /* Set interrupt pending */
  NVIC_SetPendingIRQ(IRQn);
}



uint32_t HAL_NVIC_GetPendingIRQ(IRQn_Type IRQn)
{
        /* Check the parameters */
  assert_param(IS_NVIC_DEVICE_IRQ(IRQn));

        /* Return 1 if pending else 0 */
  return NVIC_GetPendingIRQ(IRQn);
}



void HAL_NVIC_ClearPendingIRQ(IRQn_Type IRQn)
{
        /* Check the parameters */
  assert_param(IS_NVIC_DEVICE_IRQ(IRQn));

        /* Clear pending interrupt */
  NVIC_ClearPendingIRQ(IRQn);
}



void HAL_SYSTICK_CLKSourceConfig(uint32_t CLKSource)
{
        /* Check the parameters */
  assert_param(IS_SYSTICK_CLK_SOURCE(CLKSource));
  if (CLKSource == SYSTICK_CLKSOURCE_HCLK)
  {
    SysTick->CTRL |= SYSTICK_CLKSOURCE_HCLK;
  }
  else
  {
    SysTick->CTRL &= ~SYSTICK_CLKSOURCE_HCLK;
  }
}

/* @brief  Handle SYSTICK interrupt request. @retval None */
void HAL_SYSTICK_IRQHandler(void)
{
  HAL_SYSTICK_Callback();
}

/* @brief  SYSTICK callback. @retval None */
__weak void HAL_SYSTICK_Callback(void)
{

}



/* @} */

/* @} */

#endif /* HAL_CORTEX_MODULE_ENABLED */
/* @} */

/* @} */

/* (C) COPYRIGHT Puya *****END OF FILE */
