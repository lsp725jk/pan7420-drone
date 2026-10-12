/**
 * @file    py32f0xx_hal_dma.c
 * @author  MCU Application Team
 * @brief   DMA HAL module driver.
 */

/* Includes ------------------------------------------------------------------ */
#include "py32f0xx_hal.h"

/**
 * @addtogroup PY32F0xx_HAL_Driver @{
 */

/* @defgroup DMA DMA @brief DMA HAL module driver @{ */

#ifdef HAL_DMA_MODULE_ENABLED

/* Private typedef ----------------------------------------------------------- */
/* Private define ------------------------------------------------------------ */
/* Private macro ------------------------------------------------------------- */
/* Private variables --------------------------------------------------------- */
/* Private function prototypes ----------------------------------------------- */
/* @defgroup DMA_Private_Functions DMA Private Functions @{ */
static void DMA_SetConfig(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength);
/* @} */

/* Exported functions --------------------------------------------------------- */

/* @defgroup DMA_Exported_Functions DMA Exported Functions @{ */



HAL_StatusTypeDef HAL_DMA_Init(DMA_HandleTypeDef *hdma)
{
  uint32_t tmp = 0U;

        /* Check the DMA handle allocation */
  if(hdma == NULL)
  {
    return HAL_ERROR;
  }

        /* Check the parameters */
  assert_param(IS_DMA_ALL_INSTANCE(hdma->Instance));
  assert_param(IS_DMA_DIRECTION(hdma->Init.Direction));
  assert_param(IS_DMA_PERIPHERAL_INC_STATE(hdma->Init.PeriphInc));
  assert_param(IS_DMA_MEMORY_INC_STATE(hdma->Init.MemInc));
  assert_param(IS_DMA_PERIPHERAL_DATA_SIZE(hdma->Init.PeriphDataAlignment));
  assert_param(IS_DMA_MEMORY_DATA_SIZE(hdma->Init.MemDataAlignment));
  assert_param(IS_DMA_MODE(hdma->Init.Mode));
  assert_param(IS_DMA_PRIORITY(hdma->Init.Priority));



        /* DMA */
  hdma->ChannelIndex = (((uint32_t)hdma->Instance - (uint32_t)DMA1_Channel1) / ((uint32_t)DMA1_Channel2 - (uint32_t)DMA1_Channel1)) << 2;
  hdma->DmaBaseAddress = DMA1;



        /* Change DMA peripheral state */
  hdma->State = HAL_DMA_STATE_BUSY;

        /* Get the CCR register value */
  tmp = hdma->Instance->CCR;

        /* Clear PL, MSIZE, PSIZE, MINC, PINC, CIRC and DIR bits */
  tmp &= ((uint32_t)~(DMA_CCR_PL    | DMA_CCR_MSIZE  | DMA_CCR_PSIZE  | \
                      DMA_CCR_MINC  | DMA_CCR_PINC   | DMA_CCR_CIRC   | \
                      DMA_CCR_DIR));

        /* Prepare the DMA Channel configuration */
  tmp |=  hdma->Init.Direction        |
          hdma->Init.PeriphInc           | hdma->Init.MemInc           |
          hdma->Init.PeriphDataAlignment | hdma->Init.MemDataAlignment |
          hdma->Init.Mode                | hdma->Init.Priority;

        /* Write to DMA Channel CR register */
  hdma->Instance->CCR = tmp;

        /* Initialise the error code */
  hdma->ErrorCode = HAL_DMA_ERROR_NONE;

        /* Initialize the DMA state */
  hdma->State = HAL_DMA_STATE_READY;
        /* Allocate lock resource and initialize it */
  hdma->Lock = HAL_UNLOCKED;

  return HAL_OK;
}



HAL_StatusTypeDef HAL_DMA_DeInit(DMA_HandleTypeDef *hdma)
{
        /* Check the DMA handle allocation */
  if(hdma == NULL)
  {
    return HAL_ERROR;
  }

        /* Check the parameters */
  assert_param(IS_DMA_ALL_INSTANCE(hdma->Instance));

        /* Disable the selected DMA Channelx */
  __HAL_DMA_DISABLE(hdma);

        /* Reset DMA Channel control register */
  hdma->Instance->CCR  = 0U;

        /* Reset DMA Channel Number of Data to Transfer register */
  hdma->Instance->CNDTR = 0U;

        /* Reset DMA Channel peripheral address register */
  hdma->Instance->CPAR  = 0U;

        /* Reset DMA Channel memory address register */
  hdma->Instance->CMAR = 0U;



  hdma->ChannelIndex = (((uint32_t)hdma->Instance - (uint32_t)DMA1_Channel1) / ((uint32_t)DMA1_Channel2 - (uint32_t)DMA1_Channel1)) << 2;
  hdma->DmaBaseAddress = DMA1;



        /* Clear all flags */
  hdma->DmaBaseAddress->IFCR = (DMA_ISR_GIF1 << (hdma->ChannelIndex));

        /* Clean all callbacks */
  hdma->XferCpltCallback = NULL;
  hdma->XferHalfCpltCallback = NULL;
  hdma->XferErrorCallback = NULL;
  hdma->XferAbortCallback = NULL;

        /* Reset the error code */
  hdma->ErrorCode = HAL_DMA_ERROR_NONE;

        /* Reset the DMA state */
  hdma->State = HAL_DMA_STATE_RESET;

        /* Release Lock */
  __HAL_UNLOCK(hdma);

  return HAL_OK;
}

/* @} */



HAL_StatusTypeDef HAL_DMA_Start(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength)
{
  HAL_StatusTypeDef status = HAL_OK;

        /* Check the parameters */
  assert_param(IS_DMA_BUFFER_SIZE(DataLength));

        /* Process locked */
  __HAL_LOCK(hdma);

  if(HAL_DMA_STATE_READY == hdma->State)
  {
                /* Change DMA peripheral state */
    hdma->State = HAL_DMA_STATE_BUSY;
    hdma->ErrorCode = HAL_DMA_ERROR_NONE;

                /* Disable the peripheral */
    __HAL_DMA_DISABLE(hdma);

                /* Configure the source, destination address and the data length & clear flags */
    DMA_SetConfig(hdma, SrcAddress, DstAddress, DataLength);

                /* Enable the Peripheral */
    __HAL_DMA_ENABLE(hdma);
  }
  else
  {
                /* Process Unlocked */
    __HAL_UNLOCK(hdma);
    status = HAL_BUSY;
  }
  return status;
}



HAL_StatusTypeDef HAL_DMA_Start_IT(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength)
{
  HAL_StatusTypeDef status = HAL_OK;

        /* Check the parameters */
  assert_param(IS_DMA_BUFFER_SIZE(DataLength));

        /* Process locked */
  __HAL_LOCK(hdma);

  if(HAL_DMA_STATE_READY == hdma->State)
  {
                /* Change DMA peripheral state */
    hdma->State = HAL_DMA_STATE_BUSY;
    hdma->ErrorCode = HAL_DMA_ERROR_NONE;

                /* Disable the peripheral */
    __HAL_DMA_DISABLE(hdma);

                /* Configure the source, destination address and the data length & clear flags */
    DMA_SetConfig(hdma, SrcAddress, DstAddress, DataLength);

                /* Enable the transfer complete interrupt */
                /* Enable the transfer Error interrupt */
    if(NULL != hdma->XferHalfCpltCallback)
    {
                        /* Enable the Half transfer complete interrupt as well */
      __HAL_DMA_ENABLE_IT(hdma, (DMA_IT_TC | DMA_IT_HT | DMA_IT_TE));
    }
    else
    {
      __HAL_DMA_DISABLE_IT(hdma, DMA_IT_HT);
      __HAL_DMA_ENABLE_IT(hdma, (DMA_IT_TC | DMA_IT_TE));
    }
                /* Enable the Peripheral */
    __HAL_DMA_ENABLE(hdma);
  }
  else
  {
                /* Process Unlocked */
    __HAL_UNLOCK(hdma);

                /* Remain BUSY */
    status = HAL_BUSY;
  }
  return status;
}



HAL_StatusTypeDef HAL_DMA_Abort(DMA_HandleTypeDef *hdma)
{
  HAL_StatusTypeDef status = HAL_OK;

  if(hdma->State != HAL_DMA_STATE_BUSY)
  {
                /* no transfer ongoing */
    hdma->ErrorCode = HAL_DMA_ERROR_NO_XFER;

                /* Process Unlocked */
    __HAL_UNLOCK(hdma);

    return HAL_ERROR;
  }
  else
  {
                /* Disable DMA IT */
    __HAL_DMA_DISABLE_IT(hdma, (DMA_IT_TC | DMA_IT_HT | DMA_IT_TE));

                /* Disable the channel */
    __HAL_DMA_DISABLE(hdma);

                /* Clear all flags */
    hdma->DmaBaseAddress->IFCR = (DMA_ISR_GIF1 << hdma->ChannelIndex);
  }
        /* Change the DMA state */
  hdma->State = HAL_DMA_STATE_READY;

        /* Process Unlocked */
  __HAL_UNLOCK(hdma);

  return status;
}



HAL_StatusTypeDef HAL_DMA_Abort_IT(DMA_HandleTypeDef *hdma)
{
  HAL_StatusTypeDef status = HAL_OK;

  if(HAL_DMA_STATE_BUSY != hdma->State)
  {
                /* no transfer ongoing */
    hdma->ErrorCode = HAL_DMA_ERROR_NO_XFER;

    status = HAL_ERROR;
  }
  else
  {
                /* Disable DMA IT */
    __HAL_DMA_DISABLE_IT(hdma, (DMA_IT_TC | DMA_IT_HT | DMA_IT_TE));

                /* Disable the channel */
    __HAL_DMA_DISABLE(hdma);

                /* Clear all flags */
    __HAL_DMA_CLEAR_FLAG(hdma, __HAL_DMA_GET_GI_FLAG_INDEX(hdma));

                /* Change the DMA state */
    hdma->State = HAL_DMA_STATE_READY;

                /* Process Unlocked */
    __HAL_UNLOCK(hdma);

                /* Call User Abort callback */
    if(hdma->XferAbortCallback != NULL)
    {
      hdma->XferAbortCallback(hdma);
    }
  }
  return status;
}



HAL_StatusTypeDef HAL_DMA_PollForTransfer(DMA_HandleTypeDef *hdma, uint32_t CompleteLevel, uint32_t Timeout)
{
  uint32_t temp;
  uint32_t tickstart = 0U;

  if(HAL_DMA_STATE_BUSY != hdma->State)
  {
                /* no transfer ongoing */
    hdma->ErrorCode = HAL_DMA_ERROR_NO_XFER;
    __HAL_UNLOCK(hdma);
    return HAL_ERROR;
  }

        /* Polling mode not supported in circular mode */
  if (RESET != (hdma->Instance->CCR & DMA_CCR_CIRC))
  {
    hdma->ErrorCode = HAL_DMA_ERROR_NOT_SUPPORTED;
    return HAL_ERROR;
  }

        /* Get the level transfer complete flag */
  if(CompleteLevel == HAL_DMA_FULL_TRANSFER)
  {
                /* Transfer Complete flag */
    temp = __HAL_DMA_GET_TC_FLAG_INDEX(hdma);
  }
  else
  {
                /* Half Transfer Complete flag */
    temp = __HAL_DMA_GET_HT_FLAG_INDEX(hdma);
  }

        /* Get tick */
  tickstart = HAL_GetTick();

  while(__HAL_DMA_GET_FLAG(hdma, temp) == RESET)
  {
    if((__HAL_DMA_GET_FLAG(hdma, __HAL_DMA_GET_TE_FLAG_INDEX(hdma)) != RESET))
    {
                        /* When a DMA transfer error occurs */
                        /* A hardware clear of its EN bits is performed */
                        /* Clear all flags */
      hdma->DmaBaseAddress->IFCR = (DMA_ISR_GIF1 << hdma->ChannelIndex);

                        /* Update error code */
      SET_BIT(hdma->ErrorCode, HAL_DMA_ERROR_TE);

                        /* Change the DMA state */
      hdma->State= HAL_DMA_STATE_READY;

                        /* Process Unlocked */
      __HAL_UNLOCK(hdma);

      return HAL_ERROR;
    }
                /* Check for the Timeout */
    if(Timeout != HAL_MAX_DELAY)
    {
      if((Timeout == 0U) || ((HAL_GetTick() - tickstart) > Timeout))
      {
                                /* Update error code */
        SET_BIT(hdma->ErrorCode, HAL_DMA_ERROR_TIMEOUT);

                                /* Change the DMA state */
        hdma->State = HAL_DMA_STATE_READY;

                                /* Process Unlocked */
        __HAL_UNLOCK(hdma);

        return HAL_ERROR;
      }
    }
  }

  if(CompleteLevel == HAL_DMA_FULL_TRANSFER)
  {
                /* Clear the transfer complete flag */
    __HAL_DMA_CLEAR_FLAG(hdma, __HAL_DMA_GET_TC_FLAG_INDEX(hdma));

                /* The selected Channelx EN bit is cleared (DMA is disabled and all transfers are complete) */
    hdma->State = HAL_DMA_STATE_READY;
  }
  else
  {
                /* Clear the half transfer complete flag */
    __HAL_DMA_CLEAR_FLAG(hdma, __HAL_DMA_GET_HT_FLAG_INDEX(hdma));
  }

        /* Process unlocked */
  __HAL_UNLOCK(hdma);

  return HAL_OK;
}



void HAL_DMA_IRQHandler(DMA_HandleTypeDef *hdma)
{
  uint32_t flag_it = hdma->DmaBaseAddress->ISR;
  uint32_t source_it = hdma->Instance->CCR;

        /* Half Transfer Complete Interrupt management */
  if (((flag_it & (DMA_FLAG_HT1 << hdma->ChannelIndex)) != RESET) && ((source_it & DMA_IT_HT) != RESET))
  {
                /* Disable the half transfer interrupt if the DMA mode is not CIRCULAR */
    if((hdma->Instance->CCR & DMA_CCR_CIRC) == 0U)
    {
                        /* Disable the half transfer interrupt */
      __HAL_DMA_DISABLE_IT(hdma, DMA_IT_HT);
    }
                /* Clear the half transfer complete flag */
    __HAL_DMA_CLEAR_FLAG(hdma, __HAL_DMA_GET_HT_FLAG_INDEX(hdma));

                /* DMA peripheral state is not updated in Half Transfer */
                /* but in Transfer Complete case */

    if(hdma->XferHalfCpltCallback != NULL)
    {
                        /* Half transfer callback */
      hdma->XferHalfCpltCallback(hdma);
    }
  }

        /* Transfer Complete Interrupt management */
  else if (((flag_it & (DMA_FLAG_TC1 << hdma->ChannelIndex)) != RESET) && ((source_it & DMA_IT_TC) != RESET))
  {
    if((hdma->Instance->CCR & DMA_CCR_CIRC) == 0U)
    {
                        /* Disable the transfer complete and error interrupt */
      __HAL_DMA_DISABLE_IT(hdma, DMA_IT_TE | DMA_IT_TC);

                        /* Change the DMA state */
      hdma->State = HAL_DMA_STATE_READY;
    }
                /* Clear the transfer complete flag */
    __HAL_DMA_CLEAR_FLAG(hdma, __HAL_DMA_GET_TC_FLAG_INDEX(hdma));

                /* Process Unlocked */
    __HAL_UNLOCK(hdma);

    if(hdma->XferCpltCallback != NULL)
    {
                        /* Transfer complete callback */
      hdma->XferCpltCallback(hdma);
    }
  }

        /* Transfer Error Interrupt management */
  else if (( RESET != (flag_it & (DMA_FLAG_TE1 << hdma->ChannelIndex))) && (RESET != (source_it & DMA_IT_TE)))
  {
                /* When a DMA transfer error occurs */
                /* A hardware clear of its EN bits is performed */
                /* Disable ALL DMA IT */
    __HAL_DMA_DISABLE_IT(hdma, (DMA_IT_TC | DMA_IT_HT | DMA_IT_TE));

                /* Clear all flags */
    hdma->DmaBaseAddress->IFCR = (DMA_ISR_GIF1 << hdma->ChannelIndex);

                /* Update error code */
    hdma->ErrorCode = HAL_DMA_ERROR_TE;

                /* Change the DMA state */
    hdma->State = HAL_DMA_STATE_READY;

                /* Process Unlocked */
    __HAL_UNLOCK(hdma);

    if (hdma->XferErrorCallback != NULL)
    {
                        /* Transfer error callback */
      hdma->XferErrorCallback(hdma);
    }
  }
  return;
}



HAL_StatusTypeDef HAL_DMA_RegisterCallback(DMA_HandleTypeDef *hdma, HAL_DMA_CallbackIDTypeDef CallbackID, void (* pCallback)( DMA_HandleTypeDef * _hdma))
{
  HAL_StatusTypeDef status = HAL_OK;

        /* Process locked */
  __HAL_LOCK(hdma);

  if(HAL_DMA_STATE_READY == hdma->State)
  {
    switch (CallbackID)
    {
    case  HAL_DMA_XFER_CPLT_CB_ID:
      hdma->XferCpltCallback = pCallback;
      break;

    case  HAL_DMA_XFER_HALFCPLT_CB_ID:
      hdma->XferHalfCpltCallback = pCallback;
      break;

    case  HAL_DMA_XFER_ERROR_CB_ID:
      hdma->XferErrorCallback = pCallback;
      break;

    case  HAL_DMA_XFER_ABORT_CB_ID:
      hdma->XferAbortCallback = pCallback;
      break;

    default:
      status = HAL_ERROR;
      break;
    }
  }
  else
  {
    status = HAL_ERROR;
  }

        /* Release Lock */
  __HAL_UNLOCK(hdma);

  return status;
}



HAL_StatusTypeDef HAL_DMA_UnRegisterCallback(DMA_HandleTypeDef *hdma, HAL_DMA_CallbackIDTypeDef CallbackID)
{
  HAL_StatusTypeDef status = HAL_OK;

        /* Process locked */
  __HAL_LOCK(hdma);

  if(HAL_DMA_STATE_READY == hdma->State)
  {
    switch (CallbackID)
    {
    case  HAL_DMA_XFER_CPLT_CB_ID:
      hdma->XferCpltCallback = NULL;
      break;

    case  HAL_DMA_XFER_HALFCPLT_CB_ID:
      hdma->XferHalfCpltCallback = NULL;
      break;

    case  HAL_DMA_XFER_ERROR_CB_ID:
      hdma->XferErrorCallback = NULL;
      break;

    case  HAL_DMA_XFER_ABORT_CB_ID:
      hdma->XferAbortCallback = NULL;
      break;

    case   HAL_DMA_XFER_ALL_CB_ID:
      hdma->XferCpltCallback = NULL;
      hdma->XferHalfCpltCallback = NULL;
      hdma->XferErrorCallback = NULL;
      hdma->XferAbortCallback = NULL;
      break;

    default:
      status = HAL_ERROR;
      break;
    }
  }
  else
  {
    status = HAL_ERROR;
  }

        /* Release Lock */
  __HAL_UNLOCK(hdma);

  return status;
}



void HAL_DMA_ChannelMap(DMA_HandleTypeDef *hdma, uint32_t MapReqNum)
{
  uint32_t position;  //Calculate channel number
    
  assert_param(IS_DMA_MAP_VALUE(MapReqNum));

  position = ((uint32_t)hdma->Instance - (uint32_t)DMA1_Channel1) / ((uint32_t)DMA1_Channel2 - (uint32_t)DMA1_Channel1);

        /* Enable SYSCFG Clock */
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  
  MODIFY_REG(SYSCFG->CFGR3, (0x1F << (8U * (position & 0x03U))), (MapReqNum << (8U * (position & 0x03U))));
}

/* @} */



HAL_DMA_StateTypeDef HAL_DMA_GetState(DMA_HandleTypeDef *hdma)
{
        /* Return DMA handle state */
  return hdma->State;
}



uint32_t HAL_DMA_GetError(DMA_HandleTypeDef *hdma)
{
  return hdma->ErrorCode;
}

/* @} */

/* @} */

/* @addtogroup DMA_Private_Functions @{ */



static void DMA_SetConfig(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength)
{
        /* Clear all flags */
  hdma->DmaBaseAddress->IFCR = (DMA_ISR_GIF1 << hdma->ChannelIndex);

        /* Configure DMA Channel data length */
  hdma->Instance->CNDTR = DataLength;

        /* Memory to Peripheral */
  if((hdma->Init.Direction) == DMA_MEMORY_TO_PERIPH)
  {
                /* Configure DMA Channel destination address */
    hdma->Instance->CPAR = DstAddress;

                /* Configure DMA Channel source address */
    hdma->Instance->CMAR = SrcAddress;
  }
        /* Peripheral to Memory */
  else
  {
                /* Configure DMA Channel source address */
    hdma->Instance->CPAR = SrcAddress;

                /* Configure DMA Channel destination address */
    hdma->Instance->CMAR = DstAddress;
  }
}

/* @} */

#endif /* HAL_DMA_MODULE_ENABLED */
/* @} */

/* @} */

/* (C) COPYRIGHT Puya *****END OF FILE */
