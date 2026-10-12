/**
 * @file    py32f0xx_hal_led.c
 * @author  MCU Application Team
 * @brief   LED HAL module driver.
 * @attention
 */

/* Includes ------------------------------------------------------------------ */
#include "py32f0xx_hal.h"

/**
 * @addtogroup PY32F0xx_HAL_Driver @{
 */
#ifdef HAL_LED_MODULE_ENABLED

/* @defgroup LED LED @brief LED HAL module driver @{ */

/* Private Functions -------------------------------------------------------- */
/* @defgroup LED_Private_Functions  LED Private Functions @{ */
static void LED_SetConfig(LED_HandleTypeDef *hled);
/* @} */

/* Private Functions -------------------------------------------------------- */
/* @defgroup LED_Private_Functions  LED Private Functions @{ */

static void LED_SetConfig(LED_HandleTypeDef *hled)
{
  uint32_t tmpreg;

  tmpreg=hled->Init.ComDrive | (hled->Init.ComNum << LED_CR_LED_COM_SEL_Pos);
  MODIFY_REG(hled->Instance->CR,
             (uint32_t)(LED_CR_LED_COM_SEL | LED_CR_EHS),
             tmpreg);
  WRITE_REG(hled->Instance->PR, hled->Init.Prescaler);
  WRITE_REG(hled->Instance->TR, (hled->Init.LightTime | (hled->Init.DeadTime<<LED_TR_T2_Pos)));
}
/* @} */



/* Exported Functions -------------------------------------------------------- */
/* @defgroup LED_Exported_Functions LED Exported Functions @{ */

/* @defgroup LED_Exported_Functions_Group1 Initialization/de-initialization functions @brief    Initialization and Configuration functions @{ */

HAL_StatusTypeDef HAL_LED_Init(LED_HandleTypeDef *hled)
{
        /* Check the LED handle allocation */
  if (hled == NULL)
  {
    return HAL_ERROR;
  }
  
        /* Check the parameters */
  assert_param(IS_LED_ALL_INSTANCE(hled->Instance));
  assert_param(IS_LED_COM_DRIVE(hled->Init.ComDrive));
  assert_param(IS_LED_PRISCALER(hled->Init.Prescaler));
  assert_param(IS_LED_COM_NUM(hled->Init.ComNum));
  assert_param(IS_LED_LIGHT_TIME(hled->Init.LightTime));
  assert_param(IS_LED_DEAD_TIME(hled->Init.DeadTime));

  if (hled->State == HAL_LED_STATE_RESET)
  {
                /* Allocate lock resource and initialize it */
    __HAL_UNLOCK(hled);
      
#if (USE_HAL_LED_REGISTER_CALLBACKS == 1)
                /* Reset Callback pointers */
    if (hled->EwiCallback == NULL)
    {
      hled->EwiCallback = HAL_LED_LightCpltCallback;
    }
   
    if (hled->MspInitCallback == NULL)
    {
      hled->MspInitCallback = HAL_LED_MspInit;
    }
   
                /* Init the low level hardware */
    hled->MspInitCallback(hwwdg);
#else
    HAL_LED_MspInit(hled);
#endif
  }
  
  hled->State = HAL_LED_STATE_BUSY;
  
        /* LED Register config */
  LED_SetConfig(hled);
  
        /* Enable the peripheral */
  __HAL_LED_ENABLE(hled);

        /* Initialize the LED state */
  hled->State= HAL_LED_STATE_READY;
  
  return HAL_OK;
}



HAL_StatusTypeDef HAL_LED_DeInit(LED_HandleTypeDef *hled)
{
  if (hled == NULL)
  {
    return HAL_ERROR;
  }

        /* Check the parameters */
  assert_param(IS_LED_ALL_INSTANCE(hled->Instance));

  hled->Instance->CR = 0x00;
  hled->Instance->PR = 0x00;
  hled->Instance->DR0 = 0x00;
  hled->Instance->DR1 = 0x00;
  hled->Instance->DR2 = 0x00;
  hled->Instance->DR3 = 0x00;
  hled->Instance->IR = 0x01;

  hled->State = HAL_LED_STATE_RESET;
  
        /* Release Lock */
  __HAL_UNLOCK(hled);
  
  return HAL_OK;
}
/* @} */

/* @defgroup LED_Exported_Functions_Group2 LED Operation Functions @brief    LED operation functions @{ */

HAL_StatusTypeDef HAL_LED_SetComDisplay(LED_HandleTypeDef *hled, uint8_t comCh, uint8_t data)
{
  uint32_t position=0x00U;
  uint32_t chcurrent = 0x00U;
  uint32_t *pTmp;

  if (hled->State == HAL_LED_STATE_READY)
  {
                /* Process Locked */
    __HAL_LOCK(hled);
    hled->State = HAL_LED_STATE_BUSY;
      
    while ((comCh >> position) != 0)
    {
                        /* Get the COM channel position */
      chcurrent = comCh & (1U << position);
    
      if(chcurrent)
      {
        pTmp = ((uint32_t *)&hled->Instance->DR0) + position;
        WRITE_REG(*pTmp, data);
      }
    
      position++;
    }
    
                /* Process Unlocked */
    __HAL_UNLOCK(hled);
    hled->State = HAL_LED_STATE_READY;
    
    return HAL_OK;
  }
  else
  {
    return HAL_ERROR;
  }  
}

/* @brief Handle LED interrupt request. @param hled LED handle. @retval None */
void HAL_LED_IRQHandler(LED_HandleTypeDef *hled)
{
        /* LED interrupt occurred ------------------------------------- */
  if (((hled->Instance->IR & LED_IR_FLAG) != 0U) && ((hled->Instance->CR & LED_CR_IE) != 0U))
  {
    __HAL_LED_CLEAR_FLAG(hled, LED_IR_FLAG);

    HAL_LED_LightCpltCallback(hled);
  }
}

__weak void HAL_LED_MspInit(LED_HandleTypeDef *hled)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(hled);



}

__weak void HAL_LED_LightCpltCallback(LED_HandleTypeDef *hled)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(hled);



}
/* @} */

/* @} */



/* @} */

#endif /* HAL_LED_MODULE_ENABLED */
/* @} */

