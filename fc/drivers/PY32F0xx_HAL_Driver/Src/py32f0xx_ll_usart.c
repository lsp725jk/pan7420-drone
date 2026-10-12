/**
 * @file    py32f0xx_ll_usart.c
 * @author  MCU Application Team
 * @brief   USART LL module driver.
 * @attention
 */
#if defined(USE_FULL_LL_DRIVER)

/* Includes ------------------------------------------------------------------ */
#include "py32f0xx_ll_usart.h"
#include "py32f0xx_ll_rcc.h"
#include "py32f0xx_ll_bus.h"
#ifdef  USE_FULL_ASSERT
  #include "py32_assert.h"
#else
  #define assert_param(expr) ((void)0U)
#endif

/* @addtogroup PY32F0xx_LL_Driver @{ */

#if defined (USART1) || defined (USART2)

/* @addtogroup USART_LL @{ */

/* Private types ------------------------------------------------------------- */
/* Private variables --------------------------------------------------------- */
/* Private constants --------------------------------------------------------- */
/* @addtogroup USART_LL_Private_Constants @{ */

/* @} */



/* Private macros ------------------------------------------------------------ */
/* @addtogroup USART_LL_Private_Macros @{ */



#define IS_LL_USART_BAUDRATE(__BAUDRATE__) ((__BAUDRATE__) <= 4500000U)



#define IS_LL_USART_BRR_MIN(__VALUE__) ((__VALUE__) >= 16U)

/* __VALUE__ BRR content must be lower than or equal to 0xFFFF. */
#define IS_LL_USART_BRR_MAX(__VALUE__) ((__VALUE__) <= 0x0000FFFFU)

#define IS_LL_USART_DIRECTION(__VALUE__) (((__VALUE__) == LL_USART_DIRECTION_NONE) \
                                       || ((__VALUE__) == LL_USART_DIRECTION_RX) \
                                       || ((__VALUE__) == LL_USART_DIRECTION_TX) \
                                       || ((__VALUE__) == LL_USART_DIRECTION_TX_RX))

#define IS_LL_USART_PARITY(__VALUE__) (((__VALUE__) == LL_USART_PARITY_NONE) \
                                    || ((__VALUE__) == LL_USART_PARITY_EVEN) \
                                    || ((__VALUE__) == LL_USART_PARITY_ODD))

#define IS_LL_USART_DATAWIDTH(__VALUE__) (((__VALUE__) == LL_USART_DATAWIDTH_8B) \
                                       || ((__VALUE__) == LL_USART_DATAWIDTH_9B))

#define IS_LL_USART_OVERSAMPLING(__VALUE__) (((__VALUE__) == LL_USART_OVERSAMPLING_16) \
                                          || ((__VALUE__) == LL_USART_OVERSAMPLING_8))

#define IS_LL_USART_LASTBITCLKOUTPUT(__VALUE__) (((__VALUE__) == LL_USART_LASTCLKPULSE_NO_OUTPUT) \
                                              || ((__VALUE__) == LL_USART_LASTCLKPULSE_OUTPUT))

#define IS_LL_USART_CLOCKPHASE(__VALUE__) (((__VALUE__) == LL_USART_PHASE_1EDGE) \
                                        || ((__VALUE__) == LL_USART_PHASE_2EDGE))

#define IS_LL_USART_CLOCKPOLARITY(__VALUE__) (((__VALUE__) == LL_USART_POLARITY_LOW) \
                                           || ((__VALUE__) == LL_USART_POLARITY_HIGH))

#define IS_LL_USART_CLOCKOUTPUT(__VALUE__) (((__VALUE__) == LL_USART_CLOCK_DISABLE) \
                                         || ((__VALUE__) == LL_USART_CLOCK_ENABLE))

#define IS_LL_USART_STOPBITS(__VALUE__) (((__VALUE__) == LL_USART_STOPBITS_1) \
                                      || ((__VALUE__) == LL_USART_STOPBITS_2))

#define IS_LL_USART_HWCONTROL(__VALUE__) (((__VALUE__) == LL_USART_HWCONTROL_NONE) \
                                       || ((__VALUE__) == LL_USART_HWCONTROL_RTS) \
                                       || ((__VALUE__) == LL_USART_HWCONTROL_CTS) \
                                       || ((__VALUE__) == LL_USART_HWCONTROL_RTS_CTS))

/* @} */

/* Private function prototypes ----------------------------------------------- */

/* Exported functions -------------------------------------------------------- */
/* @addtogroup USART_LL_Exported_Functions @{ */

/* @addtogroup USART_LL_EF_Init @{ */



ErrorStatus LL_USART_DeInit(USART_TypeDef *USARTx)
{
  ErrorStatus status = SUCCESS;

        /* Check the parameters */
  assert_param(IS_UART_INSTANCE(USARTx));

  if (USARTx == USART1)
  {
                /* Force reset of USART clock */
    LL_APB1_GRP2_ForceReset(LL_APB1_GRP2_PERIPH_USART1);

                /* Release reset of USART clock */
    LL_APB1_GRP2_ReleaseReset(LL_APB1_GRP2_PERIPH_USART1);
  }
#if defined(USART2)
  else if (USARTx == USART2)
  {
                /* Force reset of USART clock */
    LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_USART2);

                /* Release reset of USART clock */
    LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_USART2);
  }
#endif /* USART2 */
  else
  {
    status = ERROR;
  }

  return (status);
}



ErrorStatus LL_USART_Init(USART_TypeDef *USARTx, LL_USART_InitTypeDef *USART_InitStruct)
{
  ErrorStatus status = ERROR;
  uint32_t periphclk = LL_RCC_PERIPH_FREQUENCY_NO;
  LL_RCC_ClocksTypeDef rcc_clocks;

        /* Check the parameters */
  assert_param(IS_UART_INSTANCE(USARTx));
  assert_param(IS_LL_USART_BAUDRATE(USART_InitStruct->BaudRate));
  assert_param(IS_LL_USART_DATAWIDTH(USART_InitStruct->DataWidth));
  assert_param(IS_LL_USART_STOPBITS(USART_InitStruct->StopBits));
  assert_param(IS_LL_USART_PARITY(USART_InitStruct->Parity));
  assert_param(IS_LL_USART_DIRECTION(USART_InitStruct->TransferDirection));
  assert_param(IS_LL_USART_HWCONTROL(USART_InitStruct->HardwareFlowControl));
#if defined(USART_CR3_OVER8)
  assert_param(IS_LL_USART_OVERSAMPLING(USART_InitStruct->OverSampling));
#endif /* USART_OverSampling_Feature */



  if (LL_USART_IsEnabled(USARTx) == 0U)
  {

#if defined(USART_CR3_OVER8)
    MODIFY_REG(USARTx->CR1,
               (USART_CR1_M | USART_CR1_PCE | USART_CR1_PS |
                    USART_CR1_TE | USART_CR1_RE),
               (USART_InitStruct->DataWidth | USART_InitStruct->Parity |
                USART_InitStruct->TransferDirection));
        MODIFY_REG(USARTx->CR3, USART_CR3_OVER8, USART_InitStruct->OverSampling);
#else
        MODIFY_REG(USARTx->CR1,
               (USART_CR1_M | USART_CR1_PCE | USART_CR1_PS |
                USART_CR1_TE | USART_CR1_RE),
               (USART_InitStruct->DataWidth | USART_InitStruct->Parity |
                USART_InitStruct->TransferDirection));
#endif /* USART_OverSampling_Feature */
                /* Configure USARTx CR2 (Stop bits) with parameters: */
    LL_USART_SetStopBitsLength(USARTx, USART_InitStruct->StopBits);



    LL_USART_SetHWFlowCtrl(USARTx, USART_InitStruct->HardwareFlowControl);



    LL_RCC_GetSystemClocksFreq(&rcc_clocks);
    periphclk = rcc_clocks.PCLK1_Frequency;

                /* Configure the USART Baud Rate : - valid baud rate value (different from 0) is required */
    if ((periphclk != LL_RCC_PERIPH_FREQUENCY_NO)
        && (USART_InitStruct->BaudRate != 0U))
    {
      status = SUCCESS;
#if defined(USART_CR3_OVER8)
            LL_USART_SetBaudRate(USARTx,
                           periphclk,
                           USART_InitStruct->OverSampling,
                           USART_InitStruct->BaudRate);
#else
      LL_USART_SetBaudRate(USARTx,
                           periphclk,
                           USART_InitStruct->BaudRate);
#endif /* USART_OverSampling_Feature */
                        /* Check BRR is greater than or equal to 16d */
      assert_param(IS_LL_USART_BRR_MIN(USARTx->BRR));

                        /* Check BRR is greater than or equal to 16d */
      assert_param(IS_LL_USART_BRR_MAX(USARTx->BRR));
    }
  }
        /* Endif (=> USART not in Disabled state => return ERROR) */

  return (status);
}



void LL_USART_StructInit(LL_USART_InitTypeDef *USART_InitStruct)
{
        /* Set USART_InitStruct fields to default values */
  USART_InitStruct->BaudRate            = 9600U;
  USART_InitStruct->DataWidth           = LL_USART_DATAWIDTH_8B;
  USART_InitStruct->StopBits            = LL_USART_STOPBITS_1;
  USART_InitStruct->Parity              = LL_USART_PARITY_NONE ;
  USART_InitStruct->TransferDirection   = LL_USART_DIRECTION_TX_RX;
  USART_InitStruct->HardwareFlowControl = LL_USART_HWCONTROL_NONE;
#if defined(USART_CR3_OVER8)
  USART_InitStruct->OverSampling        = LL_USART_OVERSAMPLING_16;
#endif /* USART_OverSampling_Feature */
}



ErrorStatus LL_USART_ClockInit(USART_TypeDef *USARTx, LL_USART_ClockInitTypeDef *USART_ClockInitStruct)
{
  ErrorStatus status = SUCCESS;

        /* Check USART Instance and Clock signal output parameters */
  assert_param(IS_UART_INSTANCE(USARTx));
  assert_param(IS_LL_USART_CLOCKOUTPUT(USART_ClockInitStruct->ClockOutput));



  if (LL_USART_IsEnabled(USARTx) == 0U)
  {
                /* ---------------------------- USART CR2 Configuration ----------------------- */
                /* If Clock signal has to be output */
    if (USART_ClockInitStruct->ClockOutput == LL_USART_CLOCK_DISABLE)
    {
                        /* Deactivate Clock signal delivery : - Disable Clock Output:        USART_CR2_CLKEN cleared */
      LL_USART_DisableSCLKOutput(USARTx);
    }
    else
    {
                        /* Ensure USART instance is USART capable */
      assert_param(IS_USART_INSTANCE(USARTx));

                        /* Check clock related parameters */
      assert_param(IS_LL_USART_CLOCKPOLARITY(USART_ClockInitStruct->ClockPolarity));
      assert_param(IS_LL_USART_CLOCKPHASE(USART_ClockInitStruct->ClockPhase));
      assert_param(IS_LL_USART_LASTBITCLKOUTPUT(USART_ClockInitStruct->LastBitClockPulse));

                        /* - Enable Clock Output:         USART_CR2_CLKEN set */
      MODIFY_REG(USARTx->CR2,
                 USART_CR2_CLKEN | USART_CR2_CPHA | USART_CR2_CPOL | USART_CR2_LBCL,
                 USART_CR2_CLKEN | USART_ClockInitStruct->ClockPolarity |
                 USART_ClockInitStruct->ClockPhase | USART_ClockInitStruct->LastBitClockPulse);
    }
  }
        /* Else (USART not in Disabled state => return ERROR */
  else
  {
    status = ERROR;
  }

  return (status);
}



void LL_USART_ClockStructInit(LL_USART_ClockInitTypeDef *USART_ClockInitStruct)
{
        /* Set LL_USART_ClockInitStruct fields with default values */
  USART_ClockInitStruct->ClockOutput       = LL_USART_CLOCK_DISABLE;
  USART_ClockInitStruct->ClockPolarity     = LL_USART_POLARITY_LOW;                /* Not relevant when ClockOutput = LL_USART_CLOCK_DISABLE */
  USART_ClockInitStruct->ClockPhase        = LL_USART_PHASE_1EDGE;                 /* Not relevant when ClockOutput = LL_USART_CLOCK_DISABLE */
  USART_ClockInitStruct->LastBitClockPulse = LL_USART_LASTCLKPULSE_NO_OUTPUT;      /* Not relevant when ClockOutput = LL_USART_CLOCK_DISABLE */
}

/* @} */

/* @} */

/* @} */

#endif /* USART1 || USART2 || USART3 || UART4 || UART5 */

/* @} */

#endif /* USE_FULL_LL_DRIVER */

/* (C) COPYRIGHT Puya *****END OF FILE */

