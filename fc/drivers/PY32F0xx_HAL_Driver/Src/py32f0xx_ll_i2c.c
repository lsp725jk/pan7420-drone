/**
 * @file    py32f0xx_ll_i2c.c
 * @author  MCU Application Team
 * @brief   I2C LL module driver.
 * @attention
 */
#if defined(USE_FULL_LL_DRIVER)

/* Includes ------------------------------------------------------------------ */
#include "py32f0xx_ll_i2c.h"
#include "py32f0xx_ll_bus.h"
#include "py32f0xx_ll_rcc.h"
#ifdef  USE_FULL_ASSERT
  #include "py32_assert.h"
#else
  #define assert_param(expr) ((void)0U)
#endif

/* @addtogroup PY32F0xx_LL_Driver @{ */

#if defined (I2C1) || defined (I2C2)

/* @defgroup I2C_LL I2C @{ */

/* Private types ------------------------------------------------------------- */
/* Private variables --------------------------------------------------------- */
/* Private constants --------------------------------------------------------- */
/* Private macros ------------------------------------------------------------ */
/* @addtogroup I2C_LL_Private_Macros @{ */
#define IS_LL_I2C_CLOCK_SPEED(__VALUE__)           (((__VALUE__) > 0U) && ((__VALUE__) <= LL_I2C_MAX_SPEED_FAST))

#define IS_LL_I2C_DUTY_CYCLE(__VALUE__)            (((__VALUE__) == LL_I2C_DUTYCYCLE_2) || \
                                                 ((__VALUE__) == LL_I2C_DUTYCYCLE_16_9))

#define IS_LL_I2C_OWN_ADDRESS1(__VALUE__)       ((__VALUE__) <= 0x000003FFU)

#define IS_LL_I2C_TYPE_ACKNOWLEDGE(__VALUE__)   (((__VALUE__) == LL_I2C_ACK) || \
                                                 ((__VALUE__) == LL_I2C_NACK))
/* @} */

/* Private function prototypes ----------------------------------------------- */

/* Exported functions -------------------------------------------------------- */
/* @addtogroup I2C_LL_Exported_Functions @{ */

/* @addtogroup I2C_LL_EF_Init @{ */



uint32_t LL_I2C_DeInit(I2C_TypeDef *I2Cx)
{
  ErrorStatus status = SUCCESS;

        /* Check the I2C Instance I2Cx */
  assert_param(IS_I2C_ALL_INSTANCE(I2Cx));

  if (I2Cx == I2C1)
  {
                /* Force reset of I2C clock */
    LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_I2C1);

                /* Release reset of I2C clock */
    LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_I2C1);
  }
  else
  {
    status = ERROR;
  }

  return status;
}



uint32_t LL_I2C_Init(I2C_TypeDef *I2Cx, LL_I2C_InitTypeDef *I2C_InitStruct)
{
  LL_RCC_ClocksTypeDef rcc_clocks;

        /* Check the I2C Instance I2Cx */
  assert_param(IS_I2C_ALL_INSTANCE(I2Cx));

        /* Check the I2C parameters from I2C_InitStruct */
  assert_param(IS_LL_I2C_CLOCK_SPEED(I2C_InitStruct->ClockSpeed));
  assert_param(IS_LL_I2C_DUTY_CYCLE(I2C_InitStruct->DutyCycle));
  assert_param(IS_LL_I2C_OWN_ADDRESS1(I2C_InitStruct->OwnAddress1));
  assert_param(IS_LL_I2C_TYPE_ACKNOWLEDGE(I2C_InitStruct->TypeAcknowledge));

        /* Disable the selected I2Cx Peripheral */
  LL_I2C_Disable(I2Cx);

        /* Retrieve Clock frequencies */
  LL_RCC_GetSystemClocksFreq(&rcc_clocks);

        /* Configure the SCL speed : and I2C_CCR_CCR[11:0] bits - DutyCycle: I2C_CCR_DUTY[7:0] bits */
  LL_I2C_ConfigSpeed(I2Cx, rcc_clocks.PCLK1_Frequency, I2C_InitStruct->ClockSpeed, I2C_InitStruct->DutyCycle);

        /* - OwnAddrSize:  I2C_OAR1_ADDMODE bit */
  LL_I2C_SetOwnAddress1(I2Cx, I2C_InitStruct->OwnAddress1, 0);

        /* Enable the selected I2Cx Peripheral */
  LL_I2C_Enable(I2Cx);

        /* Configure the ACKnowledge or Non ACKnowledge condition - TypeAcknowledge: I2C_CR2_NACK bit */
  LL_I2C_AcknowledgeNextData(I2Cx, I2C_InitStruct->TypeAcknowledge);

  return SUCCESS;
}



void LL_I2C_StructInit(LL_I2C_InitTypeDef *I2C_InitStruct)
{
        /* Set I2C_InitStruct fields to default values */
  I2C_InitStruct->ClockSpeed      = 5000U;
  I2C_InitStruct->DutyCycle       = LL_I2C_DUTYCYCLE_2;
  I2C_InitStruct->OwnAddress1     = 0U;
  I2C_InitStruct->TypeAcknowledge = LL_I2C_NACK;
}

/* @} */

/* @} */

/* @} */

#endif /* I2C1 || I2C2 */

/* @} */

#endif /* USE_FULL_LL_DRIVER */

/* (C) COPYRIGHT Puya *****END OF FILE */
