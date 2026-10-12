/**
 * @file    py32f0xx_it.c
 * @author  MCU Application Team
 * @brief   Interrupt Service Routines.
 * @attention
 */

/* Includes ------------------------------------------------------------------ */
#include "board.h"
#include "py32f0xx_it.h"
// extern TIM_HandleTypeDef sTimxHandle;
extern DMA_HandleTypeDef hdma_adc;
extern uint16_t aADCxConvertedData[];
extern TIM_HandleTypeDef	htim16;
/* Private includes ---------------------------------------------------------- */
/* Private typedef ----------------------------------------------------------- */
/* Private define ------------------------------------------------------------ */
/* Private macro ------------------------------------------------------------- */
/* Private variables --------------------------------------------------------- */
/* Private function prototypes ----------------------------------------------- */
/* Private user code --------------------------------------------------------- */
/* External variables -------------------------------------------------------- */



/* Cortex-M0+ Processor Interruption and Exception Handlers */

/* @brief This function handles Non maskable interrupt. */
void NMI_Handler(void)
{
}

/* @brief This function handles Hard fault interrupt. */
void HardFault_Handler(void)
{
  while (1)
  {
  }
}

/* @brief This function handles System service call via SWI instruction. */
void SVC_Handler(void)
{
}

/* @brief This function handles Pendable request for system service. */
void PendSV_Handler(void)
{
}

/* @brief This function handles System tick timer. */
void SysTick_Handler(void)
{
  HAL_IncTick();
}

/* @brief This function handles DMA1 channel 1 interrupt. */
void DMA1_Channel1_IRQHandler(void)
{
	
		// printf("ADC1: %d ADC2: %d ADC3: %d ADC4: %d\r\n", aADCxConvertedData[0],aADCxConvertedData[1],aADCxConvertedData[2],aADCxConvertedData[3]);
		// HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
	HAL_DMA_IRQHandler(&hdma_adc);
		// HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
		// printf("ADC1: %d ADC2: %d ADC3: %d ADC4: %d\r\n", aADCxConvertedData[0],aADCxConvertedData[1],aADCxConvertedData[2],aADCxConvertedData[3]);
}

void TIM16_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim16);
}



/* PY32F0xx Peripheral Interrupt Handlers */
/* Add here the Interrupt Handlers for the used peripherals. */
/* For the available peripheral interrupt handler names, */
/* please refer to the startup file. */



/* (C) COPYRIGHT Puya *****END OF FILE */
