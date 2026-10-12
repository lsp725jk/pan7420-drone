/**
 * @file    py32f0xx_ll_i2c.h
 * @author  MCU Application Team
 * @brief   Header file of I2C LL module.
 * @attention
 */

/* Define to prevent recursive inclusion ------------------------------------- */
#ifndef __PY32F0xx_LL_I2C_H
#define __PY32F0xx_LL_I2C_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------ */
#include "py32f0xx.h"
/* @addtogroup PY32F0xx_LL_Driver @{ */

#if defined (I2C1) || defined (I2C2)

/* @defgroup I2C_LL I2C @{ */

/* Private types ------------------------------------------------------------- */
/* Private variables --------------------------------------------------------- */

/* Private constants --------------------------------------------------------- */
/* @defgroup I2C_LL_Private_Constants I2C Private Constants @{ */

/* Defines used to perform compute and check in the macros */
#define LL_I2C_MAX_SPEED_STANDARD           100000U
#define LL_I2C_MAX_SPEED_FAST               400000U
/* @} */

/* Private macros ------------------------------------------------------------ */
#if defined(USE_FULL_LL_DRIVER)
/* @defgroup I2C_LL_Private_Macros I2C Private Macros @{ */
/* @} */
#endif /* USE_FULL_LL_DRIVER */

/* Exported types ------------------------------------------------------------ */
#if defined(USE_FULL_LL_DRIVER)
/* @defgroup I2C_LL_ES_INIT I2C Exported Init structure @{ */
typedef struct
{
  uint32_t ClockSpeed;              /* !< Specifies the clock frequency. */

  uint32_t DutyCycle;             /* !< Specifies the I2C fast mode duty cycle. This parameter can be a value of @ref I2C_LL_EC_DUTYCYCLE */

  uint32_t OwnAddress1;             /* !< Specifies the device own address 1. */

  uint32_t TypeAcknowledge;     
} LL_I2C_InitTypeDef;
/* @} */
#endif /* USE_FULL_LL_DRIVER */

/* Exported constants -------------------------------------------------------- */
/* @defgroup I2C_LL_Exported_Constants I2C Exported Constants @{ */

/* @defgroup I2C_LL_EC_GET_FLAG Get Flags Defines @brief    Flags defines which can be used with LL_I2C_ReadReg function @{ */
#define LL_I2C_SR1_SB                       I2C_SR1_SB              /* !< Start Bit (master mode) */
#define LL_I2C_SR1_ADDR                     I2C_SR1_ADDR            /* !< Address sent (master mode) or Address matched flag (slave mode) */
#define LL_I2C_SR1_BTF                      I2C_SR1_BTF             /* !< Byte Transfer Finished flag */
#define LL_I2C_SR1_STOPF                    I2C_SR1_STOPF           /* !< Stop detection flag (slave mode) */
#define LL_I2C_SR1_RXNE                     I2C_SR1_RXNE            /* !< Data register not empty (receivers) */
#define LL_I2C_SR1_TXE                      I2C_SR1_TXE             /* !< Data register empty (transmitters) */
#define LL_I2C_SR1_BERR                     I2C_SR1_BERR            /* !< Bus error */
#define LL_I2C_SR1_ARLO                     I2C_SR1_ARLO            /* !< Arbitration lost */
#define LL_I2C_SR1_AF                       I2C_SR1_AF              /* !< Acknowledge failure flag */
#define LL_I2C_SR1_OVR                      I2C_SR1_OVR             /* !< Overrun/Underrun */
#define LL_I2C_SR1_PECERR                   I2C_SR1_PECERR          /* !< PEC Error in reception (SMBus mode) */
#define LL_I2C_SR2_MSL                      I2C_SR2_MSL             /* !< Master/Slave flag */
#define LL_I2C_SR2_BUSY                     I2C_SR2_BUSY            /* !< Bus busy flag */
#define LL_I2C_SR2_TRA                      I2C_SR2_TRA             /* !< Transmitter/receiver direction */
#define LL_I2C_SR2_GENCALL                  I2C_SR2_GENCALL         /* !< General call address (Slave mode) */
/* @} */

/* @defgroup I2C_LL_EC_IT IT Defines @brief    IT defines which can be used with LL_I2C_ReadReg and  LL_I2C_WriteReg functions @{ */
#define LL_I2C_CR2_ITEVTEN                  I2C_CR2_ITEVTEN         /* !< Events interrupts enable */
#define LL_I2C_CR2_ITBUFEN                  I2C_CR2_ITBUFEN         /* !< Buffer interrupts enable */
#define LL_I2C_CR2_ITERREN                  I2C_CR2_ITERREN         /* !< Error interrupts enable */
/* @} */

/* @defgroup I2C_LL_EC_DUTYCYCLE Fast Mode Duty Cycle @{ */
#define LL_I2C_DUTYCYCLE_2                  0x00000000U             /* !< I2C fast mode Tlow/Thigh = 2 */
#define LL_I2C_DUTYCYCLE_16_9               I2C_CCR_DUTY            /* !< I2C fast mode Tlow/Thigh = 16/9 */
/* @} */

/* @defgroup I2C_LL_EC_CLOCK_SPEED_MODE Master Clock Speed Mode @{ */
#define LL_I2C_CLOCK_SPEED_STANDARD_MODE    0x00000000U             /* !< Master clock speed range is standard mode */
#define LL_I2C_CLOCK_SPEED_FAST_MODE        I2C_CCR_FS              /* !< Master clock speed range is fast mode */
/* @} */

/* @defgroup I2C_LL_EC_I2C_ACKNOWLEDGE Acknowledge Generation @{ */
#define LL_I2C_ACK                          I2C_CR1_ACK             /* !< ACK is sent after current received byte. */
#define LL_I2C_NACK                         0x00000000U             /* !< NACK is sent after current received byte. */
/* @} */

/* @defgroup I2C_LL_EC_DIRECTION Read Write Direction @{ */
#define LL_I2C_DIRECTION_WRITE              I2C_SR2_TRA             /* !< Bus is in write transfer */
#define LL_I2C_DIRECTION_READ               0x00000000U             /* !< Bus is in read transfer */
/* @} */

/* @} */

/* Exported macro ------------------------------------------------------------ */
/* @defgroup I2C_LL_Exported_Macros I2C Exported Macros @{ */

/* @defgroup I2C_LL_EM_WRITE_READ Common Write and read registers Macros @{ */



#define LL_I2C_WriteReg(__INSTANCE__, __REG__, __VALUE__) WRITE_REG(__INSTANCE__->__REG__, (__VALUE__))

/* @brief  Read a value in I2C register @param  __INSTANCE__ I2C Instance @param  __REG__ Register to be read @retval Register value */
#define LL_I2C_ReadReg(__INSTANCE__, __REG__) READ_REG(__INSTANCE__->__REG__)
/* @} */

/* @defgroup I2C_LL_EM_Exported_Macros_Helper Exported Macros Helper @{ */



#define __LL_I2C_FREQ_HZ_TO_MHZ(__PCLK__)                               (uint32_t)((__PCLK__)/1000000U)



#define __LL_I2C_FREQ_MHZ_TO_HZ(__PCLK__)                               (uint32_t)((__PCLK__)*1000000U)



#define __LL_I2C_RISE_TIME(__FREQRANGE__, __SPEED__)                    (uint32_t)(((__SPEED__) <= LL_I2C_MAX_SPEED_STANDARD) ? ((__FREQRANGE__) + 1U) : ((((__FREQRANGE__) * 300U) / 1000U) + 1U))



#define __LL_I2C_SPEED_TO_CCR(__PCLK__, __SPEED__, __DUTYCYCLE__)       (uint32_t)(((__SPEED__) <= LL_I2C_MAX_SPEED_STANDARD)? \
                                                                                  (__LL_I2C_SPEED_STANDARD_TO_CCR((__PCLK__), (__SPEED__))) : \
                                                                                  (__LL_I2C_SPEED_FAST_TO_CCR((__PCLK__), (__SPEED__), (__DUTYCYCLE__))))



#define __LL_I2C_SPEED_STANDARD_TO_CCR(__PCLK__, __SPEED__)             (uint32_t)(((((__PCLK__)/((__SPEED__) << 1U)) & I2C_CCR_CCR) < 4U)? 4U:((__PCLK__) / ((__SPEED__) << 1U)))



#define __LL_I2C_SPEED_FAST_TO_CCR(__PCLK__, __SPEED__, __DUTYCYCLE__)  (uint32_t)(((__DUTYCYCLE__) == LL_I2C_DUTYCYCLE_2)? \
                                                                            (((((__PCLK__) / ((__SPEED__) * 3U)) & I2C_CCR_CCR) == 0U)? 1U:((__PCLK__) / ((__SPEED__) * 3U))) : \
                                                                            (((((__PCLK__) / ((__SPEED__) * 25U)) & I2C_CCR_CCR) == 0U)? 1U:((__PCLK__) / ((__SPEED__) * 25U))))

/* @} */

/* @} */

/* Exported functions -------------------------------------------------------- */

/* @defgroup I2C_LL_Exported_Functions I2C Exported Functions @{ */

/* @defgroup I2C_LL_EF_Configuration Configuration @{ */

/* @brief  Enable I2C peripheral (PE = 1). @rmtoll CR1          PE            LL_I2C_Enable @param  I2Cx I2C Instance. @retval None */
__STATIC_INLINE void LL_I2C_Enable(I2C_TypeDef *I2Cx)
{
  SET_BIT(I2Cx->CR1, I2C_CR1_PE);
}

/* @brief  Disable I2C peripheral (PE = 0). @rmtoll CR1          PE            LL_I2C_Disable @param  I2Cx I2C Instance. @retval None */
__STATIC_INLINE void LL_I2C_Disable(I2C_TypeDef *I2Cx)
{
  CLEAR_BIT(I2Cx->CR1, I2C_CR1_PE);
}



__STATIC_INLINE uint32_t LL_I2C_IsEnabled(I2C_TypeDef *I2Cx)
{
  return (READ_BIT(I2Cx->CR1, I2C_CR1_PE) == (I2C_CR1_PE));
}



#if (defined(DMA1) || defined(DMA))

__STATIC_INLINE void LL_I2C_EnableDMAReq_TX(I2C_TypeDef *I2Cx)
{
  SET_BIT(I2Cx->CR2, I2C_CR2_DMAEN);
}



__STATIC_INLINE void LL_I2C_DisableDMAReq_TX(I2C_TypeDef *I2Cx)
{
  CLEAR_BIT(I2Cx->CR2, I2C_CR2_DMAEN);
}



__STATIC_INLINE uint32_t LL_I2C_IsEnabledDMAReq_TX(I2C_TypeDef *I2Cx)
{
  return (READ_BIT(I2Cx->CR2, I2C_CR2_DMAEN) == (I2C_CR2_DMAEN));
}



__STATIC_INLINE void LL_I2C_EnableDMAReq_RX(I2C_TypeDef *I2Cx)
{
  SET_BIT(I2Cx->CR2, I2C_CR2_DMAEN);
}



__STATIC_INLINE void LL_I2C_DisableDMAReq_RX(I2C_TypeDef *I2Cx)
{
  CLEAR_BIT(I2Cx->CR2, I2C_CR2_DMAEN);
}



__STATIC_INLINE uint32_t LL_I2C_IsEnabledDMAReq_RX(I2C_TypeDef *I2Cx)
{
  return (READ_BIT(I2Cx->CR2, I2C_CR2_DMAEN) == (I2C_CR2_DMAEN));
}



__STATIC_INLINE uint32_t LL_I2C_DMA_GetRegAddr(I2C_TypeDef *I2Cx)
{
  return (uint32_t) & (I2Cx->DR);
}
#endif /* DMA1 or DMA */



__STATIC_INLINE void LL_I2C_EnableClockStretching(I2C_TypeDef *I2Cx)
{
  CLEAR_BIT(I2Cx->CR1, I2C_CR1_NOSTRETCH);
}



__STATIC_INLINE void LL_I2C_DisableClockStretching(I2C_TypeDef *I2Cx)
{
  SET_BIT(I2Cx->CR1, I2C_CR1_NOSTRETCH);
}

/* @brief  Check if Clock stretching is enabled or disabled. @param  I2Cx I2C Instance. @retval State of bit (1 or 0). */
__STATIC_INLINE uint32_t LL_I2C_IsEnabledClockStretching(I2C_TypeDef *I2Cx)
{
  return (READ_BIT(I2Cx->CR1, I2C_CR1_NOSTRETCH) != (I2C_CR1_NOSTRETCH));
}



__STATIC_INLINE void LL_I2C_EnableGeneralCall(I2C_TypeDef *I2Cx)
{
  SET_BIT(I2Cx->CR1, I2C_CR1_ENGC);
}



__STATIC_INLINE void LL_I2C_DisableGeneralCall(I2C_TypeDef *I2Cx)
{
  CLEAR_BIT(I2Cx->CR1, I2C_CR1_ENGC);
}

/* @brief  Check if General Call is enabled or disabled. @param  I2Cx I2C Instance. @retval State of bit (1 or 0). */
__STATIC_INLINE uint32_t LL_I2C_IsEnabledGeneralCall(I2C_TypeDef *I2Cx)
{
  return (READ_BIT(I2Cx->CR1, I2C_CR1_ENGC) == (I2C_CR1_ENGC));
}



__STATIC_INLINE void LL_I2C_SetOwnAddress1(I2C_TypeDef *I2Cx, uint32_t OwnAddress1, uint32_t OwnAddrSize)
{
  (void)OwnAddrSize;
  MODIFY_REG(I2Cx->OAR1, I2C_OAR1_ADD1_7, OwnAddress1);
}



__STATIC_INLINE void LL_I2C_SetPeriphClock(I2C_TypeDef *I2Cx, uint32_t PeriphClock)
{
  MODIFY_REG(I2Cx->CR2, I2C_CR2_FREQ, __LL_I2C_FREQ_HZ_TO_MHZ(PeriphClock));
}



__STATIC_INLINE uint32_t LL_I2C_GetPeriphClock(I2C_TypeDef *I2Cx)
{
  return (uint32_t)(__LL_I2C_FREQ_MHZ_TO_HZ(READ_BIT(I2Cx->CR2, I2C_CR2_FREQ)));
}



__STATIC_INLINE void LL_I2C_SetDutyCycle(I2C_TypeDef *I2Cx, uint32_t DutyCycle)
{
  MODIFY_REG(I2Cx->CCR, I2C_CCR_DUTY, DutyCycle);
}



__STATIC_INLINE uint32_t LL_I2C_GetDutyCycle(I2C_TypeDef *I2Cx)
{
  return (uint32_t)(READ_BIT(I2Cx->CCR, I2C_CCR_DUTY));
}



__STATIC_INLINE void LL_I2C_SetClockSpeedMode(I2C_TypeDef *I2Cx, uint32_t ClockSpeedMode)
{
  MODIFY_REG(I2Cx->CCR, I2C_CCR_FS, ClockSpeedMode);
}



__STATIC_INLINE uint32_t LL_I2C_GetClockSpeedMode(I2C_TypeDef *I2Cx)
{
  return (uint32_t)(READ_BIT(I2Cx->CCR, I2C_CCR_FS));
}



__STATIC_INLINE void LL_I2C_SetRiseTime(I2C_TypeDef *I2Cx, uint32_t RiseTime)
{
  MODIFY_REG(I2Cx->TRISE, I2C_TRISE_TRISE, RiseTime);
}



__STATIC_INLINE uint32_t LL_I2C_GetRiseTime(I2C_TypeDef *I2Cx)
{
  return (uint32_t)(READ_BIT(I2Cx->TRISE, I2C_TRISE_TRISE));
}



__STATIC_INLINE void LL_I2C_SetClockPeriod(I2C_TypeDef *I2Cx, uint32_t ClockPeriod)
{
  MODIFY_REG(I2Cx->CCR, I2C_CCR_CCR, ClockPeriod);
}

/* @brief  Get the SCL high and low period. @rmtoll CCR          CCR           LL_I2C_GetClockPeriod @param  I2Cx I2C Instance. */
__STATIC_INLINE uint32_t LL_I2C_GetClockPeriod(I2C_TypeDef *I2Cx)
{
  return (uint32_t)(READ_BIT(I2Cx->CCR, I2C_CCR_CCR));
}



__STATIC_INLINE void LL_I2C_ConfigSpeed(I2C_TypeDef *I2Cx, uint32_t PeriphClock, uint32_t ClockSpeed,
                                        uint32_t DutyCycle)
{
  register uint32_t freqrange = 0x0U;
  register uint32_t clockconfig = 0x0U;

        /* Compute frequency range */
  freqrange = __LL_I2C_FREQ_HZ_TO_MHZ(PeriphClock);

        /* Configure I2Cx: Frequency range register */
  MODIFY_REG(I2Cx->CR2, I2C_CR2_FREQ, freqrange);

        /* Configure I2Cx: Rise Time register */
  MODIFY_REG(I2Cx->TRISE, I2C_TRISE_TRISE, __LL_I2C_RISE_TIME(freqrange, ClockSpeed));

        /* Configure Speed mode, Duty Cycle and Clock control register value */
  if (ClockSpeed > LL_I2C_MAX_SPEED_STANDARD)
  {
                /* Set Speed mode at fast and duty cycle for Clock Speed request in fast clock range */
    clockconfig = LL_I2C_CLOCK_SPEED_FAST_MODE                                          | \
                  __LL_I2C_SPEED_FAST_TO_CCR(PeriphClock, ClockSpeed, DutyCycle)        | \
                  DutyCycle;
  }
  else
  {
                /* Set Speed mode at standard for Clock Speed request in standard clock range */
    clockconfig = LL_I2C_CLOCK_SPEED_STANDARD_MODE                                      | \
                  __LL_I2C_SPEED_STANDARD_TO_CCR(PeriphClock, ClockSpeed);
  }

        /* Configure I2Cx: Clock control register */
  MODIFY_REG(I2Cx->CCR, (I2C_CCR_FS | I2C_CCR_DUTY | I2C_CCR_CCR), clockconfig);
}

/* @} */

/* @defgroup I2C_LL_EF_IT_Management IT Management @{ */



__STATIC_INLINE void LL_I2C_EnableIT_TX(I2C_TypeDef *I2Cx)
{
  SET_BIT(I2Cx->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN);
}



__STATIC_INLINE void LL_I2C_DisableIT_TX(I2C_TypeDef *I2Cx)
{
  CLEAR_BIT(I2Cx->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN);
}



__STATIC_INLINE uint32_t LL_I2C_IsEnabledIT_TX(I2C_TypeDef *I2Cx)
{
  return (READ_BIT(I2Cx->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN) == (I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN));
}



__STATIC_INLINE void LL_I2C_EnableIT_RX(I2C_TypeDef *I2Cx)
{
  SET_BIT(I2Cx->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN);
}



__STATIC_INLINE void LL_I2C_DisableIT_RX(I2C_TypeDef *I2Cx)
{
  CLEAR_BIT(I2Cx->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN);
}



__STATIC_INLINE uint32_t LL_I2C_IsEnabledIT_RX(I2C_TypeDef *I2Cx)
{
  return (READ_BIT(I2Cx->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN) == (I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN));
}

/* @brief  Enable Events interrupts. @note   Any of these events will generate interrupt : Start Bit (SB) Address sent, Address matched (ADDR) */
__STATIC_INLINE void LL_I2C_EnableIT_EVT(I2C_TypeDef *I2Cx)
{
  SET_BIT(I2Cx->CR2, I2C_CR2_ITEVTEN);
}

/* @brief  Disable Events interrupts. @note   Any of these events will generate interrupt : Start Bit (SB) Address sent, Address matched (ADDR) */
__STATIC_INLINE void LL_I2C_DisableIT_EVT(I2C_TypeDef *I2Cx)
{
  CLEAR_BIT(I2Cx->CR2, I2C_CR2_ITEVTEN);
}



__STATIC_INLINE uint32_t LL_I2C_IsEnabledIT_EVT(I2C_TypeDef *I2Cx)
{
  return (READ_BIT(I2Cx->CR2, I2C_CR2_ITEVTEN) == (I2C_CR2_ITEVTEN));
}



__STATIC_INLINE void LL_I2C_EnableIT_BUF(I2C_TypeDef *I2Cx)
{
  SET_BIT(I2Cx->CR2, I2C_CR2_ITBUFEN);
}



__STATIC_INLINE void LL_I2C_DisableIT_BUF(I2C_TypeDef *I2Cx)
{
  CLEAR_BIT(I2Cx->CR2, I2C_CR2_ITBUFEN);
}



__STATIC_INLINE uint32_t LL_I2C_IsEnabledIT_BUF(I2C_TypeDef *I2Cx)
{
  return (READ_BIT(I2Cx->CR2, I2C_CR2_ITBUFEN) == (I2C_CR2_ITBUFEN));
}



__STATIC_INLINE void LL_I2C_EnableIT_ERR(I2C_TypeDef *I2Cx)
{
  SET_BIT(I2Cx->CR2, I2C_CR2_ITERREN);
}



__STATIC_INLINE void LL_I2C_DisableIT_ERR(I2C_TypeDef *I2Cx)
{
  CLEAR_BIT(I2Cx->CR2, I2C_CR2_ITERREN);
}



__STATIC_INLINE uint32_t LL_I2C_IsEnabledIT_ERR(I2C_TypeDef *I2Cx)
{
  return (READ_BIT(I2Cx->CR2, I2C_CR2_ITERREN) == (I2C_CR2_ITERREN));
}

/* @} */

/* @defgroup I2C_LL_EF_FLAG_management FLAG Management @{ */



__STATIC_INLINE uint32_t LL_I2C_IsActiveFlag_TXE(I2C_TypeDef *I2Cx)
{
  return (READ_BIT(I2Cx->SR1, I2C_SR1_TXE) == (I2C_SR1_TXE));
}



__STATIC_INLINE uint32_t LL_I2C_IsActiveFlag_BTF(I2C_TypeDef *I2Cx)
{
  return (READ_BIT(I2Cx->SR1, I2C_SR1_BTF) == (I2C_SR1_BTF));
}



__STATIC_INLINE uint32_t LL_I2C_IsActiveFlag_RXNE(I2C_TypeDef *I2Cx)
{
  return (READ_BIT(I2Cx->SR1, I2C_SR1_RXNE) == (I2C_SR1_RXNE));
}



__STATIC_INLINE uint32_t LL_I2C_IsActiveFlag_SB(I2C_TypeDef *I2Cx)
{
  return (READ_BIT(I2Cx->SR1, I2C_SR1_SB) == (I2C_SR1_SB));
}



__STATIC_INLINE uint32_t LL_I2C_IsActiveFlag_ADDR(I2C_TypeDef *I2Cx)
{
  return (READ_BIT(I2Cx->SR1, I2C_SR1_ADDR) == (I2C_SR1_ADDR));
}



__STATIC_INLINE uint32_t LL_I2C_IsActiveFlag_AF(I2C_TypeDef *I2Cx)
{
  return (READ_BIT(I2Cx->SR1, I2C_SR1_AF) == (I2C_SR1_AF));
}



__STATIC_INLINE uint32_t LL_I2C_IsActiveFlag_STOP(I2C_TypeDef *I2Cx)
{
  return (READ_BIT(I2Cx->SR1, I2C_SR1_STOPF) == (I2C_SR1_STOPF));
}



__STATIC_INLINE uint32_t LL_I2C_IsActiveFlag_BERR(I2C_TypeDef *I2Cx)
{
  return (READ_BIT(I2Cx->SR1, I2C_SR1_BERR) == (I2C_SR1_BERR));
}



__STATIC_INLINE uint32_t LL_I2C_IsActiveFlag_ARLO(I2C_TypeDef *I2Cx)
{
  return (READ_BIT(I2Cx->SR1, I2C_SR1_ARLO) == (I2C_SR1_ARLO));
}



__STATIC_INLINE uint32_t LL_I2C_IsActiveFlag_OVR(I2C_TypeDef *I2Cx)
{
  return (READ_BIT(I2Cx->SR1, I2C_SR1_OVR) == (I2C_SR1_OVR));
}



__STATIC_INLINE uint32_t LL_I2C_IsActiveSMBusFlag_PECERR(I2C_TypeDef *I2Cx)
{
  return (READ_BIT(I2Cx->SR1, I2C_SR1_PECERR) == (I2C_SR1_PECERR));
}



__STATIC_INLINE uint32_t LL_I2C_IsActiveFlag_BUSY(I2C_TypeDef *I2Cx)
{
  return (READ_BIT(I2Cx->SR2, I2C_SR2_BUSY) == (I2C_SR2_BUSY));
}



__STATIC_INLINE uint32_t LL_I2C_IsActiveFlag_GENCALL(I2C_TypeDef *I2Cx)
{
  return (READ_BIT(I2Cx->SR2, I2C_SR2_GENCALL) == (I2C_SR2_GENCALL));
}



__STATIC_INLINE uint32_t LL_I2C_IsActiveFlag_MSL(I2C_TypeDef *I2Cx)
{
  return (READ_BIT(I2Cx->SR2, I2C_SR2_MSL) == (I2C_SR2_MSL));
}



__STATIC_INLINE void LL_I2C_ClearFlag_ADDR(I2C_TypeDef *I2Cx)
{
  __IO uint32_t tmpreg;
  tmpreg = I2Cx->SR1;
  (void) tmpreg;
  tmpreg = I2Cx->SR2;
  (void) tmpreg;
}

/* @brief  Clear Acknowledge failure flag. @rmtoll SR1          AF            LL_I2C_ClearFlag_AF @param  I2Cx I2C Instance. @retval None */
__STATIC_INLINE void LL_I2C_ClearFlag_AF(I2C_TypeDef *I2Cx)
{
  CLEAR_BIT(I2Cx->SR1, I2C_SR1_AF);
}



__STATIC_INLINE void LL_I2C_ClearFlag_STOP(I2C_TypeDef *I2Cx)
{
  __IO uint32_t tmpreg;
  tmpreg = I2Cx->SR1;
  (void) tmpreg;
  SET_BIT(I2Cx->CR1, I2C_CR1_PE);
}

/* @brief  Clear Bus error flag. @rmtoll SR1          BERR          LL_I2C_ClearFlag_BERR @param  I2Cx I2C Instance. @retval None */
__STATIC_INLINE void LL_I2C_ClearFlag_BERR(I2C_TypeDef *I2Cx)
{
  CLEAR_BIT(I2Cx->SR1, I2C_SR1_BERR);
}

/* @brief  Clear Arbitration lost flag. @rmtoll SR1          ARLO          LL_I2C_ClearFlag_ARLO @param  I2Cx I2C Instance. @retval None */
__STATIC_INLINE void LL_I2C_ClearFlag_ARLO(I2C_TypeDef *I2Cx)
{
  CLEAR_BIT(I2Cx->SR1, I2C_SR1_ARLO);
}

/* @brief  Clear Overrun/Underrun flag. @rmtoll SR1          OVR           LL_I2C_ClearFlag_OVR @param  I2Cx I2C Instance. @retval None */
__STATIC_INLINE void LL_I2C_ClearFlag_OVR(I2C_TypeDef *I2Cx)
{
  CLEAR_BIT(I2Cx->SR1, I2C_SR1_OVR);
}

/* @brief  Clear SMBus PEC error flag. @param  I2Cx I2C Instance. @retval None */
__STATIC_INLINE void LL_I2C_ClearSMBusFlag_PECERR(I2C_TypeDef *I2Cx)
{
  CLEAR_BIT(I2Cx->SR1, I2C_SR1_PECERR);
}

/* @} */

/* @defgroup I2C_LL_EF_Data_Management Data_Management @{ */

/* @brief  Enable Reset of I2C peripheral. @rmtoll CR1          SWRST         LL_I2C_EnableReset @param  I2Cx I2C Instance. @retval None */
__STATIC_INLINE void LL_I2C_EnableReset(I2C_TypeDef *I2Cx)
{
  SET_BIT(I2Cx->CR1, I2C_CR1_SWRST);
}

/* @brief  Disable Reset of I2C peripheral. @rmtoll CR1          SWRST         LL_I2C_DisableReset @param  I2Cx I2C Instance. @retval None */
__STATIC_INLINE void LL_I2C_DisableReset(I2C_TypeDef *I2Cx)
{
  CLEAR_BIT(I2Cx->CR1, I2C_CR1_SWRST);
}



__STATIC_INLINE uint32_t LL_I2C_IsResetEnabled(I2C_TypeDef *I2Cx)
{
  return (READ_BIT(I2Cx->CR1, I2C_CR1_SWRST) == (I2C_CR1_SWRST));
}



__STATIC_INLINE void LL_I2C_AcknowledgeNextData(I2C_TypeDef *I2Cx, uint32_t TypeAcknowledge)
{
  MODIFY_REG(I2Cx->CR1, I2C_CR1_ACK, TypeAcknowledge);
}



__STATIC_INLINE void LL_I2C_GenerateStartCondition(I2C_TypeDef *I2Cx)
{
  SET_BIT(I2Cx->CR1, I2C_CR1_START);
}

/* @brief  Generate a STOP condition after the current byte transfer (master mode). @param  I2Cx I2C Instance. @retval None */
__STATIC_INLINE void LL_I2C_GenerateStopCondition(I2C_TypeDef *I2Cx)
{
  SET_BIT(I2Cx->CR1, I2C_CR1_STOP);
}



__STATIC_INLINE void LL_I2C_EnableBitPOS(I2C_TypeDef *I2Cx)
{
  SET_BIT(I2Cx->CR1, I2C_CR1_POS);
}



__STATIC_INLINE void LL_I2C_DisableBitPOS(I2C_TypeDef *I2Cx)
{
  CLEAR_BIT(I2Cx->CR1, I2C_CR1_POS);
}



__STATIC_INLINE uint32_t LL_I2C_IsEnabledBitPOS(I2C_TypeDef *I2Cx)
{
  return (READ_BIT(I2Cx->CR1, I2C_CR1_POS) == (I2C_CR1_POS));
}



__STATIC_INLINE uint32_t LL_I2C_GetTransferDirection(I2C_TypeDef *I2Cx)
{
  return (uint32_t)(READ_BIT(I2Cx->SR2, I2C_SR2_TRA));
}

#if (defined(DMA1) || defined(DMA))

__STATIC_INLINE void LL_I2C_EnableLastDMA(I2C_TypeDef *I2Cx)
{
  SET_BIT(I2Cx->CR2, I2C_CR2_LAST);
}



__STATIC_INLINE void LL_I2C_DisableLastDMA(I2C_TypeDef *I2Cx)
{
  CLEAR_BIT(I2Cx->CR2, I2C_CR2_LAST);
}



__STATIC_INLINE uint32_t LL_I2C_IsEnabledLastDMA(I2C_TypeDef *I2Cx)
{
  return (READ_BIT(I2Cx->CR2, I2C_CR2_LAST) == (I2C_CR2_LAST));
}
#endif /* DMA1 or DMA */



__STATIC_INLINE uint8_t LL_I2C_ReceiveData8(I2C_TypeDef *I2Cx)
{
  return (uint8_t)(READ_BIT(I2Cx->DR, I2C_DR_DR));
}



__STATIC_INLINE void LL_I2C_TransmitData8(I2C_TypeDef *I2Cx, uint8_t Data)
{
  MODIFY_REG(I2Cx->DR, I2C_DR_DR, Data);
}

/* @} */

#if defined(USE_FULL_LL_DRIVER)
/* @defgroup I2C_LL_EF_Init Initialization and de-initialization functions @{ */

uint32_t LL_I2C_Init(I2C_TypeDef *I2Cx, LL_I2C_InitTypeDef *I2C_InitStruct);
uint32_t LL_I2C_DeInit(I2C_TypeDef *I2Cx);
void LL_I2C_StructInit(LL_I2C_InitTypeDef *I2C_InitStruct);



/* @} */
#endif /* USE_FULL_LL_DRIVER */

/* @} */

/* @} */

#endif /* I2C1 || I2C2 */

/* @} */

#ifdef __cplusplus
}
#endif

#endif /* __PY32F0xx_LL_I2C_H */

/* (C) COPYRIGHT Puya *****END OF FILE */
