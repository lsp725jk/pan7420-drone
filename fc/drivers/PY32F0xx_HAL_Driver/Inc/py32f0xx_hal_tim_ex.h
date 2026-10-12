/**
 * @file    py32f0xx_hal_tim_ex.h
 * @author  MCU Application Team
 * @brief   Header file of TIM HAL Extended module.
 * @attention
 */

/* Define to prevent recursive inclusion ------------------------------------- */
#ifndef __PY32F0xx_HAL_TIM_EX_H
#define __PY32F0xx_HAL_TIM_EX_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------ */
#include "py32f0xx_hal_def.h"

/* @addtogroup PY32F0xx_HAL_Driver @{ */

/* @addtogroup TIMEx @{ */

/* Exported types ------------------------------------------------------------ */
/* @defgroup TIMEx_Exported_Types TIM Extended Exported Types @{ */

/* @brief  TIM Hall sensor Configuration Structure definition */

typedef struct
{
  uint32_t IC1Polarity;         

  uint32_t IC1Prescaler;        

  uint32_t IC1Filter;           

  uint32_t Commutation_Delay;   
} TIM_HallSensor_InitTypeDef;
/* @} */
/* End of exported types ----------------------------------------------------- */

/* Exported constants -------------------------------------------------------- */
/* @defgroup TIMEx_Exported_Constants TIM Extended Exported Constants @{ */

/* @defgroup TIMEx_Remap TIM Extended Remapping @{ */
#define TIM_TIM14_GPIO                         (0x00000000U) /* !< TIM14 TI1 is connected to GPIO */
#define TIM_TIM14_RTC                          (0x00000001U) /* !< TIM14 TI1 is connected to RTC_clock */
#define TIM_TIM14_HSE                          (0x00000002U) /* !< TIM14 TI1 is connected to HSE/32U */
#define TIM_TIM14_MCO                          (0x00000003U) /* !< TIM14 TI1 is connected to MCO */
/* @} */

/* @} */
/* End of exported constants ------------------------------------------------- */

/* Exported macro ------------------------------------------------------------ */
/* @defgroup TIMEx_Exported_Macros TIM Extended Exported Macros @{ */

/* @} */
/* End of exported macro ----------------------------------------------------- */

/* Private macro ------------------------------------------------------------- */
/* @defgroup TIMEx_Private_Macros TIM Extended Private Macros @{ */
#define IS_TIM_REMAP(__INSTANCE__, __REMAP__)                                             \
          (((__INSTANCE__) == TIM14)  && (((__REMAP__) & 0xFFFFFFFCU) == 0x00000000U))

/* @} */
/* End of private macro ------------------------------------------------------ */

/* Exported functions -------------------------------------------------------- */
/* @addtogroup TIMEx_Exported_Functions TIM Extended Exported Functions @{ */

/* @brief    Timer Hall Sensor functions @{ */
/* Timer Hall Sensor functions */
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Init(TIM_HandleTypeDef *htim, TIM_HallSensor_InitTypeDef *sConfig);
HAL_StatusTypeDef HAL_TIMEx_HallSensor_DeInit(TIM_HandleTypeDef *htim);

void HAL_TIMEx_HallSensor_MspInit(TIM_HandleTypeDef *htim);
void HAL_TIMEx_HallSensor_MspDeInit(TIM_HandleTypeDef *htim);

/* Blocking mode: Polling */
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Start(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Stop(TIM_HandleTypeDef *htim);
/* Non-Blocking mode: Interrupt */
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Start_IT(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Stop_IT(TIM_HandleTypeDef *htim);
/* Non-Blocking mode: DMA */
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Start_DMA(TIM_HandleTypeDef *htim, uint32_t *pData, uint16_t Length);
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Stop_DMA(TIM_HandleTypeDef *htim);
/* @} */

/* @brief   Timer Complementary Output Compare functions @{ */
/* Timer Complementary Output Compare functions */
/* Blocking mode: Polling */
HAL_StatusTypeDef HAL_TIMEx_OCN_Start(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIMEx_OCN_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);

/* Non-Blocking mode: Interrupt */
HAL_StatusTypeDef HAL_TIMEx_OCN_Start_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIMEx_OCN_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel);

/* Non-Blocking mode: DMA */
HAL_StatusTypeDef HAL_TIMEx_OCN_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t *pData, uint16_t Length);
HAL_StatusTypeDef HAL_TIMEx_OCN_Stop_DMA(TIM_HandleTypeDef *htim, uint32_t Channel);
/* @} */

/* @brief    Timer Complementary PWM functions @{ */
/* Timer Complementary PWM functions */
/* Blocking mode: Polling */
HAL_StatusTypeDef HAL_TIMEx_PWMN_Start(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIMEx_PWMN_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);

/* Non-Blocking mode: Interrupt */
HAL_StatusTypeDef HAL_TIMEx_PWMN_Start_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIMEx_PWMN_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
/* Non-Blocking mode: DMA */
HAL_StatusTypeDef HAL_TIMEx_PWMN_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t *pData, uint16_t Length);
HAL_StatusTypeDef HAL_TIMEx_PWMN_Stop_DMA(TIM_HandleTypeDef *htim, uint32_t Channel);
/* @} */

/* @brief    Timer Complementary One Pulse functions @{ */
/* Timer Complementary One Pulse functions */
/* Blocking mode: Polling */
HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Start(TIM_HandleTypeDef *htim, uint32_t OutputChannel);
HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Stop(TIM_HandleTypeDef *htim, uint32_t OutputChannel);

/* Non-Blocking mode: Interrupt */
HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Start_IT(TIM_HandleTypeDef *htim, uint32_t OutputChannel);
HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Stop_IT(TIM_HandleTypeDef *htim, uint32_t OutputChannel);
/* @} */

/* @brief    Peripheral Control functions @{ */
/* Extended Control functions */
HAL_StatusTypeDef HAL_TIMEx_ConfigCommutEvent(TIM_HandleTypeDef *htim, uint32_t  InputTrigger,
    uint32_t  CommutationSource);
HAL_StatusTypeDef HAL_TIMEx_ConfigCommutEvent_IT(TIM_HandleTypeDef *htim, uint32_t  InputTrigger,
    uint32_t  CommutationSource);
HAL_StatusTypeDef HAL_TIMEx_ConfigCommutEvent_DMA(TIM_HandleTypeDef *htim, uint32_t  InputTrigger,
    uint32_t  CommutationSource);
HAL_StatusTypeDef HAL_TIMEx_MasterConfigSynchronization(TIM_HandleTypeDef *htim,
    TIM_MasterConfigTypeDef *sMasterConfig);
HAL_StatusTypeDef HAL_TIMEx_ConfigBreakDeadTime(TIM_HandleTypeDef *htim,
    TIM_BreakDeadTimeConfigTypeDef *sBreakDeadTimeConfig);
HAL_StatusTypeDef HAL_TIMEx_RemapConfig(TIM_HandleTypeDef *htim, uint32_t Remap);
/* @} */

/* @brief    Extended Callbacks functions @{ */
/* Extended Callback */
void HAL_TIMEx_CommutCallback(TIM_HandleTypeDef *htim);
void HAL_TIMEx_CommutHalfCpltCallback(TIM_HandleTypeDef *htim);
void HAL_TIMEx_BreakCallback(TIM_HandleTypeDef *htim);
/* @} */

/* @brief    Extended Peripheral State functions @{ */
/* Extended Peripheral State functions */
HAL_TIM_StateTypeDef HAL_TIMEx_HallSensor_GetState(TIM_HandleTypeDef *htim);
/* @} */

/* @} */
/* End of exported functions ------------------------------------------------- */

/* Private functions---------------------------------------------------------- */
/* @addtogroup TIMEx_Private_Functions TIMEx Private Functions @{ */
#if (defined(DMA) || defined(DMA1))
void TIMEx_DMACommutationCplt(DMA_HandleTypeDef *hdma);
void TIMEx_DMACommutationHalfCplt(DMA_HandleTypeDef *hdma);
#endif
/* @} */
/* End of private functions -------------------------------------------------- */

/* @} */

/* @} */

#ifdef __cplusplus
}
#endif



#endif /* __PY32F0xx_HAL_TIM_EX_H */

/* (C) COPYRIGHT Puya *****END OF FILE */
