/**
 * @file    py32f0xx_hal_msp.c
 * @author  MCU Application Team
 * @brief   This file provides code for the MSP Initialization
 * @attention
 */

/* Includes ------------------------------------------------------------------ */
#include "board.h"

DMA_HandleTypeDef hdma_adc;

/* Private typedef ----------------------------------------------------------- */
/* Private define ------------------------------------------------------------ */
/* Private macro ------------------------------------------------------------- */
/* Private variables --------------------------------------------------------- */
/* Private function prototypes ----------------------------------------------- */
/* External functions -------------------------------------------------------- */

/* @brief Initialize global MSP */
void HAL_MspInit(void)
{
	
	__HAL_RCC_SYSCFG_CLK_ENABLE();
	__HAL_RCC_PWR_CLK_ENABLE();
}

/* @brief Initialize ADC-related MSP */
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
  GPIO_InitTypeDef          GPIO_InitStruct={0};

  __HAL_RCC_SYSCFG_CLK_ENABLE();                                  /* Enable SYSCFG clock */
  __HAL_RCC_DMA_CLK_ENABLE();                                     /* Enable DMA clock */
  __HAL_RCC_GPIOA_CLK_ENABLE();                                   /* Enable GPIOA clock */
  __HAL_RCC_ADC_CLK_ENABLE();                                     /* Enable ADC clock */
  __HAL_RCC_GPIOB_CLK_ENABLE();
        /* ---------------- */
        /* Configure ADC channel as PA0 */
        /* ---------------- */
  GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1 ;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);


  
        /* ---------------------- */
        /* DMA configuration */
        /* ---------------------- */
  hdma_adc.Instance                 = DMA1_Channel1;               /* Select DMA channel 1 */
  hdma_adc.Init.Direction           = DMA_PERIPH_TO_MEMORY;        /* Direction: Peripheral to memory */
  hdma_adc.Init.PeriphInc           = DMA_PINC_DISABLE;            /* Disable peripheral address increment */
  hdma_adc.Init.MemInc              = DMA_MINC_ENABLE;            /* Disable memory address increment */
  hdma_adc.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;         /* Peripheral data width: 32-bit */
  hdma_adc.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;         /* Memory data width: 32-bit */
  hdma_adc.Init.Mode                = DMA_CIRCULAR;                /* Circular mode */
  hdma_adc.Init.Priority            = DMA_PRIORITY_VERY_HIGH;      /* Channel priority: Very high */

  HAL_DMA_DeInit(&hdma_adc);                                       /* DMA deinitialization */
  HAL_DMA_Init(&hdma_adc);                                         /* Initialize DMA channel 1 */
  __HAL_LINKDMA(hadc, DMA_Handle, hdma_adc);                       /* Link DMA handle */
  // __HAL_DMA_ENABLE_IT(&hdma_adc,DMA_IT_TC);
// HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
// HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}

/* (C) COPYRIGHT Puya *****END OF FILE */
