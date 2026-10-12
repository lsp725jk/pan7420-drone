/**
 * @file    py32f0xx_hal_irda.c
 * @author  MCU Application Team
 * @brief   IRDA HAL module driver.
 */

/* Includes ------------------------------------------------------------------ */
#include "py32f0xx_hal.h"

/**
 * @addtogroup PY32F0xx_HAL_Driver @{
 */

/* @defgroup IRDA IRDA @brief HAL IRDA module driver @{ */

#ifdef HAL_IRDA_MODULE_ENABLED

/* Private typedef ----------------------------------------------------------- */
/* Private define ------------------------------------------------------------ */
/* Private constants --------------------------------------------------------- */
/* Private macro ------------------------------------------------------------- */
/* Private variables --------------------------------------------------------- */
/* Private function prototypes ----------------------------------------------- */
/* @addtogroup IRDA_Private_Functions @{ */
#if (USE_HAL_IRDA_REGISTER_CALLBACKS == 1)
void IRDA_InitCallbacksToDefault(IRDA_HandleTypeDef *hirda);
#endif /* USE_HAL_IRDA_REGISTER_CALLBACKS */
static void IRDA_SetConfig(IRDA_HandleTypeDef *hirda);
static HAL_StatusTypeDef IRDA_Transmit_IT(IRDA_HandleTypeDef *hirda);
static HAL_StatusTypeDef IRDA_EndTransmit_IT(IRDA_HandleTypeDef *hirda);
static HAL_StatusTypeDef IRDA_Receive_IT(IRDA_HandleTypeDef *hirda);
static void IRDA_DMATransmitCplt(DMA_HandleTypeDef *hdma);
static void IRDA_DMATransmitHalfCplt(DMA_HandleTypeDef *hdma);
static void IRDA_DMAReceiveCplt(DMA_HandleTypeDef *hdma);
static void IRDA_DMAReceiveHalfCplt(DMA_HandleTypeDef *hdma);
static void IRDA_DMAError(DMA_HandleTypeDef *hdma);
static void IRDA_DMAAbortOnError(DMA_HandleTypeDef *hdma);
static void IRDA_DMATxAbortCallback(DMA_HandleTypeDef *hdma);
static void IRDA_DMARxAbortCallback(DMA_HandleTypeDef *hdma);
static void IRDA_DMATxOnlyAbortCallback(DMA_HandleTypeDef *hdma);
static void IRDA_DMARxOnlyAbortCallback(DMA_HandleTypeDef *hdma);
static HAL_StatusTypeDef IRDA_WaitOnFlagUntilTimeout(IRDA_HandleTypeDef *hirda, uint32_t Flag, FlagStatus Status, uint32_t Tickstart, uint32_t Timeout);
static void IRDA_EndTxTransfer(IRDA_HandleTypeDef *hirda);
static void IRDA_EndRxTransfer(IRDA_HandleTypeDef *hirda);
/* @} */

/* Exported functions -------------------------------------------------------- */
/* @defgroup IRDA_Exported_Functions IrDA Exported Functions @{ */



HAL_StatusTypeDef HAL_IRDA_Init(IRDA_HandleTypeDef *hirda)
{
        /* Check the IRDA handle allocation */
  if (hirda == NULL)
  {
    return HAL_ERROR;
  }

        /* Check the IRDA instance parameters */
  assert_param(IS_IRDA_INSTANCE(hirda->Instance));
        /* Check the IRDA mode parameter in the IRDA handle */
  assert_param(IS_IRDA_POWERMODE(hirda->Init.IrDAMode));

  if (hirda->gState == HAL_IRDA_STATE_RESET)
  {
                /* Allocate lock resource and initialize it */
    hirda->Lock = HAL_UNLOCKED;

#if USE_HAL_IRDA_REGISTER_CALLBACKS == 1
    IRDA_InitCallbacksToDefault(hirda);

    if (hirda->MspInitCallback == NULL)
    {
      hirda->MspInitCallback = HAL_IRDA_MspInit;
    }

                /* Init the low level hardware */
    hirda->MspInitCallback(hirda);
#else
                /* Init the low level hardware : GPIO, CLOCK */
    HAL_IRDA_MspInit(hirda);
#endif /* USE_HAL_IRDA_REGISTER_CALLBACKS */
  }

  hirda->gState = HAL_IRDA_STATE_BUSY;

        /* Disable the IRDA peripheral */
  __HAL_IRDA_DISABLE(hirda);

        /* Set the IRDA communication parameters */
  IRDA_SetConfig(hirda);



  CLEAR_BIT(hirda->Instance->CR2, (USART_CR2_LINEN | USART_CR2_STOP | USART_CR2_CLKEN));
  CLEAR_BIT(hirda->Instance->CR3, (USART_CR3_SCEN | USART_CR3_HDSEL));

        /* Enable the IRDA peripheral */
  __HAL_IRDA_ENABLE(hirda);

        /* Set the prescaler */
  MODIFY_REG(hirda->Instance->GTPR, USART_GTPR_PSC, hirda->Init.Prescaler);

        /* Configure the IrDA mode */
  MODIFY_REG(hirda->Instance->CR3, USART_CR3_IRLP, hirda->Init.IrDAMode);

        /* Enable the IrDA mode by setting the IREN bit in the CR3 register */
  SET_BIT(hirda->Instance->CR3, USART_CR3_IREN);

        /* Initialize the IRDA state */
  hirda->ErrorCode = HAL_IRDA_ERROR_NONE;
  hirda->gState = HAL_IRDA_STATE_READY;
  hirda->RxState = HAL_IRDA_STATE_READY;

  return HAL_OK;
}



HAL_StatusTypeDef HAL_IRDA_DeInit(IRDA_HandleTypeDef *hirda)
{
        /* Check the IRDA handle allocation */
  if (hirda == NULL)
  {
    return HAL_ERROR;
  }

        /* Check the parameters */
  assert_param(IS_IRDA_INSTANCE(hirda->Instance));

  hirda->gState = HAL_IRDA_STATE_BUSY;

        /* Disable the Peripheral */
  __HAL_IRDA_DISABLE(hirda);

        /* DeInit the low level hardware */
#if USE_HAL_IRDA_REGISTER_CALLBACKS == 1
  if (hirda->MspDeInitCallback == NULL)
  {
    hirda->MspDeInitCallback = HAL_IRDA_MspDeInit;
  }
        /* DeInit the low level hardware */
  hirda->MspDeInitCallback(hirda);
#else
  HAL_IRDA_MspDeInit(hirda);
#endif /* USE_HAL_IRDA_REGISTER_CALLBACKS */

  hirda->ErrorCode = HAL_IRDA_ERROR_NONE;

  hirda->gState = HAL_IRDA_STATE_RESET;
  hirda->RxState = HAL_IRDA_STATE_RESET;

        /* Release Lock */
  __HAL_UNLOCK(hirda);

  return HAL_OK;
}



__weak void HAL_IRDA_MspInit(IRDA_HandleTypeDef *hirda)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(hirda);



}



__weak void HAL_IRDA_MspDeInit(IRDA_HandleTypeDef *hirda)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(hirda);



}

#if (USE_HAL_IRDA_REGISTER_CALLBACKS == 1)

HAL_StatusTypeDef HAL_IRDA_RegisterCallback(IRDA_HandleTypeDef *hirda, HAL_IRDA_CallbackIDTypeDef CallbackID, pIRDA_CallbackTypeDef pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (pCallback == NULL)
  {
                /* Update the error code */
    hirda->ErrorCode |= HAL_IRDA_ERROR_INVALID_CALLBACK;

    return HAL_ERROR;
  }
        /* Process locked */
  __HAL_LOCK(hirda);

  if (hirda->gState == HAL_IRDA_STATE_READY)
  {
    switch (CallbackID)
    {
    case HAL_IRDA_TX_HALFCOMPLETE_CB_ID :
      hirda->TxHalfCpltCallback = pCallback;
      break;

    case HAL_IRDA_TX_COMPLETE_CB_ID :
      hirda->TxCpltCallback = pCallback;
      break;

    case HAL_IRDA_RX_HALFCOMPLETE_CB_ID :
      hirda->RxHalfCpltCallback = pCallback;
      break;

    case HAL_IRDA_RX_COMPLETE_CB_ID :
      hirda->RxCpltCallback = pCallback;
      break;

    case HAL_IRDA_ERROR_CB_ID :
      hirda->ErrorCallback = pCallback;
      break;

    case HAL_IRDA_ABORT_COMPLETE_CB_ID :
      hirda->AbortCpltCallback = pCallback;
      break;

    case HAL_IRDA_ABORT_TRANSMIT_COMPLETE_CB_ID :
      hirda->AbortTransmitCpltCallback = pCallback;
      break;

    case HAL_IRDA_ABORT_RECEIVE_COMPLETE_CB_ID :
      hirda->AbortReceiveCpltCallback = pCallback;
      break;

    case HAL_IRDA_MSPINIT_CB_ID :
      hirda->MspInitCallback = pCallback;
      break;

    case HAL_IRDA_MSPDEINIT_CB_ID :
      hirda->MspDeInitCallback = pCallback;
      break;

    default :
                        /* Update the error code */
      hirda->ErrorCode |= HAL_IRDA_ERROR_INVALID_CALLBACK;

                        /* Return error status */
      status =  HAL_ERROR;
      break;
    }
  }
  else if (hirda->gState == HAL_IRDA_STATE_RESET)
  {
    switch (CallbackID)
    {
    case HAL_IRDA_MSPINIT_CB_ID :
      hirda->MspInitCallback = pCallback;
      break;

    case HAL_IRDA_MSPDEINIT_CB_ID :
      hirda->MspDeInitCallback = pCallback;
      break;

    default :
                        /* Update the error code */
      hirda->ErrorCode |= HAL_IRDA_ERROR_INVALID_CALLBACK;

                        /* Return error status */
      status =  HAL_ERROR;
      break;
    }
  }
  else
  {
                /* Update the error code */
    hirda->ErrorCode |= HAL_IRDA_ERROR_INVALID_CALLBACK;

                /* Return error status */
    status =  HAL_ERROR;
  }

        /* Release Lock */
  __HAL_UNLOCK(hirda);

  return status;
}



HAL_StatusTypeDef HAL_IRDA_UnRegisterCallback(IRDA_HandleTypeDef *hirda, HAL_IRDA_CallbackIDTypeDef CallbackID)
{
  HAL_StatusTypeDef status = HAL_OK;

        /* Process locked */
  __HAL_LOCK(hirda);

  if (HAL_IRDA_STATE_READY == hirda->gState)
  {
    switch (CallbackID)
    {
    case HAL_IRDA_TX_HALFCOMPLETE_CB_ID :
      hirda->TxHalfCpltCallback = HAL_IRDA_TxHalfCpltCallback;                           /* Legacy weak  TxHalfCpltCallback */
      break;

    case HAL_IRDA_TX_COMPLETE_CB_ID :
      hirda->TxCpltCallback = HAL_IRDA_TxCpltCallback;                                   /* Legacy weak TxCpltCallback */
      break;

    case HAL_IRDA_RX_HALFCOMPLETE_CB_ID :
      hirda->RxHalfCpltCallback = HAL_IRDA_RxHalfCpltCallback;                           /* Legacy weak RxHalfCpltCallback */
      break;

    case HAL_IRDA_RX_COMPLETE_CB_ID :
      hirda->RxCpltCallback = HAL_IRDA_RxCpltCallback;                                   /* Legacy weak RxCpltCallback */
      break;

    case HAL_IRDA_ERROR_CB_ID :
      hirda->ErrorCallback = HAL_IRDA_ErrorCallback;                                     /* Legacy weak ErrorCallback */
      break;

    case HAL_IRDA_ABORT_COMPLETE_CB_ID :
      hirda->AbortCpltCallback = HAL_IRDA_AbortCpltCallback;                             /* Legacy weak AbortCpltCallback */
      break;

    case HAL_IRDA_ABORT_TRANSMIT_COMPLETE_CB_ID :
      hirda->AbortTransmitCpltCallback = HAL_IRDA_AbortTransmitCpltCallback;             /* Legacy weak AbortTransmitCpltCallback */
      break;

    case HAL_IRDA_ABORT_RECEIVE_COMPLETE_CB_ID :
      hirda->AbortReceiveCpltCallback = HAL_IRDA_AbortReceiveCpltCallback;               /* Legacy weak AbortReceiveCpltCallback */
      break;

    case HAL_IRDA_MSPINIT_CB_ID :
      hirda->MspInitCallback = HAL_IRDA_MspInit;                                         /* Legacy weak MspInitCallback */
      break;

    case HAL_IRDA_MSPDEINIT_CB_ID :
      hirda->MspDeInitCallback = HAL_IRDA_MspDeInit;                                     /* Legacy weak MspDeInitCallback */
      break;

    default :
                        /* Update the error code */
      hirda->ErrorCode |= HAL_IRDA_ERROR_INVALID_CALLBACK;

                        /* Return error status */
      status =  HAL_ERROR;
      break;
    }
  }
  else if (HAL_IRDA_STATE_RESET == hirda->gState)
  {
    switch (CallbackID)
    {
    case HAL_IRDA_MSPINIT_CB_ID :
      hirda->MspInitCallback = HAL_IRDA_MspInit;
      break;

    case HAL_IRDA_MSPDEINIT_CB_ID :
      hirda->MspDeInitCallback = HAL_IRDA_MspDeInit;
      break;

    default :
                        /* Update the error code */
      hirda->ErrorCode |= HAL_IRDA_ERROR_INVALID_CALLBACK;

                        /* Return error status */
      status =  HAL_ERROR;
      break;
    }
  }
  else
  {
                /* Update the error code */
    hirda->ErrorCode |= HAL_IRDA_ERROR_INVALID_CALLBACK;

                /* Return error status */
    status =  HAL_ERROR;
  }

        /* Release Lock */
  __HAL_UNLOCK(hirda);

  return status;
}
#endif /* USE_HAL_IRDA_REGISTER_CALLBACKS */

/* @} */



HAL_StatusTypeDef HAL_IRDA_Transmit(IRDA_HandleTypeDef *hirda, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  uint16_t *tmp;
  uint32_t tickstart = 0U;

        /* Check that a Tx process is not already ongoing */
  if (hirda->gState == HAL_IRDA_STATE_READY)
  {
    if ((pData == NULL) || (Size == 0U))
    {
      return  HAL_ERROR;
    }

                /* Process Locked */
    __HAL_LOCK(hirda);

    hirda->ErrorCode = HAL_IRDA_ERROR_NONE;
    hirda->gState = HAL_IRDA_STATE_BUSY_TX;

                /* Init tickstart for timeout managment */
    tickstart = HAL_GetTick();

    hirda->TxXferSize = Size;
    hirda->TxXferCount = Size;
    while (hirda->TxXferCount > 0U)
    {
      hirda->TxXferCount--;
      if (hirda->Init.WordLength == IRDA_WORDLENGTH_9B)
      {
        if (IRDA_WaitOnFlagUntilTimeout(hirda, IRDA_FLAG_TXE, RESET, tickstart, Timeout) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }
        tmp = (uint16_t *) pData;
        hirda->Instance->DR = (*tmp & (uint16_t)0x01FF);
        if (hirda->Init.Parity == IRDA_PARITY_NONE)
        {
          pData += 2U;
        }
        else
        {
          pData += 1U;
        }
      }
      else
      {
        if (IRDA_WaitOnFlagUntilTimeout(hirda, IRDA_FLAG_TXE, RESET, tickstart, Timeout) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }
        hirda->Instance->DR = (*pData++ & (uint8_t)0xFF);
      }
    }

    if (IRDA_WaitOnFlagUntilTimeout(hirda, IRDA_FLAG_TC, RESET, tickstart, Timeout) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }

                /* At end of Tx process, restore hirda->gState to Ready */
    hirda->gState = HAL_IRDA_STATE_READY;

                /* Process Unlocked */
    __HAL_UNLOCK(hirda);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}



HAL_StatusTypeDef HAL_IRDA_Receive(IRDA_HandleTypeDef *hirda, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  uint16_t *tmp;
  uint32_t tickstart = 0U;

        /* Check that a Rx process is not already ongoing */
  if (hirda->RxState == HAL_IRDA_STATE_READY)
  {
    if ((pData == NULL) || (Size == 0U))
    {
      return  HAL_ERROR;
    }

                /* Process Locked */
    __HAL_LOCK(hirda);

    hirda->ErrorCode = HAL_IRDA_ERROR_NONE;
    hirda->RxState = HAL_IRDA_STATE_BUSY_RX;

                /* Init tickstart for timeout managment */
    tickstart = HAL_GetTick();

    hirda->RxXferSize = Size;
    hirda->RxXferCount = Size;

                /* Check the remain data to be received */
    while (hirda->RxXferCount > 0U)
    {
      hirda->RxXferCount--;

      if (hirda->Init.WordLength == IRDA_WORDLENGTH_9B)
      {
        if (IRDA_WaitOnFlagUntilTimeout(hirda, IRDA_FLAG_RXNE, RESET, tickstart, Timeout) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }
        tmp = (uint16_t *) pData ;
        if (hirda->Init.Parity == IRDA_PARITY_NONE)
        {
          *tmp = (uint16_t)(hirda->Instance->DR & (uint16_t)0x01FF);
          pData += 2U;
        }
        else
        {
          *tmp = (uint16_t)(hirda->Instance->DR & (uint16_t)0x00FF);
          pData += 1U;
        }
      }
      else
      {
        if (IRDA_WaitOnFlagUntilTimeout(hirda, IRDA_FLAG_RXNE, RESET, tickstart, Timeout) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }
        if (hirda->Init.Parity == IRDA_PARITY_NONE)
        {
          *pData++ = (uint8_t)(hirda->Instance->DR & (uint8_t)0x00FF);
        }
        else
        {
          *pData++ = (uint8_t)(hirda->Instance->DR & (uint8_t)0x007F);
        }
      }
    }

                /* At end of Rx process, restore hirda->RxState to Ready */
    hirda->RxState = HAL_IRDA_STATE_READY;

                /* Process Unlocked */
    __HAL_UNLOCK(hirda);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}



HAL_StatusTypeDef HAL_IRDA_Transmit_IT(IRDA_HandleTypeDef *hirda, uint8_t *pData, uint16_t Size)
{
        /* Check that a Tx process is not already ongoing */
  if (hirda->gState == HAL_IRDA_STATE_READY)
  {
    if ((pData == NULL) || (Size == 0U))
    {
      return HAL_ERROR;
    }

                /* Process Locked */
    __HAL_LOCK(hirda);

    hirda->pTxBuffPtr = pData;
    hirda->TxXferSize = Size;
    hirda->TxXferCount = Size;

    hirda->ErrorCode = HAL_IRDA_ERROR_NONE;
    hirda->gState = HAL_IRDA_STATE_BUSY_TX;

                /* Process Unlocked */
    __HAL_UNLOCK(hirda);

                /* Enable the IRDA Transmit Data Register Empty Interrupt */
    SET_BIT(hirda->Instance->CR1, USART_CR1_TXEIE);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}



HAL_StatusTypeDef HAL_IRDA_Receive_IT(IRDA_HandleTypeDef *hirda, uint8_t *pData, uint16_t Size)
{
        /* Check that a Rx process is not already ongoing */
  if (hirda->RxState == HAL_IRDA_STATE_READY)
  {
    if ((pData == NULL) || (Size == 0U))
    {
      return HAL_ERROR;
    }

                /* Process Locked */
    __HAL_LOCK(hirda);

    hirda->pRxBuffPtr = pData;
    hirda->RxXferSize = Size;
    hirda->RxXferCount = Size;

    hirda->ErrorCode = HAL_IRDA_ERROR_NONE;
    hirda->RxState = HAL_IRDA_STATE_BUSY_RX;

                /* Process Unlocked */
    __HAL_UNLOCK(hirda);

                /* Enable the IRDA Parity Error and Data Register Not Empty Interrupts */
    SET_BIT(hirda->Instance->CR1, USART_CR1_PEIE | USART_CR1_RXNEIE);

                /* Enable the IRDA Error Interrupt: (Frame error, Noise error, Overrun error) */
    SET_BIT(hirda->Instance->CR3, USART_CR3_EIE);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}



HAL_StatusTypeDef HAL_IRDA_Transmit_DMA(IRDA_HandleTypeDef *hirda, uint8_t *pData, uint16_t Size)
{
  uint32_t *tmp;

        /* Check that a Tx process is not already ongoing */
  if (hirda->gState == HAL_IRDA_STATE_READY)
  {
    if ((pData == NULL) || (Size == 0U))
    {
      return HAL_ERROR;
    }

                /* Process Locked */
    __HAL_LOCK(hirda);

    hirda->pTxBuffPtr = pData;
    hirda->TxXferSize = Size;
    hirda->TxXferCount = Size;

    hirda->ErrorCode = HAL_IRDA_ERROR_NONE;
    hirda->gState = HAL_IRDA_STATE_BUSY_TX;

                /* Set the IRDA DMA transfer complete callback */
    hirda->hdmatx->XferCpltCallback = IRDA_DMATransmitCplt;

                /* Set the IRDA DMA half transfer complete callback */
    hirda->hdmatx->XferHalfCpltCallback = IRDA_DMATransmitHalfCplt;

                /* Set the DMA error callback */
    hirda->hdmatx->XferErrorCallback = IRDA_DMAError;

                /* Set the DMA abort callback */
    hirda->hdmatx->XferAbortCallback = NULL;

                /* Enable the IRDA transmit DMA channel */
    tmp = (uint32_t *)&pData;
    HAL_DMA_Start_IT(hirda->hdmatx, *(uint32_t *)tmp, (uint32_t)&hirda->Instance->DR, Size);

                /* Clear the TC flag in the SR register by writing 0 to it */
    __HAL_IRDA_CLEAR_FLAG(hirda, IRDA_FLAG_TC);

                /* Process Unlocked */
    __HAL_UNLOCK(hirda);



    SET_BIT(hirda->Instance->CR3, USART_CR3_DMAT);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}



HAL_StatusTypeDef HAL_IRDA_Receive_DMA(IRDA_HandleTypeDef *hirda, uint8_t *pData, uint16_t Size)
{
  uint32_t *tmp;

        /* Check that a Rx process is not already ongoing */
  if (hirda->RxState == HAL_IRDA_STATE_READY)
  {
    if ((pData == NULL) || (Size == 0U))
    {
      return HAL_ERROR;
    }

                /* Process Locked */
    __HAL_LOCK(hirda);

    hirda->pRxBuffPtr = pData;
    hirda->RxXferSize = Size;

    hirda->ErrorCode = HAL_IRDA_ERROR_NONE;
    hirda->RxState = HAL_IRDA_STATE_BUSY_RX;

                /* Set the IRDA DMA transfer complete callback */
    hirda->hdmarx->XferCpltCallback = IRDA_DMAReceiveCplt;

                /* Set the IRDA DMA half transfer complete callback */
    hirda->hdmarx->XferHalfCpltCallback = IRDA_DMAReceiveHalfCplt;

                /* Set the DMA error callback */
    hirda->hdmarx->XferErrorCallback = IRDA_DMAError;

                /* Set the DMA abort callback */
    hirda->hdmarx->XferAbortCallback = NULL;

                /* Enable the DMA channel */
    tmp = (uint32_t *)&pData;
    HAL_DMA_Start_IT(hirda->hdmarx, (uint32_t)&hirda->Instance->DR, *(uint32_t *)tmp, Size);



    __HAL_IRDA_CLEAR_OREFLAG(hirda);

                /* Process Unlocked */
    __HAL_UNLOCK(hirda);

                /* Enable the IRDA Parity Error Interrupt */
    SET_BIT(hirda->Instance->CR1, USART_CR1_PEIE);

                /* Enable the IRDA Error Interrupt: (Frame error, Noise error, Overrun error) */
    SET_BIT(hirda->Instance->CR3, USART_CR3_EIE);



    SET_BIT(hirda->Instance->CR3, USART_CR3_DMAR);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}



HAL_StatusTypeDef HAL_IRDA_DMAPause(IRDA_HandleTypeDef *hirda)
{
  uint32_t dmarequest = 0x00U;

        /* Process Locked */
  __HAL_LOCK(hirda);

  dmarequest = HAL_IS_BIT_SET(hirda->Instance->CR3, USART_CR3_DMAT);
  if ((hirda->gState == HAL_IRDA_STATE_BUSY_TX) && dmarequest)
  {
                /* Disable the IRDA DMA Tx request */
    CLEAR_BIT(hirda->Instance->CR3, USART_CR3_DMAT);
  }

  dmarequest = HAL_IS_BIT_SET(hirda->Instance->CR3, USART_CR3_DMAR);
  if ((hirda->RxState == HAL_IRDA_STATE_BUSY_RX) && dmarequest)
  {
                /* Disable PE and ERR (Frame error, noise error, overrun error) interrupts */
    CLEAR_BIT(hirda->Instance->CR1, USART_CR1_PEIE);
    CLEAR_BIT(hirda->Instance->CR3, USART_CR3_EIE);

                /* Disable the IRDA DMA Rx request */
    CLEAR_BIT(hirda->Instance->CR3, USART_CR3_DMAR);
  }

        /* Process Unlocked */
  __HAL_UNLOCK(hirda);

  return HAL_OK;
}



HAL_StatusTypeDef HAL_IRDA_DMAResume(IRDA_HandleTypeDef *hirda)
{
        /* Process Locked */
  __HAL_LOCK(hirda);

  if (hirda->gState == HAL_IRDA_STATE_BUSY_TX)
  {
                /* Enable the IRDA DMA Tx request */
    SET_BIT(hirda->Instance->CR3, USART_CR3_DMAT);
  }

  if (hirda->RxState == HAL_IRDA_STATE_BUSY_RX)
  {
                /* Clear the Overrun flag before resuming the Rx transfer */
    __HAL_IRDA_CLEAR_OREFLAG(hirda);

                /* Reenable PE and ERR (Frame error, noise error, overrun error) interrupts */
    SET_BIT(hirda->Instance->CR1, USART_CR1_PEIE);
    SET_BIT(hirda->Instance->CR3, USART_CR3_EIE);

                /* Enable the IRDA DMA Rx request */
    SET_BIT(hirda->Instance->CR3, USART_CR3_DMAR);
  }

        /* Process Unlocked */
  __HAL_UNLOCK(hirda);

  return HAL_OK;
}



HAL_StatusTypeDef HAL_IRDA_DMAStop(IRDA_HandleTypeDef *hirda)
{
  uint32_t dmarequest = 0x00U;



        /* Stop IRDA DMA Tx request if ongoing */
  dmarequest = HAL_IS_BIT_SET(hirda->Instance->CR3, USART_CR3_DMAT);
  if ((hirda->gState == HAL_IRDA_STATE_BUSY_TX) && dmarequest)
  {
    CLEAR_BIT(hirda->Instance->CR3, USART_CR3_DMAT);

                /* Abort the IRDA DMA Tx channel */
    if (hirda->hdmatx != NULL)
    {
      HAL_DMA_Abort(hirda->hdmatx);
    }
    IRDA_EndTxTransfer(hirda);
  }

        /* Stop IRDA DMA Rx request if ongoing */
  dmarequest = HAL_IS_BIT_SET(hirda->Instance->CR3, USART_CR3_DMAR);
  if ((hirda->RxState == HAL_IRDA_STATE_BUSY_RX) && dmarequest)
  {
    CLEAR_BIT(hirda->Instance->CR3, USART_CR3_DMAR);

                /* Abort the IRDA DMA Rx channel */
    if (hirda->hdmarx != NULL)
    {
      HAL_DMA_Abort(hirda->hdmarx);
    }
    IRDA_EndRxTransfer(hirda);
  }

  return HAL_OK;
}



HAL_StatusTypeDef HAL_IRDA_Abort(IRDA_HandleTypeDef *hirda)
{
        /* Disable TXEIE, TCIE, RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts */
  CLEAR_BIT(hirda->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE | USART_CR1_TXEIE | USART_CR1_TCIE));
  CLEAR_BIT(hirda->Instance->CR3, USART_CR3_EIE);

        /* Disable the IRDA DMA Tx request if enabled */
  if (HAL_IS_BIT_SET(hirda->Instance->CR3, USART_CR3_DMAT))
  {
    CLEAR_BIT(hirda->Instance->CR3, USART_CR3_DMAT);

                /* Abort the IRDA DMA Tx channel : use blocking DMA Abort API (no callback) */
    if (hirda->hdmatx != NULL)
    {

      hirda->hdmatx->XferAbortCallback = NULL;

      HAL_DMA_Abort(hirda->hdmatx);
    }
  }

        /* Disable the IRDA DMA Rx request if enabled */
  if (HAL_IS_BIT_SET(hirda->Instance->CR3, USART_CR3_DMAR))
  {
    CLEAR_BIT(hirda->Instance->CR3, USART_CR3_DMAR);

                /* Abort the IRDA DMA Rx channel : use blocking DMA Abort API (no callback) */
    if (hirda->hdmarx != NULL)
    {

      hirda->hdmarx->XferAbortCallback = NULL;

      HAL_DMA_Abort(hirda->hdmarx);
    }
  }

        /* Reset Tx and Rx transfer counters */
  hirda->TxXferCount = 0x00U;
  hirda->RxXferCount = 0x00U;

        /* Reset ErrorCode */
  hirda->ErrorCode = HAL_IRDA_ERROR_NONE;

        /* Restore hirda->RxState and hirda->gState to Ready */
  hirda->RxState = HAL_IRDA_STATE_READY;
  hirda->gState = HAL_IRDA_STATE_READY;

  return HAL_OK;
}



HAL_StatusTypeDef HAL_IRDA_AbortTransmit(IRDA_HandleTypeDef *hirda)
{
        /* Disable TXEIE and TCIE interrupts */
  CLEAR_BIT(hirda->Instance->CR1, (USART_CR1_TXEIE | USART_CR1_TCIE));

        /* Disable the IRDA DMA Tx request if enabled */
  if (HAL_IS_BIT_SET(hirda->Instance->CR3, USART_CR3_DMAT))
  {
    CLEAR_BIT(hirda->Instance->CR3, USART_CR3_DMAT);

                /* Abort the IRDA DMA Tx channel : use blocking DMA Abort API (no callback) */
    if (hirda->hdmatx != NULL)
    {

      hirda->hdmatx->XferAbortCallback = NULL;

      HAL_DMA_Abort(hirda->hdmatx);
    }
  }

        /* Reset Tx transfer counter */
  hirda->TxXferCount = 0x00U;

        /* Restore hirda->gState to Ready */
  hirda->gState = HAL_IRDA_STATE_READY;

  return HAL_OK;
}



HAL_StatusTypeDef HAL_IRDA_AbortReceive(IRDA_HandleTypeDef *hirda)
{
        /* Disable RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts */
  CLEAR_BIT(hirda->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
  CLEAR_BIT(hirda->Instance->CR3, USART_CR3_EIE);

        /* Disable the IRDA DMA Rx request if enabled */
  if (HAL_IS_BIT_SET(hirda->Instance->CR3, USART_CR3_DMAR))
  {
    CLEAR_BIT(hirda->Instance->CR3, USART_CR3_DMAR);

                /* Abort the IRDA DMA Rx channel : use blocking DMA Abort API (no callback) */
    if (hirda->hdmarx != NULL)
    {

      hirda->hdmarx->XferAbortCallback = NULL;

      HAL_DMA_Abort(hirda->hdmarx);
    }
  }

        /* Reset Rx transfer counter */
  hirda->RxXferCount = 0x00U;

        /* Restore hirda->RxState to Ready */
  hirda->RxState = HAL_IRDA_STATE_READY;

  return HAL_OK;
}



HAL_StatusTypeDef HAL_IRDA_Abort_IT(IRDA_HandleTypeDef *hirda)
{
  uint32_t AbortCplt = 0x01U;

        /* Disable TXEIE, TCIE, RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts */
  CLEAR_BIT(hirda->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE | USART_CR1_TXEIE | USART_CR1_TCIE));
  CLEAR_BIT(hirda->Instance->CR3, USART_CR3_EIE);



        /* DMA Tx Handle is valid */
  if (hirda->hdmatx != NULL)
  {

    if (HAL_IS_BIT_SET(hirda->Instance->CR3, USART_CR3_DMAT))
    {
      hirda->hdmatx->XferAbortCallback = IRDA_DMATxAbortCallback;
    }
    else
    {
      hirda->hdmatx->XferAbortCallback = NULL;
    }
  }
        /* DMA Rx Handle is valid */
  if (hirda->hdmarx != NULL)
  {

    if (HAL_IS_BIT_SET(hirda->Instance->CR3, USART_CR3_DMAR))
    {
      hirda->hdmarx->XferAbortCallback = IRDA_DMARxAbortCallback;
    }
    else
    {
      hirda->hdmarx->XferAbortCallback = NULL;
    }
  }

        /* Disable the IRDA DMA Tx request if enabled */
  if (HAL_IS_BIT_SET(hirda->Instance->CR3, USART_CR3_DMAT))
  {
                /* Disable DMA Tx at IRDA level */
    CLEAR_BIT(hirda->Instance->CR3, USART_CR3_DMAT);

                /* Abort the IRDA DMA Tx channel : use non blocking DMA Abort API (callback) */
    if (hirda->hdmatx != NULL)
    {



                        /* Abort DMA TX */
      if (HAL_DMA_Abort_IT(hirda->hdmatx) != HAL_OK)
      {
        hirda->hdmatx->XferAbortCallback = NULL;
      }
      else
      {
        AbortCplt = 0x00U;
      }
    }
  }

        /* Disable the IRDA DMA Rx request if enabled */
  if (HAL_IS_BIT_SET(hirda->Instance->CR3, USART_CR3_DMAR))
  {
    CLEAR_BIT(hirda->Instance->CR3, USART_CR3_DMAR);

                /* Abort the IRDA DMA Rx channel : use non blocking DMA Abort API (callback) */
    if (hirda->hdmarx != NULL)
    {



                        /* Abort DMA RX */
      if (HAL_DMA_Abort_IT(hirda->hdmarx) != HAL_OK)
      {
        hirda->hdmarx->XferAbortCallback = NULL;
        AbortCplt = 0x01U;
      }
      else
      {
        AbortCplt = 0x00U;
      }
    }
  }



  if (AbortCplt == 0x01U)
  {
                /* Reset Tx and Rx transfer counters */
    hirda->TxXferCount = 0x00U;
    hirda->RxXferCount = 0x00U;

                /* Reset ErrorCode */
    hirda->ErrorCode = HAL_IRDA_ERROR_NONE;

                /* Restore hirda->gState and hirda->RxState to Ready */
    hirda->gState  = HAL_IRDA_STATE_READY;
    hirda->RxState = HAL_IRDA_STATE_READY;

                /* As no DMA to be aborted, call directly user Abort complete callback */
#if (USE_HAL_IRDA_REGISTER_CALLBACKS == 1)
                /* Call registered Abort complete callback */
    hirda->AbortCpltCallback(hirda);
#else
                /* Call legacy weak Abort complete callback */
    HAL_IRDA_AbortCpltCallback(hirda);
#endif /* USE_HAL_IRDA_REGISTER_CALLBACK */
  }

  return HAL_OK;
}



HAL_StatusTypeDef HAL_IRDA_AbortTransmit_IT(IRDA_HandleTypeDef *hirda)
{
        /* Disable TXEIE and TCIE interrupts */
  CLEAR_BIT(hirda->Instance->CR1, (USART_CR1_TXEIE | USART_CR1_TCIE));

        /* Disable the IRDA DMA Tx request if enabled */
  if (HAL_IS_BIT_SET(hirda->Instance->CR3, USART_CR3_DMAT))
  {
    CLEAR_BIT(hirda->Instance->CR3, USART_CR3_DMAT);

                /* Abort the IRDA DMA Tx channel : use non blocking DMA Abort API (callback) */
    if (hirda->hdmatx != NULL)
    {

      hirda->hdmatx->XferAbortCallback = IRDA_DMATxOnlyAbortCallback;

                        /* Abort DMA TX */
      if (HAL_DMA_Abort_IT(hirda->hdmatx) != HAL_OK)
      {
                                /* Call Directly hirda->hdmatx->XferAbortCallback function in case of error */
        hirda->hdmatx->XferAbortCallback(hirda->hdmatx);
      }
    }
    else
    {
                        /* Reset Tx transfer counter */
      hirda->TxXferCount = 0x00U;

                        /* Restore hirda->gState to Ready */
      hirda->gState = HAL_IRDA_STATE_READY;

                        /* As no DMA to be aborted, call directly user Abort complete callback */
#if (USE_HAL_IRDA_REGISTER_CALLBACKS == 1)
                        /* Call registered Abort Transmit Complete Callback */
      hirda->AbortTransmitCpltCallback(hirda);
#else
                        /* Call legacy weak Abort Transmit Complete Callback */
      HAL_IRDA_AbortTransmitCpltCallback(hirda);
#endif /* USE_HAL_IRDA_REGISTER_CALLBACK */
    }
  }
  else
  {
                /* Reset Tx transfer counter */
    hirda->TxXferCount = 0x00U;

                /* Restore hirda->gState to Ready */
    hirda->gState = HAL_IRDA_STATE_READY;

                /* As no DMA to be aborted, call directly user Abort complete callback */
#if (USE_HAL_IRDA_REGISTER_CALLBACKS == 1)
                /* Call registered Abort Transmit Complete Callback */
    hirda->AbortTransmitCpltCallback(hirda);
#else
                /* Call legacy weak Abort Transmit Complete Callback */
    HAL_IRDA_AbortTransmitCpltCallback(hirda);
#endif /* USE_HAL_IRDA_REGISTER_CALLBACK */
  }

  return HAL_OK;
}



HAL_StatusTypeDef HAL_IRDA_AbortReceive_IT(IRDA_HandleTypeDef *hirda)
{
        /* Disable RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts */
  CLEAR_BIT(hirda->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
  CLEAR_BIT(hirda->Instance->CR3, USART_CR3_EIE);

        /* Disable the IRDA DMA Rx request if enabled */
  if (HAL_IS_BIT_SET(hirda->Instance->CR3, USART_CR3_DMAR))
  {
    CLEAR_BIT(hirda->Instance->CR3, USART_CR3_DMAR);

                /* Abort the IRDA DMA Rx channel : use non blocking DMA Abort API (callback) */
    if (hirda->hdmarx != NULL)
    {

      hirda->hdmarx->XferAbortCallback = IRDA_DMARxOnlyAbortCallback;

                        /* Abort DMA RX */
      if (HAL_DMA_Abort_IT(hirda->hdmarx) != HAL_OK)
      {
                                /* Call Directly hirda->hdmarx->XferAbortCallback function in case of error */
        hirda->hdmarx->XferAbortCallback(hirda->hdmarx);
      }
    }
    else
    {
                        /* Reset Rx transfer counter */
      hirda->RxXferCount = 0x00U;

                        /* Restore hirda->RxState to Ready */
      hirda->RxState = HAL_IRDA_STATE_READY;

                        /* As no DMA to be aborted, call directly user Abort complete callback */
#if (USE_HAL_IRDA_REGISTER_CALLBACKS == 1)
                        /* Call registered Abort Receive Complete Callback */
      hirda->AbortReceiveCpltCallback(hirda);
#else
                        /* Call legacy weak Abort Receive Complete Callback */
      HAL_IRDA_AbortReceiveCpltCallback(hirda);
#endif /* USE_HAL_IRDA_REGISTER_CALLBACK */
    }
  }
  else
  {
                /* Reset Rx transfer counter */
    hirda->RxXferCount = 0x00U;

                /* Restore hirda->RxState to Ready */
    hirda->RxState = HAL_IRDA_STATE_READY;

                /* As no DMA to be aborted, call directly user Abort complete callback */
#if (USE_HAL_IRDA_REGISTER_CALLBACKS == 1)
                /* Call registered Abort Receive Complete Callback */
    hirda->AbortReceiveCpltCallback(hirda);
#else
                /* Call legacy weak Abort Receive Complete Callback */
    HAL_IRDA_AbortReceiveCpltCallback(hirda);
#endif /* USE_HAL_IRDA_REGISTER_CALLBACK */
  }

  return HAL_OK;
}



void HAL_IRDA_IRQHandler(IRDA_HandleTypeDef *hirda)
{
  uint32_t isrflags   = READ_REG(hirda->Instance->SR);
  uint32_t cr1its     = READ_REG(hirda->Instance->CR1);
  uint32_t cr3its     = READ_REG(hirda->Instance->CR3);
  uint32_t errorflags = 0x00U;
  uint32_t dmarequest = 0x00U;

        /* If no error occurs */
  errorflags = (isrflags & (uint32_t)(USART_SR_PE | USART_SR_FE | USART_SR_ORE | USART_SR_NE));
  if (errorflags == RESET)
  {
                /* IRDA in mode Receiver ----------------------------------------------- */
    if (((isrflags & USART_SR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
    {
      IRDA_Receive_IT(hirda);
      return;
    }
  }

        /* If some errors occur */
  if ((errorflags != RESET) && (((cr3its & USART_CR3_EIE) != RESET) || ((cr1its & (USART_CR1_RXNEIE | USART_CR1_PEIE)) != RESET)))
  {
                /* IRDA parity error interrupt occurred ------------------------------- */
    if (((isrflags & USART_SR_PE) != RESET) && ((cr1its & USART_CR1_PEIE) != RESET))
    {
      hirda->ErrorCode |= HAL_IRDA_ERROR_PE;
    }

                /* IRDA noise error interrupt occurred -------------------------------- */
    if (((isrflags & USART_SR_NE) != RESET) && ((cr3its & USART_CR3_EIE) != RESET))
    {
      hirda->ErrorCode |= HAL_IRDA_ERROR_NE;
    }

                /* IRDA frame error interrupt occurred -------------------------------- */
    if (((isrflags & USART_SR_FE) != RESET) && ((cr3its & USART_CR3_EIE) != RESET))
    {
      hirda->ErrorCode |= HAL_IRDA_ERROR_FE;
    }

                /* IRDA Over-Run interrupt occurred ----------------------------------- */
    if (((isrflags & USART_SR_ORE) != RESET) && ((cr3its & USART_CR3_EIE) != RESET))
    {
      hirda->ErrorCode |= HAL_IRDA_ERROR_ORE;
    }
                /* Call IRDA Error Call back function if need be ----------------------- */
    if (hirda->ErrorCode != HAL_IRDA_ERROR_NONE)
    {
                        /* IRDA in mode Receiver --------------------------------------------- */
      if (((isrflags & USART_SR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
      {
        IRDA_Receive_IT(hirda);
      }



      dmarequest = HAL_IS_BIT_SET(hirda->Instance->CR3, USART_CR3_DMAR);
      if (((hirda->ErrorCode & HAL_IRDA_ERROR_ORE) != RESET) || dmarequest)
      {
                                /* Blocking error : transfer is aborted */
        IRDA_EndRxTransfer(hirda);

                                /* Disable the IRDA DMA Rx request if enabled */
        if (HAL_IS_BIT_SET(hirda->Instance->CR3, USART_CR3_DMAR))
        {
          CLEAR_BIT(hirda->Instance->CR3, USART_CR3_DMAR);

                                        /* Abort the IRDA DMA Rx channel */
          if (hirda->hdmarx != NULL)
          {

            hirda->hdmarx->XferAbortCallback = IRDA_DMAAbortOnError;

                                                /* Abort DMA RX */
            if (HAL_DMA_Abort_IT(hirda->hdmarx) != HAL_OK)
            {
                                                        /* Call Directly XferAbortCallback function in case of error */
              hirda->hdmarx->XferAbortCallback(hirda->hdmarx);
            }
          }
          else
          {
#if (USE_HAL_IRDA_REGISTER_CALLBACKS == 1)
                                                /* Call registered user error callback */
            hirda->ErrorCallback(hirda);
#else
                                                /* Call legacy weak user error callback */
            HAL_IRDA_ErrorCallback(hirda);
#endif /* USE_HAL_IRDA_REGISTER_CALLBACK */
          }
        }
        else
        {
#if (USE_HAL_IRDA_REGISTER_CALLBACKS == 1)
                                        /* Call registered user error callback */
          hirda->ErrorCallback(hirda);
#else
                                        /* Call legacy weak user error callback */
          HAL_IRDA_ErrorCallback(hirda);
#endif /* USE_HAL_IRDA_REGISTER_CALLBACK */
        }
      }
      else
      {

#if (USE_HAL_IRDA_REGISTER_CALLBACKS == 1)
                                /* Call registered user error callback */
        hirda->ErrorCallback(hirda);
#else
                                /* Call legacy weak user error callback */
        HAL_IRDA_ErrorCallback(hirda);
#endif /* USE_HAL_IRDA_REGISTER_CALLBACK */

        hirda->ErrorCode = HAL_IRDA_ERROR_NONE;
      }
    }
    return;
  }     /* End if some error occurs */

        /* IRDA in mode Transmitter ------------------------------------------------ */
  if (((isrflags & USART_SR_TXE) != RESET) && ((cr1its & USART_CR1_TXEIE) != RESET))
  {
    IRDA_Transmit_IT(hirda);
    return;
  }

        /* IRDA in mode Transmitter end -------------------------------------------- */
  if (((isrflags & USART_SR_TC) != RESET) && ((cr1its & USART_CR1_TCIE) != RESET))
  {
    IRDA_EndTransmit_IT(hirda);
    return;
  }
}



__weak void HAL_IRDA_TxCpltCallback(IRDA_HandleTypeDef *hirda)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(hirda);



}

/* @brief  Tx Half Transfer completed callback. @param  hirda  Pointer to a IRDA_HandleTypeDef structure that contains @retval None */
__weak void HAL_IRDA_TxHalfCpltCallback(IRDA_HandleTypeDef *hirda)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(hirda);



}



__weak void HAL_IRDA_RxCpltCallback(IRDA_HandleTypeDef *hirda)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(hirda);



}



__weak void HAL_IRDA_RxHalfCpltCallback(IRDA_HandleTypeDef *hirda)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(hirda);



}



__weak void HAL_IRDA_ErrorCallback(IRDA_HandleTypeDef *hirda)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(hirda);



}



__weak void HAL_IRDA_AbortCpltCallback(IRDA_HandleTypeDef *hirda)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(hirda);



}



__weak void HAL_IRDA_AbortTransmitCpltCallback(IRDA_HandleTypeDef *hirda)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(hirda);



}



__weak void HAL_IRDA_AbortReceiveCpltCallback(IRDA_HandleTypeDef *hirda)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(hirda);



}

/* @} */



HAL_IRDA_StateTypeDef HAL_IRDA_GetState(IRDA_HandleTypeDef *hirda)
{
  uint32_t temp1 = 0x00U, temp2 = 0x00U;
  temp1 = hirda->gState;
  temp2 = hirda->RxState;

  return (HAL_IRDA_StateTypeDef)(temp1 | temp2);
}



uint32_t HAL_IRDA_GetError(IRDA_HandleTypeDef *hirda)
{
  return hirda->ErrorCode;
}

/* @} */

/* @} */

/* @defgroup IRDA_Private_Functions IRDA Private Functions @{ */

#if (USE_HAL_IRDA_REGISTER_CALLBACKS == 1)
/* @brief  Initialize the callbacks to their default values. @param  hirda IRDA handle. @retval none */
void IRDA_InitCallbacksToDefault(IRDA_HandleTypeDef *hirda)
{
        /* Init the IRDA Callback settings */
  hirda->TxHalfCpltCallback        = HAL_IRDA_TxHalfCpltCallback;            /* Legacy weak TxHalfCpltCallback */
  hirda->TxCpltCallback            = HAL_IRDA_TxCpltCallback;                /* Legacy weak TxCpltCallback */
  hirda->RxHalfCpltCallback        = HAL_IRDA_RxHalfCpltCallback;            /* Legacy weak RxHalfCpltCallback */
  hirda->RxCpltCallback            = HAL_IRDA_RxCpltCallback;                /* Legacy weak RxCpltCallback */
  hirda->ErrorCallback             = HAL_IRDA_ErrorCallback;                 /* Legacy weak ErrorCallback */
  hirda->AbortCpltCallback         = HAL_IRDA_AbortCpltCallback;             /* Legacy weak AbortCpltCallback */
  hirda->AbortTransmitCpltCallback = HAL_IRDA_AbortTransmitCpltCallback;     /* Legacy weak AbortTransmitCpltCallback */
  hirda->AbortReceiveCpltCallback  = HAL_IRDA_AbortReceiveCpltCallback;      /* Legacy weak AbortReceiveCpltCallback */

}
#endif /* USE_HAL_IRDA_REGISTER_CALLBACKS */



static void IRDA_DMATransmitCplt(DMA_HandleTypeDef *hdma)
{
  IRDA_HandleTypeDef *hirda = (IRDA_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;
        /* DMA Normal mode */
  if ((hdma->Instance->CCR & DMA_CCR_CIRC) == 0U)
  {
    hirda->TxXferCount = 0U;



    CLEAR_BIT(hirda->Instance->CR3, USART_CR3_DMAT);

                /* Enable the IRDA Transmit Complete Interrupt */
    SET_BIT(hirda->Instance->CR1, USART_CR1_TCIE);
  }
        /* DMA Circular mode */
  else
  {
#if (USE_HAL_IRDA_REGISTER_CALLBACKS == 1)
                /* Call registered Tx complete callback */
    hirda->TxCpltCallback(hirda);
#else
                /* Call legacy weak Tx complete callback */
    HAL_IRDA_TxCpltCallback(hirda);
#endif /* USE_HAL_IRDA_REGISTER_CALLBACK */
  }
}



static void IRDA_DMATransmitHalfCplt(DMA_HandleTypeDef *hdma)
{
  IRDA_HandleTypeDef *hirda = (IRDA_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

#if (USE_HAL_IRDA_REGISTER_CALLBACKS == 1)
        /* Call registered Tx Half complete callback */
  hirda->TxHalfCpltCallback(hirda);
#else
        /* Call legacy weak Tx complete callback */
  HAL_IRDA_TxHalfCpltCallback(hirda);
#endif /* USE_HAL_IRDA_REGISTER_CALLBACK */
}



static void IRDA_DMAReceiveCplt(DMA_HandleTypeDef *hdma)
{
  IRDA_HandleTypeDef *hirda = (IRDA_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

        /* DMA Normal mode */
  if ((hdma->Instance->CCR & DMA_CCR_CIRC) == 0U)
  {
    hirda->RxXferCount = 0U;

                /* Disable PE and ERR (Frame error, noise error, overrun error) interrupts */
    CLEAR_BIT(hirda->Instance->CR1, USART_CR1_PEIE);
    CLEAR_BIT(hirda->Instance->CR3, USART_CR3_EIE);



    CLEAR_BIT(hirda->Instance->CR3, USART_CR3_DMAR);

                /* At end of Rx process, restore hirda->RxState to Ready */
    hirda->RxState = HAL_IRDA_STATE_READY;
  }

#if (USE_HAL_IRDA_REGISTER_CALLBACKS == 1)
        /* Call registered Rx complete callback */
  hirda->RxCpltCallback(hirda);
#else
        /* Call legacy weak Rx complete callback */
  HAL_IRDA_RxCpltCallback(hirda);
#endif /* USE_HAL_IRDA_REGISTER_CALLBACKS */
}



static void IRDA_DMAReceiveHalfCplt(DMA_HandleTypeDef *hdma)
{
  IRDA_HandleTypeDef *hirda = (IRDA_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

#if (USE_HAL_IRDA_REGISTER_CALLBACKS == 1)
        /* Call registered Rx Half complete callback */
  hirda->RxHalfCpltCallback(hirda);
#else
        /* Call legacy weak Rx Half complete callback */
  HAL_IRDA_RxHalfCpltCallback(hirda);
#endif /* USE_HAL_IRDA_REGISTER_CALLBACK */
}



static void IRDA_DMAError(DMA_HandleTypeDef *hdma)
{
  uint32_t dmarequest = 0x00U;
  IRDA_HandleTypeDef *hirda = (IRDA_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

        /* Stop IRDA DMA Tx request if ongoing */
  dmarequest = HAL_IS_BIT_SET(hirda->Instance->CR3, USART_CR3_DMAT);
  if ((hirda->gState == HAL_IRDA_STATE_BUSY_TX) && dmarequest)
  {
    hirda->TxXferCount = 0U;
    IRDA_EndTxTransfer(hirda);
  }

        /* Stop IRDA DMA Rx request if ongoing */
  dmarequest = HAL_IS_BIT_SET(hirda->Instance->CR3, USART_CR3_DMAR);
  if ((hirda->RxState == HAL_IRDA_STATE_BUSY_RX) && dmarequest)
  {
    hirda->RxXferCount = 0U;
    IRDA_EndRxTransfer(hirda);
  }

  hirda->ErrorCode |= HAL_IRDA_ERROR_DMA;

#if (USE_HAL_IRDA_REGISTER_CALLBACKS == 1)
        /* Call registered user error callback */
  hirda->ErrorCallback(hirda);
#else
        /* Call legacy weak user error callback */
  HAL_IRDA_ErrorCallback(hirda);
#endif /* USE_HAL_IRDA_REGISTER_CALLBACK */
}



static HAL_StatusTypeDef IRDA_WaitOnFlagUntilTimeout(IRDA_HandleTypeDef *hirda, uint32_t Flag, FlagStatus Status, uint32_t Tickstart, uint32_t Timeout)
{
        /* Wait until flag is set */
  while ((__HAL_IRDA_GET_FLAG(hirda, Flag) ? SET : RESET) == Status)
  {
                /* Check for the Timeout */
    if (Timeout != HAL_MAX_DELAY)
    {
      if ((Timeout == 0U) || ((HAL_GetTick() - Tickstart) > Timeout))
      {

        CLEAR_BIT(hirda->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE | USART_CR1_TXEIE));
        CLEAR_BIT(hirda->Instance->CR3, USART_CR3_EIE);

        hirda->gState  = HAL_IRDA_STATE_READY;
        hirda->RxState = HAL_IRDA_STATE_READY;

                                /* Process Unlocked */
        __HAL_UNLOCK(hirda);

        return HAL_TIMEOUT;
      }
    }
  }
  return HAL_OK;
}



static void IRDA_EndTxTransfer(IRDA_HandleTypeDef *hirda)
{
        /* Disable TXEIE and TCIE interrupts */
  CLEAR_BIT(hirda->Instance->CR1, (USART_CR1_TXEIE | USART_CR1_TCIE));

        /* At end of Tx process, restore hirda->gState to Ready */
  hirda->gState = HAL_IRDA_STATE_READY;
}



static void IRDA_EndRxTransfer(IRDA_HandleTypeDef *hirda)
{
        /* Disable RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts */
  CLEAR_BIT(hirda->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
  CLEAR_BIT(hirda->Instance->CR3, USART_CR3_EIE);

        /* At end of Rx process, restore hirda->RxState to Ready */
  hirda->RxState = HAL_IRDA_STATE_READY;
}

/* @brief  DMA IRDA communication abort callback, when initiated by HAL services on Error @param  hdma DMA handle. @retval None */
static void IRDA_DMAAbortOnError(DMA_HandleTypeDef *hdma)
{
  IRDA_HandleTypeDef *hirda = (IRDA_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;
  hirda->RxXferCount = 0x00U;
  hirda->TxXferCount = 0x00U;

#if (USE_HAL_IRDA_REGISTER_CALLBACKS == 1)
        /* Call registered user error callback */
  hirda->ErrorCallback(hirda);
#else
        /* Call legacy weak user error callback */
  HAL_IRDA_ErrorCallback(hirda);
#endif /* USE_HAL_IRDA_REGISTER_CALLBACK */
}



static void IRDA_DMATxAbortCallback(DMA_HandleTypeDef *hdma)
{
  IRDA_HandleTypeDef *hirda = (IRDA_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

  hirda->hdmatx->XferAbortCallback = NULL;

        /* Check if an Abort process is still ongoing */
  if (hirda->hdmarx != NULL)
  {
    if (hirda->hdmarx->XferAbortCallback != NULL)
    {
      return;
    }
  }



  hirda->TxXferCount = 0x00U;
  hirda->RxXferCount = 0x00U;

        /* Reset ErrorCode */
  hirda->ErrorCode = HAL_IRDA_ERROR_NONE;

        /* Restore hirda->gState and hirda->RxState to Ready */
  hirda->gState  = HAL_IRDA_STATE_READY;
  hirda->RxState = HAL_IRDA_STATE_READY;

        /* Call user Abort complete callback */
#if (USE_HAL_IRDA_REGISTER_CALLBACKS == 1)
        /* Call registered Abort complete callback */
  hirda->AbortCpltCallback(hirda);
#else
        /* Call legacy weak Abort complete callback */
  HAL_IRDA_AbortCpltCallback(hirda);
#endif /* USE_HAL_IRDA_REGISTER_CALLBACK */
}



static void IRDA_DMARxAbortCallback(DMA_HandleTypeDef *hdma)
{
  IRDA_HandleTypeDef *hirda = (IRDA_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

  hirda->hdmarx->XferAbortCallback = NULL;

        /* Check if an Abort process is still ongoing */
  if (hirda->hdmatx != NULL)
  {
    if (hirda->hdmatx->XferAbortCallback != NULL)
    {
      return;
    }
  }



  hirda->TxXferCount = 0x00U;
  hirda->RxXferCount = 0x00U;

        /* Reset ErrorCode */
  hirda->ErrorCode = HAL_IRDA_ERROR_NONE;

        /* Restore hirda->gState and hirda->RxState to Ready */
  hirda->gState  = HAL_IRDA_STATE_READY;
  hirda->RxState = HAL_IRDA_STATE_READY;

        /* Call user Abort complete callback */
#if (USE_HAL_IRDA_REGISTER_CALLBACKS == 1)
        /* Call registered Abort complete callback */
  hirda->AbortCpltCallback(hirda);
#else
        /* Call legacy weak Abort complete callback */
  HAL_IRDA_AbortCpltCallback(hirda);
#endif /* USE_HAL_IRDA_REGISTER_CALLBACK */
}



static void IRDA_DMATxOnlyAbortCallback(DMA_HandleTypeDef *hdma)
{
  IRDA_HandleTypeDef *hirda = (IRDA_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

  hirda->TxXferCount = 0x00U;

        /* Restore hirda->gState to Ready */
  hirda->gState = HAL_IRDA_STATE_READY;

        /* Call user Abort complete callback */
#if (USE_HAL_IRDA_REGISTER_CALLBACKS == 1)
        /* Call registered Abort Transmit Complete Callback */
  hirda->AbortTransmitCpltCallback(hirda);
#else
        /* Call legacy weak Abort Transmit Complete Callback */
  HAL_IRDA_AbortTransmitCpltCallback(hirda);
#endif /* USE_HAL_IRDA_REGISTER_CALLBACK */
}



static void IRDA_DMARxOnlyAbortCallback(DMA_HandleTypeDef *hdma)
{
  IRDA_HandleTypeDef *hirda = (IRDA_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

  hirda->RxXferCount = 0x00U;

        /* Restore hirda->RxState to Ready */
  hirda->RxState = HAL_IRDA_STATE_READY;

        /* Call user Abort complete callback */
#if (USE_HAL_IRDA_REGISTER_CALLBACKS == 1)
        /* Call registered Abort Receive Complete Callback */
  hirda->AbortReceiveCpltCallback(hirda);
#else
        /* Call legacy weak Abort Receive Complete Callback */
  HAL_IRDA_AbortReceiveCpltCallback(hirda);
#endif /* USE_HAL_IRDA_REGISTER_CALLBACK */
}



static HAL_StatusTypeDef IRDA_Transmit_IT(IRDA_HandleTypeDef *hirda)
{
  uint16_t *tmp;

        /* Check that a Tx process is ongoing */
  if (hirda->gState == HAL_IRDA_STATE_BUSY_TX)
  {
    if (hirda->Init.WordLength == IRDA_WORDLENGTH_9B)
    {
      tmp = (uint16_t *) hirda->pTxBuffPtr;
      hirda->Instance->DR = (uint16_t)(*tmp & (uint16_t)0x01FF);
      if (hirda->Init.Parity == IRDA_PARITY_NONE)
      {
        hirda->pTxBuffPtr += 2U;
      }
      else
      {
        hirda->pTxBuffPtr += 1U;
      }
    }
    else
    {
      hirda->Instance->DR = (uint8_t)(*hirda->pTxBuffPtr++ & (uint8_t)0x00FF);
    }

    if (--hirda->TxXferCount == 0U)
    {
                        /* Disable the IRDA Transmit Data Register Empty Interrupt */
      CLEAR_BIT(hirda->Instance->CR1, USART_CR1_TXEIE);

                        /* Enable the IRDA Transmit Complete Interrupt */
      SET_BIT(hirda->Instance->CR1, USART_CR1_TCIE);
    }

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}



static HAL_StatusTypeDef IRDA_EndTransmit_IT(IRDA_HandleTypeDef *hirda)
{
        /* Disable the IRDA Transmit Complete Interrupt */
  CLEAR_BIT(hirda->Instance->CR1, USART_CR1_TCIE);

        /* Disable the IRDA Error Interrupt: (Frame error, noise error, overrun error) */
  CLEAR_BIT(hirda->Instance->CR3, USART_CR3_EIE);

        /* Tx process is ended, restore hirda->gState to Ready */
  hirda->gState = HAL_IRDA_STATE_READY;

#if (USE_HAL_IRDA_REGISTER_CALLBACKS == 1)
        /* Call registered Tx complete callback */
  hirda->TxCpltCallback(hirda);
#else
        /* Call legacy weak Tx complete callback */
  HAL_IRDA_TxCpltCallback(hirda);
#endif /* USE_HAL_IRDA_REGISTER_CALLBACK */

  return HAL_OK;
}



static HAL_StatusTypeDef IRDA_Receive_IT(IRDA_HandleTypeDef *hirda)
{
  uint16_t *tmp;
  uint16_t  uhdata;

        /* Check that a Rx process is ongoing */
  if (hirda->RxState == HAL_IRDA_STATE_BUSY_RX)
  {
    uhdata = (uint16_t) READ_REG(hirda->Instance->DR);
    if (hirda->Init.WordLength == IRDA_WORDLENGTH_9B)
    {
      tmp = (uint16_t *) hirda->pRxBuffPtr;
      if (hirda->Init.Parity == IRDA_PARITY_NONE)
      {
        *tmp = (uint16_t)(uhdata & (uint16_t)0x01FF);
        hirda->pRxBuffPtr += 2U;
      }
      else
      {
        *tmp = (uint16_t)(uhdata & (uint16_t)0x00FF);
        hirda->pRxBuffPtr += 1U;
      }
    }
    else
    {
      if (hirda->Init.Parity == IRDA_PARITY_NONE)
      {
        *hirda->pRxBuffPtr++ = (uint8_t)(uhdata & (uint8_t)0x00FF);
      }
      else
      {
        *hirda->pRxBuffPtr++ = (uint8_t)(uhdata & (uint8_t)0x007F);
      }
    }

    if (--hirda->RxXferCount == 0U)
    {
                        /* Disable the IRDA Data Register not empty Interrupt */
      CLEAR_BIT(hirda->Instance->CR1, USART_CR1_RXNEIE);

                        /* Disable the IRDA Parity Error Interrupt */
      CLEAR_BIT(hirda->Instance->CR1, USART_CR1_PEIE);

                        /* Disable the IRDA Error Interrupt: (Frame error, noise error, overrun error) */
      CLEAR_BIT(hirda->Instance->CR3, USART_CR3_EIE);

                        /* Rx process is completed, restore hirda->RxState to Ready */
      hirda->RxState = HAL_IRDA_STATE_READY;

#if (USE_HAL_IRDA_REGISTER_CALLBACKS == 1)
                        /* Call registered Rx complete callback */
      hirda->RxCpltCallback(hirda);
#else
                        /* Call legacy weak Rx complete callback */
      HAL_IRDA_RxCpltCallback(hirda);
#endif /* USE_HAL_IRDA_REGISTER_CALLBACKS */

      return HAL_OK;
    }
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}



static void IRDA_SetConfig(IRDA_HandleTypeDef *hirda)
{
  uint32_t pclk;

        /* Check the parameters */
  assert_param(IS_IRDA_INSTANCE(hirda->Instance));
  assert_param(IS_IRDA_BAUDRATE(hirda->Init.BaudRate));
  assert_param(IS_IRDA_WORD_LENGTH(hirda->Init.WordLength));
  assert_param(IS_IRDA_PARITY(hirda->Init.Parity));
  assert_param(IS_IRDA_MODE(hirda->Init.Mode));
  assert_param(IS_IRDA_POWERMODE(hirda->Init.IrDAMode));

        /* -------------------------- USART CR2 Configuration ------------------------ */
        /* Clear STOP[13:12] bits */
  CLEAR_BIT(hirda->Instance->CR2, USART_CR2_STOP);

        /* -------------------------- USART CR1 Configuration ----------------------- */
        /* Clear M, PCE, PS, TE and RE bits */
  CLEAR_BIT(hirda->Instance->CR1, (USART_CR1_M | USART_CR1_PCE | USART_CR1_PS | USART_CR1_TE | USART_CR1_RE));



        /* Write to USART CR1 */
  SET_BIT(hirda->Instance->CR1, (hirda->Init.WordLength | hirda->Init.Parity | hirda->Init.Mode));

        /* -------------------------- USART CR3 Configuration ----------------------- */
        /* Clear CTSE and RTSE bits */
  CLEAR_BIT(hirda->Instance->CR3, (USART_CR3_RTSE | USART_CR3_CTSE));

        /* -------------------------- USART BRR Configuration ----------------------- */
  if(hirda->Instance == USART1)
  {
    pclk = HAL_RCC_GetPCLK2Freq();
    SET_BIT(hirda->Instance->BRR, IRDA_BRR(pclk, hirda->Init.BaudRate));
  }
  else
  {
    pclk = HAL_RCC_GetPCLK1Freq();
    SET_BIT(hirda->Instance->BRR, IRDA_BRR(pclk, hirda->Init.BaudRate));
  }
}

/* @} */

#endif /* HAL_IRDA_MODULE_ENABLED */
/* @} */

/* @} */

/* (C) COPYRIGHT Puya *****END OF FILE */
