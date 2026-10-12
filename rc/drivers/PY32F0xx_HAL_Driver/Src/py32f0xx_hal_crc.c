/**
 * @file    py32f0xx_hal_crc.c
 * @author  MCU Application Team
 * @brief   CRC HAL module driver.
 */

/* Includes ------------------------------------------------------------------ */
#include "py32f0xx_hal.h"

/* @addtogroup PY32F0xx_HAL_Driver @{ */

/* @defgroup CRC CRC @brief    CRC HAL module driver. @{ */

#ifdef HAL_CRC_MODULE_ENABLED

/* Private typedef ----------------------------------------------------------- */
/* Private define ------------------------------------------------------------ */
/* Private macro ------------------------------------------------------------- */
/* Private variables --------------------------------------------------------- */
/* Private function prototypes ----------------------------------------------- */

/* Exported functions -------------------------------------------------------- */

/* @defgroup CRC_Exported_Functions CRC Exported Functions @{ */



/* @brief  Initialize the CRC according to the specified @param  hcrc CRC handle @retval HAL status */
HAL_StatusTypeDef HAL_CRC_Init(CRC_HandleTypeDef *hcrc)
{
        /* Check the CRC handle allocation */
  if (hcrc == NULL)
  {
    return HAL_ERROR;
  }

        /* Check the parameters */
  assert_param(IS_CRC_ALL_INSTANCE(hcrc->Instance));

  if (hcrc->State == HAL_CRC_STATE_RESET)
  {
                /* Allocate lock resource and initialize it */
    hcrc->Lock = HAL_UNLOCKED;
                /* Init the low level hardware */
    HAL_CRC_MspInit(hcrc);
  }

        /* Change CRC peripheral state */
  hcrc->State = HAL_CRC_STATE_READY;

        /* Return function status */
  return HAL_OK;
}

/* @brief  DeInitialize the CRC peripheral. @param  hcrc CRC handle @retval HAL status */
HAL_StatusTypeDef HAL_CRC_DeInit(CRC_HandleTypeDef *hcrc)
{
        /* Check the CRC handle allocation */
  if (hcrc == NULL)
  {
    return HAL_ERROR;
  }

        /* Check the parameters */
  assert_param(IS_CRC_ALL_INSTANCE(hcrc->Instance));

        /* Check the CRC peripheral state */
  if (hcrc->State == HAL_CRC_STATE_BUSY)
  {
    return HAL_BUSY;
  }

        /* Change CRC peripheral state */
  hcrc->State = HAL_CRC_STATE_BUSY;

        /* Reset CRC calculation unit */
  __HAL_CRC_DR_RESET(hcrc);

        /* Reset IDR register content */
  CLEAR_BIT(hcrc->Instance->IDR, CRC_IDR_IDR);

        /* DeInit the low level hardware */
  HAL_CRC_MspDeInit(hcrc);

        /* Change CRC peripheral state */
  hcrc->State = HAL_CRC_STATE_RESET;

        /* Process unlocked */
  __HAL_UNLOCK(hcrc);

        /* Return function status */
  return HAL_OK;
}

/* @brief  Initializes the CRC MSP. @param  hcrc CRC handle @retval None */
__weak void HAL_CRC_MspInit(CRC_HandleTypeDef *hcrc)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(hcrc);



}

/* @brief  DeInitialize the CRC MSP. @param  hcrc CRC handle @retval None */
__weak void HAL_CRC_MspDeInit(CRC_HandleTypeDef *hcrc)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(hcrc);



}

/* @} */



uint32_t HAL_CRC_Accumulate(CRC_HandleTypeDef *hcrc, uint32_t pBuffer[], uint32_t BufferLength)
{
  uint32_t index;          /* CRC input data buffer index */
  uint32_t temp = 0U;      /* CRC output (read from hcrc->Instance->DR register) */

        /* Change CRC peripheral state */
  hcrc->State = HAL_CRC_STATE_BUSY;

        /* Enter Data to the CRC calculator */
  for (index = 0U; index < BufferLength; index++)
  {
    hcrc->Instance->DR = pBuffer[index];
  }
  temp = hcrc->Instance->DR;

        /* Change CRC peripheral state */
  hcrc->State = HAL_CRC_STATE_READY;

        /* Return the CRC computed value */
  return temp;
}



uint32_t HAL_CRC_Calculate(CRC_HandleTypeDef *hcrc, uint32_t pBuffer[], uint32_t BufferLength)
{
  uint32_t index;          /* CRC input data buffer index */
  uint32_t temp = 0U;      /* CRC output (read from hcrc->Instance->DR register) */

        /* Change CRC peripheral state */
  hcrc->State = HAL_CRC_STATE_BUSY;

        /* Reset CRC Calculation Unit (hcrc->Instance->INIT is written in hcrc->Instance->DR) */
  __HAL_CRC_DR_RESET(hcrc);

        /* Enter 32-bit input data to the CRC calculator */
  for (index = 0U; index < BufferLength; index++)
  {
    hcrc->Instance->DR = pBuffer[index];
  }
  temp = hcrc->Instance->DR;

        /* Change CRC peripheral state */
  hcrc->State = HAL_CRC_STATE_READY;

        /* Return the CRC computed value */
  return temp;
}

/* @} */



/* @brief  Return the CRC handle state. @param  hcrc CRC handle @retval HAL state */
HAL_CRC_StateTypeDef HAL_CRC_GetState(CRC_HandleTypeDef *hcrc)
{
        /* Return CRC handle state */
  return hcrc->State;
}

/* @} */

/* @} */



#endif /* HAL_CRC_MODULE_ENABLED */
/* @} */

/* @} */

/* (C) COPYRIGHT Puya *****END OF FILE */
