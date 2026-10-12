/**
 * @file    py32f0xx_hal_iwdg.c
 * @author  MCU Application Team
 * @brief   IWDG HAL module driver.
 */

/* Includes ------------------------------------------------------------------ */
#include "py32f0xx_hal.h"

/**
 * @addtogroup PY32F0xx_HAL_Driver @{
 */

#ifdef HAL_IWDG_MODULE_ENABLED
/* @defgroup IWDG IWDG @brief IWDG HAL module driver. @{ */

/* Private typedef ----------------------------------------------------------- */
/* Private define ------------------------------------------------------------ */
/* @defgroup IWDG_Private_Defines IWDG Private Defines @{ */
/* least 6 cycles so 48 ms. */
#define HAL_IWDG_DEFAULT_TIMEOUT            48U
/* @} */

/* Private macro ------------------------------------------------------------- */
/* Private variables --------------------------------------------------------- */
/* Private function prototypes ----------------------------------------------- */
/* Exported functions -------------------------------------------------------- */

/* @addtogroup IWDG_Exported_Functions @{ */



HAL_StatusTypeDef HAL_IWDG_Init(IWDG_HandleTypeDef *hiwdg)
{
  uint32_t tickstart;

        /* Check the IWDG handle allocation */
  if (hiwdg == NULL)
  {
    return HAL_ERROR;
  }

        /* Check the parameters */
  assert_param(IS_IWDG_ALL_INSTANCE(hiwdg->Instance));
  assert_param(IS_IWDG_PRESCALER(hiwdg->Init.Prescaler));
  assert_param(IS_IWDG_RELOAD(hiwdg->Init.Reload));

        /* Enable IWDG. LSI requires ready */
  __HAL_IWDG_START(hiwdg);

        /* Enable write access to IWDG_PR and IWDG_RLR registers by writing 0x5555 in KR */
  IWDG_ENABLE_WRITE_ACCESS(hiwdg);

        /* Write to IWDG registers the Prescaler & Reload values to work with */
  hiwdg->Instance->PR = hiwdg->Init.Prescaler;
  hiwdg->Instance->RLR = hiwdg->Init.Reload;

        /* Check pending flag, if previous update not done, return timeout */
  tickstart = HAL_GetTick();

        /* Wait for register to be updated */
  while (hiwdg->Instance->SR != RESET)
  {
    if ((HAL_GetTick() - tickstart) > HAL_IWDG_DEFAULT_TIMEOUT)
    {
      return HAL_TIMEOUT;
    }
  }
        /* Reload IWDG counter with value defined in the reload register */
  __HAL_IWDG_RELOAD_COUNTER(hiwdg);

        /* Return function status */
  return HAL_OK;
}

/* @} */

/* @addtogroup IWDG_Exported_Functions_Group2 @brief   IO operation functions @verbatim ##### IO operation functions ##### */



HAL_StatusTypeDef HAL_IWDG_Refresh(IWDG_HandleTypeDef *hiwdg)
{
        /* Reload IWDG counter with value defined in the reload register */
  __HAL_IWDG_RELOAD_COUNTER(hiwdg);

        /* Return function status */
  return HAL_OK;
}

/* @} */

/* @} */

#endif /* HAL_IWDG_MODULE_ENABLED */
/* @} */

/* @} */

/* (C) COPYRIGHT Puya *****END OF FILE */
