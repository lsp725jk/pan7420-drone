/**
 * @file    py32f0xx_ll_gpio.h
 * @author  MCU Application Team
 * @brief   Header file of GPIO LL module.
 * @attention
 */

/* Define to prevent recursive inclusion ------------------------------------- */
#ifndef __PY32F0xx_LL_GPIO_H
#define __PY32F0xx_LL_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------ */
#include "py32f0xx.h"
/* @addtogroup PY32F0xx_LL_Driver @{ */

#if defined (GPIOA) || defined (GPIOB) || defined (GPIOF)

/* @defgroup GPIO_LL GPIO @{ */
/* LL_GPIO_GetAFPin_0_7 LL_GPIO_SetAFPin_0_7 LL_GPIO_SetAFPin_8_15 LL_GPIO_GetAFPin_8_15 */

/* Private types ------------------------------------------------------------- */
/* Private variables --------------------------------------------------------- */
/* Private constants --------------------------------------------------------- */
/* Private macros ------------------------------------------------------------ */
#if defined(USE_FULL_LL_DRIVER)
/* @defgroup GPIO_LL_Private_Macros GPIO Private Macros @{ */

/* @} */
#endif /* USE_FULL_LL_DRIVER */

/* Exported types ------------------------------------------------------------ */
#if defined(USE_FULL_LL_DRIVER)
/* @defgroup GPIO_LL_ES_INIT GPIO Exported Init structures @{ */

/* @brief LL GPIO Init Structure definition */
typedef struct
{
  uint32_t Pin;          

  uint32_t Mode;           /* !< Specifies the operating mode for the selected pins. This parameter can be a value of @ref GPIO_LL_EC_MODE. */

  uint32_t Speed;          /* !< Specifies the speed for the selected pins. This parameter can be a value of @ref GPIO_LL_EC_SPEED. */

  uint32_t OutputType;       /* This parameter can be a value of @ref GPIO_LL_EC_OUTPUT. */

  uint32_t Pull;             /* This parameter can be a value of @ref GPIO_LL_EC_PULL. */

  uint32_t Alternate;        /* This parameter can be a value of @ref GPIO_LL_EC_AF. */
} LL_GPIO_InitTypeDef;

/* @} */
#endif /* USE_FULL_LL_DRIVER */

/* Exported constants -------------------------------------------------------- */
/* @defgroup GPIO_LL_Exported_Constants GPIO Exported Constants @{ */
#define LL_GPIO_LCKR_LCKK                  0x00030000U

/* @defgroup GPIO_LL_EC_PIN PIN @{ */
#define LL_GPIO_PIN_0                      GPIO_BSRR_BS0  /* !< Select pin 0 */
#define LL_GPIO_PIN_1                      GPIO_BSRR_BS1  /* !< Select pin 1 */
#define LL_GPIO_PIN_2                      GPIO_BSRR_BS2  /* !< Select pin 2 */
#define LL_GPIO_PIN_3                      GPIO_BSRR_BS3  /* !< Select pin 3 */
#define LL_GPIO_PIN_4                      GPIO_BSRR_BS4  /* !< Select pin 4 */
#define LL_GPIO_PIN_5                      GPIO_BSRR_BS5  /* !< Select pin 5 */
#define LL_GPIO_PIN_6                      GPIO_BSRR_BS6  /* !< Select pin 6 */
#define LL_GPIO_PIN_7                      GPIO_BSRR_BS7  /* !< Select pin 7 */
#define LL_GPIO_PIN_8                      GPIO_BSRR_BS8  /* !< Select pin 8 */
#define LL_GPIO_PIN_9                      GPIO_BSRR_BS9  /* !< Select pin 9 */
#define LL_GPIO_PIN_10                     GPIO_BSRR_BS10 /* !< Select pin 10 */
#define LL_GPIO_PIN_11                     GPIO_BSRR_BS11 /* !< Select pin 11 */
#define LL_GPIO_PIN_12                     GPIO_BSRR_BS12 /* !< Select pin 12 */
#define LL_GPIO_PIN_13                     GPIO_BSRR_BS13 /* !< Select pin 13 */
#define LL_GPIO_PIN_14                     GPIO_BSRR_BS14 /* !< Select pin 14 */
#define LL_GPIO_PIN_15                     GPIO_BSRR_BS15 /* !< Select pin 15 */
#define LL_GPIO_PIN_ALL                    (GPIO_BSRR_BS0 | GPIO_BSRR_BS1  | GPIO_BSRR_BS2  | \
                                           GPIO_BSRR_BS3  | GPIO_BSRR_BS4  | GPIO_BSRR_BS5  | \
                                           GPIO_BSRR_BS6  | GPIO_BSRR_BS7  | GPIO_BSRR_BS8  | \
                                           GPIO_BSRR_BS9  | GPIO_BSRR_BS10 | GPIO_BSRR_BS11 | \
                                           GPIO_BSRR_BS12 | GPIO_BSRR_BS13 | GPIO_BSRR_BS14 | \
                                           GPIO_BSRR_BS15)                                                                                       /* !< Select all pins */
/* @} */

/* @defgroup GPIO_LL_EC_MODE Mode @{ */
#define LL_GPIO_MODE_INPUT                 (0x00000000U)       /* !< Select input mode */
#define LL_GPIO_MODE_OUTPUT                GPIO_MODER_MODE0_0  /* !< Select output mode */
#define LL_GPIO_MODE_ALTERNATE             GPIO_MODER_MODE0_1  /* !< Select alternate function mode */
#define LL_GPIO_MODE_ANALOG                GPIO_MODER_MODE0    /* !< Select analog mode */
/* @} */

/* @defgroup GPIO_LL_EC_OUTPUT Output Type @{ */
#define LL_GPIO_OUTPUT_PUSHPULL            (0x00000000U)   /* !< Select push-pull as output type */
#define LL_GPIO_OUTPUT_OPENDRAIN           GPIO_OTYPER_OT0 /* !< Select open-drain as output type */
/* @} */

/* @defgroup GPIO_LL_EC_SPEED Output Speed @{ */
#define LL_GPIO_SPEED_FREQ_LOW             (0x00000000U)          /* !< Select I/O low output speed */
#define LL_GPIO_SPEED_FREQ_MEDIUM          GPIO_OSPEEDR_OSPEED0_0 /* !< Select I/O medium output speed */
#define LL_GPIO_SPEED_FREQ_HIGH            GPIO_OSPEEDR_OSPEED0_1 /* !< Select I/O fast output speed */
#define LL_GPIO_SPEED_FREQ_VERY_HIGH       GPIO_OSPEEDR_OSPEED0   /* !< Select I/O high output speed */
/* @} */

/* @defgroup GPIO_LL_EC_PULL Pull Up Pull Down @{ */
#define LL_GPIO_PULL_NO                    (0x00000000U)      /* !< Select I/O no pull */
#define LL_GPIO_PULL_UP                    GPIO_PUPDR_PUPD0_0 /* !< Select I/O pull up */
#define LL_GPIO_PULL_DOWN                  GPIO_PUPDR_PUPD0_1 /* !< Select I/O pull down */
/* @} */

/* @defgroup GPIO_LL_EC_AF Alternate Function @{ */
#define LL_GPIO_AF_0                       (0x0000000U) /* !< Select alternate function 0 */
#define LL_GPIO_AF_1                       (0x0000001U) /* !< Select alternate function 1 */
#define LL_GPIO_AF_2                       (0x0000002U) /* !< Select alternate function 2 */
#define LL_GPIO_AF_3                       (0x0000003U) /* !< Select alternate function 3 */
#define LL_GPIO_AF_4                       (0x0000004U) /* !< Select alternate function 4 */
#define LL_GPIO_AF_5                       (0x0000005U) /* !< Select alternate function 5 */
#define LL_GPIO_AF_6                       (0x0000006U) /* !< Select alternate function 6 */
#define LL_GPIO_AF_7                       (0x0000007U) /* !< Select alternate function 7 */
#define LL_GPIO_AF_8                       (0x0000008U) /* !< Select alternate function 8 */
#define LL_GPIO_AF_9                       (0x0000009U) /* !< Select alternate function 9 */
#define LL_GPIO_AF_10                      (0x000000AU) /* !< Select alternate function 10 */
#define LL_GPIO_AF_11                      (0x000000BU) /* !< Select alternate function 11 */
#define LL_GPIO_AF_12                      (0x000000CU) /* !< Select alternate function 12 */
#define LL_GPIO_AF_13                      (0x000000DU) /* !< Select alternate function 13 */
#define LL_GPIO_AF_14                      (0x000000EU) /* !< Select alternate function 14 */
#define LL_GPIO_AF_15                      (0x000000FU) /* !< Select alternate function 15 */

/* @} */

/* @defgroup GPIO_LL_Alternate_function_selection Alternate function selection @{ */

/* @brief   AF 0 selection */

#define LL_GPIO_AF0_SWJ           LL_GPIO_AF_0   /* !< SWJ (SWD) Alternate Function mapping */
#define LL_GPIO_AF0_SPI1          LL_GPIO_AF_0   /* !< SPI1 Alternate Function mapping */
#define LL_GPIO_AF0_SPI2          LL_GPIO_AF_0   /* !< SPI2 Alternate Function mapping */
#define LL_GPIO_AF0_TIM14         LL_GPIO_AF_0   /* !< TIM14 Alternate Function mapping */
#define LL_GPIO_AF0_USART1        LL_GPIO_AF_0   /* !< USART1 Alternate Function mapping */

/* @brief   AF 1 selection */
#define LL_GPIO_AF1_IR            LL_GPIO_AF_1   /* !< IR Alternate Function mapping */
#define LL_GPIO_AF1_SPI2          LL_GPIO_AF_1   /* !< SPI2 Alternate Function mapping */
#define LL_GPIO_AF1_TIM1          LL_GPIO_AF_1   /* !< TIM1 Alternate Function mapping */
#define LL_GPIO_AF1_TIM3          LL_GPIO_AF_1   /* !< TIM3 Alternate Function mapping */
#define LL_GPIO_AF1_USART1        LL_GPIO_AF_1   /* !< USART1 Alternate Function mapping */

/* @brief   AF 2 selection */
#define LL_GPIO_AF2_SPI2          LL_GPIO_AF_2   /* !< SPI2 Alternate Function mapping */
#define LL_GPIO_AF2_TIM1          LL_GPIO_AF_2   /* !< TIM1 Alternate Function mapping */
#define LL_GPIO_AF2_TIM14         LL_GPIO_AF_2   /* !< TIM14 Alternate Function mapping */
#define LL_GPIO_AF2_TIM16         LL_GPIO_AF_2   /* !< TIM16 Alternate Function mapping */
#define LL_GPIO_AF2_TIM17         LL_GPIO_AF_2   /* !< TIM17 Alternate Function mapping */

/* @brief   AF 3 selection */
#define LL_GPIO_AF3_LED           LL_GPIO_AF_3   /* !< AF3: LED Alternate Function mapping */
#define LL_GPIO_AF3_USART1        LL_GPIO_AF_3   /* !< AF3: USART1 Alternate Function mapping */
#define LL_GPIO_AF3_USART2        LL_GPIO_AF_3   /* !< AF3: USART2 Alternate Function mapping */
#define LL_GPIO_AF3_SPI2          LL_GPIO_AF_3   /* !< AF3: SPI2 Alternate Function mapping */

/* @brief   AF 4 selection */
#define LL_GPIO_AF4_TIM14         LL_GPIO_AF_4   /* !< TIM14 Alternate Function mapping */
#define LL_GPIO_AF4_USART2        LL_GPIO_AF_4   /* !< USART2 Alternate Function mapping */

/* @brief   AF 5 selection */
#define LL_GPIO_AF5_LPTIM         LL_GPIO_AF_5   /* !< LPTIM1 Alternate Function mapping */
#define LL_GPIO_AF5_USART2        LL_GPIO_AF_5   /* !< USART2 Alternate Function mapping */
#define LL_GPIO_AF5_EVENTOUT      LL_GPIO_AF_5   /* !< EVENTOUT Alternate Function mapping */
#define LL_GPIO_AF5_TIM16         LL_GPIO_AF_5   /* !< TIM16 Alternate Function mapping */
#define LL_GPIO_AF5_TIM17         LL_GPIO_AF_5   /* !< TIM17 Alternate Function mapping */
#define LL_GPIO_AF5_MCO           LL_GPIO_AF_5   /* !< MCO Alternate Function mapping */

/* @brief   AF 6 selection */
#define LL_GPIO_AF6_I2C           LL_GPIO_AF_6   /* !< I2C1 Alternate Function mapping */
#define LL_GPIO_AF6_LED           LL_GPIO_AF_6   /* !< AF6: LED Alternate Function mapping */
#define LL_GPIO_AF6_MCO           LL_GPIO_AF_6   /* !< MCO Alternate Function mapping */
#define LL_GPIO_AF6_EVENTOUT      LL_GPIO_AF_6   /* !< AF6: EVENTOUT Alternate Function mapping */

/* @brief   AF 7 selection */
#define LL_GPIO_AF7_EVENTOUT      LL_GPIO_AF_7   /* !< EVENTOUT Alternate Function mapping */
#define LL_GPIO_AF7_COMP1         LL_GPIO_AF_7   /* !< AF7: COMP1 Alternate Function mapping */
#define LL_GPIO_AF7_COMP2         LL_GPIO_AF_7   /* !< AF7: COMP2 Alternate Function mapping */

/* @brief   AF 8 selection */
#define LL_GPIO_AF8_USART1        LL_GPIO_AF_8   /* !< AF8: USART1 Alternate Function mapping */

/* @brief   AF 9 selection */
#define LL_GPIO_AF9_USART2        LL_GPIO_AF_9   /* !< USART2 Alternate Function mapping */

/* @brief   AF 10 selection */
#define LL_GPIO_AF10_SPI1         LL_GPIO_AF_10  /* !< SPI2 Alternate Function mapping */

/* @brief   AF 11 selection */
#define LL_GPIO_AF11_SPI2         LL_GPIO_AF_11  /* !< SPI2 Alternate Function mapping */

/* @brief   AF 12 selection */
#define LL_GPIO_AF12_I2C          LL_GPIO_AF_12  /* !< I2C1 Alternate Function mapping */

/* @brief   AF 13 selection */
#define LL_GPIO_AF13_TIM1         LL_GPIO_AF_13  /* !< TIM1 Alternate Function mapping */
#define LL_GPIO_AF13_TIM3         LL_GPIO_AF_13  /* !< TIM3 Alternate Function mapping */
#define LL_GPIO_AF13_TIM14        LL_GPIO_AF_13  /* !< TIM14 Alternate Function mapping */
#define LL_GPIO_AF13_TIM17        LL_GPIO_AF_13  /* !< TIM17 Alternate Function mapping */

/* @brief   AF 14 selection */
#define LL_GPIO_AF14_TIM1         LL_GPIO_AF_14  /* !< TIM1 Alternate Function mapping */

/* @brief   AF 15 selection */
#define LL_GPIO_AF15_RTCOUT       LL_GPIO_AF_15
#define LL_GPIO_AF15_MCO          LL_GPIO_AF_15  /* !< MCO Alternate Function mapping */
#define LL_GPIO_AF15_IR           LL_GPIO_AF_15  /* !< IR Alternate Function mapping */

/* @} */

/* @} */

/* Exported macro ------------------------------------------------------------ */
/* @defgroup GPIO_LL_Exported_Macros GPIO Exported Macros @{ */

/* @defgroup GPIO_LL_EM_WRITE_READ Common Write and read registers Macros @{ */



#define LL_GPIO_WriteReg(__INSTANCE__, __REG__, __VALUE__) WRITE_REG(__INSTANCE__->__REG__, (__VALUE__))

/* @brief  Read a value in GPIO register @param  __INSTANCE__ GPIO Instance @param  __REG__ Register to be read @retval Register value */
#define LL_GPIO_ReadReg(__INSTANCE__, __REG__) READ_REG(__INSTANCE__->__REG__)
/* @} */

/* @} */

/* Exported functions -------------------------------------------------------- */
/* @defgroup GPIO_LL_Exported_Functions GPIO Exported Functions @{ */

/* @defgroup GPIO_LL_EF_Port_Configuration Port Configuration @{ */



__STATIC_INLINE void LL_GPIO_SetPinMode(GPIO_TypeDef *GPIOx, uint32_t Pin, uint32_t Mode)
{
  MODIFY_REG(GPIOx->MODER, ((Pin * Pin) * GPIO_MODER_MODE0), ((Pin * Pin) * Mode));
}



__STATIC_INLINE uint32_t LL_GPIO_GetPinMode(GPIO_TypeDef *GPIOx, uint32_t Pin)
{
  return (uint32_t)(READ_BIT(GPIOx->MODER, ((Pin * Pin) * GPIO_MODER_MODE0)) / (Pin * Pin));
}



__STATIC_INLINE void LL_GPIO_SetPinOutputType(GPIO_TypeDef *GPIOx, uint32_t PinMask, uint32_t OutputType)
{
  MODIFY_REG(GPIOx->OTYPER, PinMask, (PinMask * OutputType));
}



__STATIC_INLINE uint32_t LL_GPIO_GetPinOutputType(GPIO_TypeDef *GPIOx, uint32_t Pin)
{
  return (uint32_t)(READ_BIT(GPIOx->OTYPER, Pin) / Pin);
}



__STATIC_INLINE void LL_GPIO_SetPinSpeed(GPIO_TypeDef *GPIOx, uint32_t Pin, uint32_t  Speed)
{
  MODIFY_REG(GPIOx->OSPEEDR, ((Pin * Pin) * GPIO_OSPEEDR_OSPEED0), ((Pin * Pin) * Speed));
}



__STATIC_INLINE uint32_t LL_GPIO_GetPinSpeed(GPIO_TypeDef *GPIOx, uint32_t Pin)
{
  return (uint32_t)(READ_BIT(GPIOx->OSPEEDR, ((Pin * Pin) * GPIO_OSPEEDR_OSPEED0)) / (Pin * Pin));
}



__STATIC_INLINE void LL_GPIO_SetPinPull(GPIO_TypeDef *GPIOx, uint32_t Pin, uint32_t Pull)
{
  MODIFY_REG(GPIOx->PUPDR, ((Pin * Pin) * GPIO_PUPDR_PUPD0), ((Pin * Pin) * Pull));
}



__STATIC_INLINE uint32_t LL_GPIO_GetPinPull(GPIO_TypeDef *GPIOx, uint32_t Pin)
{
  return (uint32_t)(READ_BIT(GPIOx->PUPDR, ((Pin * Pin) * GPIO_PUPDR_PUPD0)) / (Pin * Pin));
}



__STATIC_INLINE void LL_GPIO_SetAFPin_0_7(GPIO_TypeDef *GPIOx, uint32_t Pin, uint32_t Alternate)
{
  MODIFY_REG(GPIOx->AFR[0], ((((Pin * Pin) * Pin) * Pin) * GPIO_AFRL_AFSEL0),
             ((((Pin * Pin) * Pin) * Pin) * Alternate));
}



__STATIC_INLINE uint32_t LL_GPIO_GetAFPin_0_7(GPIO_TypeDef *GPIOx, uint32_t Pin)
{
  return (uint32_t)(READ_BIT(GPIOx->AFR[0],
                             ((((Pin * Pin) * Pin) * Pin) * GPIO_AFRL_AFSEL0)) / (((Pin * Pin) * Pin) * Pin));
}



__STATIC_INLINE void LL_GPIO_SetAFPin_8_15(GPIO_TypeDef *GPIOx, uint32_t Pin, uint32_t Alternate)
{
  MODIFY_REG(GPIOx->AFR[1], (((((Pin >> 8U) * (Pin >> 8U)) * (Pin >> 8U)) * (Pin >> 8U)) * GPIO_AFRH_AFSEL8),
             (((((Pin >> 8U) * (Pin >> 8U)) * (Pin >> 8U)) * (Pin >> 8U)) * Alternate));
}



__STATIC_INLINE uint32_t LL_GPIO_GetAFPin_8_15(GPIO_TypeDef *GPIOx, uint32_t Pin)
{
  return (uint32_t)(READ_BIT(GPIOx->AFR[1],
                             (((((Pin >> 8U) * (Pin >> 8U)) * (Pin >> 8U)) * (Pin >> 8U)) * GPIO_AFRH_AFSEL8)) /
                             ((((Pin >> 8U) * (Pin >> 8U)) * (Pin >> 8U)) * (Pin >> 8U)));
}



__STATIC_INLINE void LL_GPIO_LockPin(GPIO_TypeDef *GPIOx, uint32_t PinMask)
{
  __IO uint32_t temp;
  WRITE_REG(GPIOx->LCKR, GPIO_LCKR_LCKK | PinMask);
  WRITE_REG(GPIOx->LCKR, PinMask);
  WRITE_REG(GPIOx->LCKR, GPIO_LCKR_LCKK | PinMask);
        /* Read LCKK register. This read is mandatory to complete key lock sequence */
  temp = READ_REG(GPIOx->LCKR);
  (void) temp;
}



__STATIC_INLINE uint32_t LL_GPIO_IsPinLocked(GPIO_TypeDef *GPIOx, uint32_t PinMask)
{
  return (READ_BIT(GPIOx->LCKR, PinMask) == (PinMask));
}



__STATIC_INLINE uint32_t LL_GPIO_IsAnyPinLocked(GPIO_TypeDef *GPIOx)
{
  return (READ_BIT(GPIOx->LCKR, LL_GPIO_LCKR_LCKK) != 0U);
}

/* @} */

/* @defgroup GPIO_LL_EF_Data_Access Data Access @{ */



__STATIC_INLINE uint32_t LL_GPIO_ReadInputPort(GPIO_TypeDef *GPIOx)
{
  return (uint32_t)(READ_REG(GPIOx->IDR));
}



__STATIC_INLINE uint32_t LL_GPIO_IsInputPinSet(GPIO_TypeDef *GPIOx, uint32_t PinMask)
{
  return (READ_BIT(GPIOx->IDR, PinMask) == (PinMask));
}



__STATIC_INLINE void LL_GPIO_WriteOutputPort(GPIO_TypeDef *GPIOx, uint32_t PortValue)
{
  WRITE_REG(GPIOx->ODR, PortValue);
}



__STATIC_INLINE uint32_t LL_GPIO_ReadOutputPort(GPIO_TypeDef *GPIOx)
{
  return (uint32_t)(READ_REG(GPIOx->ODR));
}



__STATIC_INLINE uint32_t LL_GPIO_IsOutputPinSet(GPIO_TypeDef *GPIOx, uint32_t PinMask)
{
  return (READ_BIT(GPIOx->ODR, PinMask) == (PinMask));
}



__STATIC_INLINE void LL_GPIO_SetOutputPin(GPIO_TypeDef *GPIOx, uint32_t PinMask)
{
  WRITE_REG(GPIOx->BSRR, PinMask);
}



__STATIC_INLINE void LL_GPIO_ResetOutputPin(GPIO_TypeDef *GPIOx, uint32_t PinMask)
{
  WRITE_REG(GPIOx->BRR, PinMask);
}



__STATIC_INLINE void LL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint32_t PinMask)
{
  WRITE_REG(GPIOx->ODR, READ_REG(GPIOx->ODR) ^ PinMask);
}

/* @} */

#if defined(USE_FULL_LL_DRIVER)
/* @defgroup GPIO_LL_EF_Init Initialization and de-initialization functions @{ */

ErrorStatus LL_GPIO_DeInit(GPIO_TypeDef *GPIOx);
ErrorStatus LL_GPIO_Init(GPIO_TypeDef *GPIOx, LL_GPIO_InitTypeDef *GPIO_InitStruct);
void        LL_GPIO_StructInit(LL_GPIO_InitTypeDef *GPIO_InitStruct);

/* @} */
#endif /* USE_FULL_LL_DRIVER */

/* @} */

/* @} */

#endif /* defined (GPIOA) || defined (GPIOB) || defined (GPIOF) */
/* @} */

#ifdef __cplusplus
}
#endif

#endif /* __PY32F0xx_LL_GPIO_H */

/* (C) COPYRIGHT Puya *****END OF FILE */
