/**
 * @file    py32f0xx_ll_system.h
 * @author  MCU Application Team
 * @brief   Header file of SYSTEM LL module.
 * @attention
 */

/* Define to prevent recursive inclusion ------------------------------------- */
#ifndef PY32F0XX_LL_SYSTEM_H
#define PY32F0XX_LL_SYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------ */
#include "py32f0xx.h"

/* @addtogroup PY32F0xx_LL_Driver @{ */

#if defined (FLASH) || defined (SYSCFG) || defined (DBGMCU)

/* @defgroup SYSTEM_LL SYSTEM @{ */

/* Private types ------------------------------------------------------------- */
/* Private variables --------------------------------------------------------- */

/* Private constants --------------------------------------------------------- */
/* @defgroup SYSTEM_LL_Private_Constants SYSTEM Private Constants @{ */

/* @} */

/* Private macros ------------------------------------------------------------ */

/* Exported types ------------------------------------------------------------ */

/* Exported constants -------------------------------------------------------- */

/* @defgroup SYSTEM_LL_Exported_Constants SYSTEM Exported Constants @{ */

/* @defgroup SYSTEM_LL_EC_REMAP SYSCFG REMAP @{ */
#define LL_SYSCFG_REMAP_FLASH               0x00000000U                                           /* !< Main Flash memory mapped at 0x00000000 */
#define LL_SYSCFG_REMAP_SYSTEMFLASH         SYSCFG_CFGR1_MEM_MODE_0                               /* !< System Flash memory mapped at 0x00000000 */
#define LL_SYSCFG_REMAP_SRAM                (SYSCFG_CFGR1_MEM_MODE_1 | SYSCFG_CFGR1_MEM_MODE_0)   /* !< Embedded SRAM mapped at 0x00000000 */
/* @} */

/* @defgroup SYSTEM_LL_EC_I2C_ANF I2C ANALOG FILTER ENABLE CONTORL @{ */
#if defined(SYSCFG_CFGR1_I2C_PA2_ANF)
#define LL_SYSCFG_I2C_ANF_PA2                SYSCFG_CFGR1_I2C_PA2_ANF
#endif
#if defined(SYSCFG_CFGR1_I2C_PA3_ANF)
#define LL_SYSCFG_I2C_ANF_PA3                SYSCFG_CFGR1_I2C_PA3_ANF
#endif
#if defined(SYSCFG_CFGR1_I2C_PA7_ANF)
#define LL_SYSCFG_I2C_ANF_PA7                SYSCFG_CFGR1_I2C_PA7_ANF
#endif
#if defined(SYSCFG_CFGR1_I2C_PA8_ANF)
#define LL_SYSCFG_I2C_ANF_PA8                SYSCFG_CFGR1_I2C_PA8_ANF
#endif
#if defined(SYSCFG_CFGR1_I2C_PA9_ANF)
#define LL_SYSCFG_I2C_ANF_PA9                SYSCFG_CFGR1_I2C_PA9_ANF
#endif
#if defined(SYSCFG_CFGR1_I2C_PA10_ANF)
#define LL_SYSCFG_I2C_ANF_PA10               SYSCFG_CFGR1_I2C_PA10_ANF
#endif
#if defined(SYSCFG_CFGR1_I2C_PA11_ANF)
#define LL_SYSCFG_I2C_ANF_PA11               SYSCFG_CFGR1_I2C_PA11_ANF
#endif
#if defined(SYSCFG_CFGR1_I2C_PA12_ANF)
#define LL_SYSCFG_I2C_ANF_PA12               SYSCFG_CFGR1_I2C_PA12_ANF
#endif
#if defined(SYSCFG_CFGR1_I2C_PB6_ANF)
#define LL_SYSCFG_I2C_ANF_PB6                SYSCFG_CFGR1_I2C_PB6_ANF
#endif
#if defined(SYSCFG_CFGR1_I2C_PB7_ANF)
#define LL_SYSCFG_I2C_ANF_PB7                SYSCFG_CFGR1_I2C_PB7_ANF
#endif
#if defined(SYSCFG_CFGR1_I2C_PB8_ANF)
#define LL_SYSCFG_I2C_ANF_PB8                SYSCFG_CFGR1_I2C_PB8_ANF
#endif
#if defined(SYSCFG_CFGR1_I2C_PF0_ANF)
#define LL_SYSCFG_I2C_ANF_PF0                SYSCFG_CFGR1_I2C_PF0_ANF
#endif
#if defined(SYSCFG_CFGR1_I2C_PF1_ANF)
#define LL_SYSCFG_I2C_ANF_PF1                SYSCFG_CFGR1_I2C_PF1_ANF
#endif
/* @} */

/* @defgroup SYSTEM_LL_EC_TIMBREAK TIMER BREAK INPUT @{ */
#if defined(SYSCFG_CFGR2_LOCKUP_LOCK)
#define LL_SYSCFG_TIMBREAK_LOCKUP_TO_ALL      SYSCFG_CFGR2_LOCKUP_LOCK
#endif
#if defined(SYSCFG_CFGR2_PVD_LOCK)
#define LL_SYSCFG_TIMBREAK_PVD_TO_ALL         SYSCFG_CFGR2_PVD_LOCK
#endif
#if defined(SYSCFG_CFGR2_COMP1_BRK_TIM1)
#define LL_SYSCFG_TIMBREAK_COMP1_TO_TIM1      SYSCFG_CFGR2_COMP1_BRK_TIM1
#endif
#if defined(SYSCFG_CFGR2_COMP2_BRK_TIM1)
#define LL_SYSCFG_TIMBREAK_COMP2_TO_TIM1      SYSCFG_CFGR2_COMP2_BRK_TIM1
#endif
#if defined(SYSCFG_CFGR2_COMP1_BRK_TIM16)
#define LL_SYSCFG_TIMBREAK_COMP1_TO_TIM16     SYSCFG_CFGR2_COMP1_BRK_TIM16
#endif
#if defined(SYSCFG_CFGR2_COMP2_BRK_TIM16)
#define LL_SYSCFG_TIMBREAK_COMP2_TO_TIM16     SYSCFG_CFGR2_COMP2_BRK_TIM16
#endif
#if defined(SYSCFG_CFGR2_COMP1_BRK_TIM17)
#define LL_SYSCFG_TIMBREAK_COMP1_TO_TIM17     SYSCFG_CFGR2_COMP1_BRK_TIM17
#endif
#if defined(SYSCFG_CFGR2_COMP2_BRK_TIM17)
#define LL_SYSCFG_TIMBREAK_COMP2_TO_TIM17     SYSCFG_CFGR2_COMP2_BRK_TIM17
#endif
/* @} */

/* @defgroup SYSTEM_LL_EC_ETR_SRC ETR SOURCE @{ */
#define LL_SYSCFG_ETR_SRC_TIM1_GPIO          0x00000000U
#if defined(COMP1)
#define LL_SYSCFG_ETR_SRC_TIM1_COMP1         SYSCFG_CFGR2_ETR_SRC_TIM1_0
#endif
#if defined(COMP2)
#define LL_SYSCFG_ETR_SRC_TIM1_COMP2         SYSCFG_CFGR2_ETR_SRC_TIM1_1
#endif
#define LL_SYSCFG_ETR_SRC_TIM1_ADC           (SYSCFG_CFGR2_ETR_SRC_TIM1_1 | SYSCFG_CFGR2_ETR_SRC_TIM1_0)
/* @} */
  
#if (defined(DMA) || defined(DMA1))
/* @defgroup SYSTEM_LL_EC_DMA_MAP DMA MAP @{ */
#define LL_SYSCFG_DMA_MAP_ADC               0x00000000U
#define LL_SYSCFG_DMA_MAP_SPI1_TX           SYSCFG_CFGR3_DMA1_MAP_0
#define LL_SYSCFG_DMA_MAP_SPI1_RX           SYSCFG_CFGR3_DMA1_MAP_1
#define LL_SYSCFG_DMA_MAP_SPI2_TX           (SYSCFG_CFGR3_DMA1_MAP_1 | SYSCFG_CFGR3_DMA1_MAP_0)
#define LL_SYSCFG_DMA_MAP_SPI2_RX           SYSCFG_CFGR3_DMA1_MAP_2
#define LL_SYSCFG_DMA_MAP_USART1_TX         (SYSCFG_CFGR3_DMA1_MAP_2 | SYSCFG_CFGR3_DMA1_MAP_0)
#define LL_SYSCFG_DMA_MAP_USART1_RX         (SYSCFG_CFGR3_DMA1_MAP_2 | SYSCFG_CFGR3_DMA1_MAP_1)
#define LL_SYSCFG_DMA_MAP_USART2_TX         (SYSCFG_CFGR3_DMA1_MAP_2 | SYSCFG_CFGR3_DMA1_MAP_1 | SYSCFG_CFGR3_DMA1_MAP_0)
#define LL_SYSCFG_DMA_MAP_USART2_RX         SYSCFG_CFGR3_DMA1_MAP_3
#define LL_SYSCFG_DMA_MAP_I2C_TX            (SYSCFG_CFGR3_DMA1_MAP_3 | SYSCFG_CFGR3_DMA1_MAP_0)
#define LL_SYSCFG_DMA_MAP_I2C_RX            (SYSCFG_CFGR3_DMA1_MAP_3 | SYSCFG_CFGR3_DMA1_MAP_1)
#define LL_SYSCFG_DMA_MAP_TIM1_CH1          (SYSCFG_CFGR3_DMA1_MAP_3 | SYSCFG_CFGR3_DMA1_MAP_1 | SYSCFG_CFGR3_DMA1_MAP_0)
#define LL_SYSCFG_DMA_MAP_TIM1_CH2          (SYSCFG_CFGR3_DMA1_MAP_3 | SYSCFG_CFGR3_DMA1_MAP_2)
#define LL_SYSCFG_DMA_MAP_TIM1_CH3          (SYSCFG_CFGR3_DMA1_MAP_3 | SYSCFG_CFGR3_DMA1_MAP_2 | SYSCFG_CFGR3_DMA1_MAP_0)
#define LL_SYSCFG_DMA_MAP_TIM1_CH4          (SYSCFG_CFGR3_DMA1_MAP_3 | SYSCFG_CFGR3_DMA1_MAP_2 | SYSCFG_CFGR3_DMA1_MAP_1)
#define LL_SYSCFG_DMA_MAP_TIM1_COM          (SYSCFG_CFGR3_DMA1_MAP_3 | SYSCFG_CFGR3_DMA1_MAP_2 | SYSCFG_CFGR3_DMA1_MAP_1 | SYSCFG_CFGR3_DMA1_MAP_0)
#define LL_SYSCFG_DMA_MAP_TIM1_UP           SYSCFG_CFGR3_DMA1_MAP_4
#define LL_SYSCFG_DMA_MAP_TIM1_TRIG         (SYSCFG_CFGR3_DMA1_MAP_4 | SYSCFG_CFGR3_DMA1_MAP_0)
#define LL_SYSCFG_DMA_MAP_TIM3_CH1          (SYSCFG_CFGR3_DMA1_MAP_4 | SYSCFG_CFGR3_DMA1_MAP_1)
#define LL_SYSCFG_DMA_MAP_TIM3_CH3          (SYSCFG_CFGR3_DMA1_MAP_4 | SYSCFG_CFGR3_DMA1_MAP_1 | SYSCFG_CFGR3_DMA1_MAP_0)
#define LL_SYSCFG_DMA_MAP_TIM3_CH4          (SYSCFG_CFGR3_DMA1_MAP_4 | SYSCFG_CFGR3_DMA1_MAP_2)
#define LL_SYSCFG_DMA_MAP_TIM3_TRG          (SYSCFG_CFGR3_DMA1_MAP_4 | SYSCFG_CFGR3_DMA1_MAP_2 | SYSCFG_CFGR3_DMA1_MAP_0)
#define LL_SYSCFG_DMA_MAP_TIM3_UP           (SYSCFG_CFGR3_DMA1_MAP_4 | SYSCFG_CFGR3_DMA1_MAP_2 | SYSCFG_CFGR3_DMA1_MAP_1)
#define LL_SYSCFG_DMA_MAP_TIM16_CH1         (SYSCFG_CFGR3_DMA1_MAP_4 | SYSCFG_CFGR3_DMA1_MAP_3)
#define LL_SYSCFG_DMA_MAP_TIM16_UP          (SYSCFG_CFGR3_DMA1_MAP_4 | SYSCFG_CFGR3_DMA1_MAP_3 | SYSCFG_CFGR3_DMA1_MAP_0)
#define LL_SYSCFG_DMA_MAP_TIM17_CH1         (SYSCFG_CFGR3_DMA1_MAP_4 | SYSCFG_CFGR3_DMA1_MAP_3 | SYSCFG_CFGR3_DMA1_MAP_1)
#define LL_SYSCFG_DMA_MAP_TIM17_UP          (SYSCFG_CFGR3_DMA1_MAP_4 | SYSCFG_CFGR3_DMA1_MAP_3 | SYSCFG_CFGR3_DMA1_MAP_1 | SYSCFG_CFGR3_DMA1_MAP_0)
/* @} */

/* @defgroup SYSTEM_LL_EC_DMA_ACKLVL DMA SPEED ENABLE @{ */
#define LL_SYSCFG_DMA_ACKLVL_NORM          0x00000000U
#define LL_SYSCFG_DMA_ACKLVL_FAST          SYSCFG_CFGR3_DMA1_ACKLVL
/* @} */

/* @defgroup SYSTEM_LL_EC_DMA_CHNNEL_SHIFT DMA CHNNEL SHIFT ADDRESS @{ */
#define LL_SYSCFG_DMA_CH2_SHIFT            (8U)
#define LL_SYSCFG_DMA_CH3_SHIFT            (16U)
/* @} */
#endif

/* @defgroup SYSTEM_LL_EC_LATENCY FLASH LATENCY @{ */
#define LL_FLASH_LATENCY_0                 0x00000000U             /* !< FLASH Zero Latency cycle */
#define LL_FLASH_LATENCY_1                 FLASH_ACR_LATENCY       /* !< FLASH One Latency cycle */
/* @} */



/* @defgroup SYSTEM_LL_EC_APB1_GRP1_STOP_IP  DBGMCU APB1 GRP1 STOP IP @{ */
#if defined(DBGMCU_APB_FZ1_DBG_TIM3_STOP)
#define LL_DBGMCU_APB1_GRP1_TIM3_STOP      DBGMCU_APB_FZ1_DBG_TIM3_STOP        /* !< TIM3 counter stopped when core is halted */
#endif
#if defined(DBGMCU_APB_FZ1_DBG_RTC_STOP)
#define LL_DBGMCU_APB1_GRP1_RTC_STOP       DBGMCU_APB_FZ1_DBG_RTC_STOP         /* !< RTC Calendar frozen when core is halted */
#endif
#if defined(DBGMCU_APB_FZ1_DBG_WWDG_STOP)
#define LL_DBGMCU_APB1_GRP1_WWDG_STOP      DBGMCU_APB_FZ1_DBG_WWDG_STOP        /* !< Debug Window Watchdog stopped when Core is halted */
#endif
#if defined(DBGMCU_APB_FZ1_DBG_IWDG_STOP)
#define LL_DBGMCU_APB1_GRP1_IWDG_STOP      DBGMCU_APB_FZ1_DBG_IWDG_STOP        /* !< Debug Independent Watchdog stopped when Core is halted */
#endif
#if defined(DBGMCU_APB_FZ1_DBG_LPTIM_STOP)
#define LL_DBGMCU_APB1_GRP1_LPTIM1_STOP    DBGMCU_APB_FZ1_DBG_LPTIM_STOP      /* !< LPTIM1 counter stopped when Core is halted */
#endif
/* @} */

/* @defgroup SYSTEM_LL_EC_APB2_GRP1_STOP_IP DBGMCU APB2 GRP1 STOP IP @{ */
#if defined(DBGMCU_APB_FZ2_DBG_TIM1_STOP)
#define LL_DBGMCU_APB1_GRP2_TIM1_STOP      DBGMCU_APB_FZ2_DBG_TIM1_STOP        /* !< TIM1 counter stopped when core is halted */
#endif
#if defined(DBGMCU_APB_FZ2_DBG_TIM14_STOP)
#define LL_DBGMCU_APB1_GRP2_TIM14_STOP     DBGMCU_APB_FZ2_DBG_TIM14_STOP       /* !< TIM14 counter stopped when core is halted */
#endif
#if defined(DBGMCU_APB_FZ2_DBG_TIM16_STOP)
#define LL_DBGMCU_APB1_GRP2_TIM16_STOP     DBGMCU_APB_FZ2_DBG_TIM16_STOP       /* !< TIM16 counter stopped when core is halted */
#endif
#if defined(DBGMCU_APB_FZ2_DBG_TIM17_STOP)
#define LL_DBGMCU_APB1_GRP2_TIM17_STOP     DBGMCU_APB_FZ2_DBG_TIM17_STOP       /* !< TIM17 counter stopped when core is halted */
#endif
/* @} */

/* @} */

/* Exported macro ------------------------------------------------------------ */

/* Exported functions -------------------------------------------------------- */
/* @defgroup SYSTEM_LL_Exported_Functions SYSTEM Exported Functions @{ */

/* @defgroup SYSTEM_LL_EF_SYSCFG SYSCFG @{ */



__STATIC_INLINE void LL_SYSCFG_SetRemapMemory(uint32_t Memory)
{
  MODIFY_REG(SYSCFG->CFGR1, SYSCFG_CFGR1_MEM_MODE, Memory);
}



__STATIC_INLINE uint32_t LL_SYSCFG_GetRemapMemory(void)
{
  return (uint32_t)(READ_BIT(SYSCFG->CFGR1, SYSCFG_CFGR1_MEM_MODE));
}



__STATIC_INLINE void LL_SYSCFG_EnableI2CAnalogFilter(uint32_t I2CAnalogFilter)
{
  SET_BIT(SYSCFG->CFGR1, I2CAnalogFilter);
}



__STATIC_INLINE void LL_SYSCFG_DisableI2CAnalogFilter(uint32_t I2CAnalogFilter)
{
  CLEAR_BIT(SYSCFG->CFGR1, I2CAnalogFilter);
}



__STATIC_INLINE uint32_t LL_SYSCFG_IsEnabledI2CAnalogFilter(uint32_t I2CAnalogFilter)
{
  return ((READ_BIT(SYSCFG->CFGR1, I2CAnalogFilter) == (I2CAnalogFilter)) ? 1UL : 0UL);
}



__STATIC_INLINE void LL_SYSCFG_EnableTIMBreakInputs(uint32_t TIMBreakInputs)
{
  SET_BIT(SYSCFG->CFGR2, TIMBreakInputs);
}



__STATIC_INLINE void LL_SYSCFG_DisableTIMBreakInputs(uint32_t TIMBreakInputs)
{
  CLEAR_BIT(SYSCFG->CFGR2, TIMBreakInputs);
}



__STATIC_INLINE uint32_t LL_SYSCFG_IsEnabledTIMBreakInputs(uint32_t TIMBreakInputs)
{
  return ((READ_BIT(SYSCFG->CFGR2, TIMBreakInputs) == (TIMBreakInputs)) ? 1UL : 0UL);
}



__STATIC_INLINE void LL_SYSCFG_SetTIM1ETRSource(uint32_t source)
{
  MODIFY_REG(SYSCFG->CFGR2, SYSCFG_CFGR2_ETR_SRC_TIM1, source);
}



__STATIC_INLINE uint32_t LL_SYSCFG_GetTIM1ETRSource(void)
{
  return (uint32_t)(READ_BIT(SYSCFG->CFGR2, SYSCFG_CFGR2_ETR_SRC_TIM1));
}

#if (defined(DMA) || defined(DMA1))

__STATIC_INLINE void LL_SYSCFG_SetDMARemap_CH1(uint32_t Requset)
{
  MODIFY_REG(SYSCFG->CFGR3, SYSCFG_CFGR3_DMA1_MAP_Msk, Requset);
}



__STATIC_INLINE uint32_t LL_SYSCFG_GetDMARemap_CH1(void)
{
  return (uint32_t)(READ_BIT(SYSCFG->CFGR3, SYSCFG_CFGR3_DMA1_MAP_Msk));
}



__STATIC_INLINE void LL_SYSCFG_SetDMAResponseSpeed_CH1(uint32_t ResponseSpeed)
{
  MODIFY_REG(SYSCFG->CFGR3, SYSCFG_CFGR3_DMA1_ACKLVL, ResponseSpeed);
}



__STATIC_INLINE uint32_t LL_SYSCFG_GetDMAResponseSpeed_CH1(void)
{
  return (uint32_t)(READ_BIT(SYSCFG->CFGR3, SYSCFG_CFGR3_DMA1_ACKLVL));
}



__STATIC_INLINE void LL_SYSCFG_SetDMARemap_CH2(uint32_t Requset)
{
  MODIFY_REG(SYSCFG->CFGR3, SYSCFG_CFGR3_DMA2_MAP_Msk, (Requset << LL_SYSCFG_DMA_CH2_SHIFT));
}



__STATIC_INLINE uint32_t LL_SYSCFG_GetDMARemap_CH2(void)
{
  return (uint32_t)((READ_BIT(SYSCFG->CFGR3, SYSCFG_CFGR3_DMA2_MAP_Msk)) >> LL_SYSCFG_DMA_CH2_SHIFT);
}



__STATIC_INLINE void LL_SYSCFG_SetDMAResponseSpeed_CH2(uint32_t ResponseSpeed)
{
  MODIFY_REG(SYSCFG->CFGR3, SYSCFG_CFGR3_DMA2_ACKLVL, (ResponseSpeed << LL_SYSCFG_DMA_CH2_SHIFT));
}



__STATIC_INLINE uint32_t LL_SYSCFG_GetDMAResponseSpeed_CH2(void)
{
  return (uint32_t)((READ_BIT(SYSCFG->CFGR3, SYSCFG_CFGR3_DMA2_ACKLVL)) >> LL_SYSCFG_DMA_CH2_SHIFT);
}



__STATIC_INLINE void LL_SYSCFG_SetDMARemap_CH3(uint32_t Requset)
{
  MODIFY_REG(SYSCFG->CFGR3, SYSCFG_CFGR3_DMA3_MAP_Msk, (Requset << LL_SYSCFG_DMA_CH3_SHIFT));
}



__STATIC_INLINE uint32_t LL_SYSCFG_GetDMARemap_CH3(void)
{
  return (uint32_t)((READ_BIT(SYSCFG->CFGR3, SYSCFG_CFGR3_DMA3_MAP_Msk)) >> LL_SYSCFG_DMA_CH3_SHIFT);
}



__STATIC_INLINE void LL_SYSCFG_SetDMAResponseSpeed_CH3(uint32_t ResponseSpeed)
{
  MODIFY_REG(SYSCFG->CFGR3, SYSCFG_CFGR3_DMA3_ACKLVL, (ResponseSpeed << LL_SYSCFG_DMA_CH3_SHIFT));
}



__STATIC_INLINE uint32_t LL_SYSCFG_GetDMAResponseSpeed_CH3(void)
{
  return (uint32_t)((READ_BIT(SYSCFG->CFGR3, SYSCFG_CFGR3_DMA3_ACKLVL)) >> LL_SYSCFG_DMA_CH3_SHIFT);
}
#endif

/* @} */

/* @defgroup SYSTEM_LL_EF_FLASH FLASH @{ */



__STATIC_INLINE void LL_FLASH_SetLatency(uint32_t Latency)
{
  MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, Latency);
}



__STATIC_INLINE uint32_t LL_FLASH_GetLatency(void)
{
  return (uint32_t)(READ_BIT(FLASH->ACR, FLASH_ACR_LATENCY));
}

/* @} */

/* @defgroup SYSTEM_LL_EF_DBGMCU DBGMCU @{ */



__STATIC_INLINE uint32_t LL_DBGMCU_GetDeviceID(void)
{
  return (uint32_t)(READ_BIT(DBGMCU->IDCODE, DBGMCU_IDCODE_DEV_ID));
}



__STATIC_INLINE uint32_t LL_DBGMCU_GetRevisionID(void)
{
  return (uint32_t)(READ_BIT(DBGMCU->IDCODE, DBGMCU_IDCODE_REV_ID) >> DBGMCU_IDCODE_REV_ID_Pos);
}



/* @brief  Enable the Debug Module during STOP mode @retval None */
__STATIC_INLINE void LL_DBGMCU_EnableDBGStopMode(void)
{
  SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STOP);
}

/* @brief  Disable the Debug Module during STOP mode @retval None */
__STATIC_INLINE void LL_DBGMCU_DisableDBGStopMode(void)
{
  CLEAR_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STOP);
}



__STATIC_INLINE uint32_t LL_DBGMCU_IsEnabledDBGStopMode(void)
{
  return ((READ_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STOP) == (DBGMCU_CR_DBG_STOP)) ? 1UL : 0UL);
}

__STATIC_INLINE void LL_DBGMCU_APB1_GRP1_FreezePeriph(uint32_t Periphs)
{
  SET_BIT(DBGMCU->APBFZ1, Periphs);
}



__STATIC_INLINE void LL_DBGMCU_APB1_GRP1_UnFreezePeriph(uint32_t Periphs)
{
  CLEAR_BIT(DBGMCU->APBFZ1, Periphs);
}



__STATIC_INLINE uint32_t LL_DBGMCU_APB1_GRP1_IsFreezePeriph(uint32_t Periphs)
{
  return ((READ_BIT(DBGMCU->APBFZ1, Periphs) == (Periphs)) ? 1UL : 0UL);
}



__STATIC_INLINE void LL_DBGMCU_APB1_GRP2_FreezePeriph(uint32_t Periphs)
{
  SET_BIT(DBGMCU->APBFZ2, Periphs);
}



__STATIC_INLINE void LL_DBGMCU_APB1_GRP2_UnFreezePeriph(uint32_t Periphs)
{
  CLEAR_BIT(DBGMCU->APBFZ2, Periphs);
}



__STATIC_INLINE uint32_t LL_DBGMCU_APB1_GRP2_IsFreezePeriph(uint32_t Periphs)
{
  return ((READ_BIT(DBGMCU->APBFZ2, Periphs) == (Periphs)) ? 1UL : 0UL);
}
/* @} */



/* @} */

/* @} */

#endif /* defined (FLASH) || defined (SYSCFG) || defined (DBGMCU) */

/* @} */

#ifdef __cplusplus
}
#endif

#endif /* PY32F0XX_LL_SYSTEM_H */

/* (C) COPYRIGHT Puya *****END OF FILE */
