/**
 * @file    py32f0xx_hal_usart.h
 * @author  MCU Application Team
 * @brief   Header file of USART HAL module.
 * @attention
 */

/* Define to prevent recursive inclusion ------------------------------------- */
#ifndef __PY32F0xx_HAL_USART_H
#define __PY32F0xx_HAL_USART_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------ */
#include "py32f0xx_hal_def.h"

/* @addtogroup PY32F0xx_HAL_Driver @{ */

/* @addtogroup USART @{ */

/* Exported types ------------------------------------------------------------ */
/* @defgroup USART_Exported_Types USART Exported Types @{ */

/* @brief USART Init Structure definition */
typedef struct
{
  uint32_t BaudRate;                    /* !< This member configures the Usart communication baud rate. The baud rate is computed using the following formula: */

  uint32_t WordLength;                

  uint32_t StopBits;                  

  uint32_t Parity;                    

  uint32_t Mode;                      

  uint32_t CLKPolarity;               

  uint32_t CLKPhase;                  

  uint32_t CLKLastBit;                    /* This parameter can be a value of @ref USART_Last_Bit */
} USART_InitTypeDef;

/* @brief HAL State structures definition */
typedef enum
{
  HAL_USART_STATE_RESET             = 0x00U,        /* !< Peripheral is not yet Initialized */
  HAL_USART_STATE_READY             = 0x01U,        /* !< Peripheral Initialized and ready for use */
  HAL_USART_STATE_BUSY              = 0x02U,        /* !< an internal process is ongoing */
  HAL_USART_STATE_BUSY_TX           = 0x12U,        /* !< Data Transmission process is ongoing */
  HAL_USART_STATE_BUSY_RX           = 0x22U,        /* !< Data Reception process is ongoing */
  HAL_USART_STATE_BUSY_TX_RX        = 0x32U,        /* !< Data Transmission Reception process is ongoing */
  HAL_USART_STATE_TIMEOUT           = 0x03U,        /* !< Timeout state */
  HAL_USART_STATE_ERROR             = 0x04U         /* !< Error */
} HAL_USART_StateTypeDef;

/* @brief  USART handle Structure definition */
typedef struct __USART_HandleTypeDef
{
  USART_TypeDef                 *Instance;            /* !< USART registers base address */

  USART_InitTypeDef             Init;                 /* !< Usart communication parameters */

  uint8_t                       *pTxBuffPtr;          /* !< Pointer to Usart Tx transfer Buffer */

  uint16_t                      TxXferSize;           /* !< Usart Tx Transfer size */

  __IO uint16_t                 TxXferCount;          /* !< Usart Tx Transfer Counter */

  uint8_t                       *pRxBuffPtr;          /* !< Pointer to Usart Rx transfer Buffer */

  uint16_t                      RxXferSize;           /* !< Usart Rx Transfer size */

  __IO uint16_t                 RxXferCount;          /* !< Usart Rx Transfer Counter */

#if (defined(DMA1) || defined(DMA))
  DMA_HandleTypeDef             *hdmatx;              /* !< Usart Tx DMA Handle parameters */

  DMA_HandleTypeDef             *hdmarx;              /* !< Usart Rx DMA Handle parameters */
#endif /* DMA1 or DMA */

  HAL_LockTypeDef                Lock;                /* !< Locking object */

  __IO HAL_USART_StateTypeDef    State;               /* !< Usart communication state */

  __IO uint32_t                  ErrorCode;           /* !< USART Error code */

#if (USE_HAL_USART_REGISTER_CALLBACKS == 1)
  void (* TxHalfCpltCallback)(struct __USART_HandleTypeDef *husart);            /* !< USART Tx Half Complete Callback */
  void (* TxCpltCallback)(struct __USART_HandleTypeDef *husart);                /* !< USART Tx Complete Callback */
  void (* RxHalfCpltCallback)(struct __USART_HandleTypeDef *husart);            /* !< USART Rx Half Complete Callback */
  void (* RxCpltCallback)(struct __USART_HandleTypeDef *husart);                /* !< USART Rx Complete Callback */
  void (* TxRxCpltCallback)(struct __USART_HandleTypeDef *husart);              /* !< USART Tx Rx Complete Callback */
  void (* ErrorCallback)(struct __USART_HandleTypeDef *husart);                 /* !< USART Error Callback */
  void (* AbortCpltCallback)(struct __USART_HandleTypeDef *husart);             /* !< USART Abort Complete Callback */

  void (* MspInitCallback)(struct __USART_HandleTypeDef *husart);               /* !< USART Msp Init callback */
  void (* MspDeInitCallback)(struct __USART_HandleTypeDef *husart);             /* !< USART Msp DeInit callback */
#endif  /* USE_HAL_USART_REGISTER_CALLBACKS */

} USART_HandleTypeDef;

#if (USE_HAL_USART_REGISTER_CALLBACKS == 1)
/* @brief  HAL USART Callback ID enumeration definition */
typedef enum
{
  HAL_USART_TX_HALFCOMPLETE_CB_ID         = 0x00U,        /* !< USART Tx Half Complete Callback ID */
  HAL_USART_TX_COMPLETE_CB_ID             = 0x01U,        /* !< USART Tx Complete Callback ID */
  HAL_USART_RX_HALFCOMPLETE_CB_ID         = 0x02U,        /* !< USART Rx Half Complete Callback ID */
  HAL_USART_RX_COMPLETE_CB_ID             = 0x03U,        /* !< USART Rx Complete Callback ID */
  HAL_USART_TX_RX_COMPLETE_CB_ID          = 0x04U,        /* !< USART Tx Rx Complete Callback ID */
  HAL_USART_ERROR_CB_ID                   = 0x05U,        /* !< USART Error Callback ID */
  HAL_USART_ABORT_COMPLETE_CB_ID          = 0x06U,        /* !< USART Abort Complete Callback ID */

  HAL_USART_MSPINIT_CB_ID                 = 0x07U,        /* !< USART MspInit callback ID */
  HAL_USART_MSPDEINIT_CB_ID               = 0x08U         /* !< USART MspDeInit callback ID */

} HAL_USART_CallbackIDTypeDef;

/* @brief  HAL USART Callback pointer definition */
typedef  void (*pUSART_CallbackTypeDef)(USART_HandleTypeDef *husart);  /* !< pointer to an USART callback function */

#endif /* USE_HAL_USART_REGISTER_CALLBACKS */

/* @} */

/* Exported constants -------------------------------------------------------- */
/* @defgroup USART_Exported_Constants USART Exported Constants @{ */

/* @defgroup USART_Error_Code USART Error Code @brief    USART Error Code @{ */
#define HAL_USART_ERROR_NONE             0x00000000U   /* !< No error */
#define HAL_USART_ERROR_PE               0x00000001U   /* !< Parity error */
#define HAL_USART_ERROR_NE               0x00000002U   /* !< Noise error */
#define HAL_USART_ERROR_FE               0x00000004U   /* !< Frame error */
#define HAL_USART_ERROR_ORE              0x00000008U   /* !< Overrun error */
#if (defined(DMA1) || defined(DMA))
#define HAL_USART_ERROR_DMA              0x00000010U   /* !< DMA transfer error */
#endif /* DMA1 or DMA */
#if (USE_HAL_USART_REGISTER_CALLBACKS == 1)
#define HAL_USART_ERROR_INVALID_CALLBACK 0x00000020U    /* !< Invalid Callback error */
#endif /* USE_HAL_USART_REGISTER_CALLBACKS */
/* @} */

/* @defgroup USART_Word_Length USART Word Length @{ */
#define USART_WORDLENGTH_8B          0x00000000U
#define USART_WORDLENGTH_9B          ((uint32_t)USART_CR1_M)
/* @} */

/* @defgroup USART_Stop_Bits USART Number of Stop Bits @{ */
#define USART_STOPBITS_1             0x00000000U
#define USART_STOPBITS_2             ((uint32_t)USART_CR2_STOP)
/* @} */

/* @defgroup USART_Parity USART Parity @{ */
#define USART_PARITY_NONE            0x00000000U
#define USART_PARITY_EVEN            ((uint32_t)USART_CR1_PCE)
#define USART_PARITY_ODD             ((uint32_t)(USART_CR1_PCE | USART_CR1_PS))
/* @} */

/* @defgroup USART_Mode USART Mode @{ */
#define USART_MODE_RX                ((uint32_t)USART_CR1_RE)
#define USART_MODE_TX                ((uint32_t)USART_CR1_TE)
#define USART_MODE_TX_RX             ((uint32_t)(USART_CR1_TE | USART_CR1_RE))
/* @} */

/* @defgroup USART_Clock USART Clock @{ */
#define USART_CLOCK_DISABLE          0x00000000U
#define USART_CLOCK_ENABLE           ((uint32_t)USART_CR2_CLKEN)
/* @} */

/* @defgroup USART_Clock_Polarity USART Clock Polarity @{ */
#define USART_POLARITY_LOW           0x00000000U
#define USART_POLARITY_HIGH          ((uint32_t)USART_CR2_CPOL)
/* @} */

/* @defgroup USART_Clock_Phase USART Clock Phase @{ */
#define USART_PHASE_1EDGE            0x00000000U
#define USART_PHASE_2EDGE            ((uint32_t)USART_CR2_CPHA)
/* @} */

/* @defgroup USART_Last_Bit USART Last Bit @{ */
#define USART_LASTBIT_DISABLE        0x00000000U
#define USART_LASTBIT_ENABLE         ((uint32_t)USART_CR2_LBCL)
/* @} */

/* @defgroup USART_Flags USART Flags Elements values convention: 0xXXXX - 0xXXXX  : Flag mask in the SR register @{ */
#define USART_FLAG_TXE               ((uint32_t)USART_SR_TXE)
#define USART_FLAG_TC                ((uint32_t)USART_SR_TC)
#define USART_FLAG_RXNE              ((uint32_t)USART_SR_RXNE)
#define USART_FLAG_IDLE              ((uint32_t)USART_SR_IDLE)
#define USART_FLAG_ORE               ((uint32_t)USART_SR_ORE)
#define USART_FLAG_NE                ((uint32_t)USART_SR_NE)
#define USART_FLAG_FE                ((uint32_t)USART_SR_FE)
#define USART_FLAG_PE                ((uint32_t)USART_SR_PE)
/* @} */



#define USART_IT_PE                  ((uint32_t)(USART_CR1_REG_INDEX << 28U | USART_CR1_PEIE))
#define USART_IT_TXE                 ((uint32_t)(USART_CR1_REG_INDEX << 28U | USART_CR1_TXEIE))
#define USART_IT_TC                  ((uint32_t)(USART_CR1_REG_INDEX << 28U | USART_CR1_TCIE))
#define USART_IT_RXNE                ((uint32_t)(USART_CR1_REG_INDEX << 28U | USART_CR1_RXNEIE))
#define USART_IT_IDLE                ((uint32_t)(USART_CR1_REG_INDEX << 28U | USART_CR1_IDLEIE))
#define USART_IT_ERR                 ((uint32_t)(USART_CR3_REG_INDEX << 28U | USART_CR3_EIE))
/* @} */

/* @} */

/* Exported macro ------------------------------------------------------------ */
/* @defgroup USART_Exported_Macros USART Exported Macros @{ */

/* @brief Reset USART handle state @param  __HANDLE__ specifies the USART Handle. @retval None */
#if (USE_HAL_USART_REGISTER_CALLBACKS == 1)
#define __HAL_USART_RESET_HANDLE_STATE(__HANDLE__)  do{                                            \
                                                      (__HANDLE__)->State = HAL_USART_STATE_RESET; \
                                                      (__HANDLE__)->MspInitCallback = NULL;        \
                                                      (__HANDLE__)->MspDeInitCallback = NULL;      \
                                                    } while(0U)
#else
#define __HAL_USART_RESET_HANDLE_STATE(__HANDLE__)  ((__HANDLE__)->State = HAL_USART_STATE_RESET)
#endif /* USE_HAL_USART_REGISTER_CALLBACKS */



#define __HAL_USART_GET_FLAG(__HANDLE__, __FLAG__) (((__HANDLE__)->Instance->SR & (__FLAG__)) == (__FLAG__))



#define __HAL_USART_CLEAR_FLAG(__HANDLE__, __FLAG__) ((__HANDLE__)->Instance->SR = ~(__FLAG__))

/* @brief  Clear the USART PE pending flag. @param  __HANDLE__ specifies the USART Handle. @retval None */
#define __HAL_USART_CLEAR_PEFLAG(__HANDLE__)    \
  do{                                           \
    __IO uint32_t tmpreg = 0x00U;               \
    tmpreg = (__HANDLE__)->Instance->SR;        \
    tmpreg = (__HANDLE__)->Instance->DR;        \
    UNUSED(tmpreg);                             \
  } while(0U)

/* @brief  Clear the USART FE pending flag. @param  __HANDLE__ specifies the USART Handle. @retval None */
#define __HAL_USART_CLEAR_FEFLAG(__HANDLE__) __HAL_USART_CLEAR_PEFLAG(__HANDLE__)

/* @brief  Clear the USART NE pending flag. @param  __HANDLE__ specifies the USART Handle. @retval None */
#define __HAL_USART_CLEAR_NEFLAG(__HANDLE__) __HAL_USART_CLEAR_PEFLAG(__HANDLE__)

/* @brief  Clear the USART ORE pending flag. @param  __HANDLE__ specifies the USART Handle. @retval None */
#define __HAL_USART_CLEAR_OREFLAG(__HANDLE__) __HAL_USART_CLEAR_PEFLAG(__HANDLE__)

/* @brief  Clear the USART IDLE pending flag. @param  __HANDLE__ specifies the USART Handle. @retval None */
#define __HAL_USART_CLEAR_IDLEFLAG(__HANDLE__) __HAL_USART_CLEAR_PEFLAG(__HANDLE__)



#define __HAL_USART_ENABLE_IT(__HANDLE__, __INTERRUPT__)   ((((__INTERRUPT__) >> 28U) == USART_CR1_REG_INDEX)? ((__HANDLE__)->Instance->CR1 |= ((__INTERRUPT__) & USART_IT_MASK)): \
                                                            (((__INTERRUPT__) >> 28U) == USART_CR2_REG_INDEX)? ((__HANDLE__)->Instance->CR2 |= ((__INTERRUPT__) & USART_IT_MASK)): \
                                                             ((__HANDLE__)->Instance->CR3 |= ((__INTERRUPT__) & USART_IT_MASK)))
#define __HAL_USART_DISABLE_IT(__HANDLE__, __INTERRUPT__)  ((((__INTERRUPT__) >> 28U) == USART_CR1_REG_INDEX)? ((__HANDLE__)->Instance->CR1 &= ~((__INTERRUPT__) & USART_IT_MASK)): \
                                                            (((__INTERRUPT__) >> 28U) == USART_CR2_REG_INDEX)? ((__HANDLE__)->Instance->CR2 &= ~((__INTERRUPT__) & USART_IT_MASK)): \
                                                             ((__HANDLE__)->Instance->CR3 &= ~ ((__INTERRUPT__) & USART_IT_MASK)))



#define __HAL_USART_GET_IT_SOURCE(__HANDLE__, __IT__) (((((__IT__) >> 28U) == USART_CR1_REG_INDEX)? (__HANDLE__)->Instance->CR1:(((((uint32_t)(__IT__)) >> 28U) == USART_CR2_REG_INDEX)? \
                                                      (__HANDLE__)->Instance->CR2 : (__HANDLE__)->Instance->CR3)) & (((uint32_t)(__IT__)) & USART_IT_MASK))

/* @brief  Enable USART @param  __HANDLE__ specifies the USART Handle. @retval None */
#define __HAL_USART_ENABLE(__HANDLE__)               ((__HANDLE__)->Instance->CR1 |= USART_CR1_UE)

/* @brief  Disable USART @param  __HANDLE__ specifies the USART Handle. @retval None */
#define __HAL_USART_DISABLE(__HANDLE__)              ((__HANDLE__)->Instance->CR1 &= ~USART_CR1_UE)

/* @} */
/* Exported functions -------------------------------------------------------- */
/* @addtogroup USART_Exported_Functions @{ */

/* @addtogroup USART_Exported_Functions_Group1 @{ */
/* Initialization/de-initialization functions */
HAL_StatusTypeDef HAL_USART_Init(USART_HandleTypeDef *husart);
HAL_StatusTypeDef HAL_USART_DeInit(USART_HandleTypeDef *husart);
void HAL_USART_MspInit(USART_HandleTypeDef *husart);
void HAL_USART_MspDeInit(USART_HandleTypeDef *husart);

/* Callbacks Register/UnRegister functions */
#if (USE_HAL_USART_REGISTER_CALLBACKS == 1)
HAL_StatusTypeDef HAL_USART_RegisterCallback(USART_HandleTypeDef *husart, HAL_USART_CallbackIDTypeDef CallbackID, pUSART_CallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_USART_UnRegisterCallback(USART_HandleTypeDef *husart, HAL_USART_CallbackIDTypeDef CallbackID);
#endif /* USE_HAL_USART_REGISTER_CALLBACKS */

/* @} */

/* @addtogroup USART_Exported_Functions_Group2 @{ */
/* IO operation functions */
HAL_StatusTypeDef HAL_USART_Transmit(USART_HandleTypeDef *husart, uint8_t *pTxData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_USART_Receive(USART_HandleTypeDef *husart, uint8_t *pRxData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_USART_TransmitReceive(USART_HandleTypeDef *husart, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_USART_Transmit_IT(USART_HandleTypeDef *husart, uint8_t *pTxData, uint16_t Size);
HAL_StatusTypeDef HAL_USART_Receive_IT(USART_HandleTypeDef *husart, uint8_t *pRxData, uint16_t Size);
HAL_StatusTypeDef HAL_USART_TransmitReceive_IT(USART_HandleTypeDef *husart, uint8_t *pTxData, uint8_t *pRxData,  uint16_t Size);
#if (defined(DMA1) || defined(DMA))
HAL_StatusTypeDef HAL_USART_Transmit_DMA(USART_HandleTypeDef *husart, uint8_t *pTxData, uint16_t Size);
HAL_StatusTypeDef HAL_USART_Receive_DMA(USART_HandleTypeDef *husart, uint8_t *pRxData, uint16_t Size);
HAL_StatusTypeDef HAL_USART_TransmitReceive_DMA(USART_HandleTypeDef *husart, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size);
HAL_StatusTypeDef HAL_USART_DMAPause(USART_HandleTypeDef *husart);
HAL_StatusTypeDef HAL_USART_DMAResume(USART_HandleTypeDef *husart);
HAL_StatusTypeDef HAL_USART_DMAStop(USART_HandleTypeDef *husart);
#endif /* DMA1 or DMA */
/* Transfer Abort functions */
HAL_StatusTypeDef HAL_USART_Abort(USART_HandleTypeDef *husart);
HAL_StatusTypeDef HAL_USART_Abort_IT(USART_HandleTypeDef *husart);

void HAL_USART_IRQHandler(USART_HandleTypeDef *husart);
void HAL_USART_TxCpltCallback(USART_HandleTypeDef *husart);
void HAL_USART_TxHalfCpltCallback(USART_HandleTypeDef *husart);
void HAL_USART_RxCpltCallback(USART_HandleTypeDef *husart);
void HAL_USART_RxHalfCpltCallback(USART_HandleTypeDef *husart);
void HAL_USART_TxRxCpltCallback(USART_HandleTypeDef *husart);
void HAL_USART_ErrorCallback(USART_HandleTypeDef *husart);
void HAL_USART_AbortCpltCallback(USART_HandleTypeDef *husart);
/* @} */

/* @addtogroup USART_Exported_Functions_Group3 @{ */
/* Peripheral State functions */
HAL_USART_StateTypeDef HAL_USART_GetState(USART_HandleTypeDef *husart);
uint32_t               HAL_USART_GetError(USART_HandleTypeDef *husart);
/* @} */

/* @} */
/* Private types ------------------------------------------------------------- */
/* Private variables --------------------------------------------------------- */
/* Private constants --------------------------------------------------------- */
/* @defgroup USART_Private_Constants USART Private Constants @{ */
/* @brief USART interruptions flag mask */
#define USART_IT_MASK  ((uint32_t) USART_CR1_PEIE | USART_CR1_TXEIE | USART_CR1_TCIE | USART_CR1_RXNEIE | \
                                   USART_CR1_IDLEIE | USART_CR3_CTSIE | USART_CR3_EIE )

#define USART_CR1_REG_INDEX          1U
#define USART_CR2_REG_INDEX          2U
#define USART_CR3_REG_INDEX          3U
/* @} */

/* Private macros ------------------------------------------------------------ */
/* @defgroup USART_Private_Macros USART Private Macros @{ */
#define IS_USART_LASTBIT(LASTBIT)    (((LASTBIT) == USART_LASTBIT_DISABLE) || \
                                      ((LASTBIT) == USART_LASTBIT_ENABLE))

#define IS_USART_PHASE(CPHA)         (((CPHA) == USART_PHASE_1EDGE) || \
                                      ((CPHA) == USART_PHASE_2EDGE))

#define IS_USART_POLARITY(CPOL)      (((CPOL) == USART_POLARITY_LOW) || \
                                      ((CPOL) == USART_POLARITY_HIGH))

#define IS_USART_CLOCK(CLOCK)        (((CLOCK) == USART_CLOCK_DISABLE) || \
                                      ((CLOCK) == USART_CLOCK_ENABLE))

#define IS_USART_WORD_LENGTH(LENGTH) (((LENGTH) == USART_WORDLENGTH_8B) || \
                                      ((LENGTH) == USART_WORDLENGTH_9B))

#define IS_USART_STOPBITS(STOPBITS)  (((STOPBITS) == USART_STOPBITS_1) || \
                                      ((STOPBITS) == USART_STOPBITS_2))

#define IS_USART_PARITY(PARITY)      (((PARITY) == USART_PARITY_NONE) || \
                                      ((PARITY) == USART_PARITY_EVEN) || \
                                      ((PARITY) == USART_PARITY_ODD))

#define IS_USART_MODE(MODE)          ((((MODE) & (~((uint32_t)USART_MODE_TX_RX))) == 0x00U) && ((MODE) != 0x00U))

#define IS_USART_BAUDRATE(BAUDRATE)  ((BAUDRATE) <= 4500000U)

#define USART_DIV(_PCLK_, _BAUD_)      (((_PCLK_)*25U)/(4U*(_BAUD_)))

#define USART_DIVMANT(_PCLK_, _BAUD_)  (USART_DIV((_PCLK_), (_BAUD_))/100U)

#define USART_DIVFRAQ(_PCLK_, _BAUD_)  (((USART_DIV((_PCLK_), (_BAUD_)) - (USART_DIVMANT((_PCLK_), (_BAUD_)) * 100U)) * 16U + 50U) / 100U)

#define USART_BRR(_PCLK_, _BAUD_)      (((USART_DIVMANT((_PCLK_), (_BAUD_)) << 4U) + \
                                        ((USART_DIVFRAQ((_PCLK_), (_BAUD_)) & 0xF0U) << 1U)) + \
                                         (USART_DIVFRAQ((_PCLK_), (_BAUD_)) & 0x0FU))
/* @} */

/* Private functions --------------------------------------------------------- */
/* @defgroup USART_Private_Functions USART Private Functions @{ */

/* @} */

/* @} */

/* @} */

#ifdef __cplusplus
}
#endif

#endif /* __PY32F0xx_HAL_USART_H */

/* (C) COPYRIGHT Puya *****END OF FILE */
