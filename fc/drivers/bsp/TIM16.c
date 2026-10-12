/**
 * 文件：drivers/bsp/TIM16.c               【角色】两个定时器的硬件配置：
 */
#include "bsp.h"
/**
 * 定时器x句柄
 */

TIM_HandleTypeDef    TimHandle_17;     
TIM_HandleTypeDef    TimHandle_16;

/**
 * @brief  配置并启动 TIM16：一个 1 微秒一步的自由计数器（不开中断） 【参数】 无 【返回】 无 【算法：1us 是怎么算出来的】 TIM16 挂在 APB1 上，时钟与内核同为 48MHz（SystemClock_Config 里
 */
void tim16_init(void)
{
	__HAL_RCC_TIM16_CLK_ENABLE();
	TimHandle_16.Instance = TIM16;                                          		/* Select TIM16 */
  TimHandle_16.Init.Period            = 0xFFFF;                         	     /* Auto-reload value */
  TimHandle_16.Init.Prescaler         = 48 - 1;                         	     /* Prescaler of 1000-1 */
  TimHandle_16.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;               /* Clock not divided */
  TimHandle_16.Init.CounterMode       = TIM_COUNTERMODE_UP;                   /* Up counting mode */
  TimHandle_16.Init.RepetitionCounter = 0;                                /* No repetition */


  
	if (HAL_TIM_Base_Init(&TimHandle_16) != HAL_OK)                         		/* Initialize TIM16 */
  {
    Error_Handler();
  }
        /* Start TIM16 and enable interrupt */
  if (HAL_TIM_Base_Start(&TimHandle_16) != HAL_OK)
  {
    Error_Handler();
  }
}



// 返回上电后的微秒计时 (16位值)
/* @brief  读 TIM16 的当前计数值，直接当作“微秒时间戳”（16 位，会回绕） 【参数】 无 【返回】 0~65535 的计数值，单位微秒 【算法/为什么可以直接当微秒用】 */
uint16_t micros16(void)
{
    /* 直接读取 TIM16 计数器值。 【语法要点：TIM16->CNT 为什么每次都真的去读硬件】 HAL/CMSIS 把外设寄存器结构体的成员都声明成了 volatile： typedef struct { __IO uint16_t CNT; ... } TIM_TypeDef; */
  return TIM16->CNT;
}

// 返回上电后的微秒计时 (32位值，处理溢出)
/* @brief  用软件把 16 位计数扩展成 32 位“微秒时间戳”（带溢出累加） 【参数】 无 【返回】 32 位微秒计数（约 71.6 分钟回绕） 【算法】 */
uint32_t micros32(void)
{
  static uint32_t overflow_count = 0;
  static uint16_t last_count = 0;
  
  uint16_t current_count = TIM16->CNT;
  
    // 检测计数器溢出 (回绕)
  if (current_count < last_count) {
    overflow_count++;
  }
  
  last_count = current_count;



  return (overflow_count * 65536UL) + current_count;
}



/* @brief       定时器TIM17更新中断初始化函数 @retval      无 */
/* @brief  配置并启动 TIM17 的 1ms 周期中断 —— 整个飞控的系统节拍源 【参数】 无 【返回】 无 【算法：1ms 是怎么算出来的】 TIM17 同样挂在 48MHz 的 APB1 上： */
void Timer17_UpdateIT_Init(void)
{

		__HAL_RCC_TIM17_CLK_ENABLE();
    TimHandle_17.Instance = TIM17;                                                  /* 选择TIM17 */
    TimHandle_17.Init.Period            = 100 - 1;                                 /* 自动重装载值 */
    TimHandle_17.Init.Prescaler         = 480 - 1;                                 /* 预分频为1000-1 */
    TimHandle_17.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;                   /* 时钟不分频 */
    TimHandle_17.Init.CounterMode       = TIM_COUNTERMODE_UP;                       /* 向上计数 */
    TimHandle_17.Init.RepetitionCounter = 1 - 1;                                    /* 不重复计数 */
    TimHandle_17.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;           /* 自动重装载寄存器没有缓冲 */
		if (HAL_TIM_Base_Init(&TimHandle_17) != HAL_OK)                         				/* Initialize TIM16 */
		{
			Error_Handler();
		}

								/* 设置中断优先级 */
		HAL_NVIC_SetPriority(TIM17_IRQn, 0, 1); 
								/* 使能TIM1中断 */
		HAL_NVIC_EnableIRQ(TIM17_IRQn);
								/* Start TIM16 and enable interrupt */
		if (HAL_TIM_Base_Start_IT(&TimHandle_17) != HAL_OK)
		{
			Error_Handler();
		}
}



