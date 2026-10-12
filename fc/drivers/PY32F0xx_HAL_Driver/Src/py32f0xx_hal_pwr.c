/**
 * @file    py32f0xx_hal_pwr.c
 * @author  MCU Application Team
 * @brief   PWR HAL module driver.
 */

/* Includes ------------------------------------------------------------------ */
#include "py32f0xx_hal.h"

/**
 * @addtogroup PY32F0xx_HAL_Driver @{
 */

/* @addtogroup PWR @{ */

#ifdef HAL_PWR_MODULE_ENABLED

/* Private typedef ----------------------------------------------------------- */
/* Private define ------------------------------------------------------------ */
/* @defgroup PWR_Private_Defines PWR Private Defines @{ */

#if defined(PWR_PVD_SUPPORT)
/* @defgroup PWR_PVD_Mode_Mask PWR PVD Mode Mask @{ */
#define PVD_MODE_IT           0x00010000U  /* !< Mask for interruption yielded by PVD threshold crossing */
#define PVD_MODE_EVT          0x00020000U  /* !< Mask for event yielded by PVD threshold crossing */
#define PVD_RISING_EDGE       0x00000001U  /* !< Mask for rising edge set as PVD trigger */
#define PVD_FALLING_EDGE      0x00000002U  /* !< Mask for falling edge set as PVD trigger */
/* @} */
#endif

/* @} */

/* Private macro ------------------------------------------------------------- */
/* Private variables --------------------------------------------------------- */
/* Private function prototypes ----------------------------------------------- */
/* Exported functions -------------------------------------------------------- */
/* @addtogroup PWR_Exported_Functions  PWR Exported Functions @{ */

/* @brief  Initialization and de-initialization functions @verbatim ##### Initialization and de-initialization functions ##### [..] */

/* @brief  Deinitialize the HAL PWR peripheral registers to their default reset values. @retval None */
void HAL_PWR_DeInit(void)
{
  __HAL_RCC_PWR_FORCE_RESET();
  __HAL_RCC_PWR_RELEASE_RESET();
}

/* @} */

/* @brief Low Power modes configuration functions @verbatim ##### Peripheral Control functions ##### [..] */



void HAL_PWR_EnableBkUpAccess(void)
{
  SET_BIT(PWR->CR1, PWR_CR1_DBP);
}



/* @brief  Disable access to the backup domain @retval None */
void HAL_PWR_DisableBkUpAccess(void)
{
  CLEAR_BIT(PWR->CR1, PWR_CR1_DBP);
}



#if defined(PWR_PVD_SUPPORT)

HAL_StatusTypeDef HAL_PWR_ConfigPVD(PWR_PVDTypeDef *sConfigPVD)
{
        /* Check the parameters */
  assert_param(IS_PWR_PVD_LEVEL(sConfigPVD->PVDLevel));
  assert_param(IS_PWR_PVD_MODE(sConfigPVD->Mode));

        /* Set PVD level bits only according to PVDLevel value */
  MODIFY_REG(PWR->CR2, (PWR_CR2_PVDT | PWR_CR2_FLTEN | PWR_CR2_FLT_TIME | PWR_CR2_SRCSEL), \
                       (sConfigPVD->PVDLevel | sConfigPVD->PVDFilter | sConfigPVD->PVDSource));

        /* Clear any previous config, in case no event or IT mode is selected */
  __HAL_PWR_PVD_EXTI_DISABLE_EVENT();
  __HAL_PWR_PVD_EXTI_DISABLE_IT();
  __HAL_PWR_PVD_EXTI_DISABLE_FALLING_EDGE();
  __HAL_PWR_PVD_EXTI_DISABLE_RISING_EDGE();

        /* Configure interrupt mode */
  if((sConfigPVD->Mode & PVD_MODE_IT) == PVD_MODE_IT)
  {
    __HAL_PWR_PVD_EXTI_ENABLE_IT();
  }

        /* Configure event mode */
  if((sConfigPVD->Mode & PVD_MODE_EVT) == PVD_MODE_EVT)
  {
    __HAL_PWR_PVD_EXTI_ENABLE_EVENT();
  }

        /* Configure the edge */
  if((sConfigPVD->Mode & PVD_RISING_EDGE) == PVD_RISING_EDGE)
  {
    __HAL_PWR_PVD_EXTI_ENABLE_RISING_EDGE();
  }

  if((sConfigPVD->Mode & PVD_FALLING_EDGE) == PVD_FALLING_EDGE)
  {
    __HAL_PWR_PVD_EXTI_ENABLE_FALLING_EDGE();
  }

  return HAL_OK;
}



/* @brief  Enable the Power Voltage Detector (PVD). @retval None */
void HAL_PWR_EnablePVD(void)
{
  SET_BIT(PWR->CR2, PWR_CR2_PVDE);
}



/* @brief  Disable the Power Voltage Detector (PVD). @retval None */
void HAL_PWR_DisablePVD(void)
{
  CLEAR_BIT(PWR->CR2, PWR_CR2_PVDE);
}
#endif



HAL_StatusTypeDef HAL_PWR_ConfigStopMode(PWR_StopModeConfigTypeDef *sStopModeConfig)
{
        /* Check the parameters */
  assert_param(IS_PWR_STOP_LPR_VOLT(sStopModeConfig->LPVoltSelection));
  assert_param(IS_PWR_REGULATOR_SWTICH_DELAY(sStopModeConfig->RegulatorSwitchDelay));
  assert_param(IS_PWR_WAKEUP_HSIEN_TIMING(sStopModeConfig->WakeUpHsiEnableTime));
  assert_param(IS_PWR_SRAM_RETENTION_VOLT(sStopModeConfig->SramRetentionVolt));
  assert_param(IS_PWR_WAKEUP_FLASH_DELAY(sStopModeConfig->FlashDelay));

        /* Set the STOP mode and STOP wake-up timing related configurations */
  MODIFY_REG(PWR->CR1, (PWR_CR1_VOS | PWR_CR1_MRRDY_TIME | PWR_CR1_HSION_CTRL | PWR_CR1_SRAM_RETV | PWR_CR1_FLS_SLPTIME), 
                       (sStopModeConfig->LPVoltSelection) | \
                       (sStopModeConfig->WakeUpHsiEnableTime) | \
                       (sStopModeConfig->RegulatorSwitchDelay) | \
                       (sStopModeConfig->SramRetentionVolt) | \
                       (sStopModeConfig->FlashDelay));

  return HAL_OK;
}



HAL_StatusTypeDef HAL_PWR_ConfigBIAS(PWR_BIASConfigTypeDef *sBIASConfig)
{
        /* Check the parameters */
  assert_param(IS_BIAS_CURRENTS_SOURCE(sBIASConfig->BiasCurrentSource));
  
  if(((sBIASConfig->BiasCurrentSource) & PWR_BIAS_CURRENTS_FROM_BIAS_CR) == PWR_BIAS_CURRENTS_FROM_BIAS_CR)
  {
                /* Set the bias currents load source and bias currents value */
    MODIFY_REG(PWR->CR1, (PWR_CR1_BIAS_CR_SEL) | (PWR_CR1_BIAS_CR), (sBIASConfig->BiasCurrentSource) | \
                                                                    (sBIASConfig->BiasCurrentValue));
  }
  else
  {
                /* Set the bias currents load source */
    MODIFY_REG(PWR->CR1, PWR_CR1_BIAS_CR_SEL, (sBIASConfig->BiasCurrentSource));
  }
  return HAL_OK;
}



void HAL_PWR_EnterSLEEPMode(uint8_t SLEEPEntry)
{
        /* Check the parameters */
  assert_param(IS_PWR_SLEEP_ENTRY(SLEEPEntry));

        /* Clear SLEEPDEEP bit of Cortex System Control Register */
  CLEAR_BIT(SCB->SCR, ((uint32_t)SCB_SCR_SLEEPDEEP_Msk));

        /* Select SLEEP mode entry ------------------------------------------------- */
  if(SLEEPEntry == PWR_SLEEPENTRY_WFI)
  {
                /* Request Wait For Interrupt */
    __WFI();
  }
  else
  {
                /* Request Wait For Event */
    __SEV();
    __WFE();
    __WFE();
  }
}



void HAL_PWR_EnterSTOPMode(uint32_t Regulator, uint8_t STOPEntry)
{
        /* Check the parameters */
  assert_param(IS_PWR_REGULATOR(Regulator));
  assert_param(IS_PWR_STOP_ENTRY(STOPEntry));

  if (Regulator != PWR_MAINREGULATOR_ON)
  {
                /* Stop mode with Low-Power Regulator */
    SET_BIT(PWR->CR1,PWR_CR1_LPR);
  }
  else
  {
                /* Stop mode with Main Regulator */
    CLEAR_BIT(PWR->CR1,PWR_CR1_LPR);
  }

        /* Set SLEEPDEEP bit of Cortex System Control Register */
  SET_BIT(SCB->SCR, ((uint32_t)SCB_SCR_SLEEPDEEP_Msk));

        /* Select Stop mode entry -------------------------------------------------- */
  if(STOPEntry == PWR_STOPENTRY_WFI)
  {
                /* Request Wait For Interrupt */
    __WFI();
  }
  else
  {
                /* Request Wait For Event */
    __SEV();
    __WFE();
    __WFE();
  }

        /* Reset SLEEPDEEP bit of Cortex System Control Register */
  CLEAR_BIT(SCB->SCR, ((uint32_t)SCB_SCR_SLEEPDEEP_Msk));
}



/* @brief  Enable Sleep-On-Exit Cortex feature @note   Set SLEEPONEXIT bit of SCR register. When this bit is set, the handling. @retval None */
void HAL_PWR_EnableSleepOnExit(void)
{
        /* Set SLEEPONEXIT bit of Cortex System Control Register */
  SET_BIT(SCB->SCR, ((uint32_t)SCB_SCR_SLEEPONEXIT_Msk));
}



void HAL_PWR_DisableSleepOnExit(void)
{
        /* Clear SLEEPONEXIT bit of Cortex System Control Register */
  CLEAR_BIT(SCB->SCR, ((uint32_t)SCB_SCR_SLEEPONEXIT_Msk));
}



void HAL_PWR_EnableSEVOnPend(void)
{
        /* Set SEVONPEND bit of Cortex System Control Register */
  SET_BIT(SCB->SCR, ((uint32_t)SCB_SCR_SEVONPEND_Msk));
}



void HAL_PWR_DisableSEVOnPend(void)
{
        /* Clear SEVONPEND bit of Cortex System Control Register */
  CLEAR_BIT(SCB->SCR, ((uint32_t)SCB_SCR_SEVONPEND_Msk));
}

#if defined(PWR_PVD_SUPPORT)
/* @brief  This function handles the PWR PVD interrupt request. @note   This API should be called under the PVD_IRQHandler(). @retval None */
void HAL_PWR_PVD_IRQHandler(void)
{
        /* Check PWR exti Rising flag */
  if(__HAL_PWR_PVD_EXTI_GET_FLAG() != 0x0U)
  {
                /* Clear PVD exti pending bit */
    __HAL_PWR_PVD_EXTI_CLEAR_FLAG();

                /* PWR PVD interrupt rising user callback */
    HAL_PWR_PVD_Callback();
  }
}

/* @brief  PWR PVD interrupt callback @retval None */
__weak void HAL_PWR_PVD_Callback(void)
{

}



#endif

/* @} */

/* @} */

#endif /* HAL_PWR_MODULE_ENABLED */
/* @} */

/* @} */

/* (C) COPYRIGHT Puya *****END OF FILE */
