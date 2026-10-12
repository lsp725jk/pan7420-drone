/**
 * @file    py32f0xx_ll_utils.c
 * @author  MCU Application Team
 * @brief   UTILS LL module driver.
 * @attention
 */
/* Includes ------------------------------------------------------------------ */
#include "py32f0xx_ll_utils.h"
#include "py32f0xx_ll_rcc.h"
#include "py32f0xx_ll_system.h"
#ifdef  USE_FULL_ASSERT
  #include "py32_assert.h"
#else
  #define assert_param(expr) ((void)0U)
#endif /* USE_FULL_ASSERT */

/* @addtogroup PY32F0xx_LL_Driver @{ */

/* @addtogroup UTILS_LL @{ */

/* Private types ------------------------------------------------------------- */
/* Private variables --------------------------------------------------------- */
/* Private constants --------------------------------------------------------- */
/* @addtogroup UTILS_LL_Private_Constants @{ */
#if defined(RCC_PLL_SUPPORT)
/* Defines used for HSE range */
#define UTILS_HSE_FREQUENCY_MIN      4000000U      /* !< Frequency min for HSE frequency, in Hz */
#define UTILS_HSE_FREQUENCY_MAX      32000000U     /* !< Frequency max for HSE frequency, in Hz */

/* Defines used for PLL input range */
#define LL_RCC_PLLINPUT_FREQ_MIN     16000000U     /* !< Frequency min for PLL input frequency, in Hz */
#define LL_RCC_PLLINPUT_FREQ_MAX     24000000U     /* !< Frequency max for PLL input frequency, in Hz */
#endif

/* Defines used for FLASH latency according to HCLK Frequency */
#define UTILS_SCALE1_LATENCY1_FREQ  24000000U       /* !< HCLK frequency to set FLASH latency 1 in power scale 1 */
#define UTILS_SCALE1_LATENCY2_FREQ  48000000U       /* !< HCLK frequency to set FLASH latency 2 in power scale 1 */

/* @} */

/* Private macros ------------------------------------------------------------ */
/* @addtogroup UTILS_LL_Private_Macros @{ */
#define IS_LL_UTILS_SYSCLK_DIV(__VALUE__) (((__VALUE__) == LL_RCC_SYSCLK_DIV_1)   \
                                        || ((__VALUE__) == LL_RCC_SYSCLK_DIV_2)   \
                                        || ((__VALUE__) == LL_RCC_SYSCLK_DIV_4)   \
                                        || ((__VALUE__) == LL_RCC_SYSCLK_DIV_8)   \
                                        || ((__VALUE__) == LL_RCC_SYSCLK_DIV_16)  \
                                        || ((__VALUE__) == LL_RCC_SYSCLK_DIV_64)  \
                                        || ((__VALUE__) == LL_RCC_SYSCLK_DIV_128) \
                                        || ((__VALUE__) == LL_RCC_SYSCLK_DIV_256) \
                                        || ((__VALUE__) == LL_RCC_SYSCLK_DIV_512))

#define IS_LL_UTILS_APB1_DIV(__VALUE__) (((__VALUE__) == LL_RCC_APB1_DIV_1) \
                                      || ((__VALUE__) == LL_RCC_APB1_DIV_2) \
                                      || ((__VALUE__) == LL_RCC_APB1_DIV_4) \
                                      || ((__VALUE__) == LL_RCC_APB1_DIV_8) \
                                      || ((__VALUE__) == LL_RCC_APB1_DIV_16))



#define IS_LL_UTILS_HSE_BYPASS(__STATE__) (((__STATE__) == LL_UTILS_HSEBYPASS_ON) \
                                        || ((__STATE__) == LL_UTILS_HSEBYPASS_OFF))

#if defined(RCC_PLL_SUPPORT)
#define IS_LL_UTILS_HSE_FREQUENCY(__FREQUENCY__) (((__FREQUENCY__) >= UTILS_HSE_FREQUENCY_MIN) && ((__FREQUENCY__) <= UTILS_HSE_FREQUENCY_MAX))

#define IS_LL_UTILS_PLL_INPUT_FREQUENCY(__FREQUENCY__) (((__FREQUENCY__) >= LL_RCC_PLLINPUT_FREQ_MIN) && ((__FREQUENCY__) <= LL_RCC_PLLINPUT_FREQ_MAX))
#endif

/* @} */
#if defined(RCC_PLL_SUPPORT)
        /* Private function prototypes ----------------------------------------------- */
        /* @defgroup UTILS_LL_Private_Functions UTILS Private functions @{ */
  static ErrorStatus UTILS_EnablePLLAndSwitchSystem(uint32_t SYSCLK_Frequency, LL_UTILS_ClkInitTypeDef *UTILS_ClkInitStruct);
  static ErrorStatus UTILS_PLL_IsBusy(void);
        /* @} */
#endif
/* Exported functions -------------------------------------------------------- */
/* @addtogroup UTILS_LL_Exported_Functions @{ */

/* @addtogroup UTILS_LL_EF_DELAY @{ */



void LL_Init1msTick(uint32_t HCLKFrequency)
{
        /* Use frequency provided in argument */
  LL_InitTick(HCLKFrequency, 1000U);
}



void LL_mDelay(uint32_t Delay)
{
  __IO uint32_t  tmp = SysTick->CTRL;      /* Clear the COUNTFLAG first */
  uint32_t tmpDelay;     /* MISRAC2012-Rule-17.8 */
        /* Add this code to indicate that local variable is not used */
  ((void)tmp);
  tmpDelay  = Delay;
        /* Add a period to guaranty minimum wait */
  if (tmpDelay  < LL_MAX_DELAY)
  {
    tmpDelay ++;
  }

  while (tmpDelay  != 0U)
  {
    if ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) != 0U)
    {
      tmpDelay --;
    }
  }
}

/* @} */

/* @addtogroup UTILS_EF_SYSTEM @brief    System Configuration functions @verbatim ##### System Configuration functions ##### */



void LL_SetSystemCoreClock(uint32_t HCLKFrequency)
{
        /* HCLK clock frequency */
  SystemCoreClock = HCLKFrequency;
}
#if defined(RCC_PLL_SUPPORT)

ErrorStatus LL_PLL_ConfigSystemClock_HSI(LL_UTILS_ClkInitTypeDef *UTILS_ClkInitStruct)
{
  ErrorStatus status;
  uint32_t pllfreq;

        /* Check if one of the PLL is enabled */
  if (UTILS_PLL_IsBusy() == SUCCESS)
  {
                /* Check if the new PLL input frequency is correct */
    if (!IS_LL_UTILS_PLL_INPUT_FREQUENCY(LL_RCC_HSI_GetFreq()))
    {
                        /* the new PLL input frequency is error */
      return ERROR;
    }

                /* Calculate the new PLL output frequency */
    pllfreq = 2 * LL_RCC_HSI_GetFreq();

                /* Enable HSI if not enabled */
    if (LL_RCC_HSI_IsReady() != 1U)
    {
      LL_RCC_HSI_Enable();
      while (LL_RCC_HSI_IsReady() != 1U)
      {
                                /* Wait for HSI ready */
      }
    }

                /* Configure PLL */
    LL_RCC_PLL_SetMainSource(LL_RCC_PLLSOURCE_HSI);

                /* Enable PLL and switch system clock to PLL */
    status = UTILS_EnablePLLAndSwitchSystem(pllfreq, UTILS_ClkInitStruct);
  }
  else
  {
                /* Current PLL configuration cannot be modified */
    status = ERROR;
  }

  return status;
}



ErrorStatus LL_PLL_ConfigSystemClock_HSE(uint32_t HSEFrequency, uint32_t HSEBypass, LL_UTILS_ClkInitTypeDef *UTILS_ClkInitStruct)
{
  ErrorStatus status;
  uint32_t pllfreq;

        /* Check the parameters */
  assert_param(IS_LL_UTILS_HSE_FREQUENCY(HSEFrequency));
  assert_param(IS_LL_UTILS_HSE_BYPASS(HSEBypass));

        /* Check if one of the PLL is enabled */
  if (UTILS_PLL_IsBusy() == SUCCESS)
  {
                /* Check if the new PLL input frequency is correct */
    if (!IS_LL_UTILS_PLL_INPUT_FREQUENCY(HSEFrequency))
    {
                        /* the new PLL input frequency is error */
      return ERROR;
    }

                /* Calculate the new PLL output frequency */
    pllfreq = HSEFrequency * 2;

                /* Enable HSE if not enabled */
    if (LL_RCC_HSE_IsReady() != 1U)
    {
                        /* Set frequency range of the HSE */
      LL_RCC_HSE_SetFreqRegion(LL_RCC_HSE_16_32MHz);

                        /* Check if need to enable HSE bypass feature or not */
      if (HSEBypass == LL_UTILS_HSEBYPASS_ON)
      {
        LL_RCC_HSE_EnableBypass();
      }
      else
      {
        LL_RCC_HSE_DisableBypass();
      }

                        /* Enable HSE */
      LL_RCC_HSE_Enable();
      while (LL_RCC_HSE_IsReady() != 1U)
      {
                                /* Wait for HSE ready */
      }
    }

                /* Configure PLL */
    LL_RCC_PLL_SetMainSource(LL_RCC_PLLSOURCE_HSE);

                /* Enable PLL and switch system clock to PLL */
    status = UTILS_EnablePLLAndSwitchSystem(pllfreq, UTILS_ClkInitStruct);
  }
  else
  {
                /* Current PLL configuration cannot be modified */
    status = ERROR;
  }

  return status;
}
#endif



ErrorStatus LL_SetFlashLatency(uint32_t HCLKFrequency)
{
  uint32_t timeout;
  uint32_t getlatency;
  uint32_t latency;
  ErrorStatus status;

        /* Frequency cannot be equal to 0 or greater than max clock */
  if ((HCLKFrequency == 0U) || (HCLKFrequency > UTILS_SCALE1_LATENCY2_FREQ))
  {
    status = ERROR;
    return status;
  }
  else
  {
    if (HCLKFrequency > UTILS_SCALE1_LATENCY1_FREQ)
    {
                        /* 24 < HCLK <= 48 => 1WS (2 CPU cycles) */
      latency = LL_FLASH_LATENCY_1;
    }
    else
    {
                        /* else HCLKFrequency < 24MHz default LL_FLASH_LATENCY_0 0WS */
      latency = LL_FLASH_LATENCY_0;
    }
  }

  LL_FLASH_SetLatency(latency);



  timeout = 2u;
  do
  {
                /* Wait for Flash latency to be updated */
    getlatency = LL_FLASH_GetLatency();
    timeout--;
  }
  while ((getlatency != latency) && (timeout > 0u));

  if (getlatency != latency)
  {
    status = ERROR;
  }
  else
  {
    status = SUCCESS;
  }

  return status;
}

/* @} */

/* @} */
#if defined(RCC_PLL_SUPPORT)
/* @addtogroup UTILS_LL_Private_Functions @{ */



static ErrorStatus UTILS_PLL_IsBusy(void)
{
  ErrorStatus status = SUCCESS;

        /* Check if PLL is busy */
  if (LL_RCC_PLL_IsReady() != 0U)
  {
                /* PLL configuration cannot be modified */
    status = ERROR;
  }

  return status;
}



static ErrorStatus UTILS_EnablePLLAndSwitchSystem(uint32_t SYSCLK_Frequency, LL_UTILS_ClkInitTypeDef *UTILS_ClkInitStruct)
{
  ErrorStatus status = SUCCESS;
  uint32_t hclk_frequency;

  assert_param(IS_LL_UTILS_SYSCLK_DIV(UTILS_ClkInitStruct->AHBCLKDivider));
  assert_param(IS_LL_UTILS_APB1_DIV(UTILS_ClkInitStruct->APB1CLKDivider));

        /* Calculate HCLK frequency */
  hclk_frequency = __LL_RCC_CALC_HCLK_FREQ(SYSCLK_Frequency, UTILS_ClkInitStruct->AHBCLKDivider);

        /* Increasing the number of wait states because of higher CPU frequency */
  if (SystemCoreClock < hclk_frequency)
  {
                /* Set FLASH latency to highest latency */
    status = LL_SetFlashLatency(hclk_frequency);
  }

        /* Update system clock configuration */
  if (status == SUCCESS)
  {
                /* Enable PLL */
    LL_RCC_PLL_Enable();
    while (LL_RCC_PLL_IsReady() != 1U)
    {
                        /* Wait for PLL ready */
    }

                /* Sysclk activation on the main PLL */
    LL_RCC_SetAHBPrescaler(UTILS_ClkInitStruct->AHBCLKDivider);
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
    {
                        /* Wait for system clock switch to PLL */
    }

                /* Set APB1 prescaler */
    LL_RCC_SetAPB1Prescaler(UTILS_ClkInitStruct->APB1CLKDivider);
  }

        /* Decreasing the number of wait states because of lower CPU frequency */
  if (SystemCoreClock > hclk_frequency)
  {
                /* Set FLASH latency to lowest latency */
    status = LL_SetFlashLatency(hclk_frequency);
  }

        /* Update SystemCoreClock variable */
  if (status == SUCCESS)
  {
    LL_SetSystemCoreClock(hclk_frequency);
  }

  return status;
}
/* @} */
#endif
/* @} */

/* @} */

/* (C) COPYRIGHT Puya *****END OF FILE */
