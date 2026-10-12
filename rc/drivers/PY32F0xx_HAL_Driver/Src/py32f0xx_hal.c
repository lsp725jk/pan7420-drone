/**
 * @file    py32f0xx_hal.c
 * @author  MCU Application Team
 * @brief   HAL module driver.
 * @verbatim
 */

/* Includes ------------------------------------------------------------------ */
#include "py32f0xx_hal.h"

/**
 * @addtogroup PY32F0xx_HAL_Driver @{
 */

/**
 * @addtogroup HAL @brief HAL module driver @{
 */

#ifdef HAL_MODULE_ENABLED

/* Private typedef ----------------------------------------------------------- */
/* Private define ------------------------------------------------------------ */

/* @defgroup HAL_Private_Constants HAL Private Constants @{ */
/* @brief PY32F0xx HAL Driver version number */
#define __PY32F0xx_HAL_VERSION_MAIN   (0x01U) /* !< [31:24] main version */
#define __PY32F0xx_HAL_VERSION_SUB1   (0x03U) /* !< [23:16] sub1 version */
#define __PY32F0xx_HAL_VERSION_SUB2   (0x00U) /* !< [15:8]  sub2 version */
#define __PY32F0xx_HAL_VERSION_RC     (0x00U) /* !< [7:0]  release candidate */
#define __PY32F0xx_HAL_VERSION        ((__PY32F0xx_HAL_VERSION_MAIN << 24U)\
                                       |(__PY32F0xx_HAL_VERSION_SUB1 << 16U)\
                                       |(__PY32F0xx_HAL_VERSION_SUB2 << 8U )\
                                       |(__PY32F0xx_HAL_VERSION_RC))

#if defined(VREFBUF)
#define VREFBUF_TIMEOUT_VALUE     10U   /* !<  10 ms */
#endif /* VREFBUF */

/* @} */

/* Private macro ------------------------------------------------------------- */
/* Exported variables -------------------------------------------------------- */
/* @defgroup HAL_Exported_Variables HAL Exported Variables @{ */
__IO uint32_t uwTick;
uint32_t uwTickPrio = (1UL << __NVIC_PRIO_BITS); /* Invalid PRIO */
uint32_t uwTickFreq = HAL_TICK_FREQ_DEFAULT;  /* 1KHz */
/* @} */

/* Private function prototypes ----------------------------------------------- */
/* Exported functions -------------------------------------------------------- */

/* @defgroup HAL_Exported_Functions HAL Exported Functions @{ */



HAL_StatusTypeDef HAL_Init(void)
{
  HAL_StatusTypeDef  status = HAL_OK;

        /* Configure Flash prefetch, Instruction cache */
        /* Default configuration at reset is: */
        /* - Prefetch disabled */
        /* - Instruction cache enabled */

        /* Use SysTick as time base source and configure 1ms tick (default clock after Reset is HSI) */
  if (HAL_InitTick(TICK_INT_PRIORITY) != HAL_OK)
  {
    status = HAL_ERROR;
  }
  else
  {
                /* Init the low level hardware */
    HAL_MspInit();
  }

        /* Return function status */
  return status;
}



HAL_StatusTypeDef HAL_DeInit(void)
{
        /* Reset of all peripherals */
  __HAL_RCC_APB1_FORCE_RESET();
  __HAL_RCC_APB1_RELEASE_RESET();

  __HAL_RCC_APB2_FORCE_RESET();
  __HAL_RCC_APB2_RELEASE_RESET();

  __HAL_RCC_AHB_FORCE_RESET();
  __HAL_RCC_AHB_RELEASE_RESET();

  __HAL_RCC_IOP_FORCE_RESET();
  __HAL_RCC_IOP_RELEASE_RESET();

        /* De-Init the low level hardware */
  HAL_MspDeInit();

        /* Return function status */
  return HAL_OK;
}

/* @brief  Initialize the MSP. @retval None */
__weak void HAL_MspInit(void)
{

}

/* @brief  DeInitializes the MSP. @retval None */
__weak void HAL_MspDeInit(void)
{

}



__weak HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
  HAL_StatusTypeDef  status = HAL_OK;

  if (uwTickFreq != 0U)
  {
                /* Configure the SysTick to have interrupt in 1ms time basis */
    if (HAL_SYSTICK_Config(SystemCoreClock / (1000U /uwTickFreq)) == 0U)
    {
                        /* Configure the SysTick IRQ priority */
      if (TickPriority < (1UL << __NVIC_PRIO_BITS))
      {
        HAL_NVIC_SetPriority(SysTick_IRQn, TickPriority, 0U);
        uwTickPrio = TickPriority;
      }
      else
      {
        status = HAL_ERROR;
      }
    }
    else
    {
      status = HAL_ERROR;
    }
  }
  else
  {
    status = HAL_ERROR;
  }

        /* Return function status */
  return status;
}

/* @} */

/* @defgroup HAL_Exported_Functions_Group2 HAL Control functions @brief    HAL Control functions @verbatim ##### HAL Control functions ##### */



__weak void HAL_IncTick(void)
{
  uwTick += uwTickFreq;
}



__weak uint32_t HAL_GetTick(void)
{
  return uwTick;
}

/* @brief This function returns a tick priority. @retval tick priority */
uint32_t HAL_GetTickPrio(void)
{
  return uwTickPrio;
}

/* @brief Set new tick Freq. @retval status */
HAL_StatusTypeDef HAL_SetTickFreq(uint32_t Freq)
{
  HAL_StatusTypeDef status  = HAL_OK;
  assert_param(IS_TICKFREQ(Freq));

  if (uwTickFreq != Freq)
  {
                /* Apply the new tick Freq */
    status = HAL_InitTick(uwTickPrio);
    if (status == HAL_OK)
    {
      uwTickFreq = Freq;
    }
  }

  return status;
}

/* @brief return tick frequency. @retval tick period in Hz */
uint32_t HAL_GetTickFreq(void)
{
  return uwTickFreq;
}



__weak void HAL_Delay(uint32_t Delay)
{
  uint32_t tickstart = HAL_GetTick();
  uint32_t wait = Delay;

        /* Add a freq to guarantee minimum wait */
  if (wait < HAL_MAX_DELAY)
  {
    wait += (uint32_t)(uwTickFreq);
  }

  while ((HAL_GetTick() - tickstart) < wait)
  {
  }
}



__weak void HAL_SuspendTick(void)
{
        /* Disable SysTick Interrupt */
  CLEAR_BIT(SysTick->CTRL,SysTick_CTRL_TICKINT_Msk);
}



__weak void HAL_ResumeTick(void)
{
        /* Enable SysTick Interrupt */
  SET_BIT(SysTick->CTRL, SysTick_CTRL_TICKINT_Msk);
}



uint32_t HAL_GetHalVersion(void)
{
  return __PY32F0xx_HAL_VERSION;
}

/* @brief  Returns the device revision identifier. @retval Device revision identifier */
uint32_t HAL_GetREVID(void)
{
  return ((DBGMCU->IDCODE & DBGMCU_IDCODE_REV_ID) >> 16U);
}

/* @brief  Returns the device identifier. @retval Device identifier */
uint32_t HAL_GetDEVID(void)
{
  return ((DBGMCU->IDCODE) & DBGMCU_IDCODE_DEV_ID);
}



uint32_t HAL_GetUIDw0(void)
{
  return (READ_REG(*((uint32_t *)UID_BASE)));
}



uint32_t HAL_GetUIDw1(void)
{
  return (READ_REG(*((uint32_t *)(UID_BASE + 4U))));
}



uint32_t HAL_GetUIDw2(void)
{
  return (READ_REG(*((uint32_t *)(UID_BASE + 8U))));
}

/* @} */

/* @defgroup HAL_Exported_Functions_Group3 HAL Debug functions @brief    HAL Debug functions @verbatim ##### HAL Debug functions ##### */

/* @brief  Enable the Debug Module during STOP mode @retval None */
void HAL_DBGMCU_EnableDBGMCUStopMode(void)
{
  SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STOP);
}

/* @brief  Disable the Debug Module during STOP mode @retval None */
void HAL_DBGMCU_DisableDBGMCUStopMode(void)
{
  CLEAR_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STOP);
}



/* @} */



#if (defined(DMA1) || defined(DMA))
void HAL_SYSCFG_DMA_Req(uint32_t Requset)
{
        /* Check the parameter */

  SET_BIT(SYSCFG->CFGR3, Requset);
}
#endif

/* @} */

/* @} */

#endif /* HAL_MODULE_ENABLED */
/* @} */

/* @} */

/* (C) COPYRIGHT Puya *****END OF FILE */
