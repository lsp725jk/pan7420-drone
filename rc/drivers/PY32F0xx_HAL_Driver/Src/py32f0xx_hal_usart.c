/**
 * @file    py32f0xx_hal_usart.c
 * @author  MCU Application Team
 * @brief   USART HAL module driver.
 */

/* Includes ------------------------------------------------------------------ */
#include "py32f0xx_hal.h"

/**
 * @addtogroup PY32F0xx_HAL_Driver @{
 */

/* @defgroup USART USART @brief HAL USART Synchronous module driver @{ */
#ifdef HAL_USART_MODULE_ENABLED
/* Private typedef ----------------------------------------------------------- */
/* Private define ------------------------------------------------------------ */
/* @addtogroup USART_Private_Constants @{ */
#define DUMMY_DATA           0xFFFFU
/* @} */
/* Private macro ------------------------------------------------------------- */
/* Private variables --------------------------------------------------------- */
/* Private function prototypes ----------------------------------------------- */
/* Private functions --------------------------------------------------------- */
/* @addtogroup USART_Private_Functions @{ */
#if (USE_HAL_USART_REGISTER_CALLBACKS == 1)
void USART_InitCallbacksToDefault(USART_HandleTypeDef *husart);
#endif /* USE_HAL_USART_REGISTER_CALLBACKS */
static void USART_EndRxTransfer(USART_HandleTypeDef *husart);
static HAL_StatusTypeDef USART_Transmit_IT(USART_HandleTypeDef *husart);
static HAL_StatusTypeDef USART_EndTransmit_IT(USART_HandleTypeDef *husart);
static HAL_StatusTypeDef USART_Receive_IT(USART_HandleTypeDef *husart);
static HAL_StatusTypeDef USART_TransmitReceive_IT(USART_HandleTypeDef *husart);
static void USART_SetConfig(USART_HandleTypeDef *husart);
#if (defined(DMA1) || defined(DMA))
static void USART_EndTxTransfer(USART_HandleTypeDef *husart);
static void USART_DMATransmitCplt(DMA_HandleTypeDef *hdma);
static void USART_DMATxHalfCplt(DMA_HandleTypeDef *hdma);
static void USART_DMAReceiveCplt(DMA_HandleTypeDef *hdma);
static void USART_DMARxHalfCplt(DMA_HandleTypeDef *hdma);
static void USART_DMAError(DMA_HandleTypeDef *hdma);
static void USART_DMAAbortOnError(DMA_HandleTypeDef *hdma);
static void USART_DMATxAbortCallback(DMA_HandleTypeDef *hdma);
static void USART_DMARxAbortCallback(DMA_HandleTypeDef *hdma);
#endif /* DMA1 or DMA */
static HAL_StatusTypeDef USART_WaitOnFlagUntilTimeout(USART_HandleTypeDef *husart, uint32_t Flag, FlagStatus Status, uint32_t Tickstart, uint32_t Timeout);
/* @} */

/* Exported functions -------------------------------------------------------- */
/* @defgroup USART_Exported_Functions USART Exported Functions @{ */



HAL_StatusTypeDef HAL_USART_Init(USART_HandleTypeDef *husart)
{
        /* Check the USART handle allocation */
  if (husart == NULL)
  {
    return HAL_ERROR;
  }

        /* Check the parameters */
  assert_param(IS_USART_INSTANCE(husart->Instance));

  if (husart->State == HAL_USART_STATE_RESET)
  {
                /* Allocate lock resource and initialize it */
    husart->Lock = HAL_UNLOCKED;

#if (USE_HAL_USART_REGISTER_CALLBACKS == 1)
    USART_InitCallbacksToDefault(husart);

    if (husart->MspInitCallback == NULL)
    {
      husart->MspInitCallback = HAL_USART_MspInit;
    }

                /* Init the low level hardware */
    husart->MspInitCallback(husart);
#else
                /* Init the low level hardware : GPIO, CLOCK */
    HAL_USART_MspInit(husart);
#endif /* USE_HAL_USART_REGISTER_CALLBACKS */
  }

  husart->State = HAL_USART_STATE_BUSY;

        /* Set the USART Communication parameters */
  USART_SetConfig(husart);

        /* Enable the Peripheral */
  __HAL_USART_ENABLE(husart);

        /* Initialize the USART state */
  husart->ErrorCode = HAL_USART_ERROR_NONE;
  husart->State = HAL_USART_STATE_READY;

  return HAL_OK;
}

/* @brief  DeInitializes the USART peripheral. @param  husart Pointer to a USART_HandleTypeDef structure that contains @retval HAL status */
HAL_StatusTypeDef HAL_USART_DeInit(USART_HandleTypeDef *husart)
{
        /* Check the USART handle allocation */
  if (husart == NULL)
  {
    return HAL_ERROR;
  }

        /* Check the parameters */
  assert_param(IS_USART_INSTANCE(husart->Instance));

  husart->State = HAL_USART_STATE_BUSY;

        /* Disable the Peripheral */
  __HAL_USART_DISABLE(husart);

#if (USE_HAL_USART_REGISTER_CALLBACKS == 1)
  if (husart->MspDeInitCallback == NULL)
  {
    husart->MspDeInitCallback = HAL_USART_MspDeInit;
  }
        /* DeInit the low level hardware */
  husart->MspDeInitCallback(husart);
#else
        /* DeInit the low level hardware */
  HAL_USART_MspDeInit(husart);
#endif /* USE_HAL_USART_REGISTER_CALLBACKS */

  husart->ErrorCode = HAL_USART_ERROR_NONE;
  husart->State = HAL_USART_STATE_RESET;

        /* Release Lock */
  __HAL_UNLOCK(husart);

  return HAL_OK;
}

/* @brief  USART MSP Init. @param  husart Pointer to a USART_HandleTypeDef structure that contains @retval None */
__weak void HAL_USART_MspInit(USART_HandleTypeDef *husart)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(husart);

}

/* @brief  USART MSP DeInit. @param  husart Pointer to a USART_HandleTypeDef structure that contains @retval None */
__weak void HAL_USART_MspDeInit(USART_HandleTypeDef *husart)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(husart);

}

#if (USE_HAL_USART_REGISTER_CALLBACKS == 1)

HAL_StatusTypeDef HAL_USART_RegisterCallback(USART_HandleTypeDef *husart, HAL_USART_CallbackIDTypeDef CallbackID, pUSART_CallbackTypeDef pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (pCallback == NULL)
  {
                /* Update the error code */
    husart->ErrorCode |= HAL_USART_ERROR_INVALID_CALLBACK;

    return HAL_ERROR;
  }
        /* Process locked */
  __HAL_LOCK(husart);

  if (husart->State == HAL_USART_STATE_READY)
  {
    switch (CallbackID)
    {
    case HAL_USART_TX_HALFCOMPLETE_CB_ID :
      husart->TxHalfCpltCallback = pCallback;
      break;

    case HAL_USART_TX_COMPLETE_CB_ID :
      husart->TxCpltCallback = pCallback;
      break;

    case HAL_USART_RX_HALFCOMPLETE_CB_ID :
      husart->RxHalfCpltCallback = pCallback;
      break;

    case HAL_USART_RX_COMPLETE_CB_ID :
      husart->RxCpltCallback = pCallback;
      break;

    case HAL_USART_TX_RX_COMPLETE_CB_ID :
      husart->TxRxCpltCallback = pCallback;
      break;

    case HAL_USART_ERROR_CB_ID :
      husart->ErrorCallback = pCallback;
      break;

    case HAL_USART_ABORT_COMPLETE_CB_ID :
      husart->AbortCpltCallback = pCallback;
      break;

    case HAL_USART_MSPINIT_CB_ID :
      husart->MspInitCallback = pCallback;
      break;

    case HAL_USART_MSPDEINIT_CB_ID :
      husart->MspDeInitCallback = pCallback;
      break;

    default :
                        /* Update the error code */
      husart->ErrorCode |= HAL_USART_ERROR_INVALID_CALLBACK;

                        /* Return error status */
      status =  HAL_ERROR;
      break;
    }
  }
  else if (husart->State == HAL_USART_STATE_RESET)
  {
    switch (CallbackID)
    {
    case HAL_USART_MSPINIT_CB_ID :
      husart->MspInitCallback = pCallback;
      break;

    case HAL_USART_MSPDEINIT_CB_ID :
      husart->MspDeInitCallback = pCallback;
      break;

    default :
                        /* Update the error code */
      husart->ErrorCode |= HAL_USART_ERROR_INVALID_CALLBACK;

                        /* Return error status */
      status =  HAL_ERROR;
      break;
    }
  }
  else
  {
                /* Update the error code */
    husart->ErrorCode |= HAL_USART_ERROR_INVALID_CALLBACK;

                /* Return error status */
    status =  HAL_ERROR;
  }

        /* Release Lock */
  __HAL_UNLOCK(husart);

  return status;
}



HAL_StatusTypeDef HAL_USART_UnRegisterCallback(USART_HandleTypeDef *husart, HAL_USART_CallbackIDTypeDef CallbackID)
{
  HAL_StatusTypeDef status = HAL_OK;

        /* Process locked */
  __HAL_LOCK(husart);

  if (husart->State == HAL_USART_STATE_READY)
  {
    switch (CallbackID)
    {
    case HAL_USART_TX_HALFCOMPLETE_CB_ID :
      husart->TxHalfCpltCallback = HAL_USART_TxHalfCpltCallback;                           /* Legacy weak  TxHalfCpltCallback */
      break;

    case HAL_USART_TX_COMPLETE_CB_ID :
      husart->TxCpltCallback = HAL_USART_TxCpltCallback;                                   /* Legacy weak TxCpltCallback */
      break;

    case HAL_USART_RX_HALFCOMPLETE_CB_ID :
      husart->RxHalfCpltCallback = HAL_USART_RxHalfCpltCallback;                           /* Legacy weak RxHalfCpltCallback */
      break;

    case HAL_USART_RX_COMPLETE_CB_ID :
      husart->RxCpltCallback = HAL_USART_RxCpltCallback;                                   /* Legacy weak RxCpltCallback */
      break;

    case HAL_USART_TX_RX_COMPLETE_CB_ID :
      husart->TxRxCpltCallback = HAL_USART_TxRxCpltCallback;                               /* Legacy weak TxRxCpltCallback */
      break;

    case HAL_USART_ERROR_CB_ID :
      husart->ErrorCallback = HAL_USART_ErrorCallback;                                     /* Legacy weak ErrorCallback */
      break;

    case HAL_USART_ABORT_COMPLETE_CB_ID :
      husart->AbortCpltCallback = HAL_USART_AbortCpltCallback;                             /* Legacy weak AbortCpltCallback */
      break;

    case HAL_USART_MSPINIT_CB_ID :
      husart->MspInitCallback = HAL_USART_MspInit;                                         /* Legacy weak MspInitCallback */
      break;

    case HAL_USART_MSPDEINIT_CB_ID :
      husart->MspDeInitCallback = HAL_USART_MspDeInit;                                     /* Legacy weak MspDeInitCallback */
      break;

    default :
                        /* Update the error code */
      husart->ErrorCode |= HAL_USART_ERROR_INVALID_CALLBACK;

                        /* Return error status */
      status =  HAL_ERROR;
      break;
    }
  }
  else if (husart->State == HAL_USART_STATE_RESET)
  {
    switch (CallbackID)
    {
    case HAL_USART_MSPINIT_CB_ID :
      husart->MspInitCallback = HAL_USART_MspInit;
      break;

    case HAL_USART_MSPDEINIT_CB_ID :
      husart->MspDeInitCallback = HAL_USART_MspDeInit;
      break;

    default :
                        /* Update the error code */
      husart->ErrorCode |= HAL_USART_ERROR_INVALID_CALLBACK;

                        /* Return error status */
      status =  HAL_ERROR;
      break;
    }
  }
  else
  {
                /* Update the error code */
    husart->ErrorCode |= HAL_USART_ERROR_INVALID_CALLBACK;

                /* Return error status */
    status =  HAL_ERROR;
  }

        /* Release Lock */
  __HAL_UNLOCK(husart);

  return status;
}
#endif /* USE_HAL_USART_REGISTER_CALLBACKS */

/* @} */



HAL_StatusTypeDef HAL_USART_Transmit(USART_HandleTypeDef *husart, uint8_t *pTxData, uint16_t Size, uint32_t Timeout)
{
  uint16_t *tmp;
  uint32_t tickstart = 0U;

  if (husart->State == HAL_USART_STATE_READY)
  {
    if ((pTxData == NULL) || (Size == 0))
    {
      return  HAL_ERROR;
    }

                /* Process Locked */
    __HAL_LOCK(husart);

    husart->ErrorCode = HAL_USART_ERROR_NONE;
    husart->State = HAL_USART_STATE_BUSY_TX;

                /* Init tickstart for timeout management */
    tickstart = HAL_GetTick();

    husart->TxXferSize = Size;
    husart->TxXferCount = Size;
    while (husart->TxXferCount > 0U)
    {
      husart->TxXferCount--;
      if (husart->Init.WordLength == USART_WORDLENGTH_9B)
      {
                                /* Wait for TC flag in order to write data in DR */
        if (USART_WaitOnFlagUntilTimeout(husart, USART_FLAG_TXE, RESET, tickstart, Timeout) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }
        tmp = (uint16_t *) pTxData;
        husart->Instance->DR = (*tmp & (uint16_t)0x01FF);
        if (husart->Init.Parity == USART_PARITY_NONE)
        {
          pTxData += 2U;
        }
        else
        {
          pTxData += 1U;
        }
      }
      else
      {
        if (USART_WaitOnFlagUntilTimeout(husart, USART_FLAG_TXE, RESET, tickstart, Timeout) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }
        husart->Instance->DR = (*pTxData++ & (uint8_t)0xFF);
      }
    }

    if (USART_WaitOnFlagUntilTimeout(husart, USART_FLAG_TC, RESET, tickstart, Timeout) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }

    husart->State = HAL_USART_STATE_READY;

                /* Process Unlocked */
    __HAL_UNLOCK(husart);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}



HAL_StatusTypeDef HAL_USART_Receive(USART_HandleTypeDef *husart, uint8_t *pRxData, uint16_t Size, uint32_t Timeout)
{
  uint16_t *tmp;
  uint32_t tickstart = 0U;

  if (husart->State == HAL_USART_STATE_READY)
  {
    if ((pRxData == NULL) || (Size == 0))
    {
      return  HAL_ERROR;
    }
                /* Process Locked */
    __HAL_LOCK(husart);

    husart->ErrorCode = HAL_USART_ERROR_NONE;
    husart->State = HAL_USART_STATE_BUSY_RX;

                /* Init tickstart for timeout management */
    tickstart = HAL_GetTick();

    husart->RxXferSize = Size;
    husart->RxXferCount = Size;
                /* Check the remain data to be received */
    while (husart->RxXferCount > 0U)
    {
      husart->RxXferCount--;
      if (husart->Init.WordLength == USART_WORDLENGTH_9B)
      {

        if (USART_WaitOnFlagUntilTimeout(husart, USART_FLAG_TXE, RESET, tickstart, Timeout) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }
                                /* Send dummy byte in order to generate clock */
        husart->Instance->DR = (DUMMY_DATA & (uint16_t)0x01FF);

                                /* Wait for RXNE Flag */
        if (USART_WaitOnFlagUntilTimeout(husart, USART_FLAG_RXNE, RESET, tickstart, Timeout) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }
        tmp = (uint16_t *) pRxData ;
        if (husart->Init.Parity == USART_PARITY_NONE)
        {
          *tmp = (uint16_t)(husart->Instance->DR & (uint16_t)0x01FF);
          pRxData += 2U;
        }
        else
        {
          *tmp = (uint16_t)(husart->Instance->DR & (uint16_t)0x00FF);
          pRxData += 1U;
        }
      }
      else
      {

        if (USART_WaitOnFlagUntilTimeout(husart, USART_FLAG_TXE, RESET, tickstart, Timeout) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }

                                /* Send Dummy Byte in order to generate clock */
        husart->Instance->DR = (DUMMY_DATA & (uint16_t)0x00FF);

                                /* Wait until RXNE flag is set to receive the byte */
        if (USART_WaitOnFlagUntilTimeout(husart, USART_FLAG_RXNE, RESET, tickstart, Timeout) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }
        if (husart->Init.Parity == USART_PARITY_NONE)
        {
                                        /* Receive data */
          *pRxData++ = (uint8_t)(husart->Instance->DR & (uint8_t)0x00FF);
        }
        else
        {
                                        /* Receive data */
          *pRxData++ = (uint8_t)(husart->Instance->DR & (uint8_t)0x007F);
        }

      }
    }

    husart->State = HAL_USART_STATE_READY;

                /* Process Unlocked */
    __HAL_UNLOCK(husart);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}



HAL_StatusTypeDef HAL_USART_TransmitReceive(USART_HandleTypeDef *husart, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout)
{
  uint16_t *tmp;
  uint32_t tickstart = 0U;

  if (husart->State == HAL_USART_STATE_READY)
  {
    if ((pTxData == NULL) || (pRxData == NULL) || (Size == 0))
    {
      return  HAL_ERROR;
    }
                /* Process Locked */
    __HAL_LOCK(husart);

    husart->ErrorCode = HAL_USART_ERROR_NONE;
    husart->State = HAL_USART_STATE_BUSY_RX;

                /* Init tickstart for timeout management */
    tickstart = HAL_GetTick();

    husart->RxXferSize = Size;
    husart->TxXferSize = Size;
    husart->TxXferCount = Size;
    husart->RxXferCount = Size;

                /* Check the remain data to be received */
    while (husart->TxXferCount > 0U)
    {
      husart->TxXferCount--;
      husart->RxXferCount--;
      if (husart->Init.WordLength == USART_WORDLENGTH_9B)
      {
                                /* Wait for TC flag in order to write data in DR */
        if (USART_WaitOnFlagUntilTimeout(husart, USART_FLAG_TXE, RESET, tickstart, Timeout) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }
        tmp = (uint16_t *) pTxData;
        husart->Instance->DR = (*tmp & (uint16_t)0x01FF);
        if (husart->Init.Parity == USART_PARITY_NONE)
        {
          pTxData += 2U;
        }
        else
        {
          pTxData += 1U;
        }

                                /* Wait for RXNE Flag */
        if (USART_WaitOnFlagUntilTimeout(husart, USART_FLAG_RXNE, RESET, tickstart, Timeout) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }
        tmp = (uint16_t *) pRxData ;
        if (husart->Init.Parity == USART_PARITY_NONE)
        {
          *tmp = (uint16_t)(husart->Instance->DR & (uint16_t)0x01FF);
          pRxData += 2U;
        }
        else
        {
          *tmp = (uint16_t)(husart->Instance->DR & (uint16_t)0x00FF);
          pRxData += 1U;
        }
      }
      else
      {
                                /* Wait for TC flag in order to write data in DR */
        if (USART_WaitOnFlagUntilTimeout(husart, USART_FLAG_TXE, RESET, tickstart, Timeout) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }
        husart->Instance->DR = (*pTxData++ & (uint8_t)0x00FF);

                                /* Wait for RXNE Flag */
        if (USART_WaitOnFlagUntilTimeout(husart, USART_FLAG_RXNE, RESET, tickstart, Timeout) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }
        if (husart->Init.Parity == USART_PARITY_NONE)
        {
                                        /* Receive data */
          *pRxData++ = (uint8_t)(husart->Instance->DR & (uint8_t)0x00FF);
        }
        else
        {
                                        /* Receive data */
          *pRxData++ = (uint8_t)(husart->Instance->DR & (uint8_t)0x007F);
        }
      }
    }

    husart->State = HAL_USART_STATE_READY;

                /* Process Unlocked */
    __HAL_UNLOCK(husart);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}



HAL_StatusTypeDef HAL_USART_Transmit_IT(USART_HandleTypeDef *husart, uint8_t *pTxData, uint16_t Size)
{
  if (husart->State == HAL_USART_STATE_READY)
  {
    if ((pTxData == NULL) || (Size == 0))
    {
      return HAL_ERROR;
    }

                /* Process Locked */
    __HAL_LOCK(husart);

    husart->pTxBuffPtr = pTxData;
    husart->TxXferSize = Size;
    husart->TxXferCount = Size;

    husart->ErrorCode = HAL_USART_ERROR_NONE;
    husart->State = HAL_USART_STATE_BUSY_TX;

                /* configured only for transmit "USART_MODE_TX" Noise error interrupt */

                /* Process Unlocked */
    __HAL_UNLOCK(husart);

                /* Enable the USART Transmit Data Register Empty Interrupt */
    SET_BIT(husart->Instance->CR1, USART_CR1_TXEIE);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}



HAL_StatusTypeDef HAL_USART_Receive_IT(USART_HandleTypeDef *husart, uint8_t *pRxData, uint16_t Size)
{
  if (husart->State == HAL_USART_STATE_READY)
  {
    if ((pRxData == NULL) || (Size == 0))
    {
      return HAL_ERROR;
    }
                /* Process Locked */
    __HAL_LOCK(husart);

    husart->pRxBuffPtr = pRxData;
    husart->RxXferSize = Size;
    husart->RxXferCount = Size;

    husart->ErrorCode = HAL_USART_ERROR_NONE;
    husart->State = HAL_USART_STATE_BUSY_RX;

                /* Process Unlocked */
    __HAL_UNLOCK(husart);

                /* Enable the USART Parity Error and Data Register not empty Interrupts */
    SET_BIT(husart->Instance->CR1, USART_CR1_PEIE | USART_CR1_RXNEIE);

                /* Enable the USART Error Interrupt: (Frame error, noise error, overrun error) */
    SET_BIT(husart->Instance->CR3, USART_CR3_EIE);

                /* Send dummy byte in order to generate the clock for the slave to send data */
    husart->Instance->DR = (DUMMY_DATA & (uint16_t)0x01FF);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}



HAL_StatusTypeDef HAL_USART_TransmitReceive_IT(USART_HandleTypeDef *husart, uint8_t *pTxData, uint8_t *pRxData,  uint16_t Size)
{
  if (husart->State == HAL_USART_STATE_READY)
  {
    if ((pTxData == NULL) || (pRxData == NULL) || (Size == 0))
    {
      return HAL_ERROR;
    }
                /* Process Locked */
    __HAL_LOCK(husart);

    husart->pRxBuffPtr = pRxData;
    husart->RxXferSize = Size;
    husart->RxXferCount = Size;
    husart->pTxBuffPtr = pTxData;
    husart->TxXferSize = Size;
    husart->TxXferCount = Size;

    husart->ErrorCode = HAL_USART_ERROR_NONE;
    husart->State = HAL_USART_STATE_BUSY_TX_RX;

                /* Process Unlocked */
    __HAL_UNLOCK(husart);

                /* Enable the USART Data Register not empty Interrupt */
    SET_BIT(husart->Instance->CR1, USART_CR1_RXNEIE);

                /* Enable the USART Parity Error Interrupt */
    SET_BIT(husart->Instance->CR1, USART_CR1_PEIE);

                /* Enable the USART Error Interrupt: (Frame error, noise error, overrun error) */
    SET_BIT(husart->Instance->CR3, USART_CR3_EIE);

                /* Enable the USART Transmit Data Register Empty Interrupt */
    SET_BIT(husart->Instance->CR1, USART_CR1_TXEIE);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

#if (defined(DMA1) || defined(DMA))

HAL_StatusTypeDef HAL_USART_Transmit_DMA(USART_HandleTypeDef *husart, uint8_t *pTxData, uint16_t Size)
{
  uint32_t *tmp;

  if (husart->State == HAL_USART_STATE_READY)
  {
    if ((pTxData == NULL) || (Size == 0))
    {
      return HAL_ERROR;
    }
                /* Process Locked */
    __HAL_LOCK(husart);

    husart->pTxBuffPtr = pTxData;
    husart->TxXferSize = Size;
    husart->TxXferCount = Size;

    husart->ErrorCode = HAL_USART_ERROR_NONE;
    husart->State = HAL_USART_STATE_BUSY_TX;

                /* Set the USART DMA transfer complete callback */
    husart->hdmatx->XferCpltCallback = USART_DMATransmitCplt;

                /* Set the USART DMA Half transfer complete callback */
    husart->hdmatx->XferHalfCpltCallback = USART_DMATxHalfCplt;

                /* Set the DMA error callback */
    husart->hdmatx->XferErrorCallback = USART_DMAError;

                /* Set the DMA abort callback */
    husart->hdmatx->XferAbortCallback = NULL;

                /* Enable the USART transmit DMA channel */
    tmp = (uint32_t *)&pTxData;
    HAL_DMA_Start_IT(husart->hdmatx, *(uint32_t *)tmp, (uint32_t)&husart->Instance->DR, Size);

                /* Clear the TC flag in the SR register by writing 0 to it */
    __HAL_USART_CLEAR_FLAG(husart, USART_FLAG_TC);

                /* Process Unlocked */
    __HAL_UNLOCK(husart);



    SET_BIT(husart->Instance->CR3, USART_CR3_DMAT);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}



HAL_StatusTypeDef HAL_USART_Receive_DMA(USART_HandleTypeDef *husart, uint8_t *pRxData, uint16_t Size)
{
  uint32_t *tmp;

  if (husart->State == HAL_USART_STATE_READY)
  {
    if ((pRxData == NULL) || (Size == 0))
    {
      return HAL_ERROR;
    }

                /* Process Locked */
    __HAL_LOCK(husart);

    husart->pRxBuffPtr = pRxData;
    husart->RxXferSize = Size;
    husart->pTxBuffPtr = pRxData;
    husart->TxXferSize = Size;

    husart->ErrorCode = HAL_USART_ERROR_NONE;
    husart->State = HAL_USART_STATE_BUSY_RX;

                /* Set the USART DMA Rx transfer complete callback */
    husart->hdmarx->XferCpltCallback = USART_DMAReceiveCplt;

                /* Set the USART DMA Half transfer complete callback */
    husart->hdmarx->XferHalfCpltCallback = USART_DMARxHalfCplt;

                /* Set the USART DMA Rx transfer error callback */
    husart->hdmarx->XferErrorCallback = USART_DMAError;

                /* Set the DMA abort callback */
    husart->hdmarx->XferAbortCallback = NULL;



    husart->hdmatx->XferHalfCpltCallback = NULL;
    husart->hdmatx->XferCpltCallback = NULL;

                /* Set the DMA error callback */
    husart->hdmatx->XferErrorCallback = USART_DMAError;

                /* Set the DMA AbortCpltCallback */
    husart->hdmatx->XferAbortCallback = NULL;

                /* Enable the USART receive DMA channel */
    tmp = (uint32_t *)&pRxData;
    HAL_DMA_Start_IT(husart->hdmarx, (uint32_t)&husart->Instance->DR, *(uint32_t *)tmp, Size);



    HAL_DMA_Start_IT(husart->hdmatx, *(uint32_t *)tmp, (uint32_t)&husart->Instance->DR, Size);



    __HAL_USART_CLEAR_OREFLAG(husart);

                /* Process Unlocked */
    __HAL_UNLOCK(husart);

                /* Enable the USART Parity Error Interrupt */
    SET_BIT(husart->Instance->CR1, USART_CR1_PEIE);

                /* Enable the USART Error Interrupt: (Frame error, noise error, overrun error) */
    SET_BIT(husart->Instance->CR3, USART_CR3_EIE);



    SET_BIT(husart->Instance->CR3, USART_CR3_DMAR);



    SET_BIT(husart->Instance->CR3, USART_CR3_DMAT);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}



HAL_StatusTypeDef HAL_USART_TransmitReceive_DMA(USART_HandleTypeDef *husart, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size)
{
  uint32_t *tmp;

  if (husart->State == HAL_USART_STATE_READY)
  {
    if ((pTxData == NULL) || (pRxData == NULL) || (Size == 0))
    {
      return HAL_ERROR;
    }
                /* Process Locked */
    __HAL_LOCK(husart);

    husart->pRxBuffPtr = pRxData;
    husart->RxXferSize = Size;
    husart->pTxBuffPtr = pTxData;
    husart->TxXferSize = Size;

    husart->ErrorCode = HAL_USART_ERROR_NONE;
    husart->State = HAL_USART_STATE_BUSY_TX_RX;

                /* Set the USART DMA Rx transfer complete callback */
    husart->hdmarx->XferCpltCallback = USART_DMAReceiveCplt;

                /* Set the USART DMA Half transfer complete callback */
    husart->hdmarx->XferHalfCpltCallback = USART_DMARxHalfCplt;

                /* Set the USART DMA Tx transfer complete callback */
    husart->hdmatx->XferCpltCallback = USART_DMATransmitCplt;

                /* Set the USART DMA Half transfer complete callback */
    husart->hdmatx->XferHalfCpltCallback = USART_DMATxHalfCplt;

                /* Set the USART DMA Tx transfer error callback */
    husart->hdmatx->XferErrorCallback = USART_DMAError;

                /* Set the USART DMA Rx transfer error callback */
    husart->hdmarx->XferErrorCallback = USART_DMAError;

                /* Set the DMA abort callback */
    husart->hdmarx->XferAbortCallback = NULL;

                /* Enable the USART receive DMA channel */
    tmp = (uint32_t *)&pRxData;
    HAL_DMA_Start_IT(husart->hdmarx, (uint32_t)&husart->Instance->DR, *(uint32_t *)tmp, Size);

                /* Enable the USART transmit DMA channel */
    tmp = (uint32_t *)&pTxData;
    HAL_DMA_Start_IT(husart->hdmatx, *(uint32_t *)tmp, (uint32_t)&husart->Instance->DR, Size);

                /* Clear the TC flag in the SR register by writing 0 to it */
    __HAL_USART_CLEAR_FLAG(husart, USART_FLAG_TC);

                /* Clear the Overrun flag: mandatory for the second transfer in circular mode */
    __HAL_USART_CLEAR_OREFLAG(husart);

                /* Process Unlocked */
    __HAL_UNLOCK(husart);

                /* Enable the USART Parity Error Interrupt */
    SET_BIT(husart->Instance->CR1, USART_CR1_PEIE);

                /* Enable the USART Error Interrupt: (Frame error, noise error, overrun error) */
    SET_BIT(husart->Instance->CR3, USART_CR3_EIE);



    SET_BIT(husart->Instance->CR3, USART_CR3_DMAR);



    SET_BIT(husart->Instance->CR3, USART_CR3_DMAT);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}



HAL_StatusTypeDef HAL_USART_DMAPause(USART_HandleTypeDef *husart)
{
        /* Process Locked */
  __HAL_LOCK(husart);

        /* Disable the USART DMA Tx request */
  CLEAR_BIT(husart->Instance->CR3, USART_CR3_DMAT);

        /* Process Unlocked */
  __HAL_UNLOCK(husart);

  return HAL_OK;
}



HAL_StatusTypeDef HAL_USART_DMAResume(USART_HandleTypeDef *husart)
{
        /* Process Locked */
  __HAL_LOCK(husart);

        /* Enable the USART DMA Tx request */
  SET_BIT(husart->Instance->CR3, USART_CR3_DMAT);

        /* Process Unlocked */
  __HAL_UNLOCK(husart);

  return HAL_OK;
}



HAL_StatusTypeDef HAL_USART_DMAStop(USART_HandleTypeDef *husart)
{
  uint32_t dmarequest = 0x00U;



        /* Stop USART DMA Tx request if ongoing */
  dmarequest = HAL_IS_BIT_SET(husart->Instance->CR3, USART_CR3_DMAT);
  if ((husart->State == HAL_USART_STATE_BUSY_TX) && dmarequest)
  {
    USART_EndTxTransfer(husart);

                /* Abort the USART DMA Tx channel */
    if (husart->hdmatx != NULL)
    {
      HAL_DMA_Abort(husart->hdmatx);
    }

                /* Disable the USART Tx DMA request */
    CLEAR_BIT(husart->Instance->CR3, USART_CR3_DMAT);
  }

        /* Stop USART DMA Rx request if ongoing */
  dmarequest = HAL_IS_BIT_SET(husart->Instance->CR3, USART_CR3_DMAR);
  if ((husart->State == HAL_USART_STATE_BUSY_RX) && dmarequest)
  {
    USART_EndRxTransfer(husart);

                /* Abort the USART DMA Rx channel */
    if (husart->hdmarx != NULL)
    {
      HAL_DMA_Abort(husart->hdmarx);
    }

                /* Disable the USART Rx DMA request */
    CLEAR_BIT(husart->Instance->CR3, USART_CR3_DMAR);
  }

  return HAL_OK;
}
#endif /* DMA1 or DMA */



HAL_StatusTypeDef HAL_USART_Abort(USART_HandleTypeDef *husart)
{
        /* Disable TXEIE, TCIE, RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts */
  CLEAR_BIT(husart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE | USART_CR1_TXEIE | USART_CR1_TCIE));
  CLEAR_BIT(husart->Instance->CR3, USART_CR3_EIE);

#if (defined(DMA1) || defined(DMA))
        /* Disable the USART DMA Tx request if enabled */
  if (HAL_IS_BIT_SET(husart->Instance->CR3, USART_CR3_DMAT))
  {
    CLEAR_BIT(husart->Instance->CR3, USART_CR3_DMAT);

                /* Abort the USART DMA Tx channel : use blocking DMA Abort API (no callback) */
    if (husart->hdmatx != NULL)
    {

      husart->hdmatx->XferAbortCallback = NULL;

      HAL_DMA_Abort(husart->hdmatx);
    }
  }

        /* Disable the USART DMA Rx request if enabled */
  if (HAL_IS_BIT_SET(husart->Instance->CR3, USART_CR3_DMAR))
  {
    CLEAR_BIT(husart->Instance->CR3, USART_CR3_DMAR);

                /* Abort the USART DMA Rx channel : use blocking DMA Abort API (no callback) */
    if (husart->hdmarx != NULL)
    {

      husart->hdmarx->XferAbortCallback = NULL;

      HAL_DMA_Abort(husart->hdmarx);
    }
  }
#endif /* DMA1 or DMA */

        /* Reset Tx and Rx transfer counters */
  husart->TxXferCount = 0x00U;
  husart->RxXferCount = 0x00U;

        /* Restore husart->State to Ready */
  husart->State  = HAL_USART_STATE_READY;

        /* Reset Handle ErrorCode to No Error */
  husart->ErrorCode = HAL_USART_ERROR_NONE;

  return HAL_OK;
}



HAL_StatusTypeDef HAL_USART_Abort_IT(USART_HandleTypeDef *husart)
{
  uint32_t AbortCplt = 0x01U;

        /* Disable TXEIE, TCIE, RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts */
  CLEAR_BIT(husart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE | USART_CR1_TXEIE | USART_CR1_TCIE));
  CLEAR_BIT(husart->Instance->CR3, USART_CR3_EIE);

#if (defined(DMA1) || defined(DMA))

        /* DMA Tx Handle is valid */
  if (husart->hdmatx != NULL)
  {

    if (HAL_IS_BIT_SET(husart->Instance->CR3, USART_CR3_DMAT))
    {
      husart->hdmatx->XferAbortCallback = USART_DMATxAbortCallback;
    }
    else
    {
      husart->hdmatx->XferAbortCallback = NULL;
    }
  }
        /* DMA Rx Handle is valid */
  if (husart->hdmarx != NULL)
  {

    if (HAL_IS_BIT_SET(husart->Instance->CR3, USART_CR3_DMAR))
    {
      husart->hdmarx->XferAbortCallback = USART_DMARxAbortCallback;
    }
    else
    {
      husart->hdmarx->XferAbortCallback = NULL;
    }
  }

        /* Disable the USART DMA Tx request if enabled */
  if (HAL_IS_BIT_SET(husart->Instance->CR3, USART_CR3_DMAT))
  {
                /* Disable DMA Tx at USART level */
    CLEAR_BIT(husart->Instance->CR3, USART_CR3_DMAT);

                /* Abort the USART DMA Tx channel : use non blocking DMA Abort API (callback) */
    if (husart->hdmatx != NULL)
    {



                        /* Abort DMA TX */
      if (HAL_DMA_Abort_IT(husart->hdmatx) != HAL_OK)
      {
        husart->hdmatx->XferAbortCallback = NULL;
      }
      else
      {
        AbortCplt = 0x00U;
      }
    }
  }

        /* Disable the USART DMA Rx request if enabled */
  if (HAL_IS_BIT_SET(husart->Instance->CR3, USART_CR3_DMAR))
  {
    CLEAR_BIT(husart->Instance->CR3, USART_CR3_DMAR);

                /* Abort the USART DMA Rx channel : use non blocking DMA Abort API (callback) */
    if (husart->hdmarx != NULL)
    {



                        /* Abort DMA RX */
      if (HAL_DMA_Abort_IT(husart->hdmarx) != HAL_OK)
      {
        husart->hdmarx->XferAbortCallback = NULL;
        AbortCplt = 0x01U;
      }
      else
      {
        AbortCplt = 0x00U;
      }
    }
  }
#endif /* DMA1 or DMA */



  if (AbortCplt  == 0x01U)
  {
                /* Reset Tx and Rx transfer counters */
    husart->TxXferCount = 0x00U;
    husart->RxXferCount = 0x00U;

                /* Reset errorCode */
    husart->ErrorCode = HAL_USART_ERROR_NONE;

                /* Restore husart->State to Ready */
    husart->State  = HAL_USART_STATE_READY;

                /* As no DMA to be aborted, call directly user Abort complete callback */
#if (USE_HAL_USART_REGISTER_CALLBACKS == 1)
                /* Call registered Abort Complete Callback */
    husart->AbortCpltCallback(husart);
#else
                /* Call legacy weak Abort Complete Callback */
    HAL_USART_AbortCpltCallback(husart);
#endif /* USE_HAL_USART_REGISTER_CALLBACKS */
  }

  return HAL_OK;
}



void HAL_USART_IRQHandler(USART_HandleTypeDef *husart)
{
  uint32_t isrflags = READ_REG(husart->Instance->SR);
  uint32_t cr1its   = READ_REG(husart->Instance->CR1);
  uint32_t cr3its   = READ_REG(husart->Instance->CR3);
  uint32_t errorflags = 0x00U;
#if (defined(DMA1) || defined(DMA))
  uint32_t dmarequest = 0x00U;
#endif /* DMA1 or DMA */

        /* If no error occurs */
  errorflags = (isrflags & (uint32_t)(USART_SR_PE | USART_SR_FE | USART_SR_ORE | USART_SR_NE));
  if (errorflags == RESET)
  {
                /* USART in mode Receiver ------------------------------------------------- */
    if (((isrflags & USART_SR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
    {
      if (husart->State == HAL_USART_STATE_BUSY_RX)
      {
        USART_Receive_IT(husart);
      }
      else
      {
        USART_TransmitReceive_IT(husart);
      }
      return;
    }
  }
        /* If some errors occur */
  if ((errorflags != RESET) && (((cr3its & USART_CR3_EIE) != RESET) || ((cr1its & (USART_CR1_RXNEIE | USART_CR1_PEIE)) != RESET)))
  {
                /* USART parity error interrupt occurred ---------------------------------- */
    if (((isrflags & USART_SR_PE) != RESET) && ((cr1its & USART_CR1_PEIE) != RESET))
    {
      husart->ErrorCode |= HAL_USART_ERROR_PE;
    }

                /* USART noise error interrupt occurred -------------------------------- */
    if (((isrflags & USART_SR_NE) != RESET) && ((cr3its & USART_CR3_EIE) != RESET))
    {
      husart->ErrorCode |= HAL_USART_ERROR_NE;
    }

                /* USART frame error interrupt occurred -------------------------------- */
    if (((isrflags & USART_SR_FE) != RESET) && ((cr3its & USART_CR3_EIE) != RESET))
    {
      husart->ErrorCode |= HAL_USART_ERROR_FE;
    }

                /* USART Over-Run interrupt occurred ----------------------------------- */
    if (((isrflags & USART_SR_ORE) != RESET) && ((cr3its & USART_CR3_EIE) != RESET))
    {
      husart->ErrorCode |= HAL_USART_ERROR_ORE;
    }

    if (husart->ErrorCode != HAL_USART_ERROR_NONE)
    {
                        /* USART in mode Receiver ----------------------------------------------- */
      if (((isrflags & USART_SR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
      {
        if (husart->State == HAL_USART_STATE_BUSY_RX)
        {
          USART_Receive_IT(husart);
        }
        else
        {
          USART_TransmitReceive_IT(husart);
        }
      }

#if (defined(DMA1) || defined(DMA))
      dmarequest = HAL_IS_BIT_SET(husart->Instance->CR3, USART_CR3_DMAR);
      if (((husart->ErrorCode & HAL_USART_ERROR_ORE) != RESET) || dmarequest)
#else
      if ((husart->ErrorCode & HAL_USART_ERROR_ORE) != RESET)
#endif /* DMA1 or DMA */
      {

        USART_EndRxTransfer(husart);

#if (defined(DMA1) || defined(DMA))
                                /* Disable the USART DMA Rx request if enabled */
        if (HAL_IS_BIT_SET(husart->Instance->CR3, USART_CR3_DMAR))
        {
          CLEAR_BIT(husart->Instance->CR3, USART_CR3_DMAR);

                                        /* Abort the USART DMA Rx channel */
          if (husart->hdmarx != NULL)
          {

            husart->hdmarx->XferAbortCallback = USART_DMAAbortOnError;

            if (HAL_DMA_Abort_IT(husart->hdmarx) != HAL_OK)
            {
                                                        /* Call Directly XferAbortCallback function in case of error */
              husart->hdmarx->XferAbortCallback(husart->hdmarx);
            }
          }
          else
          {
                                                /* Call user error callback */
#if (USE_HAL_USART_REGISTER_CALLBACKS == 1)
                                                /* Call registered Error Callback */
            husart->ErrorCallback(husart);
#else
                                                /* Call legacy weak Error Callback */
            HAL_USART_ErrorCallback(husart);
#endif /* USE_HAL_USART_REGISTER_CALLBACKS */
          }
        }
        else
        {
#endif /* DMA1 or DMA */
                                        /* Call user error callback */
#if (USE_HAL_USART_REGISTER_CALLBACKS == 1)
                                        /* Call registered Error Callback */
          husart->ErrorCallback(husart);
#else
                                        /* Call legacy weak Error Callback */
          HAL_USART_ErrorCallback(husart);
#endif /* USE_HAL_USART_REGISTER_CALLBACKS */
#if (defined(DMA1) || defined(DMA))
        }
#endif /* DMA1 or DMA */
      }
      else
      {
                                /* Call user error callback */
#if (USE_HAL_USART_REGISTER_CALLBACKS == 1)
                                /* Call registered Error Callback */
        husart->ErrorCallback(husart);
#else
                                /* Call legacy weak Error Callback */
        HAL_USART_ErrorCallback(husart);
#endif /* USE_HAL_USART_REGISTER_CALLBACKS */
        husart->ErrorCode = HAL_USART_ERROR_NONE;
      }
    }
    return;
  }

        /* USART in mode Transmitter ----------------------------------------------- */
  if (((isrflags & USART_SR_TXE) != RESET) && ((cr1its & USART_CR1_TXEIE) != RESET))
  {
    if (husart->State == HAL_USART_STATE_BUSY_TX)
    {
      USART_Transmit_IT(husart);
    }
    else
    {
      USART_TransmitReceive_IT(husart);
    }
    return;
  }

        /* USART in mode Transmitter (transmission end) ---------------------------- */
  if (((isrflags & USART_SR_TC) != RESET) && ((cr1its & USART_CR1_TCIE) != RESET))
  {
    USART_EndTransmit_IT(husart);
    return;
  }
}

/* @brief  Tx Transfer completed callbacks. @param  husart Pointer to a USART_HandleTypeDef structure that contains @retval None */
__weak void HAL_USART_TxCpltCallback(USART_HandleTypeDef *husart)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(husart);

}

/* @brief  Tx Half Transfer completed callbacks. @param  husart Pointer to a USART_HandleTypeDef structure that contains @retval None */
__weak void HAL_USART_TxHalfCpltCallback(USART_HandleTypeDef *husart)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(husart);

}

/* @brief  Rx Transfer completed callbacks. @param  husart Pointer to a USART_HandleTypeDef structure that contains @retval None */
__weak void HAL_USART_RxCpltCallback(USART_HandleTypeDef *husart)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(husart);

}

/* @brief  Rx Half Transfer completed callbacks. @param  husart Pointer to a USART_HandleTypeDef structure that contains @retval None */
__weak void HAL_USART_RxHalfCpltCallback(USART_HandleTypeDef *husart)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(husart);

}



__weak void HAL_USART_TxRxCpltCallback(USART_HandleTypeDef *husart)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(husart);

}

/* @brief  USART error callbacks. @param  husart Pointer to a USART_HandleTypeDef structure that contains @retval None */
__weak void HAL_USART_ErrorCallback(USART_HandleTypeDef *husart)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(husart);

}

/* @brief  USART Abort Complete callback. @param  husart USART handle. @retval None */
__weak void HAL_USART_AbortCpltCallback(USART_HandleTypeDef *husart)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(husart);



}

/* @} */



/* @brief  Returns the USART state. @param  husart Pointer to a USART_HandleTypeDef structure that contains @retval HAL state */
HAL_USART_StateTypeDef HAL_USART_GetState(USART_HandleTypeDef *husart)
{
  return husart->State;
}



uint32_t HAL_USART_GetError(USART_HandleTypeDef *husart)
{
  return husart->ErrorCode;
}

/* @} */

/* @} */

/* @defgroup USART_Private_Functions USART Private Functions @{ */

#if (USE_HAL_USART_REGISTER_CALLBACKS == 1)
/* @brief  Initialize the callbacks to their default values. @param  husart USART handle. @retval none */
void USART_InitCallbacksToDefault(USART_HandleTypeDef *husart)
{
        /* Init the USART Callback settings */
  husart->TxHalfCpltCallback        = HAL_USART_TxHalfCpltCallback;            /* Legacy weak TxHalfCpltCallback */
  husart->TxCpltCallback            = HAL_USART_TxCpltCallback;                /* Legacy weak TxCpltCallback */
  husart->RxHalfCpltCallback        = HAL_USART_RxHalfCpltCallback;            /* Legacy weak RxHalfCpltCallback */
  husart->RxCpltCallback            = HAL_USART_RxCpltCallback;                /* Legacy weak RxCpltCallback */
  husart->TxRxCpltCallback          = HAL_USART_TxRxCpltCallback;              /* Legacy weak TxRxCpltCallback */
  husart->ErrorCallback             = HAL_USART_ErrorCallback;                 /* Legacy weak ErrorCallback */
  husart->AbortCpltCallback         = HAL_USART_AbortCpltCallback;             /* Legacy weak AbortCpltCallback */
}
#endif /* USE_HAL_USART_REGISTER_CALLBACKS */

#if (defined(DMA1) || defined(DMA))

static void USART_DMATransmitCplt(DMA_HandleTypeDef *hdma)
{
  USART_HandleTypeDef *husart = (USART_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;
        /* DMA Normal mode */
  if ((hdma->Instance->CCR & DMA_CCR_CIRC) == 0U)
  {
    husart->TxXferCount = 0U;
    if (husart->State == HAL_USART_STATE_BUSY_TX)
    {

      CLEAR_BIT(husart->Instance->CR3, USART_CR3_DMAT);

                        /* Enable the USART Transmit Complete Interrupt */
      SET_BIT(husart->Instance->CR1, USART_CR1_TCIE);
    }
  }
        /* DMA Circular mode */
  else
  {
    if (husart->State == HAL_USART_STATE_BUSY_TX)
    {
#if (USE_HAL_USART_REGISTER_CALLBACKS == 1)
                        /* Call registered Tx Complete Callback */
      husart->TxCpltCallback(husart);
#else
                        /* Call legacy weak Tx Complete Callback */
      HAL_USART_TxCpltCallback(husart);
#endif /* USE_HAL_USART_REGISTER_CALLBACKS */
    }
  }
}



static void USART_DMATxHalfCplt(DMA_HandleTypeDef *hdma)
{
  USART_HandleTypeDef *husart = (USART_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

#if (USE_HAL_USART_REGISTER_CALLBACKS == 1)
        /* Call registered Tx Half Complete Callback */
  husart->TxHalfCpltCallback(husart);
#else
        /* Call legacy weak Tx Half Complete Callback */
  HAL_USART_TxHalfCpltCallback(husart);
#endif /* USE_HAL_USART_REGISTER_CALLBACKS */
}



static void USART_DMAReceiveCplt(DMA_HandleTypeDef *hdma)
{
  USART_HandleTypeDef *husart = (USART_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;
        /* DMA Normal mode */
  if ((hdma->Instance->CCR & DMA_CCR_CIRC) == 0U)
  {
    husart->RxXferCount = 0x00U;

                /* Disable RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts */
    CLEAR_BIT(husart->Instance->CR1, USART_CR1_PEIE);
    CLEAR_BIT(husart->Instance->CR3, USART_CR3_EIE);



    CLEAR_BIT(husart->Instance->CR3, USART_CR3_DMAR);
    CLEAR_BIT(husart->Instance->CR3, USART_CR3_DMAT);

    husart->State = HAL_USART_STATE_READY;

                /* The USART state is HAL_USART_STATE_BUSY_RX */
    if (husart->State == HAL_USART_STATE_BUSY_RX)
    {
#if (USE_HAL_USART_REGISTER_CALLBACKS == 1)
                        /* Call registered Rx Complete Callback */
      husart->RxCpltCallback(husart);
#else
                        /* Call legacy weak Rx Complete Callback */
      HAL_USART_RxCpltCallback(husart);
#endif /* USE_HAL_USART_REGISTER_CALLBACKS */
    }
                /* The USART state is HAL_USART_STATE_BUSY_TX_RX */
    else
    {
#if (USE_HAL_USART_REGISTER_CALLBACKS == 1)
                        /* Call registered Tx Rx Complete Callback */
      husart->TxRxCpltCallback(husart);
#else
                        /* Call legacy weak Tx Rx Complete Callback */
      HAL_USART_TxRxCpltCallback(husart);
#endif /* USE_HAL_USART_REGISTER_CALLBACKS */
    }
  }
        /* DMA circular mode */
  else
  {
    if (husart->State == HAL_USART_STATE_BUSY_RX)
    {
#if (USE_HAL_USART_REGISTER_CALLBACKS == 1)
                        /* Call registered Rx Complete Callback */
      husart->RxCpltCallback(husart);
#else
                        /* Call legacy weak Rx Complete Callback */
      HAL_USART_RxCpltCallback(husart);
#endif /* USE_HAL_USART_REGISTER_CALLBACKS */
    }
                /* The USART state is HAL_USART_STATE_BUSY_TX_RX */
    else
    {
#if (USE_HAL_USART_REGISTER_CALLBACKS == 1)
                        /* Call registered Tx Rx Complete Callback */
      husart->TxRxCpltCallback(husart);
#else
                        /* Call legacy weak Tx Rx Complete Callback */
      HAL_USART_TxRxCpltCallback(husart);
#endif /* USE_HAL_USART_REGISTER_CALLBACKS */
    }
  }
}



static void USART_DMARxHalfCplt(DMA_HandleTypeDef *hdma)
{
  USART_HandleTypeDef *husart = (USART_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

#if (USE_HAL_USART_REGISTER_CALLBACKS == 1)
        /* Call registered Rx Half Complete Callback */
  husart->RxHalfCpltCallback(husart);
#else
        /* Call legacy weak Rx Half Complete Callback */
  HAL_USART_RxHalfCpltCallback(husart);
#endif /* USE_HAL_USART_REGISTER_CALLBACKS */
}



static void USART_DMAError(DMA_HandleTypeDef *hdma)
{
  uint32_t dmarequest = 0x00U;
  USART_HandleTypeDef *husart = (USART_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;
  husart->RxXferCount = 0x00U;
  husart->TxXferCount = 0x00U;

        /* Stop USART DMA Tx request if ongoing */
  dmarequest = HAL_IS_BIT_SET(husart->Instance->CR3, USART_CR3_DMAT);
  if ((husart->State == HAL_USART_STATE_BUSY_TX) && dmarequest)
  {
    USART_EndTxTransfer(husart);
  }

        /* Stop USART DMA Rx request if ongoing */
  dmarequest = HAL_IS_BIT_SET(husart->Instance->CR3, USART_CR3_DMAR);
  if ((husart->State == HAL_USART_STATE_BUSY_RX) && dmarequest)
  {
    USART_EndRxTransfer(husart);
  }

  husart->ErrorCode |= HAL_USART_ERROR_DMA;
  husart->State = HAL_USART_STATE_READY;

#if (USE_HAL_USART_REGISTER_CALLBACKS == 1)
        /* Call registered Error Callback */
  husart->ErrorCallback(husart);
#else
        /* Call legacy weak Error Callback */
  HAL_USART_ErrorCallback(husart);
#endif /* USE_HAL_USART_REGISTER_CALLBACKS */
}
#endif /* DMA1 or DMA */



static HAL_StatusTypeDef USART_WaitOnFlagUntilTimeout(USART_HandleTypeDef *husart, uint32_t Flag, FlagStatus Status, uint32_t Tickstart, uint32_t Timeout)
{
        /* Wait until flag is set */
  while ((__HAL_USART_GET_FLAG(husart, Flag) ? SET : RESET) == Status)
  {
                /* Check for the Timeout */
    if (Timeout != HAL_MAX_DELAY)
    {
      if ((Timeout == 0U) || ((HAL_GetTick() - Tickstart) > Timeout))
      {
                                /* Disable the USART Transmit Complete Interrupt */
        CLEAR_BIT(husart->Instance->CR1, USART_CR1_TXEIE);

                                /* Disable the USART RXNE Interrupt */
        CLEAR_BIT(husart->Instance->CR1, USART_CR1_RXNEIE);

                                /* Disable the USART Parity Error Interrupt */
        CLEAR_BIT(husart->Instance->CR1, USART_CR1_PEIE);

                                /* Disable the USART Error Interrupt: (Frame error, noise error, overrun error) */
        CLEAR_BIT(husart->Instance->CR3, USART_CR3_EIE);

        husart->State = HAL_USART_STATE_READY;

                                /* Process Unlocked */
        __HAL_UNLOCK(husart);

        return HAL_TIMEOUT;
      }
    }
  }
  return HAL_OK;
}

#if (defined(DMA1) || defined(DMA))

static void USART_EndTxTransfer(USART_HandleTypeDef *husart)
{
        /* Disable TXEIE and TCIE interrupts */
  CLEAR_BIT(husart->Instance->CR1, (USART_CR1_TXEIE | USART_CR1_TCIE));

        /* At end of Tx process, restore husart->State to Ready */
  husart->State = HAL_USART_STATE_READY;
}
#endif /* DMA1 or DMA */



static void USART_EndRxTransfer(USART_HandleTypeDef *husart)
{
        /* Disable RXNE, PE and ERR interrupts */
  CLEAR_BIT(husart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
  CLEAR_BIT(husart->Instance->CR3, USART_CR3_EIE);

        /* At end of Rx process, restore husart->State to Ready */
  husart->State = HAL_USART_STATE_READY;
}

#if (defined(DMA1) || defined(DMA))

static void USART_DMAAbortOnError(DMA_HandleTypeDef *hdma)
{
  USART_HandleTypeDef *husart = (USART_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;
  husart->RxXferCount = 0x00U;
  husart->TxXferCount = 0x00U;

#if (USE_HAL_USART_REGISTER_CALLBACKS == 1)
        /* Call registered Error Callback */
  husart->ErrorCallback(husart);
#else
        /* Call legacy weak Error Callback */
  HAL_USART_ErrorCallback(husart);
#endif /* USE_HAL_USART_REGISTER_CALLBACKS */
}



static void USART_DMATxAbortCallback(DMA_HandleTypeDef *hdma)
{
  USART_HandleTypeDef *husart = (USART_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

  husart->hdmatx->XferAbortCallback = NULL;

        /* Check if an Abort process is still ongoing */
  if (husart->hdmarx != NULL)
  {
    if (husart->hdmarx->XferAbortCallback != NULL)
    {
      return;
    }
  }



  husart->TxXferCount = 0x00U;
  husart->RxXferCount = 0x00U;

        /* Reset errorCode */
  husart->ErrorCode = HAL_USART_ERROR_NONE;

        /* Restore husart->State to Ready */
  husart->State  = HAL_USART_STATE_READY;

        /* Call user Abort complete callback */
#if (USE_HAL_USART_REGISTER_CALLBACKS == 1)
        /* Call registered Abort Complete Callback */
  husart->AbortCpltCallback(husart);
#else
        /* Call legacy weak Abort Complete Callback */
  HAL_USART_AbortCpltCallback(husart);
#endif /* USE_HAL_USART_REGISTER_CALLBACKS */
}



static void USART_DMARxAbortCallback(DMA_HandleTypeDef *hdma)
{
  USART_HandleTypeDef *husart = (USART_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

  husart->hdmarx->XferAbortCallback = NULL;

        /* Check if an Abort process is still ongoing */
  if (husart->hdmatx != NULL)
  {
    if (husart->hdmatx->XferAbortCallback != NULL)
    {
      return;
    }
  }



  husart->TxXferCount = 0x00U;
  husart->RxXferCount = 0x00U;

        /* Reset errorCode */
  husart->ErrorCode = HAL_USART_ERROR_NONE;

        /* Restore husart->State to Ready */
  husart->State  = HAL_USART_STATE_READY;

        /* Call user Abort complete callback */
#if (USE_HAL_USART_REGISTER_CALLBACKS == 1)
        /* Call registered Abort Complete Callback */
  husart->AbortCpltCallback(husart);
#else
        /* Call legacy weak Abort Complete Callback */
  HAL_USART_AbortCpltCallback(husart);
#endif /* USE_HAL_USART_REGISTER_CALLBACKS */
}
#endif /* DMA1 or DMA */



static HAL_StatusTypeDef USART_Transmit_IT(USART_HandleTypeDef *husart)
{
  uint16_t *tmp;

  if (husart->State == HAL_USART_STATE_BUSY_TX)
  {
    if (husart->Init.WordLength == USART_WORDLENGTH_9B)
    {
      tmp = (uint16_t *) husart->pTxBuffPtr;
      husart->Instance->DR = (uint16_t)(*tmp & (uint16_t)0x01FF);
      if (husart->Init.Parity == USART_PARITY_NONE)
      {
        husart->pTxBuffPtr += 2U;
      }
      else
      {
        husart->pTxBuffPtr += 1U;
      }
    }
    else
    {
      husart->Instance->DR = (uint8_t)(*husart->pTxBuffPtr++ & (uint8_t)0x00FF);
    }

    if (--husart->TxXferCount == 0U)
    {
                        /* Disable the USART Transmit data register empty Interrupt */
      CLEAR_BIT(husart->Instance->CR1, USART_CR1_TXEIE);

                        /* Enable the USART Transmit Complete Interrupt */
      SET_BIT(husart->Instance->CR1, USART_CR1_TCIE);
    }
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}



static HAL_StatusTypeDef USART_EndTransmit_IT(USART_HandleTypeDef *husart)
{
        /* Disable the USART Transmit Complete Interrupt */
  CLEAR_BIT(husart->Instance->CR1, USART_CR1_TCIE);

        /* Disable the USART Error Interrupt: (Frame error, noise error, overrun error) */
  CLEAR_BIT(husart->Instance->CR3, USART_CR3_EIE);

  husart->State = HAL_USART_STATE_READY;

#if (USE_HAL_USART_REGISTER_CALLBACKS == 1)
        /* Call registered Tx Complete Callback */
  husart->TxCpltCallback(husart);
#else
        /* Call legacy weak Tx Complete Callback */
  HAL_USART_TxCpltCallback(husart);
#endif /* USE_HAL_USART_REGISTER_CALLBACKS */

  return HAL_OK;
}



static HAL_StatusTypeDef USART_Receive_IT(USART_HandleTypeDef *husart)
{
  uint16_t *tmp;
  if (husart->State == HAL_USART_STATE_BUSY_RX)
  {
    if (husart->Init.WordLength == USART_WORDLENGTH_9B)
    {
      tmp = (uint16_t *) husart->pRxBuffPtr;
      if (husart->Init.Parity == USART_PARITY_NONE)
      {
        *tmp = (uint16_t)(husart->Instance->DR & (uint16_t)0x01FF);
        husart->pRxBuffPtr += 2U;
      }
      else
      {
        *tmp = (uint16_t)(husart->Instance->DR & (uint16_t)0x00FF);
        husart->pRxBuffPtr += 1U;
      }
      if (--husart->RxXferCount != 0x00U)
      {
                                /* Send dummy byte in order to generate the clock for the slave to send the next data */
        husart->Instance->DR = (DUMMY_DATA & (uint16_t)0x01FF);
      }
    }
    else
    {
      if (husart->Init.Parity == USART_PARITY_NONE)
      {
        *husart->pRxBuffPtr++ = (uint8_t)(husart->Instance->DR & (uint8_t)0x00FF);
      }
      else
      {
        *husart->pRxBuffPtr++ = (uint8_t)(husart->Instance->DR & (uint8_t)0x007F);
      }

      if (--husart->RxXferCount != 0x00U)
      {
                                /* Send dummy byte in order to generate the clock for the slave to send the next data */
        husart->Instance->DR = (DUMMY_DATA & (uint16_t)0x00FF);
      }
    }

    if (husart->RxXferCount == 0U)
    {
                        /* Disable the USART RXNE Interrupt */
      CLEAR_BIT(husart->Instance->CR1, USART_CR1_RXNEIE);

                        /* Disable the USART Parity Error Interrupt */
      CLEAR_BIT(husart->Instance->CR1, USART_CR1_PEIE);

                        /* Disable the USART Error Interrupt: (Frame error, noise error, overrun error) */
      CLEAR_BIT(husart->Instance->CR3, USART_CR3_EIE);

      husart->State = HAL_USART_STATE_READY;
#if (USE_HAL_USART_REGISTER_CALLBACKS == 1)
                        /* Call registered Rx Complete Callback */
      husart->RxCpltCallback(husart);
#else
                        /* Call legacy weak Rx Complete Callback */
      HAL_USART_RxCpltCallback(husart);
#endif /* USE_HAL_USART_REGISTER_CALLBACKS */

      return HAL_OK;
    }
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}



static HAL_StatusTypeDef USART_TransmitReceive_IT(USART_HandleTypeDef *husart)
{
  uint16_t *tmp;

  if (husart->State == HAL_USART_STATE_BUSY_TX_RX)
  {
    if (husart->TxXferCount != 0x00U)
    {
      if (__HAL_USART_GET_FLAG(husart, USART_FLAG_TXE) != RESET)
      {
        if (husart->Init.WordLength == USART_WORDLENGTH_9B)
        {
          tmp = (uint16_t *) husart->pTxBuffPtr;
          husart->Instance->DR = (uint16_t)(*tmp & (uint16_t)0x01FF);
          if (husart->Init.Parity == USART_PARITY_NONE)
          {
            husart->pTxBuffPtr += 2U;
          }
          else
          {
            husart->pTxBuffPtr += 1U;
          }
        }
        else
        {
          husart->Instance->DR = (uint8_t)(*husart->pTxBuffPtr++ & (uint8_t)0x00FF);
        }
        husart->TxXferCount--;

                                /* Check the latest data transmitted */
        if (husart->TxXferCount == 0U)
        {
          CLEAR_BIT(husart->Instance->CR1, USART_CR1_TXEIE);
        }
      }
    }

    if (husart->RxXferCount != 0x00U)
    {
      if (__HAL_USART_GET_FLAG(husart, USART_FLAG_RXNE) != RESET)
      {
        if (husart->Init.WordLength == USART_WORDLENGTH_9B)
        {
          tmp = (uint16_t *) husart->pRxBuffPtr;
          if (husart->Init.Parity == USART_PARITY_NONE)
          {
            *tmp = (uint16_t)(husart->Instance->DR & (uint16_t)0x01FF);
            husart->pRxBuffPtr += 2U;
          }
          else
          {
            *tmp = (uint16_t)(husart->Instance->DR & (uint16_t)0x00FF);
            husart->pRxBuffPtr += 1U;
          }
        }
        else
        {
          if (husart->Init.Parity == USART_PARITY_NONE)
          {
            *husart->pRxBuffPtr++ = (uint8_t)(husart->Instance->DR & (uint8_t)0x00FF);
          }
          else
          {
            *husart->pRxBuffPtr++ = (uint8_t)(husart->Instance->DR & (uint8_t)0x007F);
          }
        }
        husart->RxXferCount--;
      }
    }

                /* Check the latest data received */
    if (husart->RxXferCount == 0U)
    {
                        /* Disable the USART RXNE Interrupt */
      CLEAR_BIT(husart->Instance->CR1, USART_CR1_RXNEIE);

                        /* Disable the USART Parity Error Interrupt */
      CLEAR_BIT(husart->Instance->CR1, USART_CR1_PEIE);

                        /* Disable the USART Error Interrupt: (Frame error, noise error, overrun error) */
      CLEAR_BIT(husart->Instance->CR3, USART_CR3_EIE);

      husart->State = HAL_USART_STATE_READY;

#if (USE_HAL_USART_REGISTER_CALLBACKS == 1)
                        /* Call registered Tx Rx Complete Callback */
      husart->TxRxCpltCallback(husart);
#else
                        /* Call legacy weak Tx Rx Complete Callback */
      HAL_USART_TxRxCpltCallback(husart);
#endif /* USE_HAL_USART_REGISTER_CALLBACKS */

      return HAL_OK;
    }

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/* @brief  Configures the USART peripheral. @param  husart Pointer to a USART_HandleTypeDef structure that contains @retval None */
static void USART_SetConfig(USART_HandleTypeDef *husart)
{
  uint32_t tmpreg = 0x00U;
  uint32_t pclk;

        /* Check the parameters */
  assert_param(IS_USART_INSTANCE(husart->Instance));
  assert_param(IS_USART_POLARITY(husart->Init.CLKPolarity));
  assert_param(IS_USART_PHASE(husart->Init.CLKPhase));
  assert_param(IS_USART_LASTBIT(husart->Init.CLKLastBit));
  assert_param(IS_USART_BAUDRATE(husart->Init.BaudRate));
  assert_param(IS_USART_WORD_LENGTH(husart->Init.WordLength));
  assert_param(IS_USART_STOPBITS(husart->Init.StopBits));
  assert_param(IS_USART_PARITY(husart->Init.Parity));
  assert_param(IS_USART_MODE(husart->Init.Mode));



  CLEAR_BIT(husart->Instance->CR1, (USART_CR1_TE | USART_CR1_RE));

        /* ---------------------------- USART CR2 Configuration --------------------- */
  tmpreg = husart->Instance->CR2;
        /* Clear CLKEN, CPOL, CPHA and LBCL bits */
  tmpreg &= (uint32_t)~((uint32_t)(USART_CR2_CPHA | USART_CR2_CPOL | USART_CR2_CLKEN | USART_CR2_LBCL | USART_CR2_STOP));
        /* Configure the USART Clock, CPOL, CPHA and LastBit ----------------------- */
        /* Set CPOL bit according to husart->Init.CLKPolarity value */
        /* Set CPHA bit according to husart->Init.CLKPhase value */
        /* Set LBCL bit according to husart->Init.CLKLastBit value */
        /* Set Stop Bits: Set STOP[13:12] bits according to husart->Init.StopBits value */
  tmpreg |= (uint32_t)(USART_CLOCK_ENABLE | husart->Init.CLKPolarity |
                       husart->Init.CLKPhase | husart->Init.CLKLastBit | husart->Init.StopBits);
        /* Write to USART CR2 */
  WRITE_REG(husart->Instance->CR2, (uint32_t)tmpreg);

        /* -------------------------- USART CR1 Configuration ----------------------- */
  tmpreg = husart->Instance->CR1;

        /* Clear M, PCE, PS, TE and RE bits */
  tmpreg &= (uint32_t)~((uint32_t)(USART_CR1_M | USART_CR1_PCE | USART_CR1_PS | USART_CR1_TE | USART_CR1_RE));



  tmpreg |= (uint32_t)husart->Init.WordLength | husart->Init.Parity | husart->Init.Mode;

        /* Write to USART CR1 */
  WRITE_REG(husart->Instance->CR1, (uint32_t)tmpreg);

        /* -------------------------- USART CR3 Configuration ----------------------- */
        /* Clear CTSE and RTSE bits */
  CLEAR_BIT(husart->Instance->CR3, (USART_CR3_RTSE | USART_CR3_CTSE));

        /* -------------------------- USART BRR Configuration ----------------------- */
  pclk = HAL_RCC_GetPCLK1Freq();
  husart->Instance->BRR = USART_BRR(pclk, husart->Init.BaudRate);
}

/* @} */

#endif /* HAL_USART_MODULE_ENABLED */
/* @} */

/* @} */

/* (C) COPYRIGHT Puya *****END OF FILE */
