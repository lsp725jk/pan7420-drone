/**
 * @file    py32f0xx_ll_spi.h
 * @author  MCU Application Team
 * @brief   Header file of SPI LL module.
 * @attention
 */

/* Define to prevent recursive inclusion ------------------------------------- */
#ifndef PY32F0xx_LL_SPI_H
#define PY32F0xx_LL_SPI_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------ */
#include "py32f0xx.h"



/* @addtogroup PY32F0xx_LL_Driver @{ */

#if defined (SPI1) || defined (SPI2)

/* @defgroup SPI_LL SPI @{ */

/* Private types ------------------------------------------------------------- */
/* Private variables --------------------------------------------------------- */
/* Private macros ------------------------------------------------------------ */

/* Exported types ------------------------------------------------------------ */
#if defined(USE_FULL_LL_DRIVER)
/* @defgroup SPI_LL_ES_INIT SPI Exported Init structure @{ */

/* @brief  SPI Init structures definition */
typedef struct
{
  uint32_t TransferDirection;       

  uint32_t Mode;                      /* !< Specifies the SPI mode (Master/Slave). This parameter can be a value of @ref SPI_LL_EC_MODE. */

  uint32_t DataWidth;                 /* !< Specifies the SPI data width. This parameter can be a value of @ref SPI_LL_EC_DATAWIDTH. */

  uint32_t ClockPolarity;             /* !< Specifies the serial clock steady state. This parameter can be a value of @ref SPI_LL_EC_POLARITY. */

  uint32_t ClockPhase;                /* !< Specifies the clock active edge for the bit capture. This parameter can be a value of @ref SPI_LL_EC_PHASE. */

  uint32_t NSS;                         /* This parameter can be a value of @ref SPI_LL_EC_NSS_MODE. */

  uint32_t BaudRate;                  /* @note The communication clock is derived from the master clock. The slave clock does not need to be set. */

  uint32_t BitOrder;                    /* This parameter can be a value of @ref SPI_LL_EC_BIT_ORDER. */

  uint32_t SlaveSpeedMode;          

} LL_SPI_InitTypeDef;

/* @} */
#endif /* USE_FULL_LL_DRIVER */

/* Exported constants -------------------------------------------------------- */
/* @defgroup SPI_LL_Exported_Constants SPI Exported Constants @{ */

/* @defgroup SPI_LL_EC_GET_FLAG Get Flags Defines @brief    Flags defines which can be used with LL_SPI_ReadReg function @{ */
#define LL_SPI_SR_RXNE                     SPI_SR_RXNE               /* !< Rx buffer not empty flag */
#define LL_SPI_SR_TXE                      SPI_SR_TXE                /* !< Tx buffer empty flag */
#define LL_SPI_SR_BSY                      SPI_SR_BSY                /* !< Busy flag */
#define LL_SPI_SR_MODF                     SPI_SR_MODF               /* !< Mode fault flag */
#define LL_SPI_SR_OVR                      SPI_SR_OVR                /* !< Overrun flag */
/* @} */

/* @defgroup SPI_LL_EC_IT IT Defines @brief    IT defines which can be used with LL_SPI_ReadReg and  LL_SPI_WriteReg functions @{ */
#define LL_SPI_CR2_RXNEIE                  SPI_CR2_RXNEIE            /* !< Rx buffer not empty interrupt enable */
#define LL_SPI_CR2_TXEIE                   SPI_CR2_TXEIE             /* !< Tx buffer empty interrupt enable */
#define LL_SPI_CR2_ERRIE                   SPI_CR2_ERRIE             /* !< Error interrupt enable */
/* @} */

/* @defgroup SPI_LL_EC_MODE Operation Mode @{ */
#define LL_SPI_MODE_MASTER                 (SPI_CR1_MSTR | SPI_CR1_SSI)    /* !< Master configuration */
#define LL_SPI_MODE_SLAVE                  0x00000000U                     /* !< Slave configuration */
/* @} */

/* @defgroup SPI_LL_EC_PHASE Clock Phase @{ */
#define LL_SPI_PHASE_1EDGE                 0x00000000U               /* !< First clock transition is the first data capture edge */
#define LL_SPI_PHASE_2EDGE                 (SPI_CR1_CPHA)            /* !< Second clock transition is the first data capture edge */
/* @} */

/* @defgroup SPI_LL_EC_POLARITY Clock Polarity @{ */
#define LL_SPI_POLARITY_LOW                0x00000000U               /* !< Clock to 0 when idle */
#define LL_SPI_POLARITY_HIGH               (SPI_CR1_CPOL)            /* !< Clock to 1 when idle */
/* @} */

/* @defgroup SPI_LL_EC_BAUDRATEPRESCALER Baud Rate Prescaler @{ */
#define LL_SPI_BAUDRATEPRESCALER_DIV2      0x00000000U                                    /* !< BaudRate control equal to fPCLK/2 */
#define LL_SPI_BAUDRATEPRESCALER_DIV4      (SPI_CR1_BR_0)                                 /* !< BaudRate control equal to fPCLK/4 */
#define LL_SPI_BAUDRATEPRESCALER_DIV8      (SPI_CR1_BR_1)                                 /* !< BaudRate control equal to fPCLK/8 */
#define LL_SPI_BAUDRATEPRESCALER_DIV16     (SPI_CR1_BR_1 | SPI_CR1_BR_0)                  /* !< BaudRate control equal to fPCLK/16 */
#define LL_SPI_BAUDRATEPRESCALER_DIV32     (SPI_CR1_BR_2)                                 /* !< BaudRate control equal to fPCLK/32 */
#define LL_SPI_BAUDRATEPRESCALER_DIV64     (SPI_CR1_BR_2 | SPI_CR1_BR_0)                  /* !< BaudRate control equal to fPCLK/64 */
#define LL_SPI_BAUDRATEPRESCALER_DIV128    (SPI_CR1_BR_2 | SPI_CR1_BR_1)                  /* !< BaudRate control equal to fPCLK/128 */
#define LL_SPI_BAUDRATEPRESCALER_DIV256    (SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0)   /* !< BaudRate control equal to fPCLK/256 */
/* @} */

/* @defgroup SPI_LL_EC_BIT_ORDER Transmission Bit Order @{ */
#define LL_SPI_LSB_FIRST                   (SPI_CR1_LSBFIRST)        /* !< Data is transmitted/received with the LSB first */
#define LL_SPI_MSB_FIRST                   0x00000000U               /* !< Data is transmitted/received with the MSB first */
/* @} */

/* @defgroup SPI_LL_EC_TRANSFER_MODE Transfer Mode @{ */
#define LL_SPI_FULL_DUPLEX                 0x00000000U                          /* !< Full-duplex communication or Simplex communication Transmit-only mode */
#define LL_SPI_SIMPLEX_RX                  (SPI_CR1_RXONLY)                     /* !< Simplex communication Receive-only mode */
#define LL_SPI_HALF_DUPLEX_RX              (SPI_CR1_BIDIMODE)                   /* !< Half-duplex communication Rx mode */
#define LL_SPI_HALF_DUPLEX_TX              (SPI_CR1_BIDIMODE | SPI_CR1_BIDIOE)  /* !< Half-duplex communication Tx mode */
/* @} */

/* @defgroup SPI_LL_EC_NSS_MODE Slave Select Pin Mode @{ */
#define LL_SPI_NSS_SOFT                    (SPI_CR1_SSM)                     /* !< NSS managed internally. NSS pin not used and free */
#define LL_SPI_NSS_HARD_INPUT              0x00000000U                       /* !< NSS pin used in Input. Only used in Master mode */
#define LL_SPI_NSS_HARD_OUTPUT             (((uint32_t)SPI_CR2_SSOE << 16U)) /* !< NSS pin used in Output. Only used in Slave mode as chip select */
/* @} */

/* @defgroup SPI_LL_EC_DATAWIDTH Datawidth @{ */
#define LL_SPI_DATAWIDTH_8BIT               0x0              /* !< Data length for SPI transfer:  8 bits */
#define LL_SPI_DATAWIDTH_16BIT             (SPI_CR2_DS)      /* !< Data length for SPI transfer: 16 bits */
/* @} */
#if defined(USE_FULL_LL_DRIVER)

#endif /* USE_FULL_LL_DRIVER */



/* @defgroup SPI_LL_EC_RX_FIFO_TH RX FIFO Threshold @{ */
#define LL_SPI_RX_FIFO_TH_HALF             0x00000000U               /* !< RXNE event is generated if FIFO level is greater than or equal to 1/2 (16-bit) */
#define LL_SPI_RX_FIFO_TH_QUARTER          (SPI_CR2_FRXTH)           /* !< RXNE event is generated if FIFO level is greater than or equal to 1/4 (8-bit) */
/* @} */

/* @defgroup SPI_LL_EC_RX_FIFO RX FIFO Level @{ */
#define LL_SPI_RX_FIFO_EMPTY               0x00000000U                       /* !< FIFO reception empty */
#define LL_SPI_RX_FIFO_QUARTER_FULL        (SPI_SR_FRLVL_0)                  /* !< FIFO reception 1/4 */
#define LL_SPI_RX_FIFO_HALF_FULL           (SPI_SR_FRLVL_1)                  /* !< FIFO reception 1/2 */
#define LL_SPI_RX_FIFO_FULL                (SPI_SR_FRLVL_1 | SPI_SR_FRLVL_0) /* !< FIFO reception full */
/* @} */

/* @defgroup SPI_LL_EC_TX_FIFO TX FIFO Level @{ */
#define LL_SPI_TX_FIFO_EMPTY               0x00000000U                       /* !< FIFO transmission empty */
#define LL_SPI_TX_FIFO_QUARTER_FULL        (SPI_SR_FTLVL_0)                  /* !< FIFO transmission 1/4 */
#define LL_SPI_TX_FIFO_HALF_FULL           (SPI_SR_FTLVL_1)                  /* !< FIFO transmission 1/2 */
#define LL_SPI_TX_FIFO_FULL                (SPI_SR_FTLVL_1 | SPI_SR_FTLVL_0) /* !< FIFO transmission full */

/* @} */

#if (defined(DMA1) || defined(DMA))
/* @defgroup SPI_LL_EC_DMA_PARITY DMA Parity @{ */
#define LL_SPI_DMA_PARITY_EVEN             0x00000000U   /* !< Select DMA parity Even */
#define LL_SPI_DMA_PARITY_ODD              0x00000001U   /* !< Select DMA parity Odd */
/* @} */
#endif /* DMA1 or DMA */

/* @defgroup SPI_LL_EC_SLAVE_SPEED_MODE SLAVE SPEED MODE @{ */
#define LL_SPI_SLAVE_SPEED_NORMAL          0x00000000U      /* Slave normal mode */
#define LL_SPI_SLAVE_SPEED_FAST            SPI_CR2_SLVFM    /* Slave fast mode */
/* @} */

/* @} */

/* Exported macro ------------------------------------------------------------ */
/* @defgroup SPI_LL_Exported_Macros SPI Exported Macros @{ */

/* @defgroup SPI_LL_EM_WRITE_READ Common Write and read registers Macros @{ */



#define LL_SPI_WriteReg(__INSTANCE__, __REG__, __VALUE__) WRITE_REG(__INSTANCE__->__REG__, (__VALUE__))

/* @brief  Read a value in SPI register @param  __INSTANCE__ SPI Instance @param  __REG__ Register to be read @retval Register value */
#define LL_SPI_ReadReg(__INSTANCE__, __REG__) READ_REG(__INSTANCE__->__REG__)
/* @} */

/* @} */

/* Exported functions -------------------------------------------------------- */
/* @defgroup SPI_LL_Exported_Functions SPI Exported Functions @{ */

/* @defgroup SPI_LL_EF_Configuration Configuration @{ */

/* @brief  Enable SPI peripheral @rmtoll CR1          SPE           LL_SPI_Enable @param  SPIx SPI Instance @retval None */
__STATIC_INLINE void LL_SPI_Enable(SPI_TypeDef *SPIx)
{
  SET_BIT(SPIx->CR1, SPI_CR1_SPE);
}



__STATIC_INLINE void LL_SPI_Disable(SPI_TypeDef *SPIx)
{
  CLEAR_BIT(SPIx->CR1, SPI_CR1_SPE);
}



__STATIC_INLINE uint32_t LL_SPI_IsEnabled(SPI_TypeDef *SPIx)
{
  return ((READ_BIT(SPIx->CR1, SPI_CR1_SPE) == (SPI_CR1_SPE)) ? 1UL : 0UL);
}



__STATIC_INLINE void LL_SPI_SetMode(SPI_TypeDef *SPIx, uint32_t Mode)
{
  MODIFY_REG(SPIx->CR1, SPI_CR1_MSTR | SPI_CR1_SSI, Mode);
}



__STATIC_INLINE uint32_t LL_SPI_GetMode(SPI_TypeDef *SPIx)
{
  return (uint32_t)(READ_BIT(SPIx->CR1, SPI_CR1_MSTR | SPI_CR1_SSI));
}



__STATIC_INLINE void LL_SPI_SetClockPhase(SPI_TypeDef *SPIx, uint32_t ClockPhase)
{
  MODIFY_REG(SPIx->CR1, SPI_CR1_CPHA, ClockPhase);
}



__STATIC_INLINE uint32_t LL_SPI_GetClockPhase(SPI_TypeDef *SPIx)
{
  return (uint32_t)(READ_BIT(SPIx->CR1, SPI_CR1_CPHA));
}



__STATIC_INLINE void LL_SPI_SetClockPolarity(SPI_TypeDef *SPIx, uint32_t ClockPolarity)
{
  MODIFY_REG(SPIx->CR1, SPI_CR1_CPOL, ClockPolarity);
}



__STATIC_INLINE uint32_t LL_SPI_GetClockPolarity(SPI_TypeDef *SPIx)
{
  return (uint32_t)(READ_BIT(SPIx->CR1, SPI_CR1_CPOL));
}



__STATIC_INLINE void LL_SPI_SetBaudRatePrescaler(SPI_TypeDef *SPIx, uint32_t BaudRate)
{
  MODIFY_REG(SPIx->CR1, SPI_CR1_BR, BaudRate);
}



__STATIC_INLINE uint32_t LL_SPI_GetBaudRatePrescaler(SPI_TypeDef *SPIx)
{
  return (uint32_t)(READ_BIT(SPIx->CR1, SPI_CR1_BR));
}



__STATIC_INLINE void LL_SPI_SetTransferBitOrder(SPI_TypeDef *SPIx, uint32_t BitOrder)
{
  MODIFY_REG(SPIx->CR1, SPI_CR1_LSBFIRST, BitOrder);
}



__STATIC_INLINE uint32_t LL_SPI_GetTransferBitOrder(SPI_TypeDef *SPIx)
{
  return (uint32_t)(READ_BIT(SPIx->CR1, SPI_CR1_LSBFIRST));
}



__STATIC_INLINE void LL_SPI_SetTransferDirection(SPI_TypeDef *SPIx, uint32_t TransferDirection)
{
  MODIFY_REG(SPIx->CR1, SPI_CR1_RXONLY | SPI_CR1_BIDIMODE | SPI_CR1_BIDIOE, TransferDirection);
}



__STATIC_INLINE uint32_t LL_SPI_GetTransferDirection(SPI_TypeDef *SPIx)
{
  return (uint32_t)(READ_BIT(SPIx->CR1, SPI_CR1_RXONLY | SPI_CR1_BIDIMODE | SPI_CR1_BIDIOE));
}



__STATIC_INLINE void LL_SPI_SetDataWidth(SPI_TypeDef *SPIx, uint32_t DataWidth)
{
  MODIFY_REG(SPIx->CR2, SPI_CR2_DS, DataWidth);
}



__STATIC_INLINE uint32_t LL_SPI_GetDataWidth(SPI_TypeDef *SPIx)
{
  return (uint32_t)(READ_BIT(SPIx->CR2, SPI_CR2_DS));
}



__STATIC_INLINE void LL_SPI_SetRxFIFOThreshold(SPI_TypeDef *SPIx, uint32_t Threshold)
{
  MODIFY_REG(SPIx->CR2, SPI_CR2_FRXTH, Threshold);
}



__STATIC_INLINE uint32_t LL_SPI_GetRxFIFOThreshold(SPI_TypeDef *SPIx)
{
  return (uint32_t)(READ_BIT(SPIx->CR2, SPI_CR2_FRXTH));
}

/* @} */

/* @defgroup SPI_LL_EF_NSS_Management Slave Select Pin Management @{ */



__STATIC_INLINE void LL_SPI_SetNSSMode(SPI_TypeDef *SPIx, uint32_t NSS)
{
  MODIFY_REG(SPIx->CR1, SPI_CR1_SSM,  NSS);
  MODIFY_REG(SPIx->CR2, SPI_CR2_SSOE, ((uint32_t)(NSS >> 16U)));
}



__STATIC_INLINE uint32_t LL_SPI_GetNSSMode(SPI_TypeDef *SPIx)
{
  uint32_t Ssm  = (READ_BIT(SPIx->CR1, SPI_CR1_SSM));
  uint32_t Ssoe = (READ_BIT(SPIx->CR2,  SPI_CR2_SSOE) << 16U);
  return (Ssm | Ssoe);
}
/* @} */

/* @defgroup SPI_LL_EF_FLAG_Management FLAG Management @{ */



__STATIC_INLINE uint32_t LL_SPI_IsActiveFlag_RXNE(SPI_TypeDef *SPIx)
{
  return ((READ_BIT(SPIx->SR, SPI_SR_RXNE) == (SPI_SR_RXNE)) ? 1UL : 0UL);
}



__STATIC_INLINE uint32_t LL_SPI_IsActiveFlag_TXE(SPI_TypeDef *SPIx)
{
  return ((READ_BIT(SPIx->SR, SPI_SR_TXE) == (SPI_SR_TXE)) ? 1UL : 0UL);
}



__STATIC_INLINE uint32_t LL_SPI_IsActiveFlag_MODF(SPI_TypeDef *SPIx)
{
  return ((READ_BIT(SPIx->SR, SPI_SR_MODF) == (SPI_SR_MODF)) ? 1UL : 0UL);
}



__STATIC_INLINE uint32_t LL_SPI_IsActiveFlag_OVR(SPI_TypeDef *SPIx)
{
  return ((READ_BIT(SPIx->SR, SPI_SR_OVR) == (SPI_SR_OVR)) ? 1UL : 0UL);
}



__STATIC_INLINE uint32_t LL_SPI_IsActiveFlag_BSY(SPI_TypeDef *SPIx)
{
  return ((READ_BIT(SPIx->SR, SPI_SR_BSY) == (SPI_SR_BSY)) ? 1UL : 0UL);
}



__STATIC_INLINE uint32_t LL_SPI_GetRxFIFOLevel(SPI_TypeDef *SPIx)
{
  return (uint32_t)(READ_BIT(SPIx->SR, SPI_SR_FRLVL));
}



__STATIC_INLINE uint32_t LL_SPI_GetTxFIFOLevel(SPI_TypeDef *SPIx)
{
  return (uint32_t)(READ_BIT(SPIx->SR, SPI_SR_FTLVL));
}



__STATIC_INLINE void LL_SPI_ClearFlag_MODF(SPI_TypeDef *SPIx)
{
  __IO uint32_t tmpreg_sr;
  tmpreg_sr = SPIx->SR;
  (void) tmpreg_sr;
  CLEAR_BIT(SPIx->CR1, SPI_CR1_SPE);
}



__STATIC_INLINE void LL_SPI_ClearFlag_OVR(SPI_TypeDef *SPIx)
{
  __IO uint32_t tmpreg;
  tmpreg = SPIx->DR;
  (void) tmpreg;
  tmpreg = SPIx->SR;
  (void) tmpreg;
}

/* @} */

/* @defgroup SPI_LL_EF_IT_Management Interrupt Management @{ */



__STATIC_INLINE void LL_SPI_EnableIT_ERR(SPI_TypeDef *SPIx)
{
  SET_BIT(SPIx->CR2, SPI_CR2_ERRIE);
}

/* @brief  Enable Rx buffer not empty interrupt @rmtoll CR2          RXNEIE        LL_SPI_EnableIT_RXNE @param  SPIx SPI Instance @retval None */
__STATIC_INLINE void LL_SPI_EnableIT_RXNE(SPI_TypeDef *SPIx)
{
  SET_BIT(SPIx->CR2, SPI_CR2_RXNEIE);
}

/* @brief  Enable Tx buffer empty interrupt @rmtoll CR2          TXEIE         LL_SPI_EnableIT_TXE @param  SPIx SPI Instance @retval None */
__STATIC_INLINE void LL_SPI_EnableIT_TXE(SPI_TypeDef *SPIx)
{
  SET_BIT(SPIx->CR2, SPI_CR2_TXEIE);
}



__STATIC_INLINE void LL_SPI_DisableIT_ERR(SPI_TypeDef *SPIx)
{
  CLEAR_BIT(SPIx->CR2, SPI_CR2_ERRIE);
}



__STATIC_INLINE void LL_SPI_DisableIT_RXNE(SPI_TypeDef *SPIx)
{
  CLEAR_BIT(SPIx->CR2, SPI_CR2_RXNEIE);
}

/* @brief  Disable Tx buffer empty interrupt @rmtoll CR2          TXEIE         LL_SPI_DisableIT_TXE @param  SPIx SPI Instance @retval None */
__STATIC_INLINE void LL_SPI_DisableIT_TXE(SPI_TypeDef *SPIx)
{
  CLEAR_BIT(SPIx->CR2, SPI_CR2_TXEIE);
}



__STATIC_INLINE uint32_t LL_SPI_IsEnabledIT_ERR(SPI_TypeDef *SPIx)
{
  return ((READ_BIT(SPIx->CR2, SPI_CR2_ERRIE) == (SPI_CR2_ERRIE)) ? 1UL : 0UL);
}



__STATIC_INLINE uint32_t LL_SPI_IsEnabledIT_RXNE(SPI_TypeDef *SPIx)
{
  return ((READ_BIT(SPIx->CR2, SPI_CR2_RXNEIE) == (SPI_CR2_RXNEIE)) ? 1UL : 0UL);
}



__STATIC_INLINE uint32_t LL_SPI_IsEnabledIT_TXE(SPI_TypeDef *SPIx)
{
  return ((READ_BIT(SPIx->CR2, SPI_CR2_TXEIE) == (SPI_CR2_TXEIE)) ? 1UL : 0UL);
}

/* @} */

#if (defined(DMA1) || defined(DMA))
/* @defgroup SPI_LL_EF_DMA_Management DMA Management @{ */



__STATIC_INLINE void LL_SPI_EnableDMAReq_RX(SPI_TypeDef *SPIx)
{
  SET_BIT(SPIx->CR2, SPI_CR2_RXDMAEN);
}



__STATIC_INLINE void LL_SPI_DisableDMAReq_RX(SPI_TypeDef *SPIx)
{
  CLEAR_BIT(SPIx->CR2, SPI_CR2_RXDMAEN);
}



__STATIC_INLINE uint32_t LL_SPI_IsEnabledDMAReq_RX(SPI_TypeDef *SPIx)
{
  return ((READ_BIT(SPIx->CR2, SPI_CR2_RXDMAEN) == (SPI_CR2_RXDMAEN)) ? 1UL : 0UL);
}



__STATIC_INLINE void LL_SPI_EnableDMAReq_TX(SPI_TypeDef *SPIx)
{
  SET_BIT(SPIx->CR2, SPI_CR2_TXDMAEN);
}



__STATIC_INLINE void LL_SPI_DisableDMAReq_TX(SPI_TypeDef *SPIx)
{
  CLEAR_BIT(SPIx->CR2, SPI_CR2_TXDMAEN);
}



__STATIC_INLINE uint32_t LL_SPI_IsEnabledDMAReq_TX(SPI_TypeDef *SPIx)
{
  return ((READ_BIT(SPIx->CR2, SPI_CR2_TXDMAEN) == (SPI_CR2_TXDMAEN)) ? 1UL : 0UL);
}



__STATIC_INLINE void LL_SPI_SetDMAParity_RX(SPI_TypeDef *SPIx, uint32_t Parity)
{
  MODIFY_REG(SPIx->CR2, SPI_CR2_LDMA_RX, (Parity << SPI_CR2_LDMA_RX_Pos));
}



__STATIC_INLINE uint32_t LL_SPI_GetDMAParity_RX(SPI_TypeDef *SPIx)
{
  return (uint32_t)(READ_BIT(SPIx->CR2, SPI_CR2_LDMA_RX) >> SPI_CR2_LDMA_RX_Pos);
}



__STATIC_INLINE void LL_SPI_SetDMAParity_TX(SPI_TypeDef *SPIx, uint32_t Parity)
{
  MODIFY_REG(SPIx->CR2, SPI_CR2_LDMA_TX, (Parity << SPI_CR2_LDMA_TX_Pos));
}



__STATIC_INLINE uint32_t LL_SPI_GetDMAParity_TX(SPI_TypeDef *SPIx)
{
  return (uint32_t)(READ_BIT(SPIx->CR2, SPI_CR2_LDMA_TX) >> SPI_CR2_LDMA_TX_Pos);
}



__STATIC_INLINE uint32_t LL_SPI_DMA_GetRegAddr(SPI_TypeDef *SPIx)
{
  return (uint32_t) & (SPIx->DR);
}
/* @} */
#endif /* DMA1 or DMA */

/* @defgroup SPI_LL_EF_DATA_Management DATA Management @{ */



__STATIC_INLINE uint8_t LL_SPI_ReceiveData8(SPI_TypeDef *SPIx)
{
  return (*((__IO uint8_t *)&SPIx->DR));
}

/* @brief  Read 16-Bits in the data register @rmtoll DR           DR            LL_SPI_ReceiveData16 @param  SPIx SPI Instance */
__STATIC_INLINE uint16_t LL_SPI_ReceiveData16(SPI_TypeDef *SPIx)
{
  return (uint16_t)(READ_REG(SPIx->DR));
}



__STATIC_INLINE void LL_SPI_TransmitData8(SPI_TypeDef *SPIx, uint8_t TxData)
{
#if defined (__GNUC__)
  __IO uint8_t *spidr = ((__IO uint8_t *)&SPIx->DR);
  *spidr = TxData;
#else
  *((__IO uint8_t *)&SPIx->DR) = TxData;
#endif /* __GNUC__ */
}



__STATIC_INLINE void LL_SPI_TransmitData16(SPI_TypeDef *SPIx, uint16_t TxData)
{
#if defined (__GNUC__)
  __IO uint16_t *spidr = ((__IO uint16_t *)&SPIx->DR);
  *spidr = TxData;
#else
  SPIx->DR = TxData;
#endif /* __GNUC__ */
}



__STATIC_INLINE void LL_SPI_SetSlaveSpeedMode(SPI_TypeDef *SPIx, uint32_t SlaveSpeedMode)
{
  MODIFY_REG(SPIx->CR2, SPI_CR2_SLVFM, SlaveSpeedMode);
}



__STATIC_INLINE uint32_t LL_SPI_GetSlaveSpeedMode(SPI_TypeDef *SPIx)
{
  return (uint32_t)(READ_BIT(SPIx->CR2, SPI_CR2_SLVFM));
}

/* @} */
#if defined(USE_FULL_LL_DRIVER)
/* @defgroup SPI_LL_EF_Init Initialization and de-initialization functions @{ */

ErrorStatus LL_SPI_DeInit(SPI_TypeDef *SPIx);
ErrorStatus LL_SPI_Init(SPI_TypeDef *SPIx, LL_SPI_InitTypeDef *SPI_InitStruct);
void        LL_SPI_StructInit(LL_SPI_InitTypeDef *SPI_InitStruct);

/* @} */
#endif /* USE_FULL_LL_DRIVER */
/* @} */

/* @} */
#endif /* defined (SPI1) || defined (SPI2) */

/* @} */

#ifdef __cplusplus
}
#endif

#endif /* PY32F0xx_LL_SPI_H */

/* (C) COPYRIGHT Puya *****END OF FILE */
