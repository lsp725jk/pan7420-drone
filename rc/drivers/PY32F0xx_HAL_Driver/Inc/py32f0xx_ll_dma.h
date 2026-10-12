/**
 * @file    py32f0xx_ll_dma.h
 * @author  MCU Application Team
 * @brief   Header file of DMA LL module.
 * @attention
 */

/* Define to prevent recursive inclusion ------------------------------------- */
#ifndef __PY32F0XX_LL_DMA_H
#define __PY32F0XX_LL_DMA_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------ */
#include "py32f0xx.h"

/* @addtogroup PY32F0xx_LL_Driver @{ */
#if defined (DMA1)

/* @defgroup DMA_LL DMA @{ */

/* Private types ------------------------------------------------------------- */
/* Private variables --------------------------------------------------------- */
/* @defgroup DMA_LL_Private_Variables DMA Private Variables @{ */
/* Array used to get the DMA channel register offset versus channel index LL_DMA_CHANNEL_x */
static const uint8_t CHANNEL_OFFSET_TAB[] =
{
  (uint8_t)(DMA1_Channel1_BASE - DMA1_BASE),
  (uint8_t)(DMA1_Channel2_BASE - DMA1_BASE),
  (uint8_t)(DMA1_Channel3_BASE - DMA1_BASE),
};
/* @} */
/* Private constants --------------------------------------------------------- */
/* Private macros ------------------------------------------------------------ */
#if defined(USE_FULL_LL_DRIVER)
/* @defgroup DMA_LL_Private_Macros DMA Private Macros @{ */
/* @} */
#endif /* USE_FULL_LL_DRIVER */

/* Exported types ------------------------------------------------------------ */
#if defined(USE_FULL_LL_DRIVER)
/* @defgroup DMA_LL_ES_INIT DMA Exported Init structure @{ */
typedef struct
{
  uint32_t PeriphOrM2MSrcAddress;      /* !< Specifies the peripheral base address for DMA transfer */

  uint32_t MemoryOrM2MDstAddress;      /* !< Specifies the memory base address for DMA transfer */

  uint32_t Direction;                /* from memory to memory or from peripheral to memory. This parameter can be a value of @ref DMA_LL_EC_DIRECTION */

  uint32_t Mode;                   

  uint32_t PeriphOrM2MSrcIncMode;      /* is incremented or not. This parameter can be a value of @ref DMA_LL_EC_PERIPH */

  uint32_t MemoryOrM2MDstIncMode;      /* is incremented or not. This parameter can be a value of @ref DMA_LL_EC_MEMORY */

  uint32_t PeriphOrM2MSrcDataSize;   /* in case of memory to memory transfer direction. This parameter can be a value of @ref DMA_LL_EC_PDATAALIGN */

  uint32_t MemoryOrM2MDstDataSize;   /* in case of memory to memory transfer direction. This parameter can be a value of @ref DMA_LL_EC_MDATAALIGN */

  uint32_t NbData;                     /* !< Specifies the number of data to transfer, in data unit. */

  uint32_t Priority;                 /* !< Specifies the channel priority level. This parameter can be a value of @ref DMA_LL_EC_PRIORITY */

} LL_DMA_InitTypeDef;
/* @} */
#endif /* USE_FULL_LL_DRIVER */

/* Exported constants -------------------------------------------------------- */
/* @defgroup DMA_LL_Exported_Constants DMA Exported Constants @{ */
/* @defgroup DMA_LL_EC_CLEAR_FLAG Clear Flags Defines @brief    Flags defines which can be used with LL_DMA_WriteReg function @{ */
#define LL_DMA_IFCR_CGIF1                 DMA_IFCR_CGIF1        /* !< Channel 1 global flag */
#define LL_DMA_IFCR_CTCIF1                DMA_IFCR_CTCIF1       /* !< Channel 1 transfer complete flag */
#define LL_DMA_IFCR_CHTIF1                DMA_IFCR_CHTIF1       /* !< Channel 1 half transfer flag */
#define LL_DMA_IFCR_CTEIF1                DMA_IFCR_CTEIF1       /* !< Channel 1 transfer error flag */
#define LL_DMA_IFCR_CGIF2                 DMA_IFCR_CGIF2        /* !< Channel 2 global flag */
#define LL_DMA_IFCR_CTCIF2                DMA_IFCR_CTCIF2       /* !< Channel 2 transfer complete flag */
#define LL_DMA_IFCR_CHTIF2                DMA_IFCR_CHTIF2       /* !< Channel 2 half transfer flag */
#define LL_DMA_IFCR_CTEIF2                DMA_IFCR_CTEIF2       /* !< Channel 2 transfer error flag */
#define LL_DMA_IFCR_CGIF3                 DMA_IFCR_CGIF3        /* !< Channel 3 global flag */
#define LL_DMA_IFCR_CTCIF3                DMA_IFCR_CTCIF3       /* !< Channel 3 transfer complete flag */
#define LL_DMA_IFCR_CHTIF3                DMA_IFCR_CHTIF3       /* !< Channel 3 half transfer flag */
#define LL_DMA_IFCR_CTEIF3                DMA_IFCR_CTEIF3       /* !< Channel 3 transfer error flag */
/* @} */

/* @defgroup DMA_LL_EC_GET_FLAG Get Flags Defines @brief    Flags defines which can be used with LL_DMA_ReadReg function @{ */
#define LL_DMA_ISR_GIF1                   DMA_ISR_GIF1          /* !< Channel 1 global flag */
#define LL_DMA_ISR_TCIF1                  DMA_ISR_TCIF1         /* !< Channel 1 transfer complete flag */
#define LL_DMA_ISR_HTIF1                  DMA_ISR_HTIF1         /* !< Channel 1 half transfer flag */
#define LL_DMA_ISR_TEIF1                  DMA_ISR_TEIF1         /* !< Channel 1 transfer error flag */
#define LL_DMA_ISR_GIF2                   DMA_ISR_GIF2          /* !< Channel 2 global flag */
#define LL_DMA_ISR_TCIF2                  DMA_ISR_TCIF2         /* !< Channel 2 transfer complete flag */
#define LL_DMA_ISR_HTIF2                  DMA_ISR_HTIF2         /* !< Channel 2 half transfer flag */
#define LL_DMA_ISR_TEIF2                  DMA_ISR_TEIF2         /* !< Channel 2 transfer error flag */
#define LL_DMA_ISR_GIF3                   DMA_ISR_GIF3          /* !< Channel 3 global flag */
#define LL_DMA_ISR_TCIF3                  DMA_ISR_TCIF3         /* !< Channel 3 transfer complete flag */
#define LL_DMA_ISR_HTIF3                  DMA_ISR_HTIF3         /* !< Channel 3 half transfer flag */
#define LL_DMA_ISR_TEIF3                  DMA_ISR_TEIF3         /* !< Channel 3 transfer error flag */
/* @} */

/* @defgroup DMA_LL_EC_IT IT Defines @brief    IT defines which can be used with LL_DMA_ReadReg and  LL_DMA_WriteReg functions @{ */
#define LL_DMA_CCR_TCIE                   DMA_CCR_TCIE          /* !< Transfer complete interrupt */
#define LL_DMA_CCR_HTIE                   DMA_CCR_HTIE          /* !< Half Transfer interrupt */
#define LL_DMA_CCR_TEIE                   DMA_CCR_TEIE          /* !< Transfer error interrupt */
/* @} */

/* @defgroup DMA_LL_EC_CHANNEL CHANNEL @{ */
#define LL_DMA_CHANNEL_1                  0x00000001U /* !< DMA Channel 1 */
#define LL_DMA_CHANNEL_2                  0x00000002U /* !< DMA Channel 2 */
#define LL_DMA_CHANNEL_3                  0x00000003U /* !< DMA Channel 3 */
/* @} */

/* @defgroup DMA_LL_EC_DIRECTION Transfer Direction @{ */
#define LL_DMA_DIRECTION_PERIPH_TO_MEMORY 0x00000000U             /* !< Peripheral to memory direction */
#define LL_DMA_DIRECTION_MEMORY_TO_PERIPH DMA_CCR_DIR             /* !< Memory to peripheral direction */
#define LL_DMA_DIRECTION_MEMORY_TO_MEMORY DMA_CCR_MEM2MEM         /* !< Memory to memory direction */
/* @} */

/* @defgroup DMA_LL_EC_MODE Transfer mode @{ */
#define LL_DMA_MODE_NORMAL                0x00000000U             /* !< Normal Mode */
#define LL_DMA_MODE_CIRCULAR              DMA_CCR_CIRC            /* !< Circular Mode */
/* @} */

/* @defgroup DMA_LL_EC_PERIPH Peripheral increment mode @{ */
#define LL_DMA_PERIPH_INCREMENT           DMA_CCR_PINC            /* !< Peripheral increment mode Enable */
#define LL_DMA_PERIPH_NOINCREMENT         0x00000000U             /* !< Peripheral increment mode Disable */
/* @} */

/* @defgroup DMA_LL_EC_MEMORY Memory increment mode @{ */
#define LL_DMA_MEMORY_INCREMENT           DMA_CCR_MINC            /* !< Memory increment mode Enable */
#define LL_DMA_MEMORY_NOINCREMENT         0x00000000U             /* !< Memory increment mode Disable */
/* @} */

/* @defgroup DMA_LL_EC_PDATAALIGN Peripheral data alignment @{ */
#define LL_DMA_PDATAALIGN_BYTE            0x00000000U             /* !< Peripheral data alignment : Byte */
#define LL_DMA_PDATAALIGN_HALFWORD        DMA_CCR_PSIZE_0         /* !< Peripheral data alignment : HalfWord */
#define LL_DMA_PDATAALIGN_WORD            DMA_CCR_PSIZE_1         /* !< Peripheral data alignment : Word */
/* @} */

/* @defgroup DMA_LL_EC_MDATAALIGN Memory data alignment @{ */
#define LL_DMA_MDATAALIGN_BYTE            0x00000000U             /* !< Memory data alignment : Byte */
#define LL_DMA_MDATAALIGN_HALFWORD        DMA_CCR_MSIZE_0         /* !< Memory data alignment : HalfWord */
#define LL_DMA_MDATAALIGN_WORD            DMA_CCR_MSIZE_1         /* !< Memory data alignment : Word */
/* @} */

/* @defgroup DMA_LL_EC_PRIORITY Transfer Priority level @{ */
#define LL_DMA_PRIORITY_LOW               0x00000000U             /* !< Priority level : Low */
#define LL_DMA_PRIORITY_MEDIUM            DMA_CCR_PL_0            /* !< Priority level : Medium */
#define LL_DMA_PRIORITY_HIGH              DMA_CCR_PL_1            /* !< Priority level : High */
#define LL_DMA_PRIORITY_VERYHIGH          DMA_CCR_PL              /* !< Priority level : Very_High */
/* @} */

/* @} */

/* Exported macro ------------------------------------------------------------ */
/* @defgroup DMA_LL_Exported_Macros DMA Exported Macros @{ */

/* @defgroup DMA_LL_EM_WRITE_READ Common Write and read registers macros @{ */

#define LL_DMA_WriteReg(__INSTANCE__, __REG__, __VALUE__) WRITE_REG(__INSTANCE__->__REG__, (__VALUE__))

/* @brief  Read a value in DMA register @param  __INSTANCE__ DMA Instance @param  __REG__ Register to be read @retval Register value */
#define LL_DMA_ReadReg(__INSTANCE__, __REG__) READ_REG(__INSTANCE__->__REG__)
/* @} */

/* @defgroup DMA_LL_EM_CONVERT_DMAxCHANNELy Convert DMAxChannely @{ */

/* @brief  Convert DMAx_Channely into DMAx @param  __CHANNEL_INSTANCE__ DMAx_Channely @retval DMAx */
#define __LL_DMA_GET_INSTANCE(__CHANNEL_INSTANCE__)  (DMA1)

/* @brief  Convert DMAx_Channely into LL_DMA_CHANNEL_y @param  __CHANNEL_INSTANCE__ DMAx_Channely @retval LL_DMA_CHANNEL_y */
#define __LL_DMA_GET_CHANNEL(__CHANNEL_INSTANCE__)   \
(((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)DMA1_Channel1)) ? LL_DMA_CHANNEL_1 : \
 ((uint32_t)(__CHANNEL_INSTANCE__) == ((uint32_t)DMA1_Channel2)) ? LL_DMA_CHANNEL_2 : \
LL_DMA_CHANNEL_3 )



#define __LL_DMA_GET_CHANNEL_INSTANCE(__DMA_INSTANCE__, __CHANNEL__)   \
((((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)DMA1)) && ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_DMA_CHANNEL_1))) ? DMA1_Channel1 : \
 (((uint32_t)(__DMA_INSTANCE__) == ((uint32_t)DMA1)) && ((uint32_t)(__CHANNEL__) == ((uint32_t)LL_DMA_CHANNEL_2))) ? DMA1_Channel2 : \
DMA1_Channel3 )

/* @} */

/* @} */

/* Exported functions -------------------------------------------------------- */
/* @defgroup DMA_LL_Exported_Functions DMA Exported Functions @{ */

/* @defgroup DMA_LL_EF_Configuration Configuration @{ */

__STATIC_INLINE void LL_DMA_EnableChannel(DMA_TypeDef *DMAx, uint32_t Channel)
{
  SET_BIT(((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)DMAx + CHANNEL_OFFSET_TAB[Channel - 1U])))->CCR, DMA_CCR_EN);
}



__STATIC_INLINE void LL_DMA_DisableChannel(DMA_TypeDef *DMAx, uint32_t Channel)
{
  CLEAR_BIT(((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)DMAx + CHANNEL_OFFSET_TAB[Channel - 1U])))->CCR, DMA_CCR_EN);
}



__STATIC_INLINE uint32_t LL_DMA_IsEnabledChannel(DMA_TypeDef *DMAx, uint32_t Channel)
{
  return (READ_BIT(((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)DMAx + CHANNEL_OFFSET_TAB[Channel - 1U])))->CCR,
                   DMA_CCR_EN) == (DMA_CCR_EN));
}



__STATIC_INLINE void LL_DMA_ConfigTransfer(DMA_TypeDef *DMAx, uint32_t Channel, uint32_t Configuration)
{
  MODIFY_REG(((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)DMAx + CHANNEL_OFFSET_TAB[Channel - 1U])))->CCR,
             DMA_CCR_DIR | DMA_CCR_MEM2MEM | DMA_CCR_CIRC | DMA_CCR_PINC | DMA_CCR_MINC | DMA_CCR_PSIZE | DMA_CCR_MSIZE | DMA_CCR_PL,
             Configuration);
}



__STATIC_INLINE void LL_DMA_SetDataTransferDirection(DMA_TypeDef *DMAx, uint32_t Channel, uint32_t Direction)
{
  MODIFY_REG(((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)DMAx + CHANNEL_OFFSET_TAB[Channel - 1U])))->CCR,
             DMA_CCR_DIR | DMA_CCR_MEM2MEM, Direction);
}



__STATIC_INLINE uint32_t LL_DMA_GetDataTransferDirection(DMA_TypeDef *DMAx, uint32_t Channel)
{
  return (READ_BIT(((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)DMAx + CHANNEL_OFFSET_TAB[Channel - 1U])))->CCR,
                   DMA_CCR_DIR | DMA_CCR_MEM2MEM));
}



__STATIC_INLINE void LL_DMA_SetMode(DMA_TypeDef *DMAx, uint32_t Channel, uint32_t Mode)
{
  MODIFY_REG(((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)DMAx + CHANNEL_OFFSET_TAB[Channel - 1U])))->CCR, DMA_CCR_CIRC,
             Mode);
}



__STATIC_INLINE uint32_t LL_DMA_GetMode(DMA_TypeDef *DMAx, uint32_t Channel)
{
  return (READ_BIT(((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)DMAx + CHANNEL_OFFSET_TAB[Channel - 1U])))->CCR,
                   DMA_CCR_CIRC));
}



__STATIC_INLINE void LL_DMA_SetPeriphIncMode(DMA_TypeDef *DMAx, uint32_t Channel, uint32_t PeriphOrM2MSrcIncMode)
{
  MODIFY_REG(((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)DMAx + CHANNEL_OFFSET_TAB[Channel - 1U])))->CCR, DMA_CCR_PINC,
             PeriphOrM2MSrcIncMode);
}



__STATIC_INLINE uint32_t LL_DMA_GetPeriphIncMode(DMA_TypeDef *DMAx, uint32_t Channel)
{
  return (READ_BIT(((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)DMAx + CHANNEL_OFFSET_TAB[Channel - 1U])))->CCR,
                   DMA_CCR_PINC));
}



__STATIC_INLINE void LL_DMA_SetMemoryIncMode(DMA_TypeDef *DMAx, uint32_t Channel, uint32_t MemoryOrM2MDstIncMode)
{
  MODIFY_REG(((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)DMAx + CHANNEL_OFFSET_TAB[Channel - 1U])))->CCR, DMA_CCR_MINC,
             MemoryOrM2MDstIncMode);
}



__STATIC_INLINE uint32_t LL_DMA_GetMemoryIncMode(DMA_TypeDef *DMAx, uint32_t Channel)
{
  return (READ_BIT(((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)DMAx + CHANNEL_OFFSET_TAB[Channel - 1U])))->CCR,
                   DMA_CCR_MINC));
}



__STATIC_INLINE void LL_DMA_SetPeriphSize(DMA_TypeDef *DMAx, uint32_t Channel, uint32_t PeriphOrM2MSrcDataSize)
{
  MODIFY_REG(((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)DMAx + CHANNEL_OFFSET_TAB[Channel - 1U])))->CCR, DMA_CCR_PSIZE,
             PeriphOrM2MSrcDataSize);
}



__STATIC_INLINE uint32_t LL_DMA_GetPeriphSize(DMA_TypeDef *DMAx, uint32_t Channel)
{
  return (READ_BIT(((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)DMAx + CHANNEL_OFFSET_TAB[Channel - 1U])))->CCR,
                   DMA_CCR_PSIZE));
}



__STATIC_INLINE void LL_DMA_SetMemorySize(DMA_TypeDef *DMAx, uint32_t Channel, uint32_t MemoryOrM2MDstDataSize)
{
  MODIFY_REG(((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)DMAx + CHANNEL_OFFSET_TAB[Channel - 1U])))->CCR, DMA_CCR_MSIZE,
             MemoryOrM2MDstDataSize);
}



__STATIC_INLINE uint32_t LL_DMA_GetMemorySize(DMA_TypeDef *DMAx, uint32_t Channel)
{
  return (READ_BIT(((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)DMAx + CHANNEL_OFFSET_TAB[Channel - 1U])))->CCR,
                   DMA_CCR_MSIZE));
}



__STATIC_INLINE void LL_DMA_SetChannelPriorityLevel(DMA_TypeDef *DMAx, uint32_t Channel, uint32_t Priority)
{
  MODIFY_REG(((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)DMAx + CHANNEL_OFFSET_TAB[Channel - 1U])))->CCR, DMA_CCR_PL,
             Priority);
}



__STATIC_INLINE uint32_t LL_DMA_GetChannelPriorityLevel(DMA_TypeDef *DMAx, uint32_t Channel)
{
  return (READ_BIT(((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)DMAx + CHANNEL_OFFSET_TAB[Channel - 1U])))->CCR,
                   DMA_CCR_PL));
}



__STATIC_INLINE void LL_DMA_SetDataLength(DMA_TypeDef *DMAx, uint32_t Channel, uint32_t NbData)
{
  MODIFY_REG(((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)DMAx + CHANNEL_OFFSET_TAB[Channel - 1U])))->CNDTR,
             DMA_CNDTR_NDT, NbData);
}



__STATIC_INLINE uint32_t LL_DMA_GetDataLength(DMA_TypeDef *DMAx, uint32_t Channel)
{
  return (READ_BIT(((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)DMAx + CHANNEL_OFFSET_TAB[Channel - 1U])))->CNDTR,
                   DMA_CNDTR_NDT));
}



__STATIC_INLINE void LL_DMA_ConfigAddresses(DMA_TypeDef *DMAx, uint32_t Channel, uint32_t SrcAddress,
    uint32_t DstAddress, uint32_t Direction)
{
        /* Direction Memory to Periph */
  if (Direction == LL_DMA_DIRECTION_MEMORY_TO_PERIPH)
  {
    WRITE_REG(((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)DMAx + CHANNEL_OFFSET_TAB[Channel - 1U])))->CMAR, SrcAddress);
    WRITE_REG(((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)DMAx + CHANNEL_OFFSET_TAB[Channel - 1U])))->CPAR, DstAddress);
  }
        /* Direction Periph to Memory and Memory to Memory */
  else
  {
    WRITE_REG(((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)DMAx + CHANNEL_OFFSET_TAB[Channel - 1U])))->CPAR, SrcAddress);
    WRITE_REG(((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)DMAx + CHANNEL_OFFSET_TAB[Channel - 1U])))->CMAR, DstAddress);
  }
}



__STATIC_INLINE void LL_DMA_SetMemoryAddress(DMA_TypeDef *DMAx, uint32_t Channel, uint32_t MemoryAddress)
{
  WRITE_REG(((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)DMAx + CHANNEL_OFFSET_TAB[Channel - 1U])))->CMAR, MemoryAddress);
}



__STATIC_INLINE void LL_DMA_SetPeriphAddress(DMA_TypeDef *DMAx, uint32_t Channel, uint32_t PeriphAddress)
{
  WRITE_REG(((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)DMAx + CHANNEL_OFFSET_TAB[Channel - 1U])))->CPAR, PeriphAddress);
}



__STATIC_INLINE uint32_t LL_DMA_GetMemoryAddress(DMA_TypeDef *DMAx, uint32_t Channel)
{
  return (READ_REG(((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)DMAx + CHANNEL_OFFSET_TAB[Channel - 1U])))->CMAR));
}



__STATIC_INLINE uint32_t LL_DMA_GetPeriphAddress(DMA_TypeDef *DMAx, uint32_t Channel)
{
  return (READ_REG(((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)DMAx + CHANNEL_OFFSET_TAB[Channel - 1U])))->CPAR));
}



__STATIC_INLINE void LL_DMA_SetM2MSrcAddress(DMA_TypeDef *DMAx, uint32_t Channel, uint32_t MemoryAddress)
{
  WRITE_REG(((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)DMAx + CHANNEL_OFFSET_TAB[Channel - 1U])))->CPAR, MemoryAddress);
}



__STATIC_INLINE void LL_DMA_SetM2MDstAddress(DMA_TypeDef *DMAx, uint32_t Channel, uint32_t MemoryAddress)
{
  WRITE_REG(((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)DMAx + CHANNEL_OFFSET_TAB[Channel - 1U])))->CMAR, MemoryAddress);
}



__STATIC_INLINE uint32_t LL_DMA_GetM2MSrcAddress(DMA_TypeDef *DMAx, uint32_t Channel)
{
  return (READ_REG(((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)DMAx + CHANNEL_OFFSET_TAB[Channel - 1U])))->CPAR));
}



__STATIC_INLINE uint32_t LL_DMA_GetM2MDstAddress(DMA_TypeDef *DMAx, uint32_t Channel)
{
  return (READ_REG(((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)DMAx + CHANNEL_OFFSET_TAB[Channel - 1U])))->CMAR));
}

/* @} */

/* @defgroup DMA_LL_EF_FLAG_Management FLAG Management @{ */



__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_GI1(DMA_TypeDef *DMAx)
{
  return (READ_BIT(DMAx->ISR, DMA_ISR_GIF1) == (DMA_ISR_GIF1));
}



__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_GI2(DMA_TypeDef *DMAx)
{
  return (READ_BIT(DMAx->ISR, DMA_ISR_GIF2) == (DMA_ISR_GIF2));
}



__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_GI3(DMA_TypeDef *DMAx)
{
  return (READ_BIT(DMAx->ISR, DMA_ISR_GIF3) == (DMA_ISR_GIF3));
}



__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_TC1(DMA_TypeDef *DMAx)
{
  return (READ_BIT(DMAx->ISR, DMA_ISR_TCIF1) == (DMA_ISR_TCIF1));
}



__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_TC2(DMA_TypeDef *DMAx)
{
  return (READ_BIT(DMAx->ISR, DMA_ISR_TCIF2) == (DMA_ISR_TCIF2));
}



__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_TC3(DMA_TypeDef *DMAx)
{
  return (READ_BIT(DMAx->ISR, DMA_ISR_TCIF3) == (DMA_ISR_TCIF3));
}



__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_HT1(DMA_TypeDef *DMAx)
{
  return (READ_BIT(DMAx->ISR, DMA_ISR_HTIF1) == (DMA_ISR_HTIF1));
}



__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_HT2(DMA_TypeDef *DMAx)
{
  return (READ_BIT(DMAx->ISR, DMA_ISR_HTIF2) == (DMA_ISR_HTIF2));
}



__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_HT3(DMA_TypeDef *DMAx)
{
  return (READ_BIT(DMAx->ISR, DMA_ISR_HTIF3) == (DMA_ISR_HTIF3));
}



__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_TE1(DMA_TypeDef *DMAx)
{
  return (READ_BIT(DMAx->ISR, DMA_ISR_TEIF1) == (DMA_ISR_TEIF1));
}



__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_TE2(DMA_TypeDef *DMAx)
{
  return (READ_BIT(DMAx->ISR, DMA_ISR_TEIF2) == (DMA_ISR_TEIF2));
}



__STATIC_INLINE uint32_t LL_DMA_IsActiveFlag_TE3(DMA_TypeDef *DMAx)
{
  return (READ_BIT(DMAx->ISR, DMA_ISR_TEIF3) == (DMA_ISR_TEIF3));
}



__STATIC_INLINE void LL_DMA_ClearFlag_GI1(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->IFCR, DMA_IFCR_CGIF1);
}



__STATIC_INLINE void LL_DMA_ClearFlag_GI2(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->IFCR, DMA_IFCR_CGIF2);
}



__STATIC_INLINE void LL_DMA_ClearFlag_GI3(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->IFCR, DMA_IFCR_CGIF3);
}



__STATIC_INLINE void LL_DMA_ClearFlag_TC1(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->IFCR, DMA_IFCR_CTCIF1);
}



__STATIC_INLINE void LL_DMA_ClearFlag_TC2(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->IFCR, DMA_IFCR_CTCIF2);
}



__STATIC_INLINE void LL_DMA_ClearFlag_TC3(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->IFCR, DMA_IFCR_CTCIF3);
}

/* @brief  Clear Channel 1  half transfer flag. @rmtoll IFCR         CHTIF1        LL_DMA_ClearFlag_HT1 @param  DMAx DMAx Instance @retval None */
__STATIC_INLINE void LL_DMA_ClearFlag_HT1(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->IFCR, DMA_IFCR_CHTIF1);
}

/* @brief  Clear Channel 2  half transfer flag. @rmtoll IFCR         CHTIF2        LL_DMA_ClearFlag_HT2 @param  DMAx DMAx Instance @retval None */
__STATIC_INLINE void LL_DMA_ClearFlag_HT2(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->IFCR, DMA_IFCR_CHTIF2);
}

/* @brief  Clear Channel 3  half transfer flag. @rmtoll IFCR         CHTIF3        LL_DMA_ClearFlag_HT3 @param  DMAx DMAx Instance @retval None */
__STATIC_INLINE void LL_DMA_ClearFlag_HT3(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->IFCR, DMA_IFCR_CHTIF3);
}

/* @brief  Clear Channel 1 transfer error flag. @rmtoll IFCR         CTEIF1        LL_DMA_ClearFlag_TE1 @param  DMAx DMAx Instance @retval None */
__STATIC_INLINE void LL_DMA_ClearFlag_TE1(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->IFCR, DMA_IFCR_CTEIF1);
}

/* @brief  Clear Channel 2 transfer error flag. @rmtoll IFCR         CTEIF2        LL_DMA_ClearFlag_TE2 @param  DMAx DMAx Instance @retval None */
__STATIC_INLINE void LL_DMA_ClearFlag_TE2(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->IFCR, DMA_IFCR_CTEIF2);
}

/* @brief  Clear Channel 3 transfer error flag. @rmtoll IFCR         CTEIF3        LL_DMA_ClearFlag_TE3 @param  DMAx DMAx Instance @retval None */
__STATIC_INLINE void LL_DMA_ClearFlag_TE3(DMA_TypeDef *DMAx)
{
  WRITE_REG(DMAx->IFCR, DMA_IFCR_CTEIF3);
}

/* @} */

/* @defgroup DMA_LL_EF_IT_Management IT Management @{ */



__STATIC_INLINE void LL_DMA_EnableIT_TC(DMA_TypeDef *DMAx, uint32_t Channel)
{
  SET_BIT(((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)DMAx + CHANNEL_OFFSET_TAB[Channel - 1U])))->CCR, DMA_CCR_TCIE);
}



__STATIC_INLINE void LL_DMA_EnableIT_HT(DMA_TypeDef *DMAx, uint32_t Channel)
{
  SET_BIT(((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)DMAx + CHANNEL_OFFSET_TAB[Channel - 1U])))->CCR, DMA_CCR_HTIE);
}



__STATIC_INLINE void LL_DMA_EnableIT_TE(DMA_TypeDef *DMAx, uint32_t Channel)
{
  SET_BIT(((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)DMAx + CHANNEL_OFFSET_TAB[Channel - 1U])))->CCR, DMA_CCR_TEIE);
}



__STATIC_INLINE void LL_DMA_DisableIT_TC(DMA_TypeDef *DMAx, uint32_t Channel)
{
  CLEAR_BIT(((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)DMAx + CHANNEL_OFFSET_TAB[Channel - 1U])))->CCR, DMA_CCR_TCIE);
}



__STATIC_INLINE void LL_DMA_DisableIT_HT(DMA_TypeDef *DMAx, uint32_t Channel)
{
  CLEAR_BIT(((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)DMAx + CHANNEL_OFFSET_TAB[Channel - 1U])))->CCR, DMA_CCR_HTIE);
}



__STATIC_INLINE void LL_DMA_DisableIT_TE(DMA_TypeDef *DMAx, uint32_t Channel)
{
  CLEAR_BIT(((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)DMAx + CHANNEL_OFFSET_TAB[Channel - 1U])))->CCR, DMA_CCR_TEIE);
}



__STATIC_INLINE uint32_t LL_DMA_IsEnabledIT_TC(DMA_TypeDef *DMAx, uint32_t Channel)
{
  return (READ_BIT(((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)DMAx + CHANNEL_OFFSET_TAB[Channel - 1U])))->CCR,
                   DMA_CCR_TCIE) == (DMA_CCR_TCIE));
}



__STATIC_INLINE uint32_t LL_DMA_IsEnabledIT_HT(DMA_TypeDef *DMAx, uint32_t Channel)
{
  return (READ_BIT(((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)DMAx + CHANNEL_OFFSET_TAB[Channel - 1U])))->CCR,
                   DMA_CCR_HTIE) == (DMA_CCR_HTIE));
}



__STATIC_INLINE uint32_t LL_DMA_IsEnabledIT_TE(DMA_TypeDef *DMAx, uint32_t Channel)
{
  return (READ_BIT(((DMA_Channel_TypeDef *)((uint32_t)((uint32_t)DMAx + CHANNEL_OFFSET_TAB[Channel - 1U])))->CCR,
                   DMA_CCR_TEIE) == (DMA_CCR_TEIE));
}

/* @} */

#if defined(USE_FULL_LL_DRIVER)
/* @defgroup DMA_LL_EF_Init Initialization and de-initialization functions @{ */

uint32_t LL_DMA_Init(DMA_TypeDef *DMAx, uint32_t Channel, LL_DMA_InitTypeDef *DMA_InitStruct);
uint32_t LL_DMA_DeInit(DMA_TypeDef *DMAx, uint32_t Channel);
void LL_DMA_StructInit(LL_DMA_InitTypeDef *DMA_InitStruct);

/* @} */
#endif /* USE_FULL_LL_DRIVER */

/* @} */

/* @} */

#endif /* DMA1 */

/* @} */

#ifdef __cplusplus
}
#endif

#endif /* __PY32F0XX_LL_DMA_H */

/* (C) COPYRIGHT Puya *****END OF FILE */
