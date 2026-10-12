#include "adc.h"

ADC_HandleTypeDef	hadc;
uint16_t	aADCxConvertedData[ADC_SAMPLE_NUM];

// #define ADC_SAMPLE_NUM	10
// uint16_t adc_value[5*ADC_SAMPLE_NUM];//ADC采集值存放缓冲区
/**
 * @brief  ADC configuration function
 * @param  None
 * @retval None
 */
void MX_ADC_Init(void)
{
	ADC_ChannelConfTypeDef sConfig = {0};
	__HAL_RCC_ADC_FORCE_RESET();
	__HAL_RCC_ADC_RELEASE_RESET();                                                  		/* Reset ADC */
	__HAL_RCC_ADC_CLK_ENABLE();                                                     		/* Enable ADC clock */

  hadc.Instance = ADC1;
  
  hadc.Instance                   = ADC1;                                        /* ADC1 */
  hadc.Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV4;                    /* Set ADC clock */
  hadc.Init.Resolution            = ADC_RESOLUTION_12B;                          /* 12-bit resolution for converted data */
  hadc.Init.DataAlign             = ADC_DATAALIGN_RIGHT;                         /* Right-alignment for converted data */
  hadc.Init.ScanConvMode          = ADC_SCAN_ENABLE;                 /* Scan sequence direction: backward */
  hadc.Init.EOCSelection          = ADC_EOC_SEQ_CONV;
  hadc.Init.LowPowerAutoWait      = ENABLE;                                      /* Enable wait for conversion mode */
  hadc.Init.ContinuousConvMode    = DISABLE;                                     /* Single conversion mode */
  hadc.Init.DiscontinuousConvMode = DISABLE;                                     /* Disable discontinuous mode */
  hadc.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T3_TRGO;                /* Set the external trigger for conversion start event to TIM1_TRGO */
  hadc.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_RISING;             /* Trigger on rising edges */
  hadc.Init.DMAContinuousRequests = ENABLE;                                      /* DMA continuous mode selection */
  hadc.Init.Overrun               = ADC_OVR_DATA_OVERWRITTEN;                    /* When an overload occurs, overwrite the previous value */
  hadc.Init.SamplingTimeCommon    = ADC_SAMPLETIME_71CYCLES_5;                  /* The channel sampling time is 239.5 ADC（模数转换器） clock cycles */
        /* ADC initialization */
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    APP_ErrorHandler();
  }
  
        /* ADC calibration */
  if (HAL_ADCEx_Calibration_Start(&hadc) != HAL_OK)                          
  {
    APP_ErrorHandler();
  }


  
  sConfig.Channel      = ADC_CHANNEL_5;
  sConfig.Rank         = ADC_RANK_CHANNEL_NUMBER;
// sConfig.SamplingTime = ADC_SAMPLETIME_71CYCLES_5;
        /* Channel 0 configuration */
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)                     
  {
    APP_ErrorHandler();
  }
  
  sConfig.Channel      = ADC_CHANNEL_6;
  sConfig.Rank         = ADC_RANK_CHANNEL_NUMBER;
// sConfig.SamplingTime = ADC_SAMPLETIME_71CYCLES_5;
        /* Channel 0 configuration */
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)                     
  {
    APP_ErrorHandler();
  }
  
   sConfig.Channel      = ADC_CHANNEL_7;
  sConfig.Rank         = ADC_RANK_CHANNEL_NUMBER;
// sConfig.SamplingTime = ADC_SAMPLETIME_71CYCLES_5;
        /* Channel 0 configuration */
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)                     
  {
    APP_ErrorHandler();
  }
  
   sConfig.Channel      = ADC_CHANNEL_8;
  sConfig.Rank         = ADC_RANK_CHANNEL_NUMBER;
// sConfig.SamplingTime = ADC_SAMPLETIME_71CYCLES_5;
        /* Channel 0 configuration */
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)                     
  {
    APP_ErrorHandler();
  }
  
     sConfig.Channel      = ADC_CHANNEL_9;
  sConfig.Rank         = ADC_RANK_CHANNEL_NUMBER;
// sConfig.SamplingTime = ADC_SAMPLETIME_71CYCLES_5;
        /* Channel 0 configuration */
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)                     
  {
    APP_ErrorHandler();
  }


  
   sConfig.Channel = ADC_CHANNEL_VREFINT;                                       /* ADC channel selection */
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;                                    /* Set the rank for the ADC channel order */
 
        /* Configure ADC channels */
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)            
  {
    APP_ErrorHandler();
  }
}

// //ADC均值滤波
// void ADC_Average_Filter(uint16_t* adc_val)
// {
// uint16_t i=0;
// uint32_t sum[5]={0,0,0,0};

// for(;i<ADC_SAMPLE_NUM;i++)
// {
// sum[0]+=adc_value[5*i+0];
// sum[1]+=adc_value[5*i+1];
// sum[2]+=adc_value[5*i+2];
// sum[3]+=adc_value[5*i+3];
// sum[4]+=adc_value[5*i+4];
// }
// adc_val[0]=sum[0]/ADC_SAMPLE_NUM;
// adc_val[1]=sum[1]/ADC_SAMPLE_NUM;
// adc_val[2]=sum[2]/ADC_SAMPLE_NUM;
// adc_val[3]=sum[3]/ADC_SAMPLE_NUM;
// adc_val[4]=sum[4]/ADC_SAMPLE_NUM;
// }



// uint16_t getAdcValue(uint8_t axis)
// {
// uint32_t sum=0;
// for(uint8_t i=0;i<ADC_SAMPLE_NUM;i++)
// {
// sum += adc_value[5*i+axis];
// }
// return sum/ADC_SAMPLE_NUM;
// }

