#include "board.h"

/**
 * @brief  错误执行函数
 * @param  无
 * @retval 无
 */
void APP_ErrorHandler(void)
{
        /**
     * 无限循环
     */
  while (1)
  {
		
  }
}

/**
 * @brief  系统时钟配置函数 内部HSI倍频 主频48M
 * @param  无
 * @retval 无
 */
void APP_SystemClockConfig(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

        /**
     * 振荡器配置
     */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;     /* 选择振荡器HSE,HSI,LSI,LSE（外部低速时钟源） */
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;                              /* 开启HSI（内部高速时钟源） */
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;                              /* HSI 1分频 */
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_24MHz;      /* 配置HSI时钟24MHz */
  RCC_OscInitStruct.HSEState = RCC_HSE_OFF;                             /* 关闭HSE（外部高速时钟源） */
    // RCC_OscInitStruct.HSEFreq = RCC_HSE_16_32MHz;
  RCC_OscInitStruct.LSIState = RCC_LSI_OFF;                             /* 关闭LSI（内部低速时钟源） */
  RCC_OscInitStruct.LSEState = RCC_LSE_OFF;                             /* 关闭LSE */
        /* RCC_OscInitStruct.LSEDriver = RCC_LSEDRIVE_MEDIUM; */
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;                         /* 开启PLL（锁相环倍频电路） */
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
        /* 配置振荡器 */
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    APP_ErrorHandler();
  }

        /* 时钟源配置 */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;     /* 选择配置时钟 HCLK,SYSCLK,PCLK1（APB1 外设时钟） */
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;     /* 选择PLLCLK作为系统时钟 */
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;         /* AHB时钟 1分频 */
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;          /* APB时钟 1分频 */
        /* 配置时钟源 */
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    APP_ErrorHandler();
  }
}

void init_random_seed(void) {
    uint32_t seed = 0;
    
        // 1. 使用 SysTick 计数值
    seed = SysTick->VAL;
    
        // 2. 结合系统启动时间（如果已初始化）
    if (HAL_GetTick() > 0) {
        seed ^= HAL_GetTick();
    }
    
        // 3. 添加 ADC 噪声（如果配置了 ADC）
        // seed ^= get_adc_noise(); // 参考之前的 ADC 噪声采样函数
    
        // 初始化随机数生成器
    srand(seed);
    
        // 丢弃前几个值以提高随机性
    for (int i = 0; i < 5; i++) {
        rand();
    }
}
