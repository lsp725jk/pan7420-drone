/**
 * @file    py32f0xx_hal_rcc.c
 * @author  MCU Application Team
 * @brief   RCC HAL module driver.
 */

/* Includes ------------------------------------------------------------------ */
#include "py32f0xx_hal.h"

/* @addtogroup PY32F0xx_HAL_Driver @{ */

/* @defgroup RCC RCC @brief RCC HAL module driver @{ */

#ifdef HAL_RCC_MODULE_ENABLED

/* Private typedef ----------------------------------------------------------- */
/* Private define ------------------------------------------------------------ */
/* @defgroup RCC_Private_Constants RCC Private Constants @{ */
#define CLOCKSWITCH_TIMEOUT_VALUE  (5000U) /* 5 s */
#define HSE_TIMEOUT_VALUE          HSE_STARTUP_TIMEOUT
#define HSI_TIMEOUT_VALUE          (2U)    /* 2 ms (minimum Tick + 1) */
#define LSI_TIMEOUT_VALUE          (2U)    /* 2 ms (minimum Tick + 1) */
#if defined(RCC_PLL_SUPPORT)
#define PLL_TIMEOUT_VALUE          (2U)    /* 2 ms (minimum Tick + 1) */

#define PLLSOURCE_NONE             (0U)
#define PLLSOURCE_FREQ_MIN         (16000000U)
#define PLLSOURCE_FREQ_MAX         (24000000U)
#endif
/* @} */

/* Private macro ------------------------------------------------------------- */
/* @defgroup RCC_Private_Macros RCC Private Macros @{ */

#define RCC_PLL_OSCSOURCE_CONFIG(__HAL_RCC_PLLSOURCE__) \
            (MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC, (uint32_t)(__HAL_RCC_PLLSOURCE__)))
/* @} */

/* Private variables --------------------------------------------------------- */
/* @defgroup RCC_Private_Variables RCC Private Variables @{ */

/* @} */

/* Private function prototypes ----------------------------------------------- */
#if defined(RCC_PLL_SUPPORT)
static uint32_t RCC_GetHSIFreq(void);
#endif
/* Exported functions -------------------------------------------------------- */

/* @defgroup RCC_Exported_Functions RCC Exported Functions @{ */



HAL_StatusTypeDef HAL_RCC_DeInit(void)
{
  uint32_t tickstart;

        /* Get Start Tick */
  tickstart = HAL_GetTick();

        /* Set HSION bit to the reset value */
  SET_BIT(RCC->CR, RCC_CR_HSION);

        /* Wait till HSI is ready */
  while (READ_BIT(RCC->CR, RCC_CR_HSIRDY) == 0U)
  {
    if ((HAL_GetTick() - tickstart) > HSI_TIMEOUT_VALUE)
    {
      return HAL_TIMEOUT;
    }
  }

        /* Set the HSI clock to 8MHz by default */
  __HAL_RCC_HSI_CALIBRATIONVALUE_ADJUST(RCC_HSICALIBRATION_8MHz);

        /* Get Start Tick */
  tickstart = HAL_GetTick();

        /* Reset CFGR register (HSI is selected as system clock source) */
  RCC->CFGR = 0x00000000u;

        /* Wait till HSI is ready */
  while (READ_BIT(RCC->CFGR, RCC_CFGR_SWS) != 0U)
  {
    if ((HAL_GetTick() - tickstart) > CLOCKSWITCH_TIMEOUT_VALUE)
    {
      return HAL_TIMEOUT;
    }
  }

        /* Clear CR register in 2 steps: first to clear HSEON in case bypass was enabled */
  RCC->CR = RCC_CR_HSION;

        /* Then again to HSEBYP in case bypass was enabled */
  RCC->CR = RCC_CR_HSION;

#if defined(RCC_PLL_SUPPORT)
        /* Get Start Tick */
  tickstart = HAL_GetTick();

        /* Wait till PLL is ready */
  while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) != 0U)
  {
    if ((HAL_GetTick() - tickstart) > PLL_TIMEOUT_VALUE)
    {
      return HAL_TIMEOUT;
    }
  }

        /* once PLL is OFF, reset PLLCFGR register to default value */
  RCC->PLLCFGR = 0x00000000u;
#endif
        /* Disable all interrupts */
  RCC->CIER = 0x00000000u;

        /* Clear all flags */
  RCC->CICR = 0xFFFFFFFFu;

        /* Update the SystemCoreClock global variable */
  SystemCoreClock = HSI_VALUE;

        /* Adapt Systick interrupt period */
  if (HAL_InitTick(uwTickPrio) != HAL_OK)
  {
    return HAL_ERROR;
  }
  else
  {
    return HAL_OK;
  }
}



HAL_StatusTypeDef HAL_RCC_OscConfig(RCC_OscInitTypeDef  *RCC_OscInitStruct)
{
  uint32_t tickstart;
  uint32_t temp_sysclksrc;
#if defined(RCC_PLL_SUPPORT)
  uint32_t temp_pllckcfg;
#endif
        /* Check Null pointer */
  if (RCC_OscInitStruct == NULL)
  {
    return HAL_ERROR;
  }

        /* Check the parameters */
  assert_param(IS_RCC_OSCILLATORTYPE(RCC_OscInitStruct->OscillatorType));

        /* ------------------------------- HSE Configuration ------------------------ */
  if (((RCC_OscInitStruct->OscillatorType) & RCC_OSCILLATORTYPE_HSE) == RCC_OSCILLATORTYPE_HSE)
  {
                /* Check the parameters */
    assert_param(IS_RCC_HSE(RCC_OscInitStruct->HSEState));

    temp_sysclksrc = __HAL_RCC_GET_SYSCLK_SOURCE();      
#if defined(RCC_PLL_SUPPORT)
    temp_pllckcfg = __HAL_RCC_GET_PLL_OSCSOURCE();



    if (((temp_sysclksrc == RCC_CFGR_SWS_PLL) && (temp_pllckcfg == RCC_PLLSOURCE_HSE)) || (temp_sysclksrc == RCC_CFGR_SWS_HSE))
#else
    if (temp_sysclksrc == RCC_CFGR_SWS_HSE)
#endif
    {
      if ((READ_BIT(RCC->CR, RCC_CR_HSERDY) != 0U) && (RCC_OscInitStruct->HSEState == RCC_HSE_OFF))
      {
        return HAL_ERROR;
      }
    }
    else
    {
                        /* Set frequency range of the HSE */
      if (RCC_OscInitStruct->HSEState != RCC_HSE_OFF)
      {
        assert_param(IS_RCC_HSE_FREQ(RCC_OscInitStruct->HSEFreq));
        
        if (RCC_OscInitStruct->HSEFreq != 0)
        {
          MODIFY_REG(RCC->ECSCR, RCC_ECSCR_HSE_FREQ_Msk, RCC_OscInitStruct->HSEFreq);
        } else
        {
          return HAL_ERROR;
        }
      }

                        /* Set the new HSE configuration --------------------------------------- */
      __HAL_RCC_HSE_CONFIG(RCC_OscInitStruct->HSEState);

                        /* Check the HSE State */
      if (RCC_OscInitStruct->HSEState != RCC_HSE_OFF)
      {
#ifndef RCC_NO_DETECT_HSE_READY
                                /* Get Start Tick */
        tickstart = HAL_GetTick();

                                /* Wait till HSE is ready */
        while (READ_BIT(RCC->CR, RCC_CR_HSERDY) == 0U)
        {
          if ((HAL_GetTick() - tickstart) > HSE_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }
#endif
      }
      else
      {
                                /* Get Start Tick */
        tickstart = HAL_GetTick();

                                /* Wait till HSE is disabled */
        while (READ_BIT(RCC->CR, RCC_CR_HSERDY) != 0U)
        {
          if ((HAL_GetTick() - tickstart) > HSE_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }
      }
    }
  }
        /* ----------------------------- HSI Configuration -------------------------- */
  if (((RCC_OscInitStruct->OscillatorType) & RCC_OSCILLATORTYPE_HSI) == RCC_OSCILLATORTYPE_HSI)
  {
                /* Check the parameters */
    assert_param(IS_RCC_HSI(RCC_OscInitStruct->HSIState));
    assert_param(IS_RCC_HSI_CALIBRATION_VALUE(RCC_OscInitStruct->HSICalibrationValue));
    assert_param(IS_RCC_HSIDIV(RCC_OscInitStruct->HSIDiv));

                /* Check if HSI is used as system clock or as PLL source when PLL is selected as system clock */
    temp_sysclksrc = __HAL_RCC_GET_SYSCLK_SOURCE();
#if defined(RCC_PLL_SUPPORT)
    temp_pllckcfg = __HAL_RCC_GET_PLL_OSCSOURCE();
    if (((temp_sysclksrc == RCC_CFGR_SWS_PLL) && (temp_pllckcfg == RCC_PLLSOURCE_HSI)) || (temp_sysclksrc == RCC_CFGR_SWS_HSI))
#else
    if (temp_sysclksrc == RCC_CFGR_SWS_HSI)
#endif
    {
                        /* When HSI is used as system clock or as PLL input clock it can not be disabled */
      if ((READ_BIT(RCC->CR, RCC_CR_HSIRDY) != 0U) && (RCC_OscInitStruct->HSIState == RCC_HSI_OFF))
      {
        return HAL_ERROR;
      }
                        /* Otherwise, just the calibration is allowed */
      else
      {
                                /* Adjusts the Internal High Speed oscillator (HSI) calibration value. */
        __HAL_RCC_HSI_CALIBRATIONVALUE_ADJUST(RCC_OscInitStruct->HSICalibrationValue);

                                /* Get Start Tick */
        tickstart = HAL_GetTick();

                                /* Wait till HSI is ready */
        while (READ_BIT(RCC->CR, RCC_CR_HSIRDY) == 0U)
        {
          if ((HAL_GetTick() - tickstart) > HSI_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }

        if (temp_sysclksrc == RCC_CFGR_SWS_HSI)
        {
                                        /* Adjust the HSI division factor */
          __HAL_RCC_HSI_CONFIG(RCC_OscInitStruct->HSIDiv);

                                        /* Update the SystemCoreClock global variable with HSISYS value */
          SystemCoreClock = (HAL_RCC_GetSysClockFreq() >> ((AHBPrescTable[(RCC->CFGR & RCC_CFGR_HPRE) >> RCC_CFGR_HPRE_Pos]) & 0x1FU));
        }

                                /* Adapt Systick interrupt period */
        if (HAL_InitTick(uwTickPrio) != HAL_OK)
        {
          return HAL_ERROR;
        }
      }
    }
    else
    {
                        /* Check the HSI State */
      if (RCC_OscInitStruct->HSIState != RCC_HSI_OFF)
      {
                                /* Configure the HSI division factor */
        __HAL_RCC_HSI_CONFIG(RCC_OscInitStruct->HSIDiv);

                                /* Enable the Internal High Speed oscillator (HSI). */
        __HAL_RCC_HSI_ENABLE();

                                /* Get Start Tick */
        tickstart = HAL_GetTick();

                                /* Wait till HSI is ready */
        while (READ_BIT(RCC->CR, RCC_CR_HSIRDY) == 0U)
        {
          if ((HAL_GetTick() - tickstart) > HSI_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }

                                /* Adjusts the Internal High Speed oscillator (HSI) calibration value. */
        __HAL_RCC_HSI_CALIBRATIONVALUE_ADJUST(RCC_OscInitStruct->HSICalibrationValue);
      }
      else
      {
                                /* Disable the Internal High Speed oscillator (HSI). */
        __HAL_RCC_HSI_DISABLE();

                                /* Get Start Tick */
        tickstart = HAL_GetTick();

                                /* Wait till HSI is disabled */
        while (READ_BIT(RCC->CR, RCC_CR_HSIRDY) != 0U)
        {
          if ((HAL_GetTick() - tickstart) > HSI_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }
      }
    }
  }
        /* ------------------------------ LSI Configuration ------------------------- */
  if (((RCC_OscInitStruct->OscillatorType) & RCC_OSCILLATORTYPE_LSI) == RCC_OSCILLATORTYPE_LSI)
  {
                /* Check the parameters */
    assert_param(IS_RCC_LSI(RCC_OscInitStruct->LSIState));

                /* Check if LSI is used as system clock */
    if (__HAL_RCC_GET_SYSCLK_SOURCE() == RCC_CFGR_SWS_LSI)
    {
                        /* When LSI is used as system clock it will not be disabled */
      if ((((RCC->CSR) & RCC_CSR_LSIRDY) != 0U) && (RCC_OscInitStruct->LSIState == RCC_LSI_OFF))
      {
        return HAL_ERROR;
      }
    }
    else
    {
                        /* Check the LSI State */
      if (RCC_OscInitStruct->LSIState != RCC_LSI_OFF)
      {
                                /* Enable the Internal Low Speed oscillator (LSI). */
        __HAL_RCC_LSI_ENABLE();

                                /* Get Start Tick */
        tickstart = HAL_GetTick();

                                /* Wait till LSI is ready */
        while (READ_BIT(RCC->CSR, RCC_CSR_LSIRDY) == 0U)
        {
          if ((HAL_GetTick() - tickstart) > LSI_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }
      }
      else
      {
                                /* Disable the Internal Low Speed oscillator (LSI). */
        __HAL_RCC_LSI_DISABLE();

                                /* Get Start Tick */
        tickstart = HAL_GetTick();

                                /* Wait till LSI is disabled */
        while (READ_BIT(RCC->CSR, RCC_CSR_LSIRDY) != 0U)
        {
          if ((HAL_GetTick() - tickstart) > LSI_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }
      }
    }
  }

#if defined(RCC_LSE_SUPPORT)
        /* ------------------------------ LSE Configuration ------------------------- */
  if (((RCC_OscInitStruct->OscillatorType) & RCC_OSCILLATORTYPE_LSE) == RCC_OSCILLATORTYPE_LSE)
  {
    FlagStatus       pwrclkchanged = RESET;

                /* Check the parameters */
    assert_param(IS_RCC_LSE(RCC_OscInitStruct->LSEState));

                /* When the LSE is used as system clock, it is not allowed disable it */
    if (__HAL_RCC_GET_SYSCLK_SOURCE() == RCC_CFGR_SWS_LSE)
    {
      if ((((RCC->BDCR) & RCC_BDCR_LSERDY) != 0U) && (RCC_OscInitStruct->LSEState == RCC_LSE_OFF))
      {
        return HAL_ERROR;
      }
    }
    else
    {
                        /* Update LSE configuration in Backup Domain control register */
                        /* Requires to enable write access to Backup Domain of necessary */
      if (__HAL_RCC_PWR_IS_CLK_DISABLED() != 0U) 
      {
        __HAL_RCC_PWR_CLK_ENABLE();
        pwrclkchanged = SET;
      }

      if (HAL_IS_BIT_CLR(PWR->CR1, PWR_CR1_DBP))
      {
                                /* Enable write access to Backup domain */
        SET_BIT(PWR->CR1, PWR_CR1_DBP);

                                /* Wait for Backup domain Write protection disable */
        tickstart = HAL_GetTick();

        while (HAL_IS_BIT_CLR(PWR->CR1, PWR_CR1_DBP))
        {
          if ((HAL_GetTick() - tickstart) > RCC_DBP_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }
      }
                        /* Set driver factor of the LSE */
      if (RCC_OscInitStruct->LSEState != RCC_LSE_OFF)
      {
        if (((RCC_OscInitStruct->LSEDriver) & RCC_ECSCR_LSE_DRIVER) == 0U)
        {
          MODIFY_REG(RCC->ECSCR, RCC_ECSCR_LSE_DRIVER_Msk, RCC_ECSCR_LSE_DRIVER_1);
        } else
        {
          MODIFY_REG(RCC->ECSCR, RCC_ECSCR_LSE_DRIVER_Msk, RCC_OscInitStruct->LSEDriver);
        }
      }
                        /* Set the new LSE configuration ----------------------------------------- */
      __HAL_RCC_LSE_CONFIG(RCC_OscInitStruct->LSEState);

                        /* Check the LSE State */
      if (RCC_OscInitStruct->LSEState != RCC_LSE_OFF)
      {
#ifndef RCC_NO_DETECT_LSE_READY
                                /* Get Start Tick */
        tickstart = HAL_GetTick();

                                /* Wait till LSE is ready */
        while (READ_BIT(RCC->BDCR, RCC_BDCR_LSERDY) == 0U)
        {
          if ((HAL_GetTick() - tickstart) > RCC_LSE_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }
#endif
      }
      else
      {
                                /* Get Start Tick */
        tickstart = HAL_GetTick();

                                /* Wait till LSE is disabled */
        while (READ_BIT(RCC->BDCR, RCC_BDCR_LSERDY) != 0U)
        {
          if ((HAL_GetTick() - tickstart) > RCC_LSE_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }
      }

                        /* Restore clock configuration if changed */
      if (pwrclkchanged == SET)
      {
        __HAL_RCC_PWR_CLK_DISABLE();
      }
    }
  }
#endif
#if defined(RCC_PLL_SUPPORT)
        /* -------------------------------- PLL Configuration ----------------------- */
        /* Check the parameters */
  assert_param(IS_RCC_PLL(RCC_OscInitStruct->PLL.PLLState));

  if (RCC_OscInitStruct->PLL.PLLState != RCC_PLL_NONE)
  {
                /* Check if the PLL is used as system clock or not */
    if (__HAL_RCC_GET_SYSCLK_SOURCE() != RCC_CFGR_SWS_PLL)
    {
      if (RCC_OscInitStruct->PLL.PLLState == RCC_PLL_ON)
      {
                                /* Check the parameters */
        assert_param(IS_RCC_PLLSOURCE(RCC_OscInitStruct->PLL.PLLSource));

                                /* Disable the main PLL. */
        __HAL_RCC_PLL_DISABLE();

                                /* Get Start Tick */
        tickstart = HAL_GetTick();

                                /* Wait till PLL is cancel the ready */
        while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) != 0U)
        {
          if ((HAL_GetTick() - tickstart) > PLL_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }

                                /* PLL input source frequency must be greater than or equal to PLLSOURCE_MIN_FREQ */
        if(((RCC_OscInitStruct->PLL.PLLSource == RCC_PLLSOURCE_HSE) && \
            (((HSE_VALUE < PLLSOURCE_FREQ_MIN)) || (HSE_VALUE > PLLSOURCE_FREQ_MAX))) || \
            ((RCC_OscInitStruct->PLL.PLLSource == RCC_PLLSOURCE_HSI) && (RCC_GetHSIFreq() < PLLSOURCE_FREQ_MIN)))
        {
          return HAL_ERROR;
        }

                                /* Configure the main PLL clock source, multiplication and division factors. */
        __HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_OscInitStruct->PLL.PLLSource);

                                /* Enable the main PLL. */
        __HAL_RCC_PLL_ENABLE();

                                /* Get Start Tick */
        tickstart = HAL_GetTick();

                                /* Wait till PLL is ready */
        while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) == 0U)
        {
          if ((HAL_GetTick() - tickstart) > PLL_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }
      }
      else
      {
                                /* Disable the main PLL. */
        __HAL_RCC_PLL_DISABLE();

                                /* Disable all PLL outputs to save power */
        MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC, PLLSOURCE_NONE);

                                /* Get Start Tick */
        tickstart = HAL_GetTick();

                                /* Wait till PLL is disabled */
        while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) != 0U)
        {
          if ((HAL_GetTick() - tickstart) > PLL_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }
      }
    }
    else
    {
                        /* Check if there is a request to disable the PLL used as System clock source */
      if((RCC_OscInitStruct->PLL.PLLState) == RCC_PLL_OFF)
      {
        return HAL_ERROR;
      }
      else
      {
                                /* Do not return HAL_ERROR if request repeats the current configuration */
        temp_pllckcfg = RCC->PLLCFGR;

        if(READ_BIT(temp_pllckcfg, RCC_PLLCFGR_PLLSRC) != RCC_OscInitStruct->PLL.PLLSource)
        {
          return HAL_ERROR;
        }
      }
    }
  }
#endif
  return HAL_OK;
}



HAL_StatusTypeDef HAL_RCC_ClockConfig(RCC_ClkInitTypeDef  *RCC_ClkInitStruct, uint32_t FLatency)
{
  uint32_t tickstart;

        /* Check Null pointer */
  if (RCC_ClkInitStruct == NULL)
  {
    return HAL_ERROR;
  }

        /* Check the parameters */
  assert_param(IS_RCC_CLOCKTYPE(RCC_ClkInitStruct->ClockType));
  assert_param(IS_FLASH_LATENCY(FLatency));

        /* (HCLK) and the supply voltage of the device. */

        /* Increasing the number of wait states because of higher CPU frequency */
  if (FLatency > __HAL_FLASH_GET_LATENCY())
  {
                /* Program the new number of wait states to the LATENCY bits in the FLASH_ACR register */
    __HAL_FLASH_SET_LATENCY(FLatency);



    tickstart = HAL_GetTick();

    while ((FLASH->ACR & FLASH_ACR_LATENCY) != FLatency)
    {
      if ((HAL_GetTick() - tickstart) > CLOCKSWITCH_TIMEOUT_VALUE)
      {
        return HAL_TIMEOUT;
      }
    }
  }

        /* -------------------------- HCLK Configuration -------------------------- */
  if (((RCC_ClkInitStruct->ClockType) & RCC_CLOCKTYPE_HCLK) == RCC_CLOCKTYPE_HCLK)
  {

    if (((RCC_ClkInitStruct->ClockType) & RCC_CLOCKTYPE_PCLK1) == RCC_CLOCKTYPE_PCLK1)
    {
      MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE, RCC_HCLK_DIV16);
    }

                /* Set the new HCLK clock divider */
    assert_param(IS_RCC_HCLK(RCC_ClkInitStruct->AHBCLKDivider));
    MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE, RCC_ClkInitStruct->AHBCLKDivider);
  }

        /* ------------------------- SYSCLK Configuration --------------------------- */
  if (((RCC_ClkInitStruct->ClockType) & RCC_CLOCKTYPE_SYSCLK) == RCC_CLOCKTYPE_SYSCLK)
  {
    assert_param(IS_RCC_SYSCLKSOURCE(RCC_ClkInitStruct->SYSCLKSource));

                /* HSE is selected as System Clock Source */
    if (RCC_ClkInitStruct->SYSCLKSource == RCC_SYSCLKSOURCE_HSE)
    {
                        /* Check the HSE ready flag */
      if (READ_BIT(RCC->CR, RCC_CR_HSERDY) == 0U)
      {
        return HAL_ERROR;
      }
    }
                /* HSI is selected as System Clock Source */
    else if (RCC_ClkInitStruct->SYSCLKSource == RCC_SYSCLKSOURCE_HSI)
    {
                        /* Check the HSI ready flag */
      if (READ_BIT(RCC->CR, RCC_CR_HSIRDY) == 0U)
      {
        return HAL_ERROR;
      }
    }
#if defined(RCC_PLL_SUPPORT)
                /* PLL is selected as System Clock Source */
    else if (RCC_ClkInitStruct->SYSCLKSource == RCC_SYSCLKSOURCE_PLLCLK)
    {
                        /* Check the PLL ready flag */
      if (READ_BIT(RCC->CR, RCC_CR_PLLRDY) == 0U)
      {
        return HAL_ERROR;
      }
    }
#endif
#if defined(RCC_LSE_SUPPORT)
                /* LSE is selected as System Clock Source */
    else if (RCC_ClkInitStruct->SYSCLKSource == RCC_SYSCLKSOURCE_LSE)
    {
                        /* Check the LSE ready flag */
      if (READ_BIT(RCC->BDCR, RCC_BDCR_LSERDY) == 0U)
      {
        return HAL_ERROR;
      }
    }
#endif
                /* LSI is selected as System Clock Source */
    else 
    {
                        /* Check the LSI ready flag */
      if (READ_BIT(RCC->CSR, RCC_CSR_LSIRDY) == 0U)
      {
        return HAL_ERROR;
      }
    }
    MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_ClkInitStruct->SYSCLKSource);

                /* Get Start Tick */
    tickstart = HAL_GetTick();

    while (__HAL_RCC_GET_SYSCLK_SOURCE() != (RCC_ClkInitStruct->SYSCLKSource << RCC_CFGR_SWS_Pos))
    {
      if ((HAL_GetTick() - tickstart) > CLOCKSWITCH_TIMEOUT_VALUE)
      {
        return HAL_TIMEOUT;
      }
    }
  }

        /* Decreasing the number of wait states because of lower CPU frequency */
  if (FLatency < __HAL_FLASH_GET_LATENCY())
  {
                /* Program the new number of wait states to the LATENCY bits in the FLASH_ACR register */
    __HAL_FLASH_SET_LATENCY(FLatency);



    tickstart = HAL_GetTick();

    while ((FLASH->ACR & FLASH_ACR_LATENCY) != FLatency)
    {
      if ((HAL_GetTick() - tickstart) > CLOCKSWITCH_TIMEOUT_VALUE)
      {
        return HAL_TIMEOUT;
      }
    }
  }

        /* -------------------------- PCLK1 Configuration --------------------------- */
  if (((RCC_ClkInitStruct->ClockType) & RCC_CLOCKTYPE_PCLK1) == RCC_CLOCKTYPE_PCLK1)
  {
    assert_param(IS_RCC_PCLK(RCC_ClkInitStruct->APB1CLKDivider));
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE, RCC_ClkInitStruct->APB1CLKDivider);
  }

        /* Update the SystemCoreClock global variable */
  SystemCoreClock = (HAL_RCC_GetSysClockFreq() >> ((AHBPrescTable[(RCC->CFGR & RCC_CFGR_HPRE) >> RCC_CFGR_HPRE_Pos]) & 0x1FU));

        /* Configure the source of time base considering new system clocks settings */
  return HAL_InitTick(uwTickPrio);
}

/* @} */



void HAL_RCC_MCOConfig(uint32_t RCC_MCOx, uint32_t RCC_MCOSource, uint32_t RCC_MCODiv)
{
  GPIO_InitTypeDef GPIO_InitStruct;

        /* Check the parameters */
  assert_param(IS_RCC_MCO(RCC_MCOx));
  assert_param(IS_RCC_MCODIV(RCC_MCODiv));
  assert_param(IS_RCC_MCO1SOURCE(RCC_MCOSource));

  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  switch(RCC_MCOx)
  {
  case RCC_MCO2 :      /* Configure PA01 as the clock output */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Alternate = GPIO_AF15_MCO;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    break;
  case RCC_MCO3 :      /* Configure PA05 as the clock output */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Alternate = GPIO_AF15_MCO;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    break;
  case RCC_MCO4 :      /* Configure PA09 as the clock output */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Alternate = GPIO_AF5_MCO;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    break;
  case RCC_MCO5 :      /* Configure PA13 as the clock output. Note:PA13 is SWD_SWDIO Pin */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Alternate = GPIO_AF15_MCO;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    break;
  case RCC_MCO6 :      /* Configure PA14 as the clock output. Note:PA14 is SWD_SWCLK Pin */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_14;
    GPIO_InitStruct.Alternate = GPIO_AF15_MCO;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    break;
  case RCC_MCO7 :  
    __HAL_RCC_GPIOF_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Alternate = GPIO_AF6_MCO;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
    break;
  case RCC_MCO1 :      /* PA08 */
  default :
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Alternate = GPIO_AF5_MCO;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    break;
  }

        /* Mask MCOSEL[] and MCOPRE[] bits then set MCO1 clock source and prescaler */
  MODIFY_REG(RCC->CFGR, (RCC_CFGR_MCOSEL | RCC_CFGR_MCOPRE), (RCC_MCOSource | RCC_MCODiv));
}



uint32_t HAL_RCC_GetSysClockFreq(void)
{
  uint32_t hsidiv;
  uint32_t sysclockfreq;
  const uint32_t hsiValue[5] = {4000000,8000000,16000000,22120000,24000000};
  uint32_t hsiIndex;
#if defined(RCC_PLL_SUPPORT)
  uint32_t pllsource;
#endif
  
  if (__HAL_RCC_GET_SYSCLK_SOURCE() == RCC_CFGR_SWS_HSI)
  {
                /* HSISYS can be derived for HSI */
    hsidiv = (1UL << ((READ_BIT(RCC->CR, RCC_CR_HSIDIV)) >> RCC_CR_HSIDIV_Pos));

                /* HSISYS used as system clock source */
    hsiIndex = (RCC->ICSCR&RCC_ICSCR_HSI_FS_Msk)>>RCC_ICSCR_HSI_FS_Pos;
    if (hsiIndex > 4)
    {
      hsiIndex = 0;
    }
    sysclockfreq = (hsiValue[hsiIndex] / hsidiv);
  }
  else if (__HAL_RCC_GET_SYSCLK_SOURCE() == RCC_CFGR_SWS_HSE)
  {
                /* HSE used as system clock source */
    sysclockfreq = HSE_VALUE;
  }
#if defined(RCC_PLL_SUPPORT)
  else if (__HAL_RCC_GET_SYSCLK_SOURCE() == RCC_CFGR_SWS_PLL)
  {
    pllsource = (RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC);

    switch (pllsource)
    {
    case RCC_PLLSOURCE_HSE:          /* HSE used as PLL clock source */
      sysclockfreq =  HSE_VALUE  * 2;
      break;

    case RCC_PLLSOURCE_HSI:          /* HSI used as PLL clock source */
      hsiIndex = (RCC->ICSCR&RCC_ICSCR_HSI_FS_Msk)>>RCC_ICSCR_HSI_FS_Pos;
      if (hsiIndex > 4)
      {
        hsiIndex = 0;
      }
      sysclockfreq =  hsiValue[hsiIndex]  * 2;
      break;

    default:                         /* HSI used as PLL clock source */
      sysclockfreq = hsiValue[4]  * 2;
      break;
    }
  }
#endif
#if defined(RCC_LSE_SUPPORT)
  else if (__HAL_RCC_GET_SYSCLK_SOURCE() == RCC_CFGR_SWS_LSE)
  {
                /* LSE used as system clock source */
    sysclockfreq = LSE_VALUE;
  }
#endif
  else if (__HAL_RCC_GET_SYSCLK_SOURCE() == RCC_CFGR_SWS_LSI)
  {
                /* LSI used as system clock source */
    sysclockfreq = LSI_VALUE;
  }
  else
  {
    sysclockfreq = 0U;
  }

  return sysclockfreq;
}



uint32_t HAL_RCC_GetHCLKFreq(void)
{
  return SystemCoreClock;
}



uint32_t HAL_RCC_GetPCLK1Freq(void)
{
        /* Get HCLK source and Compute PCLK1 frequency --------------------------- */
  return (uint32_t)((HAL_RCC_GetHCLKFreq()) >> (APBPrescTable[(RCC->CFGR & RCC_CFGR_PPRE) >> RCC_CFGR_PPRE_Pos] & 0x1FU));
}



void HAL_RCC_GetOscConfig(RCC_OscInitTypeDef  *RCC_OscInitStruct)
{
        /* Check the parameters */
  assert_param(RCC_OscInitStruct != (void *)NULL);

        /* Set all possible values for the Oscillator type parameter --------------- */
#if defined(RCC_LSE_SUPPORT)    
  RCC_OscInitStruct->OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_HSI | \
                                      RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_LSI;
#else
  RCC_OscInitStruct->OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_HSI | \
                                      RCC_OSCILLATORTYPE_LSI;
#endif
        /* Get the HSE configuration ----------------------------------------------- */
  if ((RCC->CR & RCC_CR_HSEBYP) == RCC_CR_HSEBYP)
  {
    RCC_OscInitStruct->HSEState = RCC_HSE_BYPASS;
  }
  else if ((RCC->CR & RCC_CR_HSEON) == RCC_CR_HSEON)
  {
    RCC_OscInitStruct->HSEState = RCC_HSE_ON;
  }
  else
  {
    RCC_OscInitStruct->HSEState = RCC_HSE_OFF;
  }
  
  RCC_OscInitStruct->HSEFreq = (RCC->ECSCR & RCC_ECSCR_HSE_FREQ);
  
        /* Get the HSI configuration ----------------------------------------------- */
  if ((RCC->CR & RCC_CR_HSION) == RCC_CR_HSION)
  {
    RCC_OscInitStruct->HSIState = RCC_HSI_ON;
  }
  else
  {
    RCC_OscInitStruct->HSIState = RCC_HSI_OFF;
  }

  RCC_OscInitStruct->HSICalibrationValue = (RCC->ICSCR & (RCC_ICSCR_HSI_FS | RCC_ICSCR_HSI_TRIM));
  RCC_OscInitStruct->HSIDiv = (RCC->CR & RCC_CR_HSIDIV);

        /* Get the LSI configuration ----------------------------------------------- */
  if ((RCC->CSR & RCC_CSR_LSION) == RCC_CSR_LSION)
  {
    RCC_OscInitStruct->LSIState = RCC_LSI_ON;
  }
  else
  {
    RCC_OscInitStruct->LSIState = RCC_LSI_OFF;
  }

#if defined(RCC_LSE_SUPPORT)
        /* Get the LSE configuration ----------------------------------------------- */
  if ((RCC->BDCR & RCC_BDCR_LSEBYP) == RCC_BDCR_LSEBYP)
  {
    RCC_OscInitStruct->LSEState = RCC_LSE_BYPASS;
  }
  else if ((RCC->BDCR & RCC_BDCR_LSEON) == RCC_BDCR_LSEON)
  {
    RCC_OscInitStruct->LSEState = RCC_LSE_ON;
  }
  else
  {
    RCC_OscInitStruct->LSEState = RCC_LSE_OFF;
  }
  
  RCC_OscInitStruct->LSEDriver = (RCC->ECSCR & RCC_ECSCR_LSE_DRIVER);
#endif
#if defined(RCC_PLL_SUPPORT)
        /* Get the PLL configuration ----------------------------------------------- */
  if ((RCC->CR & RCC_CR_PLLON) == RCC_CR_PLLON)
  {
    RCC_OscInitStruct->PLL.PLLState = RCC_PLL_ON;
  }
  else
  {
    RCC_OscInitStruct->PLL.PLLState = RCC_PLL_OFF;
  }
  
  RCC_OscInitStruct->PLL.PLLSource = (RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC);
#endif
}



void HAL_RCC_GetClockConfig(RCC_ClkInitTypeDef  *RCC_ClkInitStruct, uint32_t *pFLatency)
{
        /* Check the parameters */
  assert_param(RCC_ClkInitStruct != (void *)NULL);
  assert_param(pFLatency != (void *)NULL);

        /* Set all possible values for the Clock type parameter -------------------- */
  RCC_ClkInitStruct->ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1;

        /* Get the SYSCLK configuration -------------------------------------------- */
  RCC_ClkInitStruct->SYSCLKSource = (uint32_t)(RCC->CFGR & RCC_CFGR_SW);

        /* Get the HCLK configuration ---------------------------------------------- */
  RCC_ClkInitStruct->AHBCLKDivider = (uint32_t)(RCC->CFGR & RCC_CFGR_HPRE);

        /* Get the APB1 configuration ---------------------------------------------- */
  RCC_ClkInitStruct->APB1CLKDivider = (uint32_t)(RCC->CFGR & RCC_CFGR_PPRE);



        /* Get the Flash Wait State (Latency) configuration ------------------------ */
  *pFLatency = (uint32_t)(FLASH->ACR & FLASH_ACR_LATENCY);
}



void HAL_RCC_EnableCSS(void)
{
  SET_BIT(RCC->CR, RCC_CR_CSSON) ;
}

#if defined(RCC_LSE_SUPPORT)

void HAL_RCC_EnableLSECSS(void)
{
  SET_BIT(RCC->BDCR, RCC_BDCR_LSECSSON) ;
}



void HAL_RCC_DisableLSECSS(void)
{
  CLEAR_BIT(RCC->BDCR, RCC_BDCR_LSECSSON) ;
}
#endif

/* @brief Handle the RCC Clock Security System interrupt request. @note  This API should be called under the NMI_Handler(). @retval None */
void HAL_RCC_NMI_IRQHandler(void)
{
  uint32_t itflag = RCC->CIFR;

        /* Clear interrupt flags related to CSS */
#if defined(RCC_LSE_SUPPORT)
  RCC->CICR = (itflag & (RCC_CIFR_CSSF | RCC_CIFR_LSECSSF));
#else
  RCC->CICR = (itflag & RCC_CIFR_CSSF);
#endif
        /* Check RCC CSSF interrupt flag */
  if ((itflag & RCC_CIFR_CSSF) != 0x00u)
  {
                /* RCC Clock Security System interrupt user callback */
    HAL_RCC_CSSCallback();
  }

#if defined(RCC_LSE_SUPPORT)
        /* Check RCC LSECSSF interrupt flag */
  if ((itflag & RCC_CIFR_LSECSSF) != 0x00u)
  {
                /* RCC Clock Security System interrupt user callback */
    HAL_RCC_LSECSSCallback();
  }
#endif
}

#if defined(RCC_PLL_SUPPORT)
/* @brief Get HSI frequency. @retval HSI frequency */
static uint32_t RCC_GetHSIFreq(void)
{
  uint32_t hsifs;

  hsifs = ((READ_BIT(RCC->ICSCR, RCC_ICSCR_HSI_FS)) >> RCC_ICSCR_HSI_FS_Pos);

  return HSIFreqTable[hsifs%8];
}
#endif
/* @brief Handle the RCC HSE Clock Security System interrupt callback. @retval none */
__weak void HAL_RCC_CSSCallback(void)
{

}

#if defined(RCC_LSE_SUPPORT)
/* @brief  RCC LSE Clock Security System interrupt callback. @retval none */
__weak void HAL_RCC_LSECSSCallback(void)
{

}
#endif
/* @} */

/* @} */

#endif /* HAL_RCC_MODULE_ENABLED */
/* @} */

/* @} */

/* (C) COPYRIGHT Puya *****END OF FILE */
