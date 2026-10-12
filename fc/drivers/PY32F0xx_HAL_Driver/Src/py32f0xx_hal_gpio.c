/**
 * @file    py32f0xx_hal_gpio.c
 * @author  MCU Application Team
 * @brief   GPIO HAL module driver.
 */

/* Includes ------------------------------------------------------------------ */
#include "py32f0xx_hal.h"

/* @addtogroup PY32F0xx_HAL_Driver @{ */

/* @addtogroup GPIO @{ */
/* range of the shift operator in following API : HAL_GPIO_Init HAL_GPIO_DeInit */

#ifdef HAL_GPIO_MODULE_ENABLED

/* Private typedef ----------------------------------------------------------- */
/* Private defines ------------------------------------------------------------ */
/* @defgroup GPIO_Private_Constants GPIO Private Constants @{ */
#define GPIO_MODE             (0x00000003u)
#define EXTI_MODE             (0x10000000u)
#define GPIO_MODE_IT          (0x00010000u)
#define GPIO_MODE_EVT         (0x00020000u)
#define RISING_EDGE           (0x00100000u)
#define FALLING_EDGE          (0x00200000u)
#define GPIO_OUTPUT_TYPE      (0x00000010u)

#define GPIO_NUMBER           (16u)
/* @} */

#define __GPIO_LOCK_KEY_Msk   (0x00030000U)
/* Private macros ------------------------------------------------------------ */
/* Private variables --------------------------------------------------------- */
/* Private function prototypes ----------------------------------------------- */
/* Exported functions -------------------------------------------------------- */

/* @addtogroup GPIO_Exported_Functions @{ */



void HAL_GPIO_Init(GPIO_TypeDef  *GPIOx, GPIO_InitTypeDef *GPIO_Init)
{
  uint32_t position = 0x00u;
  uint32_t iocurrent;
  uint32_t temp;

        /* Check the parameters */
  assert_param(IS_GPIO_ALL_INSTANCE(GPIOx));
  assert_param(IS_GPIO_PIN(GPIO_Init->Pin));
  assert_param(IS_GPIO_MODE(GPIO_Init->Mode));
  assert_param(IS_GPIO_PULL(GPIO_Init->Pull));

        /* Configure the port pins */
  while (((GPIO_Init->Pin) >> position) != 0x00u)
  {
                /* Get current io position */
    iocurrent = (GPIO_Init->Pin) & (1uL << position);

    if (iocurrent != 0x00u)
    {
                        /* --------------------- GPIO Mode Configuration ------------------------ */
                        /* In case of Alternate function mode selection */
      if ((GPIO_Init->Mode == GPIO_MODE_AF_PP) || (GPIO_Init->Mode == GPIO_MODE_AF_OD))
      {
                                /* Check the Alternate function parameters */
        assert_param(IS_GPIO_AF_INSTANCE(GPIOx));
        assert_param(IS_GPIO_AF(GPIO_Init->Alternate));

                                /* Configure Alternate function mapped with the current IO */
        temp = GPIOx->AFR[position >> 3u];
        temp &= ~(0xFu << ((position & 0x07u) * 4u));
        temp |= ((GPIO_Init->Alternate) << ((position & 0x07u) * 4u));
        GPIOx->AFR[position >> 3u] = temp;
      }

                        /* Configure IO Direction mode (Input, Output, Alternate or Analog) */
      temp = GPIOx->MODER;
      temp &= ~(GPIO_MODER_MODE0 << (position * 2u));
      temp |= ((GPIO_Init->Mode & GPIO_MODE) << (position * 2u));
      GPIOx->MODER = temp;

                        /* In case of Output or Alternate function mode selection */
      if ((GPIO_Init->Mode == GPIO_MODE_OUTPUT_PP) || (GPIO_Init->Mode == GPIO_MODE_AF_PP) ||
          (GPIO_Init->Mode == GPIO_MODE_OUTPUT_OD) || (GPIO_Init->Mode == GPIO_MODE_AF_OD))
      {
                                /* Check the Speed parameter */
        assert_param(IS_GPIO_SPEED(GPIO_Init->Speed));
                                /* Configure the IO Speed */
        temp = GPIOx->OSPEEDR;
        temp &= ~(GPIO_OSPEEDR_OSPEED0 << (position * 2u));
        temp |= (GPIO_Init->Speed << (position * 2u));
        GPIOx->OSPEEDR = temp;

                                /* Configure the IO Output Type */
        temp = GPIOx->OTYPER;
        temp &= ~(GPIO_OTYPER_OT0 << position) ;
        temp |= (((GPIO_Init->Mode & GPIO_OUTPUT_TYPE) >> 4u) << position);
        GPIOx->OTYPER = temp;
      }

                        /* Activate the Pull-up or Pull down resistor for the current IO */
      temp = GPIOx->PUPDR;
      temp &= ~(GPIO_PUPDR_PUPD0 << (position * 2u));
      temp |= ((GPIO_Init->Pull) << (position * 2u));
      GPIOx->PUPDR = temp;

                        /* --------------------- EXTI Mode Configuration ------------------------ */
                        /* Configure the External Interrupt or event for the current IO */
      if ((GPIO_Init->Mode & EXTI_MODE) == EXTI_MODE)
      {
        temp = EXTI->EXTICR[position >> 2u];
        temp &= ~(0x0FuL << (8u * (position & 0x03u)));
        temp |= (GPIO_GET_INDEX(GPIOx) << (8u * (position & 0x03u)));
        EXTI->EXTICR[position >> 2u] = temp;

                                /* Clear EXTI line configuration */
        temp = EXTI->IMR;
        temp &= ~(iocurrent);
        if ((GPIO_Init->Mode & GPIO_MODE_IT) == GPIO_MODE_IT)
        {
          temp |= iocurrent;
        }
        EXTI->IMR = temp;

        temp = EXTI->EMR;
        temp &= ~(iocurrent);
        if ((GPIO_Init->Mode & GPIO_MODE_EVT) == GPIO_MODE_EVT)
        {
          temp |= iocurrent;
        }
        EXTI->EMR = temp;

                                /* Clear Rising Falling edge configuration */
        temp = EXTI->RTSR;
        temp &= ~(iocurrent);
        if ((GPIO_Init->Mode & RISING_EDGE) == RISING_EDGE)
        {
          temp |= iocurrent;
        }
        EXTI->RTSR = temp;

        temp = EXTI->FTSR;
        temp &= ~(iocurrent);
        if ((GPIO_Init->Mode & FALLING_EDGE) == FALLING_EDGE)
        {
          temp |= iocurrent;
        }
        EXTI->FTSR = temp;
      }
    }

    position++;
  }
}



void HAL_GPIO_DeInit(GPIO_TypeDef  *GPIOx, uint32_t GPIO_Pin)
{
  uint32_t position = 0x00u;
  uint32_t iocurrent;
  uint32_t tmp;

        /* Check the parameters */
  assert_param(IS_GPIO_ALL_INSTANCE(GPIOx));
  assert_param(IS_GPIO_PIN(GPIO_Pin));

        /* Configure the port pins */
  while ((GPIO_Pin >> position) != 0x00u)
  {
                /* Get current io position */
    iocurrent = (GPIO_Pin) & (1uL << position);

    if (iocurrent != 0x00u)
    {
                        /* ------------------------- EXTI Mode Configuration -------------------- */
                        /* Clear the External Interrupt or Event for the current IO */

      tmp = EXTI->EXTICR[position >> 2u];
      tmp &= (0x0FuL << (8u * (position & 0x03u)));
      if (tmp == (GPIO_GET_INDEX(GPIOx) << (8u * (position & 0x03u))))
      {
                                /* Clear EXTI line configuration */
        EXTI->IMR &= ~(iocurrent);
        EXTI->EMR &= ~(iocurrent);

                                /* Clear Rising Falling edge configuration */
        EXTI->RTSR &= ~(iocurrent);
        EXTI->FTSR &= ~(iocurrent);

        tmp = 0x0FuL << (8u * (position & 0x03u));
        EXTI->EXTICR[position >> 2u] &= ~tmp;
      }

                        /* ------------------------- GPIO Mode Configuration -------------------- */
                        /* Configure IO in Analog Mode */
      GPIOx->MODER |= (GPIO_MODER_MODE0 << (position * 2u));

                        /* Configure the default Alternate Function in current IO */
      GPIOx->AFR[position >> 3u] &= ~(0xFu << ((position & 0x07u) * 4u)) ;

                        /* Configure the default value for IO Speed */
      GPIOx->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED0 << (position * 2u));

                        /* Configure the default value IO Output Type */
      GPIOx->OTYPER  &= ~(GPIO_OTYPER_OT0 << position) ;

                        /* Deactivate the Pull-up and Pull-down resistor for the current IO */
      GPIOx->PUPDR &= ~(GPIO_PUPDR_PUPD0 << (position * 2u));
    }

    position++;
  }
}

/* @} */



GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
  GPIO_PinState bitstatus;

        /* Check the parameters */
  assert_param(IS_GPIO_PIN(GPIO_Pin));

  if ((GPIOx->IDR & GPIO_Pin) != 0x00u)
  {
    bitstatus = GPIO_PIN_SET;
  }
  else
  {
    bitstatus = GPIO_PIN_RESET;
  }
  return bitstatus;
}



void HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState)
{
        /* Check the parameters */
  assert_param(IS_GPIO_PIN(GPIO_Pin));
  assert_param(IS_GPIO_PIN_ACTION(PinState));

  if (PinState != GPIO_PIN_RESET)
  {
    GPIOx->BSRR = (uint32_t)GPIO_Pin;
  }
  else
  {
    GPIOx->BRR = (uint32_t)GPIO_Pin;
  }
}



void HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
        /* Check the parameters */
  assert_param(IS_GPIO_PIN(GPIO_Pin));

  if ((GPIOx->ODR & GPIO_Pin) != 0x00u)
  {
    GPIOx->BRR = (uint32_t)GPIO_Pin;
  }
  else
  {
    GPIOx->BSRR = (uint32_t)GPIO_Pin;
  }
}



HAL_StatusTypeDef HAL_GPIO_LockPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
  __IO uint32_t tmp = GPIO_LCKR_LCKK;

        /* Check the parameters */
  assert_param(IS_GPIO_LOCK_INSTANCE(GPIOx));
  assert_param(IS_GPIO_PIN(GPIO_Pin));

        /* Apply lock key write sequence */
  tmp |= GPIO_Pin;
        /* Set LCKx bit(s): LCKK='1' + LCK[15-0] */
  GPIOx->LCKR = tmp;
        /* Reset LCKx bit(s): LCKK='0' + LCK[15-0] */
  GPIOx->LCKR = GPIO_Pin;
        /* Set LCKx bit(s): LCKK='1' + LCK[15-0] */
  GPIOx->LCKR = tmp;
        /* Read LCKK register. This read is mandatory to complete key lock sequence */
  tmp = GPIOx->LCKR;

        /* read again in order to confirm lock is active */
  if ((GPIOx->LCKR & __GPIO_LOCK_KEY_Msk) != 0x00u)
  {
    return HAL_OK;
  }
  else
  {
    return HAL_ERROR;
  }
}
__weak void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
        /* Prevent unused argument(s) compilation warning */
  UNUSED(GPIO_Pin);



}
/* @brief  Handle EXTI interrupt request. @param  GPIO_Pin Specifies the port pin connected to corresponding EXTI line. @retval None */
void HAL_GPIO_EXTI_IRQHandler(uint16_t GPIO_Pin)
{
        /* EXTI line interrupt detected */
  if(__HAL_GPIO_EXTI_GET_IT(GPIO_Pin) != 0x00u)
  {
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);
    HAL_GPIO_EXTI_Callback(GPIO_Pin);
  }
}



/* @} */



/* @} */

#endif /* HAL_GPIO_MODULE_ENABLED */
/* @} */

/* @} */

/* (C) COPYRIGHT Puya *****END OF FILE */
