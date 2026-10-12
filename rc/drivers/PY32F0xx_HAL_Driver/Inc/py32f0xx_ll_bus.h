/**
 * @file    py32f0xx_ll_bus.h
 * @author  MCU Application Team
 * @brief   Header file of BUS LL module.
 * @verbatim
 */

/* Define to prevent recursive inclusion ------------------------------------- */
#ifndef PY32F0XX_LL_BUS_H
#define PY32F0XX_LL_BUS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------ */
#include "py32f0xx.h"

/* @addtogroup PY32F0xx_LL_Driver @{ */

#if defined(RCC)

/* @defgroup BUS_LL BUS @{ */

/* Private types ------------------------------------------------------------- */
/* Private variables --------------------------------------------------------- */

/* Private constants --------------------------------------------------------- */

/* Private macros ------------------------------------------------------------ */

/* Exported types ------------------------------------------------------------ */
/* Exported constants -------------------------------------------------------- */
/* @defgroup BUS_LL_Exported_Constants BUS Exported Constants @{ */

/* @defgroup BUS_LL_EC_AHB1_GRP1_PERIPH  AHB1 GRP1 PERIPH @{ */
#define LL_AHB1_GRP1_PERIPH_ALL            0xFFFFFFFFU
#if (defined(DMA) || defined(DMA1))
#define LL_AHB1_GRP1_PERIPH_DMA1           RCC_AHBENR_DMAEN
#endif
#define LL_AHB1_GRP1_PERIPH_FLASH          RCC_AHBENR_FLASHEN
#define LL_AHB1_GRP1_PERIPH_SRAM           RCC_AHBENR_SRAMEN
#define LL_AHB1_GRP1_PERIPH_CRC            RCC_AHBENR_CRCEN
/* @} */



/* @defgroup BUS_LL_EC_APB1_GRP1_PERIPH  APB1 GRP1 PERIPH @{ */
#define LL_APB1_GRP1_PERIPH_ALL            0xFFFFFFFFU
#if defined(TIM3)
#define LL_APB1_GRP1_PERIPH_TIM3           RCC_APBENR1_TIM3EN
#endif
#if defined(RTC)
#define LL_APB1_GRP1_PERIPH_RTC            RCC_APBENR1_RTCAPBEN
#endif
#if defined(WWDG)
#define LL_APB1_GRP1_PERIPH_WWDG           RCC_APBENR1_WWDGEN
#endif
#if defined(SPI2)
#define LL_APB1_GRP1_PERIPH_SPI2           RCC_APBENR1_SPI2EN
#endif
#if defined(USART2)
#define LL_APB1_GRP1_PERIPH_USART2         RCC_APBENR1_USART2EN
#endif
#define LL_APB1_GRP1_PERIPH_I2C1           RCC_APBENR1_I2CEN
#define LL_APB1_GRP1_PERIPH_DBGMCU         RCC_APBENR1_DBGEN
#define LL_APB1_GRP1_PERIPH_PWR            RCC_APBENR1_PWREN
#define LL_APB1_GRP1_PERIPH_LPTIM1         RCC_APBENR1_LPTIMEN
/* @} */

/* @defgroup BUS_LL_EC_APB1_GRP2_PERIPH  APB1 GRP2 PERIPH @{ */
#define LL_APB1_GRP2_PERIPH_ALL            0xFFFFFFFFU
#define LL_APB1_GRP2_PERIPH_SYSCFG         RCC_APBENR2_SYSCFGEN
#define LL_APB1_GRP2_PERIPH_TIM1           RCC_APBENR2_TIM1EN
#define LL_APB1_GRP2_PERIPH_SPI1           RCC_APBENR2_SPI1EN
#define LL_APB1_GRP2_PERIPH_USART1         RCC_APBENR2_USART1EN
#if defined(TIM14)
#define LL_APB1_GRP2_PERIPH_TIM14          RCC_APBENR2_TIM14EN
#endif
#define LL_APB1_GRP2_PERIPH_TIM16          RCC_APBENR2_TIM16EN
#if defined(TIM17)
#define LL_APB1_GRP2_PERIPH_TIM17          RCC_APBENR2_TIM17EN
#endif
#define LL_APB1_GRP2_PERIPH_ADC1           RCC_APBENR2_ADCEN
#if defined(COMP1)
#define LL_APB1_GRP2_PERIPH_COMP1          RCC_APBENR2_COMP1EN
#endif
#if defined(COMP2)
#define LL_APB1_GRP2_PERIPH_COMP2          RCC_APBENR2_COMP2EN
#endif
#if defined(LED)
#define LL_APB1_GRP2_PERIPH_LED            RCC_APBENR2_LEDEN
#endif
/* @} */

/* @defgroup BUS_LL_EC_IOP_GRP1_PERIPH  IOP GRP1 PERIPH @{ */
#define LL_IOP_GRP1_PERIPH_ALL             0xFFFFFFFFU
#define LL_IOP_GRP1_PERIPH_GPIOA           RCC_IOPENR_GPIOAEN
#define LL_IOP_GRP1_PERIPH_GPIOB           RCC_IOPENR_GPIOBEN
#define LL_IOP_GRP1_PERIPH_GPIOF           RCC_IOPENR_GPIOFEN
/* @} */

/* @} */

/* Exported macro ------------------------------------------------------------ */
/* Exported functions -------------------------------------------------------- */
/* @defgroup BUS_LL_Exported_Functions BUS Exported Functions @{ */

/* @defgroup BUS_LL_EF_AHB1 AHB1 @{ */



__STATIC_INLINE void LL_AHB1_GRP1_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC->AHBENR, Periphs);
        /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC->AHBENR, Periphs);
  (void)tmpreg;
}



__STATIC_INLINE uint32_t LL_AHB1_GRP1_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC->AHBENR, Periphs) == Periphs) ? 1UL : 0UL);
}



__STATIC_INLINE void LL_AHB1_GRP1_DisableClock(uint32_t Periphs)
{
  CLEAR_BIT(RCC->AHBENR, Periphs);
}



__STATIC_INLINE void LL_AHB1_GRP1_ForceReset(uint32_t Periphs)
{
  SET_BIT(RCC->AHBRSTR, Periphs);
}



__STATIC_INLINE void LL_AHB1_GRP1_ReleaseReset(uint32_t Periphs)
{
  CLEAR_BIT(RCC->AHBRSTR, Periphs);
}
/* @} */

/* @defgroup BUS_LL_EF_APB1_GRP1 APB1 GRP1 @{ */



__STATIC_INLINE void LL_APB1_GRP1_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC->APBENR1, Periphs);
        /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC->APBENR1, Periphs);
  (void)tmpreg;
}



__STATIC_INLINE uint32_t LL_APB1_GRP1_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC->APBENR1, Periphs) == (Periphs)) ? 1UL : 0UL);
}



__STATIC_INLINE void LL_APB1_GRP1_DisableClock(uint32_t Periphs)
{
  CLEAR_BIT(RCC->APBENR1, Periphs);
}



__STATIC_INLINE void LL_APB1_GRP1_ForceReset(uint32_t Periphs)
{
  SET_BIT(RCC->APBRSTR1, Periphs);
}



__STATIC_INLINE void LL_APB1_GRP1_ReleaseReset(uint32_t Periphs)
{
  CLEAR_BIT(RCC->APBRSTR1, Periphs);
}
/* @} */

/* @defgroup BUS_LL_EF_APB1_GRP2 APB1 GRP2 @{ */



__STATIC_INLINE void LL_APB1_GRP2_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC->APBENR2, Periphs);
        /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC->APBENR2, Periphs);
  (void)tmpreg;
}



__STATIC_INLINE uint32_t LL_APB1_GRP2_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC->APBENR2, Periphs) == (Periphs)) ? 1UL : 0UL);
}



__STATIC_INLINE void LL_APB1_GRP2_DisableClock(uint32_t Periphs)
{
  CLEAR_BIT(RCC->APBENR2, Periphs);
}



__STATIC_INLINE void LL_APB1_GRP2_ForceReset(uint32_t Periphs)
{
  SET_BIT(RCC->APBRSTR2, Periphs);
}



__STATIC_INLINE void LL_APB1_GRP2_ReleaseReset(uint32_t Periphs)
{
  CLEAR_BIT(RCC->APBRSTR2, Periphs);
}

/* @} */

/* @defgroup BUS_LL_EF_IOP IOP @{ */



__STATIC_INLINE void LL_IOP_GRP1_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC->IOPENR, Periphs);
        /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC->IOPENR, Periphs);
  (void)tmpreg;
}



__STATIC_INLINE uint32_t LL_IOP_GRP1_IsEnabledClock(uint32_t Periphs)
{
  return ((READ_BIT(RCC->IOPENR, Periphs) == Periphs) ? 1UL : 0UL);
}



__STATIC_INLINE void LL_IOP_GRP1_DisableClock(uint32_t Periphs)
{
  CLEAR_BIT(RCC->IOPENR, Periphs);
}



__STATIC_INLINE void LL_IOP_GRP1_ForceReset(uint32_t Periphs)
{
  SET_BIT(RCC->IOPRSTR, Periphs);
}



__STATIC_INLINE void LL_IOP_GRP1_ReleaseReset(uint32_t Periphs)
{
  CLEAR_BIT(RCC->IOPRSTR, Periphs);
}
/* @} */



/* @} */

/* @} */

#endif /* RCC */

/* @} */

#ifdef __cplusplus
}
#endif

#endif /* PY32F0XX_LL_BUS_H */

/* (C) COPYRIGHT Puya *****END OF FILE */
