#include "tim.h"
#include "adc.h"
TIM_HandleTypeDef             htim3;
TIM_MasterConfigTypeDef       sMasterConfig;

/**
 * @brief  TIM configuration function 50ms
 * @param  None
 * @retval None
 */
    // 10ms
void MX_TIM1_Init(void)
{
  __HAL_RCC_TIM3_CLK_ENABLE();                                            /**
   * Enable TIM1 clock
   */
  htim3.Instance = TIM3;                                              /* TIM1 */
  htim3.Init.Period            = 1000 - 1;                            /* TIM1 reload value */
  htim3.Init.Prescaler         = 480 - 1;                            /* Prescaler value */
  htim3.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;              /* No clock division */
  htim3.Init.CounterMode       = TIM_COUNTERMODE_UP;                  /* Up-counting mode */
  htim3.Init.RepetitionCounter = 0;                                   /* No repetition */
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;      /* Auto-reload register not buffered */
        /* Initialize TIM1 */
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)                        
  {
    APP_ErrorHandler();
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;                    /* Select update event as trigger source */
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;            /* No effect in master/slave mode */
  HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig);      /* Configure TIM1 */
        /* Start TIM1 clock */
  if (HAL_TIM_Base_Start(&htim3) != HAL_OK)                       
  {
    APP_ErrorHandler();
  }
  
                /* Start ADC */
  if (HAL_ADC_Start_DMA(&hadc, (uint32_t *)aADCxConvertedData, 6) != HAL_OK)            
  {
    APP_ErrorHandler();
  }
}
