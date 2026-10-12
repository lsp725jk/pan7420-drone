/**
 * @file    py32f0xx_hal_uart.c
 * @author  MCU Application Team
 * @brief   UART HAL module driver.
 */

/* Includes ------------------------------------------------------------------ */
#include "py32f0xx_hal.h"

/**
 * @addtogroup PY32F0xx_HAL_Driver @{
 */

/* @defgroup UART UART @brief HAL UART module driver @{ */
#ifdef HAL_UART_MODULE_ENABLED

/* Private typedef ----------------------------------------------------------- */
/* Private define ------------------------------------------------------------ */
/* @addtogroup UART_Private_Constants @{ */
/* @} */
/* Private macro ------------------------------------------------------------- */
/* Private variables --------------------------------------------------------- */
/* Private function prototypes ----------------------------------------------- */
/* @addtogroup UART_Private_Functions  UART Private Functions @{ */

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
void UART_InitCallbacksToDefault(UART_HandleTypeDef *huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
static void UART_EndRxTransfer(UART_HandleTypeDef *huart);
#if (defined(DMA1) || defined(DMA))
static void UART_EndTxTransfer(UART_HandleTypeDef *huart);
static void UART_DMATransmitCplt(DMA_HandleTypeDef *hdma);
static void UART_DMAReceiveCplt(DMA_HandleTypeDef *hdma);
static void UART_DMATxHalfCplt(DMA_HandleTypeDef *hdma);
static void UART_DMARxHalfCplt(DMA_HandleTypeDef *hdma);
static void UART_DMAError(DMA_HandleTypeDef *hdma);
static void UART_DMAAbortOnError(DMA_HandleTypeDef *hdma);
static void UART_DMATxAbortCallback(DMA_HandleTypeDef *hdma);
static void UART_DMARxAbortCallback(DMA_HandleTypeDef *hdma);
static void UART_DMATxOnlyAbortCallback(DMA_HandleTypeDef *hdma);
static void UART_DMARxOnlyAbortCallback(DMA_HandleTypeDef *hdma);
#endif /* DMA1 or DMA */
static HAL_StatusTypeDef UART_Transmit_IT(UART_HandleTypeDef *huart);
static HAL_StatusTypeDef UART_EndTransmit_IT(UART_HandleTypeDef *huart);
static HAL_StatusTypeDef UART_Receive_IT(UART_HandleTypeDef *huart);
static HAL_StatusTypeDef UART_WaitOnFlagUntilTimeout(UART_HandleTypeDef *huart, uint32_t Flag, FlagStatus Status, uint32_t Tickstart, uint32_t Timeout);
static void UART_SetConfig(UART_HandleTypeDef *huart);
static void UART_AdvFeatureConfig(UART_HandleTypeDef *huart);

/* @} */

/* Exported functions --------------------------------------------------------- */
/* @defgroup UART_Exported_Functions UART Exported Functions @{ */



HAL_StatusTypeDef HAL_UART_Init(UART_HandleTypeDef *huart)
{
        /* Check the UART handle allocation */
  if (huart == NULL)
  {
    return HAL_ERROR;
  }

        /* Check the parameters */
  if (huart->Init.HwFlowCtl != UART_HWCONTROL_NONE)
  {
                /* The hardware flow control is available only for USART1 and USART2 */
    assert_param(IS_UART_HWFLOW_INSTANCE(huart->Instance));
    assert_param(IS_UART_HARDWARE_FLOW_CONTROL(huart->Init.HwFlowCtl));
  }
  else
  {
    assert_param(IS_UART_INSTANCE(huart->Instance));
  }
  assert_param(IS_UART_WORD_LENGTH(huart->Init.WordLength));
#if defined(USART_CR3_OVER8)
  assert_param(IS_UART_OVERSAMPLING(huart->Init.OverSampling));
#endif /* USART_CR3_OVER8 */

  if (huart->gState == HAL_UART_STATE_RESET)
  {
                /* Allocate lock resource and initialize it */
    huart->Lock = HAL_UNLOCKED;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
    UART_InitCallbacksToDefault(huart);

    if (huart->MspInitCallback == NULL)
    {
      huart->MspInitCallback = HAL_UART_MspInit;
    }

                /* Init the low level hardware */
    huart->MspInitCallback(huart);
#else
                /* Init the low level hardware : GPIO, CLOCK */
    HAL_UART_MspInit(huart);
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS) */
  }

  huart->gState = HAL_UART_STATE_BUSY;

        /* Disable the peripheral */
  __HAL_UART_DISABLE(huart);

        /* Set the UART Communication parameters */
  UART_SetConfig(huart);

  if (huart->AdvancedInit.AdvFeatureInit != UART_ADVFEATURE_NO_INIT)
  {
    UART_AdvFeatureConfig(huart);
  }
        /* - CLKEN bit in the USART_CR2 register, - HDSEL bit in the USART_CR3 register. */
  CLEAR_BIT(huart->Instance->CR2, USART_CR2_CLKEN);
  CLEAR_BIT(huart->Instance->CR3, USART_CR3_HDSEL);

        /* Enable the peripheral */
  __HAL_UART_ENABLE(huart);

        /* Initialize the UART state */
  huart->ErrorCode = HAL_UART_ERROR_NONE;
  huart->gState = HAL_UART_STATE_READY;
  huart->RxState = HAL_UART_STATE_READY;

  return HAL_OK;
}



HAL_StatusTypeDef HAL_HalfDuplex_Init(UART_HandleTypeDef *huart)
{
        /* Check the UART handle allocation */
  if (huart == NULL)
  {
    return HAL_ERROR;
  }

        /* Check the parameters */
  assert_param(IS_UART_HALFDUPLEX_INSTANCE(huart->Instance));
  assert_param(IS_UART_WORD_LENGTH(huart->Init.WordLength));
#if defined(USART_CR3_OVER8)
  assert_param(IS_UART_OVERSAMPLING(huart->Init.OverSampling));
#endif /* USART_CR3_OVER8 */

  if (huart->gState == HAL_UART_STATE_RESET)
  {
                /* Allocate lock resource and initialize it */
    huart->Lock = HAL_UNLOCKED;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
    UART_InitCallbacksToDefault(huart);

    if (huart->MspInitCallback == NULL)
    {
      huart->MspInitCallback = HAL_UART_MspInit;
    }

                /* Init the low level hardware */
    huart->MspInitCallback(huart);
#else
                /* Init the low level hardware : GPIO, CLOCK */
    HAL_UART_MspInit(huart);
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS) */
  }

  huart->gState = HAL_UART_STATE_BUSY;

        /* Disable the peripheral */
  __HAL_UART_DISABLE(huart);

        /* Set the UART Communication parameters */
  UART_SetConfig(huart);



  CLEAR_BIT(huart->Instance->CR2, USART_CR2_CLKEN);

        /* Enable the Half-Duplex mode by setting the HDSEL bit in the CR3 register */
  SET_BIT(huart->Instance->CR3, USART_CR3_HDSEL);

        /* Enable the peripheral */
  __HAL_UART_ENABLE(huart);

        /* Initialize the UART state */
  huart->ErrorCode = HAL_UART_ERROR_NONE;
  huart->gState = HAL_UART_STATE_READY;
  huart->RxState = HAL_UART_STATE_READY;

  return HAL_OK;
}



HAL_StatusTypeDef HAL_MultiProcessor_Init(UART_HandleTypeDef *huart, uint8_t Address, uint32_t WakeUpMethod)
{
        /* Check the UART handle allocation */
  if (huart == NULL)
  {
    return HAL_ERROR;
  }

        /* Check the parameters */
  assert_param(IS_UART_INSTANCE(huart->Instance));

        /* Check the Address & wake up method parameters */
  assert_param(IS_UART_WAKEUPMETHOD(WakeUpMethod));
  assert_param(IS_UART_ADDRESS(Address));
  assert_param(IS_UART_WORD_LENGTH(huart->Init.WordLength));
#if defined(USART_CR3_OVER8)
  assert_param(IS_UART_OVERSAMPLING(huart->Init.OverSampling));
#endif /* USART_CR3_OVER8 */

  if (huart->gState == HAL_UART_STATE_RESET)
  {
                /* Allocate lock resource and initialize it */
    huart->Lock = HAL_UNLOCKED;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
    UART_InitCallbacksToDefault(huart);

    if (huart->MspInitCallback == NULL)
    {
      huart->MspInitCallback = HAL_UART_MspInit;
    }

                /* Init the low level hardware */
    huart->MspInitCallback(huart);
#else
                /* Init the low level hardware : GPIO, CLOCK */
    HAL_UART_MspInit(huart);
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS) */
  }

  huart->gState = HAL_UART_STATE_BUSY;

        /* Disable the peripheral */
  __HAL_UART_DISABLE(huart);

        /* Set the UART Communication parameters */
  UART_SetConfig(huart);

    /* - LINEN and CLKEN bits in the USART_CR2 register, - SCEN, HDSEL and IREN  bits in the USART_CR3 register */
  CLEAR_BIT(huart->Instance->CR2, USART_CR2_CLKEN);
  CLEAR_BIT(huart->Instance->CR3, USART_CR3_HDSEL);

        /* Set the USART address node */
  CLEAR_BIT(huart->Instance->CR2, USART_CR2_ADD);
  SET_BIT(huart->Instance->CR2, Address);

        /* Set the wake up method by setting the WAKE bit in the CR1 register */
  CLEAR_BIT(huart->Instance->CR1, USART_CR1_WAKE);
  SET_BIT(huart->Instance->CR1, WakeUpMethod);

        /* Enable the peripheral */
  __HAL_UART_ENABLE(huart);

        /* Initialize the UART state */
  huart->ErrorCode = HAL_UART_ERROR_NONE;
  huart->gState = HAL_UART_STATE_READY;
  huart->RxState = HAL_UART_STATE_READY;

  return HAL_OK;
}



HAL_StatusTypeDef HAL_UART_DeInit(UART_HandleTypeDef *huart)
{
        /* Check the UART handle allocation */
  if (huart == NULL)
  {
    return HAL_ERROR;
  }

        /* Check the parameters */
  assert_param(IS_UART_INSTANCE(huart->Instance));

  huart->gState = HAL_UART_STATE_BUSY;

        /* Disable the Peripheral */
  __HAL_UART_DISABLE(huart);

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
  if (huart->MspDeInitCallback == NULL)
  {
    huart->MspDeInitCallback = HAL_UART_MspDeInit;
  }
        /* DeInit the low level hardware */
  huart->MspDeInitCallback(huart);
#else
        /* DeInit the low level hardware */
  HAL_UART_MspDeInit(huart);
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS) */

  huart->ErrorCode = HAL_UART_ERROR_NONE;
  huart->gState = HAL_UART_STATE_RESET;
  huart->RxState = HAL_UART_STATE_RESET;

        /* Process Unlock */
  __HAL_UNLOCK(huart);

  return HAL_OK;
}



__weak void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);

}



__weak void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);

}

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)

HAL_StatusTypeDef HAL_UART_RegisterCallback(UART_HandleTypeDef *huart, HAL_UART_CallbackIDTypeDef CallbackID, pUART_CallbackTypeDef pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (pCallback == NULL)
  {
                /* Update the error code */
    huart->ErrorCode |= HAL_UART_ERROR_INVALID_CALLBACK;

    return HAL_ERROR;
  }
        /* Process locked */
  __HAL_LOCK(huart);

  if (huart->gState == HAL_UART_STATE_READY)
  {
    switch (CallbackID)
    {
    case HAL_UART_TX_HALFCOMPLETE_CB_ID :
      huart->TxHalfCpltCallback = pCallback;
      break;

    case HAL_UART_TX_COMPLETE_CB_ID :
      huart->TxCpltCallback = pCallback;
      break;

    case HAL_UART_RX_HALFCOMPLETE_CB_ID :
      huart->RxHalfCpltCallback = pCallback;
      break;

    case HAL_UART_RX_COMPLETE_CB_ID :
      huart->RxCpltCallback = pCallback;
      break;

    case HAL_UART_ERROR_CB_ID :
      huart->ErrorCallback = pCallback;
      break;

    case HAL_UART_ABORT_COMPLETE_CB_ID :
      huart->AbortCpltCallback = pCallback;
      break;

    case HAL_UART_ABORT_TRANSMIT_COMPLETE_CB_ID :
      huart->AbortTransmitCpltCallback = pCallback;
      break;

    case HAL_UART_ABORT_RECEIVE_COMPLETE_CB_ID :
      huart->AbortReceiveCpltCallback = pCallback;
      break;

    case HAL_UART_MSPINIT_CB_ID :
      huart->MspInitCallback = pCallback;
      break;

    case HAL_UART_MSPDEINIT_CB_ID :
      huart->MspDeInitCallback = pCallback;
      break;

    default :
                        /* Update the error code */
      huart->ErrorCode |= HAL_UART_ERROR_INVALID_CALLBACK;

                        /* Return error status */
      status =  HAL_ERROR;
      break;
    }
  }
  else if (huart->gState == HAL_UART_STATE_RESET)
  {
    switch (CallbackID)
    {
    case HAL_UART_MSPINIT_CB_ID :
      huart->MspInitCallback = pCallback;
      break;

    case HAL_UART_MSPDEINIT_CB_ID :
      huart->MspDeInitCallback = pCallback;
      break;

    default :
                        /* Update the error code */
      huart->ErrorCode |= HAL_UART_ERROR_INVALID_CALLBACK;

                        /* Return error status */
      status =  HAL_ERROR;
      break;
    }
  }
  else
  {
                /* Update the error code */
    huart->ErrorCode |= HAL_UART_ERROR_INVALID_CALLBACK;

                /* Return error status */
    status =  HAL_ERROR;
  }

        /* Release Lock */
  __HAL_UNLOCK(huart);

  return status;
}



HAL_StatusTypeDef HAL_UART_UnRegisterCallback(UART_HandleTypeDef *huart, HAL_UART_CallbackIDTypeDef CallbackID)
{
  HAL_StatusTypeDef status = HAL_OK;

        /* Process locked */
  __HAL_LOCK(huart);

  if (HAL_UART_STATE_READY == huart->gState)
  {
    switch (CallbackID)
    {
    case HAL_UART_TX_HALFCOMPLETE_CB_ID :
      huart->TxHalfCpltCallback = HAL_UART_TxHalfCpltCallback;                           /* Legacy weak  TxHalfCpltCallback */
      break;

    case HAL_UART_TX_COMPLETE_CB_ID :
      huart->TxCpltCallback = HAL_UART_TxCpltCallback;                                   /* Legacy weak TxCpltCallback */
      break;

    case HAL_UART_RX_HALFCOMPLETE_CB_ID :
      huart->RxHalfCpltCallback = HAL_UART_RxHalfCpltCallback;                           /* Legacy weak RxHalfCpltCallback */
      break;

    case HAL_UART_RX_COMPLETE_CB_ID :
      huart->RxCpltCallback = HAL_UART_RxCpltCallback;                                   /* Legacy weak RxCpltCallback */
      break;

    case HAL_UART_ERROR_CB_ID :
      huart->ErrorCallback = HAL_UART_ErrorCallback;                                     /* Legacy weak ErrorCallback */
      break;

    case HAL_UART_ABORT_COMPLETE_CB_ID :
      huart->AbortCpltCallback = HAL_UART_AbortCpltCallback;                             /* Legacy weak AbortCpltCallback */
      break;

    case HAL_UART_ABORT_TRANSMIT_COMPLETE_CB_ID :
      huart->AbortTransmitCpltCallback = HAL_UART_AbortTransmitCpltCallback;             /* Legacy weak AbortTransmitCpltCallback */
      break;

    case HAL_UART_ABORT_RECEIVE_COMPLETE_CB_ID :
      huart->AbortReceiveCpltCallback = HAL_UART_AbortReceiveCpltCallback;               /* Legacy weak AbortReceiveCpltCallback */
      break;

    case HAL_UART_MSPINIT_CB_ID :
      huart->MspInitCallback = HAL_UART_MspInit;                                         /* Legacy weak MspInitCallback */
      break;

    case HAL_UART_MSPDEINIT_CB_ID :
      huart->MspDeInitCallback = HAL_UART_MspDeInit;                                     /* Legacy weak MspDeInitCallback */
      break;

    default :
                        /* Update the error code */
      huart->ErrorCode |= HAL_UART_ERROR_INVALID_CALLBACK;

                        /* Return error status */
      status =  HAL_ERROR;
      break;
    }
  }
  else if (HAL_UART_STATE_RESET == huart->gState)
  {
    switch (CallbackID)
    {
    case HAL_UART_MSPINIT_CB_ID :
      huart->MspInitCallback = HAL_UART_MspInit;
      break;

    case HAL_UART_MSPDEINIT_CB_ID :
      huart->MspDeInitCallback = HAL_UART_MspDeInit;
      break;

    default :
                        /* Update the error code */
      huart->ErrorCode |= HAL_UART_ERROR_INVALID_CALLBACK;

                        /* Return error status */
      status =  HAL_ERROR;
      break;
    }
  }
  else
  {
                /* Update the error code */
    huart->ErrorCode |= HAL_UART_ERROR_INVALID_CALLBACK;

                /* Return error status */
    status =  HAL_ERROR;
  }

        /* Release Lock */
  __HAL_UNLOCK(huart);

  return status;
}
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */

/* @} */



HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  uint8_t  *pdata8bits;
  uint16_t *pdata16bits;
  uint32_t tickstart = 0U;

        /* Check that a Tx process is not already ongoing */
  if (huart->gState == HAL_UART_STATE_READY)
  {
    if ((pData == NULL) || (Size == 0U))
    {
      return  HAL_ERROR;
    }
                /* handled through a u16 cast. */
    if ((huart->Init.WordLength == UART_WORDLENGTH_9B) && (huart->Init.Parity == UART_PARITY_NONE))
    {
      if ((((uint32_t)pData) & 1U) != 0U)
      {
        return  HAL_ERROR;
      }
    }
                /* Process Locked */
    __HAL_LOCK(huart);

    huart->ErrorCode = HAL_UART_ERROR_NONE;
    huart->gState = HAL_UART_STATE_BUSY_TX;

                /* Init tickstart for timeout managment */
    tickstart = HAL_GetTick();

    huart->TxXferSize = Size;
    huart->TxXferCount = Size;

                /* In case of 9bits/No Parity transfer, pData needs to be handled as a uint16_t pointer */
    if ((huart->Init.WordLength == UART_WORDLENGTH_9B) && (huart->Init.Parity == UART_PARITY_NONE))
    {
      pdata8bits  = NULL;
      pdata16bits = (uint16_t *) pData;
    }
    else
    {
      pdata8bits  = pData;
      pdata16bits = NULL;
    }



    while (huart->TxXferCount > 0U)
    {

      if (UART_WaitOnFlagUntilTimeout(huart, UART_FLAG_TXE, RESET, tickstart, Timeout) != HAL_OK)
      {
        return HAL_TIMEOUT;
      }
      if (pdata8bits == NULL)
      {
        huart->Instance->DR = (uint16_t)(*pdata16bits & 0x01FFU);
        pdata16bits++;
      }
      else
      {
        huart->Instance->DR = (uint8_t)(*pdata8bits & 0xFFU);
        pdata8bits++;
      }

      huart->TxXferCount--;
    }

    if (UART_WaitOnFlagUntilTimeout(huart, UART_FLAG_TC, RESET, tickstart, Timeout) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }

                /* At end of Tx process, restore huart->gState to Ready */
    huart->gState = HAL_UART_STATE_READY;

                /* Process Unlocked */
    __HAL_UNLOCK(huart);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}



HAL_StatusTypeDef HAL_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  uint8_t  *pdata8bits;
  uint16_t *pdata16bits;
  uint32_t tickstart = 0U;

        /* Check that a Rx process is not already ongoing */
  if (huart->RxState == HAL_UART_STATE_READY)
  {
    if ((pData == NULL) || (Size == 0U))
    {
      return  HAL_ERROR;
    }
                /* handled through a u16 cast. */
    if ((huart->Init.WordLength == UART_WORDLENGTH_9B) && (huart->Init.Parity == UART_PARITY_NONE))
    {
      if ((((uint32_t)pData) & 1U) != 0U)
      {
        return  HAL_ERROR;
      }
    }
                /* Process Locked */
    __HAL_LOCK(huart);

    huart->ErrorCode = HAL_UART_ERROR_NONE;
    huart->RxState = HAL_UART_STATE_BUSY_RX;

                /* Init tickstart for timeout managment */
    tickstart = HAL_GetTick();

    huart->RxXferSize = Size;
    huart->RxXferCount = Size;

                /* In case of 9bits/No Parity transfer, pRxData needs to be handled as a uint16_t pointer */
    if ((huart->Init.WordLength == UART_WORDLENGTH_9B) && (huart->Init.Parity == UART_PARITY_NONE))
    {
      pdata8bits  = NULL;
      pdata16bits = (uint16_t *) pData;
    }
    else
    {
      pdata8bits  = pData;
      pdata16bits = NULL;
    }
                /* as long as data have to be received */
    while (huart->RxXferCount > 0U)
    {
      if (UART_WaitOnFlagUntilTimeout(huart, UART_FLAG_RXNE, RESET, tickstart, Timeout) != HAL_OK)
      {
        return HAL_TIMEOUT;
      }
      if (pdata8bits == NULL)
      {
        *pdata16bits = (uint16_t)(huart->Instance->DR & 0x1FF);
        pdata16bits++;
      }
      else
      {
        *pdata8bits = (uint8_t)(huart->Instance->DR & (uint8_t)0xFF);
        pdata8bits++;
      }
      huart->RxXferCount--;
    }
                /* At end of Rx process, restore huart->RxState to Ready */
    huart->RxState = HAL_UART_STATE_READY;

                /* Process Unlocked */
    __HAL_UNLOCK(huart);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}



HAL_StatusTypeDef HAL_UART_Transmit_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
        /* Check that a Tx process is not already ongoing */
  if (huart->gState == HAL_UART_STATE_READY)
  {
    if ((pData == NULL) || (Size == 0U))
    {
      return HAL_ERROR;
    }

                /* Process Locked */
    __HAL_LOCK(huart);

    huart->pTxBuffPtr = pData;
    huart->TxXferSize = Size;
    huart->TxXferCount = Size;

    huart->ErrorCode = HAL_UART_ERROR_NONE;
    huart->gState = HAL_UART_STATE_BUSY_TX;

                /* Process Unlocked */
    __HAL_UNLOCK(huart);

                /* Enable the UART Transmit data register empty Interrupt */
    __HAL_UART_ENABLE_IT(huart, UART_IT_TXE);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}



HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
        /* Check that a Rx process is not already ongoing */
  if (huart->RxState == HAL_UART_STATE_READY)
  {
    if ((pData == NULL) || (Size == 0U))
    {
      return HAL_ERROR;
    }

                /* Process Locked */
    __HAL_LOCK(huart);

    huart->pRxBuffPtr = pData;
    huart->RxXferSize = Size;
    huart->RxXferCount = Size;

    huart->ErrorCode = HAL_UART_ERROR_NONE;
    huart->RxState = HAL_UART_STATE_BUSY_RX;

                /* Process Unlocked */
    __HAL_UNLOCK(huart);

                /* Enable the UART Parity Error Interrupt */
    __HAL_UART_ENABLE_IT(huart, UART_IT_PE);

                /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
    __HAL_UART_ENABLE_IT(huart, UART_IT_ERR);

                /* Enable the UART Data Register not empty Interrupt */
    __HAL_UART_ENABLE_IT(huart, UART_IT_RXNE);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

#if (defined(DMA1) || defined(DMA))

HAL_StatusTypeDef HAL_UART_Transmit_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
  uint32_t *tmp;

        /* Check that a Tx process is not already ongoing */
  if (huart->gState == HAL_UART_STATE_READY)
  {
    if ((pData == NULL) || (Size == 0U))
    {
      return HAL_ERROR;
    }

                /* Process Locked */
    __HAL_LOCK(huart);

    huart->pTxBuffPtr = pData;
    huart->TxXferSize = Size;
    huart->TxXferCount = Size;

    huart->ErrorCode = HAL_UART_ERROR_NONE;
    huart->gState = HAL_UART_STATE_BUSY_TX;

                /* Set the UART DMA transfer complete callback */
    huart->hdmatx->XferCpltCallback = UART_DMATransmitCplt;

                /* Set the UART DMA Half transfer complete callback */
    huart->hdmatx->XferHalfCpltCallback = UART_DMATxHalfCplt;

                /* Set the DMA error callback */
    huart->hdmatx->XferErrorCallback = UART_DMAError;

                /* Set the DMA abort callback */
    huart->hdmatx->XferAbortCallback = NULL;

                /* Enable the UART transmit DMA channel */
    tmp = (uint32_t *)&pData;
    HAL_DMA_Start_IT(huart->hdmatx, *(uint32_t *)tmp, (uint32_t)&huart->Instance->DR, Size);

                /* Clear the TC flag in the SR register by writing 0 to it */
    __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_TC);

                /* Process Unlocked */
    __HAL_UNLOCK(huart);



    SET_BIT(huart->Instance->CR3, USART_CR3_DMAT);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}



HAL_StatusTypeDef HAL_UART_Receive_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{

        /* Check that a Rx process is not already ongoing */
  if (huart->RxState == HAL_UART_STATE_READY)
  {
    if ((pData == NULL) || (Size == 0U))
    {
      return HAL_ERROR;
    }
                /* handled by DMA from a u16 frontier. */
    if ((huart->Init.WordLength == UART_WORDLENGTH_9B) && (huart->Init.Parity == UART_PARITY_NONE))
    {
      if ((((uint32_t)pData) & 1U) != 0U)
      {
        return  HAL_ERROR;
      }
    }
                /* Process Locked */
    __HAL_LOCK(huart);

    huart->pRxBuffPtr = pData;
    huart->RxXferSize = Size;

    huart->ErrorCode = HAL_UART_ERROR_NONE;
    huart->RxState = HAL_UART_STATE_BUSY_RX;

                /* Set the UART DMA transfer complete callback */
    huart->hdmarx->XferCpltCallback = UART_DMAReceiveCplt;

                /* Set the UART DMA Half transfer complete callback */
    huart->hdmarx->XferHalfCpltCallback = UART_DMARxHalfCplt;

                /* Set the DMA error callback */
    huart->hdmarx->XferErrorCallback = UART_DMAError;

                /* Set the DMA abort callback */
    huart->hdmarx->XferAbortCallback = NULL;

                /* Enable the DMA channel */
    HAL_DMA_Start_IT(huart->hdmarx, (uint32_t)&huart->Instance->DR,(uint32_t)huart->pRxBuffPtr, Size);



    __HAL_UART_CLEAR_OREFLAG(huart);

                /* Process Unlocked */
    __HAL_UNLOCK(huart);

                /* Enable the UART Parity Error Interrupt */
    SET_BIT(huart->Instance->CR1, USART_CR1_PEIE);

                /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
    SET_BIT(huart->Instance->CR3, USART_CR3_EIE);



    SET_BIT(huart->Instance->CR3, USART_CR3_DMAR);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}



HAL_StatusTypeDef HAL_UART_DMAPause(UART_HandleTypeDef *huart)
{
  uint32_t dmarequest = 0x00U;

        /* Process Locked */
  __HAL_LOCK(huart);

  dmarequest = HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAT);
  if ((huart->gState == HAL_UART_STATE_BUSY_TX) && dmarequest)
  {
                /* Disable the UART DMA Tx request */
    CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAT);
  }

  dmarequest = HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR);
  if ((huart->RxState == HAL_UART_STATE_BUSY_RX) && dmarequest)
  {
                /* Disable RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts */
    CLEAR_BIT(huart->Instance->CR1, USART_CR1_PEIE);
    CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);

                /* Disable the UART DMA Rx request */
    CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);
  }

        /* Process Unlocked */
  __HAL_UNLOCK(huart);

  return HAL_OK;
}



HAL_StatusTypeDef HAL_UART_DMAResume(UART_HandleTypeDef *huart)
{
        /* Process Locked */
  __HAL_LOCK(huart);

  if (huart->gState == HAL_UART_STATE_BUSY_TX)
  {
                /* Enable the UART DMA Tx request */
    SET_BIT(huart->Instance->CR3, USART_CR3_DMAT);
  }

  if (huart->RxState == HAL_UART_STATE_BUSY_RX)
  {
                /* Clear the Overrun flag before resuming the Rx transfer */
    __HAL_UART_CLEAR_OREFLAG(huart);

                /* Reenable PE and ERR (Frame error, noise error, overrun error) interrupts */
    SET_BIT(huart->Instance->CR1, USART_CR1_PEIE);
    SET_BIT(huart->Instance->CR3, USART_CR3_EIE);

                /* Enable the UART DMA Rx request */
    SET_BIT(huart->Instance->CR3, USART_CR3_DMAR);
  }

        /* Process Unlocked */
  __HAL_UNLOCK(huart);

  return HAL_OK;
}



HAL_StatusTypeDef HAL_UART_DMAStop(UART_HandleTypeDef *huart)
{
  uint32_t dmarequest = 0x00U;



        /* Stop UART DMA Tx request if ongoing */
  dmarequest = HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAT);
  if ((huart->gState == HAL_UART_STATE_BUSY_TX) && dmarequest)
  {
    CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAT);

                /* Abort the UART DMA Tx channel */
    if (huart->hdmatx != NULL)
    {
      HAL_DMA_Abort(huart->hdmatx);
    }
    UART_EndTxTransfer(huart);
  }

        /* Stop UART DMA Rx request if ongoing */
  dmarequest = HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR);
  if ((huart->RxState == HAL_UART_STATE_BUSY_RX) && dmarequest)
  {
    CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);

                /* Abort the UART DMA Rx channel */
    if (huart->hdmarx != NULL)
    {
      HAL_DMA_Abort(huart->hdmarx);
    }
    UART_EndRxTransfer(huart);
  }

  return HAL_OK;
}
#endif /* DMA1 or DMA */



HAL_StatusTypeDef HAL_UART_Abort(UART_HandleTypeDef *huart)
{
        /* Disable TXEIE, TCIE, RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts */
  CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE | USART_CR1_TXEIE | USART_CR1_TCIE));
  CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);

#if (defined(DMA1) || defined(DMA))
        /* Disable the UART DMA Tx request if enabled */
  if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAT))
  {
    CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAT);

                /* Abort the UART DMA Tx channel: use blocking DMA Abort API (no callback) */
    if (huart->hdmatx != NULL)
    {

      huart->hdmatx->XferAbortCallback = NULL;

      if (HAL_DMA_Abort(huart->hdmatx) != HAL_OK)
      {
        if (HAL_DMA_GetError(huart->hdmatx) == HAL_DMA_ERROR_TIMEOUT)
        {
                                        /* Set error code to DMA */
          huart->ErrorCode = HAL_UART_ERROR_DMA;

          return HAL_TIMEOUT;
        }
      }
    }
  }

        /* Disable the UART DMA Rx request if enabled */
  if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR))
  {
    CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);

                /* Abort the UART DMA Rx channel: use blocking DMA Abort API (no callback) */
    if (huart->hdmarx != NULL)
    {

      huart->hdmarx->XferAbortCallback = NULL;

      if (HAL_DMA_Abort(huart->hdmarx) != HAL_OK)
      {
        if (HAL_DMA_GetError(huart->hdmarx) == HAL_DMA_ERROR_TIMEOUT)
        {
                                        /* Set error code to DMA */
          huart->ErrorCode = HAL_UART_ERROR_DMA;

          return HAL_TIMEOUT;
        }
      }
    }
  }
#endif /* DMA1 or DMA */

        /* Reset Tx and Rx transfer counters */
  huart->TxXferCount = 0x00U;
  huart->RxXferCount = 0x00U;

        /* Reset ErrorCode */
  huart->ErrorCode = HAL_UART_ERROR_NONE;

        /* Restore huart->RxState and huart->gState to Ready */
  huart->RxState = HAL_UART_STATE_READY;
  huart->gState = HAL_UART_STATE_READY;

  return HAL_OK;
}



HAL_StatusTypeDef HAL_UART_AbortTransmit(UART_HandleTypeDef *huart)
{
        /* Disable TXEIE and TCIE interrupts */
  CLEAR_BIT(huart->Instance->CR1, (USART_CR1_TXEIE | USART_CR1_TCIE));

#if (defined(DMA1) || defined(DMA))
        /* Disable the UART DMA Tx request if enabled */
  if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAT))
  {
    CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAT);

                /* Abort the UART DMA Tx channel : use blocking DMA Abort API (no callback) */
    if (huart->hdmatx != NULL)
    {

      huart->hdmatx->XferAbortCallback = NULL;

      if (HAL_DMA_Abort(huart->hdmatx) != HAL_OK)
      {
        if (HAL_DMA_GetError(huart->hdmatx) == HAL_DMA_ERROR_TIMEOUT)
        {
                                        /* Set error code to DMA */
          huart->ErrorCode = HAL_UART_ERROR_DMA;

          return HAL_TIMEOUT;
        }
      }
    }
  }
#endif /* DMA1 or DMA */

        /* Reset Tx transfer counter */
  huart->TxXferCount = 0x00U;

        /* Restore huart->gState to Ready */
  huart->gState = HAL_UART_STATE_READY;

  return HAL_OK;
}



HAL_StatusTypeDef HAL_UART_AbortReceive(UART_HandleTypeDef *huart)
{
        /* Disable RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts */
  CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
  CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);

#if (defined(DMA1) || defined(DMA))
        /* Disable the UART DMA Rx request if enabled */
  if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR))
  {
    CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);

                /* Abort the UART DMA Rx channel : use blocking DMA Abort API (no callback) */
    if (huart->hdmarx != NULL)
    {

      huart->hdmarx->XferAbortCallback = NULL;

      if (HAL_DMA_Abort(huart->hdmarx) != HAL_OK)
      {
        if (HAL_DMA_GetError(huart->hdmarx) == HAL_DMA_ERROR_TIMEOUT)
        {
                                        /* Set error code to DMA */
          huart->ErrorCode = HAL_UART_ERROR_DMA;

          return HAL_TIMEOUT;
        }
      }
    }
  }
#endif /* DMA1 or DMA */

        /* Reset Rx transfer counter */
  huart->RxXferCount = 0x00U;

        /* Restore huart->RxState to Ready */
  huart->RxState = HAL_UART_STATE_READY;

  return HAL_OK;
}



HAL_StatusTypeDef HAL_UART_Abort_IT(UART_HandleTypeDef *huart)
{
  uint32_t AbortCplt = 0x01U;

        /* Disable TXEIE, TCIE, RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts */
  CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE | USART_CR1_TXEIE | USART_CR1_TCIE));
  CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);

#if (defined(DMA1) || defined(DMA))

        /* DMA Tx Handle is valid */
  if (huart->hdmatx != NULL)
  {

    if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAT))
    {
      huart->hdmatx->XferAbortCallback = UART_DMATxAbortCallback;
    }
    else
    {
      huart->hdmatx->XferAbortCallback = NULL;
    }
  }
        /* DMA Rx Handle is valid */
  if (huart->hdmarx != NULL)
  {

    if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR))
    {
      huart->hdmarx->XferAbortCallback = UART_DMARxAbortCallback;
    }
    else
    {
      huart->hdmarx->XferAbortCallback = NULL;
    }
  }

        /* Disable the UART DMA Tx request if enabled */
  if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAT))
  {
                /* Disable DMA Tx at UART level */
    CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAT);

                /* Abort the UART DMA Tx channel : use non blocking DMA Abort API (callback) */
    if (huart->hdmatx != NULL)
    {



                        /* Abort DMA TX */
      if (HAL_DMA_Abort_IT(huart->hdmatx) != HAL_OK)
      {
        huart->hdmatx->XferAbortCallback = NULL;
      }
      else
      {
        AbortCplt = 0x00U;
      }
    }
  }

        /* Disable the UART DMA Rx request if enabled */
  if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR))
  {
    CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);

                /* Abort the UART DMA Rx channel : use non blocking DMA Abort API (callback) */
    if (huart->hdmarx != NULL)
    {



                        /* Abort DMA RX */
      if (HAL_DMA_Abort_IT(huart->hdmarx) != HAL_OK)
      {
        huart->hdmarx->XferAbortCallback = NULL;
        AbortCplt = 0x01U;
      }
      else
      {
        AbortCplt = 0x00U;
      }
    }
  }
#endif /* DMA1 or DMA */



  if (AbortCplt == 0x01U)
  {
                /* Reset Tx and Rx transfer counters */
    huart->TxXferCount = 0x00U;
    huart->RxXferCount = 0x00U;

                /* Reset ErrorCode */
    huart->ErrorCode = HAL_UART_ERROR_NONE;

                /* Restore huart->gState and huart->RxState to Ready */
    huart->gState  = HAL_UART_STATE_READY;
    huart->RxState = HAL_UART_STATE_READY;

                /* As no DMA to be aborted, call directly user Abort complete callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
                /* Call registered Abort complete callback */
    huart->AbortCpltCallback(huart);
#else
                /* Call legacy weak Abort complete callback */
    HAL_UART_AbortCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
  }

  return HAL_OK;
}



HAL_StatusTypeDef HAL_UART_AbortTransmit_IT(UART_HandleTypeDef *huart)
{
        /* Disable TXEIE and TCIE interrupts */
  CLEAR_BIT(huart->Instance->CR1, (USART_CR1_TXEIE | USART_CR1_TCIE));

#if (defined(DMA1) || defined(DMA))
        /* Disable the UART DMA Tx request if enabled */
  if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAT))
  {
    CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAT);

                /* Abort the UART DMA Tx channel : use blocking DMA Abort API (no callback) */
    if (huart->hdmatx != NULL)
    {

      huart->hdmatx->XferAbortCallback = UART_DMATxOnlyAbortCallback;

                        /* Abort DMA TX */
      if (HAL_DMA_Abort_IT(huart->hdmatx) != HAL_OK)
      {
                                /* Call Directly huart->hdmatx->XferAbortCallback function in case of error */
        huart->hdmatx->XferAbortCallback(huart->hdmatx);
      }
    }
    else
    {
                        /* Reset Tx transfer counter */
      huart->TxXferCount = 0x00U;

                        /* Restore huart->gState to Ready */
      huart->gState = HAL_UART_STATE_READY;

                        /* As no DMA to be aborted, call directly user Abort complete callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
                        /* Call registered Abort Transmit Complete Callback */
      huart->AbortTransmitCpltCallback(huart);
#else
                        /* Call legacy weak Abort Transmit Complete Callback */
      HAL_UART_AbortTransmitCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
    }
  }
  else
  {
#endif /* DMA1 or DMA */
                /* Reset Tx transfer counter */
    huart->TxXferCount = 0x00U;

                /* Restore huart->gState to Ready */
    huart->gState = HAL_UART_STATE_READY;

                /* As no DMA to be aborted, call directly user Abort complete callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
                /* Call registered Abort Transmit Complete Callback */
    huart->AbortTransmitCpltCallback(huart);
#else
                /* Call legacy weak Abort Transmit Complete Callback */
    HAL_UART_AbortTransmitCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
#if (defined(DMA1) || defined(DMA))
  }
#endif /* DMA1 or DMA */  

  return HAL_OK;
}



HAL_StatusTypeDef HAL_UART_AbortReceive_IT(UART_HandleTypeDef *huart)
{
        /* Disable RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts */
  CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
  CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);

#if (defined(DMA1) || defined(DMA))
        /* Disable the UART DMA Rx request if enabled */
  if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR))
  {
    CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);

                /* Abort the UART DMA Rx channel : use blocking DMA Abort API (no callback) */
    if (huart->hdmarx != NULL)
    {

      huart->hdmarx->XferAbortCallback = UART_DMARxOnlyAbortCallback;

                        /* Abort DMA RX */
      if (HAL_DMA_Abort_IT(huart->hdmarx) != HAL_OK)
      {
                                /* Call Directly huart->hdmarx->XferAbortCallback function in case of error */
        huart->hdmarx->XferAbortCallback(huart->hdmarx);
      }
    }
    else
    {
                        /* Reset Rx transfer counter */
      huart->RxXferCount = 0x00U;

                        /* Restore huart->RxState to Ready */
      huart->RxState = HAL_UART_STATE_READY;

                        /* As no DMA to be aborted, call directly user Abort complete callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
                        /* Call registered Abort Receive Complete Callback */
      huart->AbortReceiveCpltCallback(huart);
#else
                        /* Call legacy weak Abort Receive Complete Callback */
      HAL_UART_AbortReceiveCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
    }
  }
  else
  {
#endif /* DMA1 or DMA */
                /* Reset Rx transfer counter */
    huart->RxXferCount = 0x00U;

                /* Restore huart->RxState to Ready */
    huart->RxState = HAL_UART_STATE_READY;

                /* As no DMA to be aborted, call directly user Abort complete callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
                /* Call registered Abort Receive Complete Callback */
    huart->AbortReceiveCpltCallback(huart);
#else
                /* Call legacy weak Abort Receive Complete Callback */
    HAL_UART_AbortReceiveCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
#if (defined(DMA1) || defined(DMA))
  }
#endif /* DMA1 or DMA */

  return HAL_OK;
}



void HAL_UART_IRQHandler(UART_HandleTypeDef *huart)
{
  uint32_t isrflags   = READ_REG(huart->Instance->SR);
  uint32_t cr1its     = READ_REG(huart->Instance->CR1);
  uint32_t cr3its     = READ_REG(huart->Instance->CR3);
  uint32_t errorflags = 0x00U;
#if (defined(DMA1) || defined(DMA))
  uint32_t dmarequest = 0x00U;
#endif /* DMA1 or DMA */

        /* If no error occurs */
  errorflags = (isrflags & (uint32_t)(USART_SR_PE | USART_SR_FE | USART_SR_ORE | USART_SR_NE));
  if (errorflags == RESET)
  {
                /* UART in mode Receiver ------------------------------------------------- */
    if (((isrflags & USART_SR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
    {
      UART_Receive_IT(huart);
      return;
    }
  }

        /* If some errors occur */
  if ((errorflags != RESET) && (((cr3its & USART_CR3_EIE) != RESET) || ((cr1its & (USART_CR1_RXNEIE | USART_CR1_PEIE)) != RESET)))
  {
                /* UART parity error interrupt occurred ---------------------------------- */
    if (((isrflags & USART_SR_PE) != RESET) && ((cr1its & USART_CR1_PEIE) != RESET))
    {
      huart->ErrorCode |= HAL_UART_ERROR_PE;
    }

                /* UART noise error interrupt occurred ----------------------------------- */
    if (((isrflags & USART_SR_NE) != RESET) && ((cr3its & USART_CR3_EIE) != RESET))
    {
      huart->ErrorCode |= HAL_UART_ERROR_NE;
    }

                /* UART frame error interrupt occurred ----------------------------------- */
    if (((isrflags & USART_SR_FE) != RESET) && ((cr3its & USART_CR3_EIE) != RESET))
    {
      huart->ErrorCode |= HAL_UART_ERROR_FE;
    }

                /* UART Over-Run interrupt occurred -------------------------------------- */
    if (((isrflags & USART_SR_ORE) != RESET) && ((cr3its & USART_CR3_EIE) != RESET))
    {
      huart->ErrorCode |= HAL_UART_ERROR_ORE;
    }

                /* Call UART Error Call back function if need be -------------------------- */
    if (huart->ErrorCode != HAL_UART_ERROR_NONE)
    {
                        /* UART in mode Receiver ----------------------------------------------- */
      if (((isrflags & USART_SR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
      {
        UART_Receive_IT(huart);
      }



#if (defined(DMA1) || defined(DMA))
      dmarequest = HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR);
      if (((huart->ErrorCode & HAL_UART_ERROR_ORE) != RESET) || dmarequest)
#else
      if ((huart->ErrorCode & HAL_UART_ERROR_ORE) != RESET)
#endif /* DMA1 or DMA */
      {
                                /* Blocking error : transfer is aborted */
        UART_EndRxTransfer(huart);

#if (defined(DMA1) || defined(DMA))
                                /* Disable the UART DMA Rx request if enabled */
        if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR))
        {
          CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);

                                        /* Abort the UART DMA Rx channel */
          if (huart->hdmarx != NULL)
          {

            huart->hdmarx->XferAbortCallback = UART_DMAAbortOnError;
            if (HAL_DMA_Abort_IT(huart->hdmarx) != HAL_OK)
            {
                                                        /* Call Directly XferAbortCallback function in case of error */
              huart->hdmarx->XferAbortCallback(huart->hdmarx);
            }
          }
          else
          {
                                                /* Call user error callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
                                                /* Call registered error callback */
            huart->ErrorCallback(huart);
#else
                                                /* Call legacy weak error callback */
            HAL_UART_ErrorCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
          }
        }
        else
        {
#endif /* DMA1 or DMA */
                                        /* Call user error callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
                                        /* Call registered error callback */
          huart->ErrorCallback(huart);
#else
                                        /* Call legacy weak error callback */
          HAL_UART_ErrorCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
#if (defined(DMA1) || defined(DMA))
        }
#endif /* DMA1 or DMA */
      }
      else
      {

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
                                /* Call registered error callback */
        huart->ErrorCallback(huart);
#else
                                /* Call legacy weak error callback */
        HAL_UART_ErrorCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */

        huart->ErrorCode = HAL_UART_ERROR_NONE;
      }
    }
    return;
  }     /* End if some error occurs */
  
        /* Idle frame detect */
  if (((isrflags & USART_SR_IDLE) != RESET) && ((cr1its & USART_CR1_IDLEIE) != RESET))
  {
    __HAL_UART_CLEAR_IDLEFLAG(huart);
    
    HAL_UART_IdleFrameDetectCpltCallback(huart);
  }

        /* UART in mode Transmitter ------------------------------------------------ */
  if (((isrflags & USART_SR_TXE) != RESET) && ((cr1its & USART_CR1_TXEIE) != RESET))
  {
    UART_Transmit_IT(huart);
    return;
  }

        /* UART in mode Transmitter end -------------------------------------------- */
  if (((isrflags & USART_SR_TC) != RESET) && ((cr1its & USART_CR1_TCIE) != RESET))
  {
    UART_EndTransmit_IT(huart);
    return;
  }
}



__weak void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);

}



__weak void HAL_UART_TxHalfCpltCallback(UART_HandleTypeDef *huart)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);

}



__weak void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);

}



__weak void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);

}



__weak void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);

}

/* @brief  UART Abort Complete callback. @param  huart UART handle. @retval None */
__weak void HAL_UART_AbortCpltCallback(UART_HandleTypeDef *huart)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);



}

/* @brief  UART Abort Complete callback. @param  huart UART handle. @retval None */
__weak void HAL_UART_AbortTransmitCpltCallback(UART_HandleTypeDef *huart)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);



}

/* @brief  UART Abort Receive Complete callback. @param  huart UART handle. @retval None */
__weak void HAL_UART_AbortReceiveCpltCallback(UART_HandleTypeDef *huart)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);



}

/* @brief  UART Idle Frame Detect Complete callback. @param  huart UART handle. @retval None */
__weak void HAL_UART_IdleFrameDetectCpltCallback(UART_HandleTypeDef *huart)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);



}

/* @} */



HAL_StatusTypeDef HAL_UART_SendBreak(UART_HandleTypeDef *huart)
{
        /* Check the parameters */
  assert_param(IS_UART_INSTANCE(huart->Instance));

        /* Process Locked */
  __HAL_LOCK(huart);

  huart->gState = HAL_UART_STATE_BUSY;

        /* Send break characters */
  SET_BIT(huart->Instance->CR1, USART_CR1_SBK);

  huart->gState = HAL_UART_STATE_READY;

        /* Process Unlocked */
  __HAL_UNLOCK(huart);

  return HAL_OK;
}



HAL_StatusTypeDef HAL_MultiProcessor_EnterMuteMode(UART_HandleTypeDef *huart)
{
        /* Check the parameters */
  assert_param(IS_UART_INSTANCE(huart->Instance));

        /* Process Locked */
  __HAL_LOCK(huart);

  huart->gState = HAL_UART_STATE_BUSY;

        /* Enable the USART mute mode  by setting the RWU bit in the CR1 register */
  SET_BIT(huart->Instance->CR1, USART_CR1_RWU);

  huart->gState = HAL_UART_STATE_READY;

        /* Process Unlocked */
  __HAL_UNLOCK(huart);

  return HAL_OK;
}



HAL_StatusTypeDef HAL_MultiProcessor_ExitMuteMode(UART_HandleTypeDef *huart)
{
        /* Check the parameters */
  assert_param(IS_UART_INSTANCE(huart->Instance));

        /* Process Locked */
  __HAL_LOCK(huart);

  huart->gState = HAL_UART_STATE_BUSY;

        /* Disable the USART mute mode by clearing the RWU bit in the CR1 register */
  CLEAR_BIT(huart->Instance->CR1, USART_CR1_RWU);

  huart->gState = HAL_UART_STATE_READY;

        /* Process Unlocked */
  __HAL_UNLOCK(huart);

  return HAL_OK;
}



HAL_StatusTypeDef HAL_HalfDuplex_EnableTransmitter(UART_HandleTypeDef *huart)
{
  uint32_t tmpreg = 0x00U;

        /* Process Locked */
  __HAL_LOCK(huart);

  huart->gState = HAL_UART_STATE_BUSY;

        /* -------------------------- USART CR1 Configuration ----------------------- */
  tmpreg = huart->Instance->CR1;

        /* Clear TE and RE bits */
  tmpreg &= (uint32_t)~((uint32_t)(USART_CR1_TE | USART_CR1_RE));

        /* Enable the USART's transmit interface by setting the TE bit in the USART CR1 register */
  tmpreg |= (uint32_t)USART_CR1_TE;

        /* Write to USART CR1 */
  WRITE_REG(huart->Instance->CR1, (uint32_t)tmpreg);

  huart->gState = HAL_UART_STATE_READY;

        /* Process Unlocked */
  __HAL_UNLOCK(huart);

  return HAL_OK;
}



HAL_StatusTypeDef HAL_HalfDuplex_EnableReceiver(UART_HandleTypeDef *huart)
{
  uint32_t tmpreg = 0x00U;

        /* Process Locked */
  __HAL_LOCK(huart);

  huart->gState = HAL_UART_STATE_BUSY;

        /* -------------------------- USART CR1 Configuration ----------------------- */
  tmpreg = huart->Instance->CR1;

        /* Clear TE and RE bits */
  tmpreg &= (uint32_t)~((uint32_t)(USART_CR1_TE | USART_CR1_RE));

        /* Enable the USART's receive interface by setting the RE bit in the USART CR1 register */
  tmpreg |= (uint32_t)USART_CR1_RE;

        /* Write to USART CR1 */
  WRITE_REG(huart->Instance->CR1, (uint32_t)tmpreg);

  huart->gState = HAL_UART_STATE_READY;

        /* Process Unlocked */
  __HAL_UNLOCK(huart);

  return HAL_OK;
}

/* @} */



HAL_UART_StateTypeDef HAL_UART_GetState(UART_HandleTypeDef *huart)
{
  uint32_t temp1 = 0x00U, temp2 = 0x00U;
  temp1 = huart->gState;
  temp2 = huart->RxState;

  return (HAL_UART_StateTypeDef)(temp1 | temp2);
}



uint32_t HAL_UART_GetError(UART_HandleTypeDef *huart)
{
  return huart->ErrorCode;
}

/* @} */

/* @} */

/* @defgroup UART_Private_Functions UART Private Functions @{ */

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
/* @brief  Initialize the callbacks to their default values. @param  huart UART handle. @retval none */
void UART_InitCallbacksToDefault(UART_HandleTypeDef *huart)
{
        /* Init the UART Callback settings */
  huart->TxHalfCpltCallback        = HAL_UART_TxHalfCpltCallback;            /* Legacy weak TxHalfCpltCallback */
  huart->TxCpltCallback            = HAL_UART_TxCpltCallback;                /* Legacy weak TxCpltCallback */
  huart->RxHalfCpltCallback        = HAL_UART_RxHalfCpltCallback;            /* Legacy weak RxHalfCpltCallback */
  huart->RxCpltCallback            = HAL_UART_RxCpltCallback;                /* Legacy weak RxCpltCallback */
  huart->ErrorCallback             = HAL_UART_ErrorCallback;                 /* Legacy weak ErrorCallback */
  huart->AbortCpltCallback         = HAL_UART_AbortCpltCallback;             /* Legacy weak AbortCpltCallback */
  huart->AbortTransmitCpltCallback = HAL_UART_AbortTransmitCpltCallback;     /* Legacy weak AbortTransmitCpltCallback */
  huart->AbortReceiveCpltCallback  = HAL_UART_AbortReceiveCpltCallback;      /* Legacy weak AbortReceiveCpltCallback */

}
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */

#if (defined(DMA1) || defined(DMA))

static void UART_DMATransmitCplt(DMA_HandleTypeDef *hdma)
{
  UART_HandleTypeDef *huart = (UART_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;
        /* DMA Normal mode */
  if ((hdma->Instance->CCR & DMA_CCR_CIRC) == 0U)
  {
    huart->TxXferCount = 0x00U;



    CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAT);

                /* Enable the UART Transmit Complete Interrupt */
    SET_BIT(huart->Instance->CR1, USART_CR1_TCIE);

  }
        /* DMA Circular mode */
  else
  {
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
                /* Call registered Tx complete callback */
    huart->TxCpltCallback(huart);
#else
                /* Call legacy weak Tx complete callback */
    HAL_UART_TxCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
  }
}



static void UART_DMATxHalfCplt(DMA_HandleTypeDef *hdma)
{
  UART_HandleTypeDef *huart = (UART_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
        /* Call registered Tx complete callback */
  huart->TxHalfCpltCallback(huart);
#else
        /* Call legacy weak Tx complete callback */
  HAL_UART_TxHalfCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
}



static void UART_DMAReceiveCplt(DMA_HandleTypeDef *hdma)
{
  UART_HandleTypeDef *huart = (UART_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;
        /* DMA Normal mode */
  if ((hdma->Instance->CCR & DMA_CCR_CIRC) == 0U)
  {
    huart->RxXferCount = 0U;

                /* Disable RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts */
    CLEAR_BIT(huart->Instance->CR1, USART_CR1_PEIE);
    CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);



    CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);

                /* At end of Rx process, restore huart->RxState to Ready */
    huart->RxState = HAL_UART_STATE_READY;
  }
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
        /* Call registered Rx complete callback */
  huart->RxCpltCallback(huart);
#else
        /* Call legacy weak Rx complete callback */
  HAL_UART_RxCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
}



static void UART_DMARxHalfCplt(DMA_HandleTypeDef *hdma)
{
  UART_HandleTypeDef *huart = (UART_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
        /* Call registered Rx Half complete callback */
  huart->RxHalfCpltCallback(huart);
#else
        /* Call legacy weak Rx Half complete callback */
  HAL_UART_RxHalfCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
}



static void UART_DMAError(DMA_HandleTypeDef *hdma)
{
  uint32_t dmarequest = 0x00U;
  UART_HandleTypeDef *huart = (UART_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

        /* Stop UART DMA Tx request if ongoing */
  dmarequest = HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAT);
  if ((huart->gState == HAL_UART_STATE_BUSY_TX) && dmarequest)
  {
    huart->TxXferCount = 0x00U;
    UART_EndTxTransfer(huart);
  }

        /* Stop UART DMA Rx request if ongoing */
  dmarequest = HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR);
  if ((huart->RxState == HAL_UART_STATE_BUSY_RX) && dmarequest)
  {
    huart->RxXferCount = 0x00U;
    UART_EndRxTransfer(huart);
  }

  huart->ErrorCode |= HAL_UART_ERROR_DMA;
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
        /* Call registered error callback */
  huart->ErrorCallback(huart);
#else
        /* Call legacy weak error callback */
  HAL_UART_ErrorCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
}
#endif /* DMA1 or DMA */



static HAL_StatusTypeDef UART_WaitOnFlagUntilTimeout(UART_HandleTypeDef *huart, uint32_t Flag, FlagStatus Status, uint32_t Tickstart, uint32_t Timeout)
{
        /* Wait until flag is set */
  while ((__HAL_UART_GET_FLAG(huart, Flag) ? SET : RESET) == Status)
  {
                /* Check for the Timeout */
    if (Timeout != HAL_MAX_DELAY)
    {
      if ((Timeout == 0U) || ((HAL_GetTick() - Tickstart) > Timeout))
      {

        CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE | USART_CR1_TXEIE));
        CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);

        huart->gState  = HAL_UART_STATE_READY;
        huart->RxState = HAL_UART_STATE_READY;

                                /* Process Unlocked */
        __HAL_UNLOCK(huart);

        return HAL_TIMEOUT;
      }
    }
  }
  return HAL_OK;
}

#if (defined(DMA1) || defined(DMA))

static void UART_EndTxTransfer(UART_HandleTypeDef *huart)
{
        /* Disable TXEIE and TCIE interrupts */
  CLEAR_BIT(huart->Instance->CR1, (USART_CR1_TXEIE | USART_CR1_TCIE));

        /* At end of Tx process, restore huart->gState to Ready */
  huart->gState = HAL_UART_STATE_READY;
}
#endif /* DMA1 or DMA */



static void UART_EndRxTransfer(UART_HandleTypeDef *huart)
{
        /* Disable RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts */
  CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
  CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);

        /* At end of Rx process, restore huart->RxState to Ready */
  huart->RxState = HAL_UART_STATE_READY;
}

#if (defined(DMA1) || defined(DMA))

static void UART_DMAAbortOnError(DMA_HandleTypeDef *hdma)
{
  UART_HandleTypeDef *huart = (UART_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;
  huart->RxXferCount = 0x00U;
  huart->TxXferCount = 0x00U;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
        /* Call registered error callback */
  huart->ErrorCallback(huart);
#else
        /* Call legacy weak error callback */
  HAL_UART_ErrorCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
}



static void UART_DMATxAbortCallback(DMA_HandleTypeDef *hdma)
{
  UART_HandleTypeDef *huart = (UART_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

  huart->hdmatx->XferAbortCallback = NULL;

        /* Check if an Abort process is still ongoing */
  if (huart->hdmarx != NULL)
  {
    if (huart->hdmarx->XferAbortCallback != NULL)
    {
      return;
    }
  }



  huart->TxXferCount = 0x00U;
  huart->RxXferCount = 0x00U;

        /* Reset ErrorCode */
  huart->ErrorCode = HAL_UART_ERROR_NONE;

        /* Restore huart->gState and huart->RxState to Ready */
  huart->gState  = HAL_UART_STATE_READY;
  huart->RxState = HAL_UART_STATE_READY;

        /* Call user Abort complete callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
        /* Call registered Abort complete callback */
  huart->AbortCpltCallback(huart);
#else
        /* Call legacy weak Abort complete callback */
  HAL_UART_AbortCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
}



static void UART_DMARxAbortCallback(DMA_HandleTypeDef *hdma)
{
  UART_HandleTypeDef *huart = (UART_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

  huart->hdmarx->XferAbortCallback = NULL;

        /* Check if an Abort process is still ongoing */
  if (huart->hdmatx != NULL)
  {
    if (huart->hdmatx->XferAbortCallback != NULL)
    {
      return;
    }
  }



  huart->TxXferCount = 0x00U;
  huart->RxXferCount = 0x00U;

        /* Reset ErrorCode */
  huart->ErrorCode = HAL_UART_ERROR_NONE;

        /* Restore huart->gState and huart->RxState to Ready */
  huart->gState  = HAL_UART_STATE_READY;
  huart->RxState = HAL_UART_STATE_READY;

        /* Call user Abort complete callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
        /* Call registered Abort complete callback */
  huart->AbortCpltCallback(huart);
#else
        /* Call legacy weak Abort complete callback */
  HAL_UART_AbortCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
}



static void UART_DMATxOnlyAbortCallback(DMA_HandleTypeDef *hdma)
{
  UART_HandleTypeDef *huart = (UART_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

  huart->TxXferCount = 0x00U;

        /* Restore huart->gState to Ready */
  huart->gState = HAL_UART_STATE_READY;

        /* Call user Abort complete callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
        /* Call registered Abort Transmit Complete Callback */
  huart->AbortTransmitCpltCallback(huart);
#else
        /* Call legacy weak Abort Transmit Complete Callback */
  HAL_UART_AbortTransmitCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
}



static void UART_DMARxOnlyAbortCallback(DMA_HandleTypeDef *hdma)
{
  UART_HandleTypeDef *huart = (UART_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

  huart->RxXferCount = 0x00U;

        /* Restore huart->RxState to Ready */
  huart->RxState = HAL_UART_STATE_READY;

        /* Call user Abort complete callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
        /* Call registered Abort Receive Complete Callback */
  huart->AbortReceiveCpltCallback(huart);
#else
        /* Call legacy weak Abort Receive Complete Callback */
  HAL_UART_AbortReceiveCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
}
#endif /* DMA1 or DMA */



static HAL_StatusTypeDef UART_Transmit_IT(UART_HandleTypeDef *huart)
{
  uint16_t *tmp16bit;
  uint8_t *tmp8bit;
        /* Check that a Tx process is ongoing */
  if (huart->gState == HAL_UART_STATE_BUSY_TX)
  {
    if ((huart->Init.WordLength == UART_WORDLENGTH_9B)&&(huart->Init.Parity == UART_PARITY_NONE))
    {
      tmp16bit=(uint16_t *) huart->pTxBuffPtr;
      huart->Instance->DR = (uint16_t)(*tmp16bit & (uint16_t)0x01FF);
      huart->pTxBuffPtr += 2U;
    }
    else
    {
      tmp8bit=(uint8_t *) huart->pTxBuffPtr;
      huart->Instance->DR = (uint8_t)(*tmp8bit & (uint8_t)0x00FF);
      huart->pTxBuffPtr++;
    }

    if (--huart->TxXferCount == 0U)
    {
                        /* Disable the UART Transmit Complete Interrupt */
      __HAL_UART_DISABLE_IT(huart, UART_IT_TXE);

                        /* Enable the UART Transmit Complete Interrupt */
      __HAL_UART_ENABLE_IT(huart, UART_IT_TC);
    }
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}



static HAL_StatusTypeDef UART_EndTransmit_IT(UART_HandleTypeDef *huart)
{
        /* Disable the UART Transmit Complete Interrupt */
  __HAL_UART_DISABLE_IT(huart, UART_IT_TC);

        /* Tx process is ended, restore huart->gState to Ready */
  huart->gState = HAL_UART_STATE_READY;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
        /* Call registered Tx complete callback */
  huart->TxCpltCallback(huart);
#else
        /* Call legacy weak Tx complete callback */
  HAL_UART_TxCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */

  return HAL_OK;
}



static HAL_StatusTypeDef UART_Receive_IT(UART_HandleTypeDef *huart)
{
  uint16_t *tmp16bit;
        /* Check that a Rx process is ongoing */
  if (huart->RxState == HAL_UART_STATE_BUSY_RX)
  {
                /* Set the Rx ISR function pointer according to the data word length */
    if((huart->Init.WordLength == UART_WORDLENGTH_9B)&& (huart->Init.Parity == UART_PARITY_NONE))
    {
      tmp16bit = (uint16_t *) huart->pRxBuffPtr;
      *tmp16bit = (uint16_t)(huart->Instance->DR & (uint16_t)0x01FF);
      huart->pRxBuffPtr += 2U;

    }
    else
    {

      *huart->pRxBuffPtr++ = (uint8_t)(huart->Instance->DR & (uint8_t)0x00FF);
    }

    if (--huart->RxXferCount == 0U)
    {
                        /* Disable the UART Data Register not empty Interrupt */
      __HAL_UART_DISABLE_IT(huart, UART_IT_RXNE);

                        /* Disable the UART Parity Error Interrupt */
      __HAL_UART_DISABLE_IT(huart, UART_IT_PE);

                        /* Disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
      __HAL_UART_DISABLE_IT(huart, UART_IT_ERR);

                        /* Rx process is completed, restore huart->RxState to Ready */
      huart->RxState = HAL_UART_STATE_READY;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
                        /* Call registered Rx complete callback */
      huart->RxCpltCallback(huart);
#else
                        /* Call legacy weak Rx complete callback */
      HAL_UART_RxCpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */

      return HAL_OK;
    }
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}



static void UART_SetConfig(UART_HandleTypeDef *huart)
{
  uint32_t tmpreg;
  uint32_t pclk;

        /* Check the parameters */
  assert_param(IS_UART_BAUDRATE(huart->Init.BaudRate));
  assert_param(IS_UART_STOPBITS(huart->Init.StopBits));
  assert_param(IS_UART_PARITY(huart->Init.Parity));
  assert_param(IS_UART_MODE(huart->Init.Mode));

        /* -------------------------- USART CR2 Configuration ----------------------- */
        /* Configure the UART Stop Bits: Set STOP[13:12] bits according to huart->Init.StopBits value */
  MODIFY_REG(huart->Instance->CR2, USART_CR2_STOP, huart->Init.StopBits);

        /* -------------------------- USART CR1 Configuration ----------------------- */



#if defined(USART_CR3_OVER8)
  tmpreg = (uint32_t)huart->Init.WordLength | huart->Init.Parity | huart->Init.Mode;
  MODIFY_REG(huart->Instance->CR1,
             (uint32_t)(USART_CR1_M | USART_CR1_PCE | USART_CR1_PS | USART_CR1_TE | USART_CR1_RE),
             tmpreg);

  tmpreg = (uint32_t) huart->Init.OverSampling;
  MODIFY_REG(huart->Instance->CR3,
             (uint32_t)(USART_CR3_OVER8),
             tmpreg);
#else
  tmpreg = (uint32_t)huart->Init.WordLength | huart->Init.Parity | huart->Init.Mode;
  MODIFY_REG(huart->Instance->CR1,
             (uint32_t)(USART_CR1_M | USART_CR1_PCE | USART_CR1_PS | USART_CR1_TE | USART_CR1_RE),
             tmpreg);
#endif /* USART_CR3_OVER8 */

        /* -------------------------- USART CR3 Configuration ----------------------- */
        /* Configure the UART HFC: Set CTSE and RTSE bits according to huart->Init.HwFlowCtl value */
  MODIFY_REG(huart->Instance->CR3, (USART_CR3_RTSE | USART_CR3_CTSE), huart->Init.HwFlowCtl);

#if defined(USART_CR3_OVER8)
        /* Check the Over Sampling */
  if(huart->Init.OverSampling == UART_OVERSAMPLING_8)
  {
                /* -------------------------- USART BRR Configuration --------------------- */
    pclk = HAL_RCC_GetPCLK1Freq();
    huart->Instance->BRR = UART_BRR_SAMPLING8(pclk, huart->Init.BaudRate);
  }
  else
  {
                /* -------------------------- USART BRR Configuration --------------------- */
    pclk = HAL_RCC_GetPCLK1Freq();
    huart->Instance->BRR = UART_BRR_SAMPLING16(pclk, huart->Init.BaudRate);
  }
#else
        /* -------------------------- USART BRR Configuration --------------------- */

  pclk = HAL_RCC_GetPCLK1Freq();
  huart->Instance->BRR = UART_BRR_SAMPLING16(pclk, huart->Init.BaudRate);

#endif /* USART_CR3_OVER8 */
}
/* @brief Configure the UART peripheral advanced features. @param huart UART handle. @retval None */
void UART_AdvFeatureConfig(UART_HandleTypeDef *huart)
{
        /* Check whether the set of advanced features to configure is properly set */
  assert_param(IS_UART_ADVFEATURE_INIT(huart->AdvancedInit.AdvFeatureInit));

        /* if required, configure auto Baud rate detection scheme */
  if (HAL_IS_BIT_SET(huart->AdvancedInit.AdvFeatureInit, UART_ADVFEATURE_AUTOBAUDRATE_INIT))
  {
    assert_param(IS_USART_AUTOBAUDRATE_DETECTION_INSTANCE(huart->Instance));
    assert_param(IS_UART_ADVFEATURE_AUTOBAUDRATE(huart->AdvancedInit.AutoBaudRateEnable));
    MODIFY_REG(huart->Instance->CR3, USART_CR3_ABREN, huart->AdvancedInit.AutoBaudRateEnable);
                /* set auto Baudrate detection parameters if detection is enabled */
    if (huart->AdvancedInit.AutoBaudRateEnable == UART_ADVFEATURE_AUTOBAUDRATE_ENABLE)
    {
      assert_param(IS_UART_ADVFEATURE_AUTOBAUDRATEMODE(huart->AdvancedInit.AutoBaudRateMode));
      MODIFY_REG(huart->Instance->CR3, USART_CR3_ABRMODE, huart->AdvancedInit.AutoBaudRateMode);
    }
  }
}
/* @} */

#endif /* HAL_UART_MODULE_ENABLED */
/* @} */

/* @} */

/* (C) COPYRIGHT Puya *****END OF FILE */
