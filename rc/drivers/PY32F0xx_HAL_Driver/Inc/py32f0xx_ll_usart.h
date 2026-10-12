/**
 * @file    py32f0xx_ll_usart.h
 * @author  MCU Application Team
 * @brief   Header file of USART LL module.
 * @attention
 */

/* Define to prevent recursive inclusion ------------------------------------- */
#ifndef __PY32F0xx_LL_USART_H
#define __PY32F0xx_LL_USART_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------ */
#include "py32f0xx.h"

/* @addtogroup PY32F0xx_LL_Driver @{ */

#if defined (USART1) || defined (USART2)

/* @defgroup USART_LL USART @{ */

/* Private types ------------------------------------------------------------- */
/* Private variables --------------------------------------------------------- */

/* Private constants --------------------------------------------------------- */
/* @defgroup USART_LL_Private_Constants USART Private Constants @{ */

/* @} */

/* Private macros ------------------------------------------------------------ */
#if defined(USE_FULL_LL_DRIVER)
/* @defgroup USART_LL_Private_Macros USART Private Macros @{ */
/* @} */
#endif /* USE_FULL_LL_DRIVER */

/* Exported types ------------------------------------------------------------ */
#if defined(USE_FULL_LL_DRIVER)
/* @defgroup USART_LL_ES_INIT USART Exported Init structures @{ */

/* @brief LL USART Init Structure definition */
typedef struct
{
  uint32_t BaudRate;                  

  uint32_t DataWidth;                     /* This parameter can be a value of @ref USART_LL_EC_DATAWIDTH. */

  uint32_t StopBits;                    /* !< Specifies the number of stop bits transmitted. This parameter can be a value of @ref USART_LL_EC_STOPBITS. */

  uint32_t Parity;                        /* !< Specifies the parity mode. This parameter can be a value of @ref USART_LL_EC_PARITY. */

  uint32_t TransferDirection;             /* This parameter can be a value of @ref USART_LL_EC_DIRECTION. */

  uint32_t HardwareFlowControl;           /* This parameter can be a value of @ref USART_LL_EC_HWCONTROL. */

  uint32_t OverSampling;                  /* !< Specifies whether USART oversampling mode is 16 or 8. */

} LL_USART_InitTypeDef;

/* @brief LL USART Clock Init Structure definition */
typedef struct
{
  uint32_t ClockOutput;               

  uint32_t ClockPolarity;             

  uint32_t ClockPhase;                  /* This parameter can be a value of @ref USART_LL_EC_PHASE. For more details, refer to description of this function. */

  uint32_t LastBitClockPulse;             /* For more details, refer to description of this function. */

} LL_USART_ClockInitTypeDef;

/* @} */
#endif /* USE_FULL_LL_DRIVER */

/* Exported constants -------------------------------------------------------- */
/* @defgroup USART_LL_Exported_Constants USART Exported Constants @{ */

/* @defgroup USART_LL_EC_GET_FLAG Get Flags Defines @brief    Flags defines which can be used with LL_USART_ReadReg function @{ */
#define LL_USART_SR_PE                          USART_SR_PE                   /* !< Parity error flag */
#define LL_USART_SR_FE                          USART_SR_FE                   /* !< Framing error flag */
#define LL_USART_SR_NE                          USART_SR_NE                   /* !< Noise detected flag */
#define LL_USART_SR_ORE                         USART_SR_ORE                  /* !< Overrun error flag */
#define LL_USART_SR_IDLE                        USART_SR_IDLE                 /* !< Idle line detected flag */
#define LL_USART_SR_RXNE                        USART_SR_RXNE                 /* !< Read data register not empty flag */
#define LL_USART_SR_TC                          USART_SR_TC                   /* !< Transmission complete flag */
#define LL_USART_SR_TXE                         USART_SR_TXE                  /* !< Transmit data register empty flag */
#define LL_USART_SR_CTS                         USART_SR_CTS                  /* !< CTS flag */
/* @} */

/* @defgroup USART_LL_EC_IT IT Defines @brief    IT defines which can be used with LL_USART_ReadReg and  LL_USART_WriteReg functions @{ */
#define LL_USART_CR1_IDLEIE                     USART_CR1_IDLEIE              /* !< IDLE interrupt enable */
#define LL_USART_CR1_RXNEIE                     USART_CR1_RXNEIE              /* !< Read data register not empty interrupt enable */
#define LL_USART_CR1_TCIE                       USART_CR1_TCIE                /* !< Transmission complete interrupt enable */
#define LL_USART_CR1_TXEIE                      USART_CR1_TXEIE               /* !< Transmit data register empty interrupt enable */
#define LL_USART_CR1_PEIE                       USART_CR1_PEIE                /* !< Parity error */
#define LL_USART_CR3_EIE                        USART_CR3_EIE                 /* !< Error interrupt enable */
#define LL_USART_CR3_CTSIE                      USART_CR3_CTSIE               /* !< CTS interrupt enable */
/* @} */

/* @defgroup USART_LL_EC_DIRECTION Communication Direction @{ */
#define LL_USART_DIRECTION_NONE                 0x00000000U                        /* !< Transmitter and Receiver are disabled */
#define LL_USART_DIRECTION_RX                   USART_CR1_RE                       /* !< Transmitter is disabled and Receiver is enabled */
#define LL_USART_DIRECTION_TX                   USART_CR1_TE                       /* !< Transmitter is enabled and Receiver is disabled */
#define LL_USART_DIRECTION_TX_RX                (USART_CR1_TE |USART_CR1_RE)       /* !< Transmitter and Receiver are enabled */
/* @} */

/* @defgroup USART_LL_EC_PARITY Parity Control @{ */
#define LL_USART_PARITY_NONE                    0x00000000U                          /* !< Parity control disabled */
#define LL_USART_PARITY_EVEN                    USART_CR1_PCE                        /* !< Parity control enabled and Even Parity is selected */
#define LL_USART_PARITY_ODD                     (USART_CR1_PCE | USART_CR1_PS)       /* !< Parity control enabled and Odd Parity is selected */
/* @} */

/* @defgroup USART_LL_EC_WAKEUP Wakeup @{ */
#define LL_USART_WAKEUP_IDLELINE                0x00000000U           /* !<  USART wake up from Mute mode on Idle Line */
#define LL_USART_WAKEUP_ADDRESSMARK             USART_CR1_WAKE        /* !<  USART wake up from Mute mode on Address Mark */
/* @} */

/* @defgroup USART_LL_EC_DATAWIDTH Datawidth @{ */
#define LL_USART_DATAWIDTH_8B                   0x00000000U             /* !< 8 bits word length : Start bit, 8 data bits, n stop bits */
#define LL_USART_DATAWIDTH_9B                   USART_CR1_M             /* !< 9 bits word length : Start bit, 9 data bits, n stop bits */
/* @} */

/* @defgroup USART_LL_EC_OVERSAMPLING Oversampling @{ */
#define LL_USART_OVERSAMPLING_16                0x00000000U            /* !< Oversampling by 16 */
#if  defined(USART_CR3_OVER8)
#define LL_USART_OVERSAMPLING_8                 USART_CR3_OVER8        /* !< Oversampling by 8 */
#endif /* USART_OverSampling_Feature */
/* @} */

#if defined(USE_FULL_LL_DRIVER)
/* @defgroup USART_LL_EC_CLOCK Clock Signal @{ */

#define LL_USART_CLOCK_DISABLE                  0x00000000U            /* !< Clock signal not provided */
#define LL_USART_CLOCK_ENABLE                   USART_CR2_CLKEN        /* !< Clock signal provided */
/* @} */
#endif /* USE_FULL_LL_DRIVER */

/* @defgroup USART_LL_EC_LASTCLKPULSE Last Clock Pulse @{ */
#define LL_USART_LASTCLKPULSE_NO_OUTPUT         0x00000000U           /* !< The clock pulse of the last data bit is not output to the SCLK pin */
#define LL_USART_LASTCLKPULSE_OUTPUT            USART_CR2_LBCL        /* !< The clock pulse of the last data bit is output to the SCLK pin */
/* @} */

/* @defgroup USART_LL_EC_PHASE Clock Phase @{ */
#define LL_USART_PHASE_1EDGE                    0x00000000U           /* !< The first clock transition is the first data capture edge */
#define LL_USART_PHASE_2EDGE                    USART_CR2_CPHA        /* !< The second clock transition is the first data capture edge */
/* @} */

/* @defgroup USART_LL_EC_POLARITY Clock Polarity @{ */
#define LL_USART_POLARITY_LOW                   0x00000000U           /* !< Steady low value on SCLK pin outside transmission window */
#define LL_USART_POLARITY_HIGH                  USART_CR2_CPOL        /* !< Steady high value on SCLK pin outside transmission window */
/* @} */

/* @defgroup USART_LL_EC_STOPBITS Stop Bits @{ */
#define LL_USART_STOPBITS_1                     0x00000000U                                /* !< 1 stop bit */
#define LL_USART_STOPBITS_2                     USART_CR2_STOP                             /* !< 2 stop bits */
/* @} */

/* @defgroup USART_LL_EC_HWCONTROL Hardware Control @{ */
#define LL_USART_HWCONTROL_NONE                 0x00000000U                          /* !< CTS and RTS hardware flow control disabled */
#define LL_USART_HWCONTROL_RTS                  USART_CR3_RTSE                       /* !< RTS output enabled, data is only requested when there is space in the receive buffer */
#define LL_USART_HWCONTROL_CTS                  USART_CR3_CTSE                       /* !< CTS mode enabled, data is only transmitted when the nCTS input is asserted (tied to 0) */
#define LL_USART_HWCONTROL_RTS_CTS              (USART_CR3_RTSE | USART_CR3_CTSE)    /* !< CTS and RTS hardware flow control enabled */
/* @} */

/* @defgroup USART_LL_EC_AUTOBAUNDMODE Auto baud rate detection mode @{ */
#define LL_USART_AUTOBAUDRATE_ONSTARTBIT    0x00000000U           /* !< Auto Baud rate detection on start bit */
#define LL_USART_AUTOBAUDRATE_ONFALLINGEDGE USART_CR3_ABRMODE_0   /* !< Auto Baud rate detection on falling edge */
/* @} */

/* @} */

/* Exported macro ------------------------------------------------------------ */
/* @defgroup USART_LL_Exported_Macros USART Exported Macros @{ */

/* @defgroup USART_LL_EM_WRITE_READ Common Write and read registers Macros @{ */



#define LL_USART_WriteReg(__INSTANCE__, __REG__, __VALUE__) WRITE_REG(__INSTANCE__->__REG__, (__VALUE__))

/* @brief  Read a value in USART register @param  __INSTANCE__ USART Instance @param  __REG__ Register to be read @retval Register value */
#define LL_USART_ReadReg(__INSTANCE__, __REG__) READ_REG(__INSTANCE__->__REG__)
/* @} */

/* @defgroup USART_LL_EM_Exported_Macros_Helper Exported Macros Helper @{ */



#define __LL_USART_DIV_SAMPLING8_100(__PERIPHCLK__, __BAUDRATE__)      (((__PERIPHCLK__)*25)/(2*(__BAUDRATE__)))
#define __LL_USART_DIVMANT_SAMPLING8(__PERIPHCLK__, __BAUDRATE__)      (__LL_USART_DIV_SAMPLING8_100((__PERIPHCLK__), (__BAUDRATE__))/100)
#define __LL_USART_DIVFRAQ_SAMPLING8(__PERIPHCLK__, __BAUDRATE__)      (((__LL_USART_DIV_SAMPLING8_100((__PERIPHCLK__), (__BAUDRATE__)) - (__LL_USART_DIVMANT_SAMPLING8((__PERIPHCLK__), (__BAUDRATE__)) * 100)) * 8 + 50) / 100)

#define __LL_USART_DIV_SAMPLING8(__PERIPHCLK__, __BAUDRATE__)             (((__LL_USART_DIVMANT_SAMPLING8((__PERIPHCLK__), (__BAUDRATE__)) << 4) + \
                                                                           ((__LL_USART_DIVFRAQ_SAMPLING8((__PERIPHCLK__), (__BAUDRATE__)) & 0xF8) << 1)) + \
                                                                           (__LL_USART_DIVFRAQ_SAMPLING8((__PERIPHCLK__), (__BAUDRATE__)) & 0x07))



#define __LL_USART_DIV_SAMPLING16_100(__PERIPHCLK__, __BAUDRATE__)     (((__PERIPHCLK__)*25)/(4*(__BAUDRATE__)))
#define __LL_USART_DIVMANT_SAMPLING16(__PERIPHCLK__, __BAUDRATE__)     (__LL_USART_DIV_SAMPLING16_100((__PERIPHCLK__), (__BAUDRATE__))/100)
#define __LL_USART_DIVFRAQ_SAMPLING16(__PERIPHCLK__, __BAUDRATE__)     (((__LL_USART_DIV_SAMPLING16_100((__PERIPHCLK__), (__BAUDRATE__)) - (__LL_USART_DIVMANT_SAMPLING16((__PERIPHCLK__), (__BAUDRATE__)) * 100)) * 16 + 50) / 100)

#define __LL_USART_DIV_SAMPLING16(__PERIPHCLK__, __BAUDRATE__)            (((__LL_USART_DIVMANT_SAMPLING16((__PERIPHCLK__), (__BAUDRATE__)) << 4) + \
                                                                           (__LL_USART_DIVFRAQ_SAMPLING16((__PERIPHCLK__), (__BAUDRATE__)) & 0xF0)) + \
                                                                           (__LL_USART_DIVFRAQ_SAMPLING16((__PERIPHCLK__), (__BAUDRATE__)) & 0x0F))

/* @} */

/* @} */

/* Exported functions -------------------------------------------------------- */

/* @defgroup USART_LL_Exported_Functions USART Exported Functions @{ */

/* @defgroup USART_LL_EF_Configuration Configuration functions @{ */

/* @brief  USART Enable @rmtoll CR1          UE            LL_USART_Enable @param  USARTx USART Instance @retval None */
__STATIC_INLINE void LL_USART_Enable(USART_TypeDef *USARTx)
{
  SET_BIT(USARTx->CR1, USART_CR1_UE);
}



__STATIC_INLINE void LL_USART_Disable(USART_TypeDef *USARTx)
{
  CLEAR_BIT(USARTx->CR1, USART_CR1_UE);
}



__STATIC_INLINE uint32_t LL_USART_IsEnabled(USART_TypeDef *USARTx)
{
  return (READ_BIT(USARTx->CR1, USART_CR1_UE) == (USART_CR1_UE));
}

/* @brief  Receiver Enable (Receiver is enabled and begins searching for a start bit) @param  USARTx USART Instance @retval None */
__STATIC_INLINE void LL_USART_EnableDirectionRx(USART_TypeDef *USARTx)
{
  SET_BIT(USARTx->CR1, USART_CR1_RE);
}

/* @brief  Receiver Disable @param  USARTx USART Instance @retval None */
__STATIC_INLINE void LL_USART_DisableDirectionRx(USART_TypeDef *USARTx)
{
  CLEAR_BIT(USARTx->CR1, USART_CR1_RE);
}

/* @brief  Transmitter Enable @param  USARTx USART Instance @retval None */
__STATIC_INLINE void LL_USART_EnableDirectionTx(USART_TypeDef *USARTx)
{
  SET_BIT(USARTx->CR1, USART_CR1_TE);
}

/* @brief  Transmitter Disable @param  USARTx USART Instance @retval None */
__STATIC_INLINE void LL_USART_DisableDirectionTx(USART_TypeDef *USARTx)
{
  CLEAR_BIT(USARTx->CR1, USART_CR1_TE);
}



__STATIC_INLINE void LL_USART_SetTransferDirection(USART_TypeDef *USARTx, uint32_t TransferDirection)
{
  MODIFY_REG(USARTx->CR1, USART_CR1_RE | USART_CR1_TE, TransferDirection);
}



__STATIC_INLINE uint32_t LL_USART_GetTransferDirection(USART_TypeDef *USARTx)
{
  return (uint32_t)(READ_BIT(USARTx->CR1, USART_CR1_RE | USART_CR1_TE));
}



__STATIC_INLINE void LL_USART_SetParity(USART_TypeDef *USARTx, uint32_t Parity)
{
  MODIFY_REG(USARTx->CR1, USART_CR1_PS | USART_CR1_PCE, Parity);
}



__STATIC_INLINE uint32_t LL_USART_GetParity(USART_TypeDef *USARTx)
{
  return (uint32_t)(READ_BIT(USARTx->CR1, USART_CR1_PS | USART_CR1_PCE));
}



__STATIC_INLINE void LL_USART_SetWakeUpMethod(USART_TypeDef *USARTx, uint32_t Method)
{
  MODIFY_REG(USARTx->CR1, USART_CR1_WAKE, Method);
}



__STATIC_INLINE uint32_t LL_USART_GetWakeUpMethod(USART_TypeDef *USARTx)
{
  return (uint32_t)(READ_BIT(USARTx->CR1, USART_CR1_WAKE));
}



__STATIC_INLINE void LL_USART_SetDataWidth(USART_TypeDef *USARTx, uint32_t DataWidth)
{
  MODIFY_REG(USARTx->CR1, USART_CR1_M, DataWidth);
}



__STATIC_INLINE uint32_t LL_USART_GetDataWidth(USART_TypeDef *USARTx)
{
  return (uint32_t)(READ_BIT(USARTx->CR1, USART_CR1_M));
}

#if defined(USART_CR3_OVER8)

__STATIC_INLINE void LL_USART_SetOverSampling(USART_TypeDef *USARTx, uint32_t OverSampling)
{
  MODIFY_REG(USARTx->CR3, USART_CR3_OVER8, OverSampling);
}



__STATIC_INLINE uint32_t LL_USART_GetOverSampling(USART_TypeDef *USARTx)
{
  return (uint32_t)(READ_BIT(USARTx->CR3, USART_CR3_OVER8));
}
#endif /* USART_OverSampling_Feature */



__STATIC_INLINE void LL_USART_SetLastClkPulseOutput(USART_TypeDef *USARTx, uint32_t LastBitClockPulse)
{
  MODIFY_REG(USARTx->CR2, USART_CR2_LBCL, LastBitClockPulse);
}



__STATIC_INLINE uint32_t LL_USART_GetLastClkPulseOutput(USART_TypeDef *USARTx)
{
  return (uint32_t)(READ_BIT(USARTx->CR2, USART_CR2_LBCL));
}



__STATIC_INLINE void LL_USART_SetClockPhase(USART_TypeDef *USARTx, uint32_t ClockPhase)
{
  MODIFY_REG(USARTx->CR2, USART_CR2_CPHA, ClockPhase);
}



__STATIC_INLINE uint32_t LL_USART_GetClockPhase(USART_TypeDef *USARTx)
{
  return (uint32_t)(READ_BIT(USARTx->CR2, USART_CR2_CPHA));
}



__STATIC_INLINE void LL_USART_SetClockPolarity(USART_TypeDef *USARTx, uint32_t ClockPolarity)
{
  MODIFY_REG(USARTx->CR2, USART_CR2_CPOL, ClockPolarity);
}



__STATIC_INLINE uint32_t LL_USART_GetClockPolarity(USART_TypeDef *USARTx)
{
  return (uint32_t)(READ_BIT(USARTx->CR2, USART_CR2_CPOL));
}



__STATIC_INLINE void LL_USART_ConfigClock(USART_TypeDef *USARTx, uint32_t Phase, uint32_t Polarity, uint32_t LBCPOutput)
{
  MODIFY_REG(USARTx->CR2, USART_CR2_CPHA | USART_CR2_CPOL | USART_CR2_LBCL, Phase | Polarity | LBCPOutput);
}



__STATIC_INLINE void LL_USART_EnableSCLKOutput(USART_TypeDef *USARTx)
{
  SET_BIT(USARTx->CR2, USART_CR2_CLKEN);
}



__STATIC_INLINE void LL_USART_DisableSCLKOutput(USART_TypeDef *USARTx)
{
  CLEAR_BIT(USARTx->CR2, USART_CR2_CLKEN);
}



__STATIC_INLINE uint32_t LL_USART_IsEnabledSCLKOutput(USART_TypeDef *USARTx)
{
  return (READ_BIT(USARTx->CR2, USART_CR2_CLKEN) == (USART_CR2_CLKEN));
}



__STATIC_INLINE void LL_USART_SetStopBitsLength(USART_TypeDef *USARTx, uint32_t StopBits)
{
  MODIFY_REG(USARTx->CR2, USART_CR2_STOP, StopBits);
}



__STATIC_INLINE uint32_t LL_USART_GetStopBitsLength(USART_TypeDef *USARTx)
{
  return (uint32_t)(READ_BIT(USARTx->CR2, USART_CR2_STOP));
}



__STATIC_INLINE void LL_USART_ConfigCharacter(USART_TypeDef *USARTx, uint32_t DataWidth, uint32_t Parity,
    uint32_t StopBits)
{
  MODIFY_REG(USARTx->CR1, USART_CR1_PS | USART_CR1_PCE | USART_CR1_M, Parity | DataWidth);
  MODIFY_REG(USARTx->CR2, USART_CR2_STOP, StopBits);
}



__STATIC_INLINE void LL_USART_SetNodeAddress(USART_TypeDef *USARTx, uint32_t NodeAddress)
{
  MODIFY_REG(USARTx->CR2, USART_CR2_ADD, (NodeAddress & USART_CR2_ADD));
}



__STATIC_INLINE uint32_t LL_USART_GetNodeAddress(USART_TypeDef *USARTx)
{
  return (uint32_t)(READ_BIT(USARTx->CR2, USART_CR2_ADD));
}



__STATIC_INLINE void LL_USART_EnableRTSHWFlowCtrl(USART_TypeDef *USARTx)
{
  SET_BIT(USARTx->CR3, USART_CR3_RTSE);
}



__STATIC_INLINE void LL_USART_DisableRTSHWFlowCtrl(USART_TypeDef *USARTx)
{
  CLEAR_BIT(USARTx->CR3, USART_CR3_RTSE);
}



__STATIC_INLINE void LL_USART_EnableCTSHWFlowCtrl(USART_TypeDef *USARTx)
{
  SET_BIT(USARTx->CR3, USART_CR3_CTSE);
}



__STATIC_INLINE void LL_USART_DisableCTSHWFlowCtrl(USART_TypeDef *USARTx)
{
  CLEAR_BIT(USARTx->CR3, USART_CR3_CTSE);
}



__STATIC_INLINE void LL_USART_SetHWFlowCtrl(USART_TypeDef *USARTx, uint32_t HardwareFlowControl)
{
  MODIFY_REG(USARTx->CR3, USART_CR3_RTSE | USART_CR3_CTSE, HardwareFlowControl);
}



__STATIC_INLINE uint32_t LL_USART_GetHWFlowCtrl(USART_TypeDef *USARTx)
{
  return (uint32_t)(READ_BIT(USARTx->CR3, USART_CR3_RTSE | USART_CR3_CTSE));
}



#if defined(USART_CR3_OVER8)

__STATIC_INLINE void LL_USART_SetBaudRate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t OverSampling,
                                          uint32_t BaudRate)
{
  if (OverSampling == LL_USART_OVERSAMPLING_8)
  {
    USARTx->BRR = (uint16_t)(__LL_USART_DIV_SAMPLING8(PeriphClk, BaudRate));
  }
  else
  {
    USARTx->BRR = (uint16_t)(__LL_USART_DIV_SAMPLING16(PeriphClk, BaudRate));
  }
}



__STATIC_INLINE uint32_t LL_USART_GetBaudRate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t OverSampling)
{
  register uint32_t usartdiv = 0x0U;
  register uint32_t brrresult = 0x0U;

  usartdiv = USARTx->BRR;

  if (OverSampling == LL_USART_OVERSAMPLING_8)
  {
    if ((usartdiv & 0xFFF7U) != 0U)
    {
      usartdiv = (uint16_t)((usartdiv & 0xFFF0U) | ((usartdiv & 0x0007U) << 1U)) ;
      brrresult = (PeriphClk * 2U) / usartdiv;
    }
  }
  else
  {
    if ((usartdiv & 0xFFFFU) != 0U)
    {
      brrresult = PeriphClk / usartdiv;
    }
  }
  return (brrresult);
}
#else

__STATIC_INLINE void LL_USART_SetBaudRate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate)
{
  USARTx->BRR = (uint16_t)(__LL_USART_DIV_SAMPLING16(PeriphClk, BaudRate));
}



__STATIC_INLINE uint32_t LL_USART_GetBaudRate(USART_TypeDef *USARTx, uint32_t PeriphClk)
{
  register uint32_t usartdiv = 0x0U;
  register uint32_t brrresult = 0x0U;

  usartdiv = USARTx->BRR;

  if ((usartdiv & 0xFFFFU) != 0U)
  {
    brrresult = PeriphClk / usartdiv;
  }
  return (brrresult);
}
#endif /* USART_OverSampling_Feature */

/* @} */



__STATIC_INLINE void LL_USART_EnableHalfDuplex(USART_TypeDef *USARTx)
{
  SET_BIT(USARTx->CR3, USART_CR3_HDSEL);
}



__STATIC_INLINE void LL_USART_DisableHalfDuplex(USART_TypeDef *USARTx)
{
  CLEAR_BIT(USARTx->CR3, USART_CR3_HDSEL);
}



__STATIC_INLINE uint32_t LL_USART_IsEnabledHalfDuplex(USART_TypeDef *USARTx)
{
  return (READ_BIT(USARTx->CR3, USART_CR3_HDSEL) == (USART_CR3_HDSEL));
}

/* @} */

/* @defgroup USART_LL_EF_Advanced_Configuration Advanced Configurations services @{ */



__STATIC_INLINE void LL_USART_ConfigAsyncMode(USART_TypeDef *USARTx)
{
        /* - CLKEN bits in the USART_CR2 register, - HDSEL bits in the USART_CR3 register. */
  CLEAR_BIT(USARTx->CR2, USART_CR2_CLKEN);
  CLEAR_BIT(USARTx->CR3, USART_CR3_HDSEL);
}



__STATIC_INLINE void LL_USART_ConfigSyncMode(USART_TypeDef *USARTx)
{

  CLEAR_BIT(USARTx->CR3, USART_CR3_HDSEL);
        /* set the UART/USART in Synchronous mode */
  SET_BIT(USARTx->CR2, USART_CR2_CLKEN);
}



__STATIC_INLINE void LL_USART_ConfigHalfDuplexMode(USART_TypeDef *USARTx)
{

  CLEAR_BIT(USARTx->CR2, USART_CR2_CLKEN);
        /* set the UART/USART in Half Duplex mode */
  SET_BIT(USARTx->CR3, USART_CR3_HDSEL);
}



__STATIC_INLINE void LL_USART_ConfigMultiProcessMode(USART_TypeDef *USARTx)
{
        /* - CLKEN bits in the USART_CR2 register, - HDSEL bits in the USART_CR3 register. */
  CLEAR_BIT(USARTx->CR2, USART_CR2_CLKEN);
  CLEAR_BIT(USARTx->CR3, USART_CR3_HDSEL);
}

/* @} */

/* @defgroup USART_LL_EF_FLAG_Management FLAG Management @{ */



__STATIC_INLINE uint32_t LL_USART_IsActiveFlag_PE(USART_TypeDef *USARTx)
{
  return (READ_BIT(USARTx->SR, USART_SR_PE) == (USART_SR_PE));
}



__STATIC_INLINE uint32_t LL_USART_IsActiveFlag_FE(USART_TypeDef *USARTx)
{
  return (READ_BIT(USARTx->SR, USART_SR_FE) == (USART_SR_FE));
}



__STATIC_INLINE uint32_t LL_USART_IsActiveFlag_NE(USART_TypeDef *USARTx)
{
  return (READ_BIT(USARTx->SR, USART_SR_NE) == (USART_SR_NE));
}



__STATIC_INLINE uint32_t LL_USART_IsActiveFlag_ORE(USART_TypeDef *USARTx)
{
  return (READ_BIT(USARTx->SR, USART_SR_ORE) == (USART_SR_ORE));
}

/* @brief  Check if the USART IDLE line detected Flag is set or not @param  USARTx USART Instance @retval State of bit (1 or 0). */
__STATIC_INLINE uint32_t LL_USART_IsActiveFlag_IDLE(USART_TypeDef *USARTx)
{
  return (READ_BIT(USARTx->SR, USART_SR_IDLE) == (USART_SR_IDLE));
}

/* @brief  Check if the USART Read Data Register Not Empty Flag is set or not @param  USARTx USART Instance @retval State of bit (1 or 0). */
__STATIC_INLINE uint32_t LL_USART_IsActiveFlag_RXNE(USART_TypeDef *USARTx)
{
  return (READ_BIT(USARTx->SR, USART_SR_RXNE) == (USART_SR_RXNE));
}



__STATIC_INLINE uint32_t LL_USART_IsActiveFlag_TC(USART_TypeDef *USARTx)
{
  return (READ_BIT(USARTx->SR, USART_SR_TC) == (USART_SR_TC));
}



__STATIC_INLINE uint32_t LL_USART_IsActiveFlag_TXE(USART_TypeDef *USARTx)
{
  return (READ_BIT(USARTx->SR, USART_SR_TXE) == (USART_SR_TXE));
}



__STATIC_INLINE uint32_t LL_USART_IsActiveFlag_nCTS(USART_TypeDef *USARTx)
{
  return (READ_BIT(USARTx->SR, USART_SR_CTS) == (USART_SR_CTS));
}

/* @brief  Check if the USART ABRF Flag is set or not @param  USARTx USART Instance @retval State of bit (1 or 0). */
__STATIC_INLINE uint32_t LL_USART_IsActiveFlag_ABRF(USART_TypeDef *USARTx)
{
  return (READ_BIT(USARTx->SR, USART_SR_ABRF) == (USART_SR_ABRF));
}

/* @brief  Check if the USART ABRE Flag is set or not @param  USARTx USART Instance @retval State of bit (1 or 0). */
__STATIC_INLINE uint32_t LL_USART_IsActiveFlag_ABRE(USART_TypeDef *USARTx)
{
  return (READ_BIT(USARTx->SR, USART_SR_ABRE) == (USART_SR_ABRE));
}



__STATIC_INLINE uint32_t LL_USART_IsActiveFlag_SBK(USART_TypeDef *USARTx)
{
  return (READ_BIT(USARTx->CR1, USART_CR1_SBK) == (USART_CR1_SBK));
}



__STATIC_INLINE uint32_t LL_USART_IsActiveFlag_RWU(USART_TypeDef *USARTx)
{
  return (READ_BIT(USARTx->CR1, USART_CR1_RWU) == (USART_CR1_RWU));
}



__STATIC_INLINE void LL_USART_ClearFlag_PE(USART_TypeDef *USARTx)
{
  __IO uint32_t tmpreg;
  tmpreg = USARTx->SR;
  (void) tmpreg;
  tmpreg = USARTx->DR;
  (void) tmpreg;
}



__STATIC_INLINE void LL_USART_ClearFlag_FE(USART_TypeDef *USARTx)
{
  __IO uint32_t tmpreg;
  tmpreg = USARTx->SR;
  (void) tmpreg;
  tmpreg = USARTx->DR;
  (void) tmpreg;
}



__STATIC_INLINE void LL_USART_ClearFlag_NE(USART_TypeDef *USARTx)
{
  __IO uint32_t tmpreg;
  tmpreg = USARTx->SR;
  (void) tmpreg;
  tmpreg = USARTx->DR;
  (void) tmpreg;
}



__STATIC_INLINE void LL_USART_ClearFlag_ORE(USART_TypeDef *USARTx)
{
  __IO uint32_t tmpreg;
  tmpreg = USARTx->SR;
  (void) tmpreg;
  tmpreg = USARTx->DR;
  (void) tmpreg;
}



__STATIC_INLINE void LL_USART_ClearFlag_IDLE(USART_TypeDef *USARTx)
{
  __IO uint32_t tmpreg;
  tmpreg = USARTx->SR;
  (void) tmpreg;
  tmpreg = USARTx->DR;
  (void) tmpreg;
}

/* @brief  Clear Transmission Complete Flag @rmtoll SR           TC            LL_USART_ClearFlag_TC @param  USARTx USART Instance @retval None */
__STATIC_INLINE void LL_USART_ClearFlag_TC(USART_TypeDef *USARTx)
{
  WRITE_REG(USARTx->SR, ~(USART_SR_TC));
}

/* @brief  Clear RX Not Empty Flag @rmtoll SR           RXNE          LL_USART_ClearFlag_RXNE @param  USARTx USART Instance @retval None */
__STATIC_INLINE void LL_USART_ClearFlag_RXNE(USART_TypeDef *USARTx)
{
  WRITE_REG(USARTx->SR, ~(USART_SR_RXNE));
}



__STATIC_INLINE void LL_USART_ClearFlag_nCTS(USART_TypeDef *USARTx)
{
  WRITE_REG(USARTx->SR, ~(USART_SR_CTS));
}

/* @} */

/* @defgroup USART_LL_EF_IT_Management IT Management @{ */

/* @brief  Enable IDLE Interrupt @rmtoll CR1          IDLEIE        LL_USART_EnableIT_IDLE @param  USARTx USART Instance @retval None */
__STATIC_INLINE void LL_USART_EnableIT_IDLE(USART_TypeDef *USARTx)
{
  SET_BIT(USARTx->CR1, USART_CR1_IDLEIE);
}

/* @brief  Enable RX Not Empty Interrupt @rmtoll CR1          RXNEIE        LL_USART_EnableIT_RXNE @param  USARTx USART Instance @retval None */
__STATIC_INLINE void LL_USART_EnableIT_RXNE(USART_TypeDef *USARTx)
{
  SET_BIT(USARTx->CR1, USART_CR1_RXNEIE);
}



__STATIC_INLINE void LL_USART_EnableIT_TC(USART_TypeDef *USARTx)
{
  SET_BIT(USARTx->CR1, USART_CR1_TCIE);
}

/* @brief  Enable TX Empty Interrupt @rmtoll CR1          TXEIE         LL_USART_EnableIT_TXE @param  USARTx USART Instance @retval None */
__STATIC_INLINE void LL_USART_EnableIT_TXE(USART_TypeDef *USARTx)
{
  SET_BIT(USARTx->CR1, USART_CR1_TXEIE);
}

/* @brief  Enable Parity Error Interrupt @rmtoll CR1          PEIE          LL_USART_EnableIT_PE @param  USARTx USART Instance @retval None */
__STATIC_INLINE void LL_USART_EnableIT_PE(USART_TypeDef *USARTx)
{
  SET_BIT(USARTx->CR1, USART_CR1_PEIE);
}



__STATIC_INLINE void LL_USART_EnableIT_ERROR(USART_TypeDef *USARTx)
{
  SET_BIT(USARTx->CR3, USART_CR3_EIE);
}



__STATIC_INLINE void LL_USART_EnableIT_CTS(USART_TypeDef *USARTx)
{
  SET_BIT(USARTx->CR3, USART_CR3_CTSIE);
}

/* @brief  Disable IDLE Interrupt @rmtoll CR1          IDLEIE        LL_USART_DisableIT_IDLE @param  USARTx USART Instance @retval None */
__STATIC_INLINE void LL_USART_DisableIT_IDLE(USART_TypeDef *USARTx)
{
  CLEAR_BIT(USARTx->CR1, USART_CR1_IDLEIE);
}

/* @brief  Disable RX Not Empty Interrupt @rmtoll CR1          RXNEIE        LL_USART_DisableIT_RXNE @param  USARTx USART Instance @retval None */
__STATIC_INLINE void LL_USART_DisableIT_RXNE(USART_TypeDef *USARTx)
{
  CLEAR_BIT(USARTx->CR1, USART_CR1_RXNEIE);
}



__STATIC_INLINE void LL_USART_DisableIT_TC(USART_TypeDef *USARTx)
{
  CLEAR_BIT(USARTx->CR1, USART_CR1_TCIE);
}

/* @brief  Disable TX Empty Interrupt @rmtoll CR1          TXEIE         LL_USART_DisableIT_TXE @param  USARTx USART Instance @retval None */
__STATIC_INLINE void LL_USART_DisableIT_TXE(USART_TypeDef *USARTx)
{
  CLEAR_BIT(USARTx->CR1, USART_CR1_TXEIE);
}

/* @brief  Disable Parity Error Interrupt @rmtoll CR1          PEIE          LL_USART_DisableIT_PE @param  USARTx USART Instance @retval None */
__STATIC_INLINE void LL_USART_DisableIT_PE(USART_TypeDef *USARTx)
{
  CLEAR_BIT(USARTx->CR1, USART_CR1_PEIE);
}



__STATIC_INLINE void LL_USART_DisableIT_ERROR(USART_TypeDef *USARTx)
{
  CLEAR_BIT(USARTx->CR3, USART_CR3_EIE);
}



__STATIC_INLINE void LL_USART_DisableIT_CTS(USART_TypeDef *USARTx)
{
  CLEAR_BIT(USARTx->CR3, USART_CR3_CTSIE);
}



__STATIC_INLINE uint32_t LL_USART_IsEnabledIT_IDLE(USART_TypeDef *USARTx)
{
  return (READ_BIT(USARTx->CR1, USART_CR1_IDLEIE) == (USART_CR1_IDLEIE));
}



__STATIC_INLINE uint32_t LL_USART_IsEnabledIT_RXNE(USART_TypeDef *USARTx)
{
  return (READ_BIT(USARTx->CR1, USART_CR1_RXNEIE) == (USART_CR1_RXNEIE));
}



__STATIC_INLINE uint32_t LL_USART_IsEnabledIT_TC(USART_TypeDef *USARTx)
{
  return (READ_BIT(USARTx->CR1, USART_CR1_TCIE) == (USART_CR1_TCIE));
}



__STATIC_INLINE uint32_t LL_USART_IsEnabledIT_TXE(USART_TypeDef *USARTx)
{
  return (READ_BIT(USARTx->CR1, USART_CR1_TXEIE) == (USART_CR1_TXEIE));
}



__STATIC_INLINE uint32_t LL_USART_IsEnabledIT_PE(USART_TypeDef *USARTx)
{
  return (READ_BIT(USARTx->CR1, USART_CR1_PEIE) == (USART_CR1_PEIE));
}



/* @brief  Check if the USART Error Interrupt is enabled or disabled. @param  USARTx USART Instance @retval State of bit (1 or 0). */
__STATIC_INLINE uint32_t LL_USART_IsEnabledIT_ERROR(USART_TypeDef *USARTx)
{
  return (READ_BIT(USARTx->CR3, USART_CR3_EIE) == (USART_CR3_EIE));
}



__STATIC_INLINE uint32_t LL_USART_IsEnabledIT_CTS(USART_TypeDef *USARTx)
{
  return (READ_BIT(USARTx->CR3, USART_CR3_CTSIE) == (USART_CR3_CTSIE));
}

/* @} */

#if (defined(DMA1) || defined(DMA))
/* @defgroup USART_LL_EF_DMA_Management DMA_Management @{ */



__STATIC_INLINE void LL_USART_EnableDMAReq_RX(USART_TypeDef *USARTx)
{
  SET_BIT(USARTx->CR3, USART_CR3_DMAR);
}



__STATIC_INLINE void LL_USART_DisableDMAReq_RX(USART_TypeDef *USARTx)
{
  CLEAR_BIT(USARTx->CR3, USART_CR3_DMAR);
}



__STATIC_INLINE uint32_t LL_USART_IsEnabledDMAReq_RX(USART_TypeDef *USARTx)
{
  return (READ_BIT(USARTx->CR3, USART_CR3_DMAR) == (USART_CR3_DMAR));
}



__STATIC_INLINE void LL_USART_EnableDMAReq_TX(USART_TypeDef *USARTx)
{
  SET_BIT(USARTx->CR3, USART_CR3_DMAT);
}



__STATIC_INLINE void LL_USART_DisableDMAReq_TX(USART_TypeDef *USARTx)
{
  CLEAR_BIT(USARTx->CR3, USART_CR3_DMAT);
}



__STATIC_INLINE uint32_t LL_USART_IsEnabledDMAReq_TX(USART_TypeDef *USARTx)
{
  return (READ_BIT(USARTx->CR3, USART_CR3_DMAT) == (USART_CR3_DMAT));
}



__STATIC_INLINE uint32_t LL_USART_DMA_GetRegAddr(USART_TypeDef *USARTx)
{
        /* return address of DR register */
  return ((uint32_t) & (USARTx->DR));
}
/* @} */
#endif /* DMA1 or DMA */

/* @defgroup USART_LL_EF_Data_Management Data_Management @{ */



__STATIC_INLINE uint8_t LL_USART_ReceiveData8(USART_TypeDef *USARTx)
{
  return (uint8_t)(READ_BIT(USARTx->DR, USART_DR_DR));
}



__STATIC_INLINE uint16_t LL_USART_ReceiveData9(USART_TypeDef *USARTx)
{
  return (uint16_t)(READ_BIT(USARTx->DR, USART_DR_DR));
}



__STATIC_INLINE void LL_USART_TransmitData8(USART_TypeDef *USARTx, uint8_t Value)
{
  USARTx->DR = Value;
}



__STATIC_INLINE void LL_USART_TransmitData9(USART_TypeDef *USARTx, uint16_t Value)
{
  USARTx->DR = Value & 0x1FFU;
}

/* @} */

/* @defgroup USART_LL_EF_Execution Execution @{ */

/* @brief  Request Break sending @param  USARTx USART Instance @retval None */
__STATIC_INLINE void LL_USART_RequestBreakSending(USART_TypeDef *USARTx)
{
  SET_BIT(USARTx->CR1, USART_CR1_SBK);
}

/* @brief  Put USART in Mute mode @param  USARTx USART Instance @retval None */
__STATIC_INLINE void LL_USART_RequestEnterMuteMode(USART_TypeDef *USARTx)
{
  SET_BIT(USARTx->CR1, USART_CR1_RWU);
}

/* @brief  Put USART in Active mode @param  USARTx USART Instance @retval None */
__STATIC_INLINE void LL_USART_RequestExitMuteMode(USART_TypeDef *USARTx)
{
  CLEAR_BIT(USARTx->CR1, USART_CR1_RWU);
}

/* @brief  Enable automatic baud rate detection @param  USARTx USART Instance @retval None */
__STATIC_INLINE void LL_USART_EnableAutoBaudRate(USART_TypeDef *USARTx)
{
  SET_BIT(USARTx->CR3, USART_CR3_ABREN);
}

/* @brief  Disable automatic baud rate detection @param  USARTx USART Instance @retval None */
__STATIC_INLINE void LL_USART_DisableAutoBaudRate(USART_TypeDef *USARTx)
{
  CLEAR_BIT(USARTx->CR3, USART_CR3_ABREN);
}

/* @brief  Indicate if enable automatic baud rate detection @param  USARTx USART Instance @retval State of bit (1 or 0). */
__STATIC_INLINE uint32_t LL_USART_IsEnabledAutoBaudRate(USART_TypeDef *USARTx)
{
  return (READ_BIT(USARTx->CR3, USART_CR3_ABREN) == (USART_CR3_ABREN));
}



__STATIC_INLINE void LL_USART_SetAutoBaudRateMode(USART_TypeDef *USARTx, uint32_t mode)
{
  MODIFY_REG(USARTx->CR3, USART_CR3_ABRMODE, mode);
}



__STATIC_INLINE uint32_t LL_USART_GetAutoBaudRateMode(USART_TypeDef *USARTx)
{
  return (uint32_t)(READ_BIT(USARTx->CR3, USART_CR3_ABRMODE));
}

/* @brief  Request automatic baud rate detection @param  USARTx USART Instance @retval None */
__STATIC_INLINE void LL_USART_SendAutoBaudRateReq(USART_TypeDef *USARTx)
{
  SET_BIT(USARTx->SR, USART_SR_ABRRQ);
}

/* @} */

#if defined(USE_FULL_LL_DRIVER)
/* @defgroup USART_LL_EF_Init Initialization and de-initialization functions @{ */
ErrorStatus LL_USART_DeInit(USART_TypeDef *USARTx);
ErrorStatus LL_USART_Init(USART_TypeDef *USARTx, LL_USART_InitTypeDef *USART_InitStruct);
void        LL_USART_StructInit(LL_USART_InitTypeDef *USART_InitStruct);
ErrorStatus LL_USART_ClockInit(USART_TypeDef *USARTx, LL_USART_ClockInitTypeDef *USART_ClockInitStruct);
void        LL_USART_ClockStructInit(LL_USART_ClockInitTypeDef *USART_ClockInitStruct);
/* @} */
#endif /* USE_FULL_LL_DRIVER */

/* @} */

/* @} */

#endif /* USART1 || USART2 */

/* @} */

#ifdef __cplusplus
}
#endif

#endif /* __PY32F0XX_LL_USART_H */

/* (C) COPYRIGHT Puya *****END OF FILE */
