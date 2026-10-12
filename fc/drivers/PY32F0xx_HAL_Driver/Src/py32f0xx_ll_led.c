/**
 * @file    py32f0xx_ll_led.c
 * @author  MCU Application Team
 * @brief   LED LL module driver.
 * @attention
 */
#if defined(USE_FULL_LL_DRIVER)

/* Includes ------------------------------------------------------------------ */
#include "py32f0xx_ll_led.h"
#include "py32f0xx_ll_bus.h"
#ifdef  USE_FULL_ASSERT
  #include "py32_assert.h"
#else
  #define assert_param(expr) ((void)0U)
#endif

/* @addtogroup PY32F0xx_LL_Driver @{ */

#if defined (LED)

/* @addtogroup LED_LL @{ */

/* Private types ------------------------------------------------------------- */
/* Private variables --------------------------------------------------------- */
/* Private constants --------------------------------------------------------- */
/* Private macros ------------------------------------------------------------ */

/* @defgroup LED_LL_Private_Macros LED Private Macros @{ */
#define IS_LL_LED_COM_DRIVE(__VALUE__)      (((__VALUE__) == LL_LED_COMDRIVE_LOW)     ||\
                                             ((__VALUE__) == LL_LED_COMDRIVE_HIGH))

#define IS_LL_LED_PRESCALER(__VALUE__)      (((0x00u) < (__VALUE__)) && ((__VALUE__) <= (0xFFu)))

#define IS_LL_LED_COM_SELECT(__VALUE__)     (((__VALUE__) == LL_LED_COMSELECT_1COM)     ||\
                                             ((__VALUE__) == LL_LED_COMSELECT_2COM)    ||\
                                             ((__VALUE__) == LL_LED_COMSELECT_3COM) ||\
                                             ((__VALUE__) == LL_LED_COMSELECT_4COM))

#define IS_LL_LED_LIGHT_TIME(__VALUE__)     (((0x01u) < (__VALUE__)) && ((__VALUE__) <= (0xFFu)))

#define IS_LL_LED_DEAD_TIME(__VALUE__)      (((0x01u) < (__VALUE__)) && ((__VALUE__) <= (0xFFu)))
/* @} */

/* Private function prototypes ----------------------------------------------- */

/* Exported functions -------------------------------------------------------- */
/* @addtogroup LED_LL_Exported_Functions @{ */

/* @addtogroup LED_LL_EF_Init @{ */



ErrorStatus LL_LED_DeInit(LED_TypeDef *LEDx)
{
  ErrorStatus status = SUCCESS;

        /* Check the parameters */
  assert_param(IS_LED_ALL_INSTANCE(LEDx));

        /* Force and Release reset on clock of LEDx */
  if (LEDx == LED)
  {
    LL_APB1_GRP2_ForceReset(LL_APB1_GRP2_PERIPH_LED);
    LL_APB1_GRP2_ReleaseReset(LL_APB1_GRP2_PERIPH_LED);
  }
  else
  {
    status = ERROR;
  }

  return (status);
}



ErrorStatus LL_LED_Init(LED_TypeDef *LEDx, LL_LED_InitTypeDef *LED_InitStruct)
{
        /* Check the parameters */
  assert_param(IS_LED_ALL_INSTANCE(LEDx));
  assert_param(IS_LL_LED_COM_DRIVE(LED_InitStruct->ComDrive));
  assert_param(IS_LL_LED_PRESCALER(LED_InitStruct->Prescaler));
  assert_param(IS_LL_LED_COM_SELECT(LED_InitStruct->ComSelect));
  assert_param(IS_LL_LED_LIGHT_TIME(LED_InitStruct->LightTime));
  assert_param(IS_LL_LED_DEAD_TIME(LED_InitStruct->DeadTime));
  
        /* LED Register config */
  MODIFY_REG(LEDx->CR, (uint32_t)(LED_CR_LED_COM_SEL | LED_CR_EHS),
             (LED_InitStruct->ComSelect | LED_InitStruct->ComDrive));
  LL_LED_SetPrescaler(LEDx, LED_InitStruct->Prescaler);
  LL_LED_SetLightAndDeadTime(LEDx, LED_InitStruct->LightTime, LED_InitStruct->DeadTime);
  LL_LED_Enable(LEDx);
  
  return (SUCCESS);
}



void LL_LED_StructInit(LL_LED_InitTypeDef *LED_InitStruct)
{
        /* Reset LED init structure parameters values */
  LED_InitStruct->ComDrive = LL_LED_COMDRIVE_LOW;
  LED_InitStruct->Prescaler = 0x0u;
  LED_InitStruct->ComSelect = LL_LED_COMSELECT_1COM;
  LED_InitStruct->LightTime = 0x0u;
  LED_InitStruct->DeadTime = 0x0u;
}



/* @} */

/* @} */

/* @} */

#endif /* defined (LED) */

/* @} */

#endif /* USE_FULL_LL_DRIVER */

/* (C) COPYRIGHT Puya Semiconductor *****END OF FILE */
