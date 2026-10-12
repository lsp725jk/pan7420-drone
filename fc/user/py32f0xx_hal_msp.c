/**
 * @file    py32f0xx_hal_msp.c
 * @author  MCU Application Team
 * @brief   This file provides code for the MSP Initialization
 * @attention
 */

/* 一、这个文件在项目里干什么 HAL 库把每个外设的初始化拆成两半： - 外设寄存器部分（分辨率、波特率、周期…）由 HAL_xxx_Init() 负责； - 芯片相关部分（开哪个 RCC 时钟、用哪些引脚、什么复用功能）由 MSP 回调负责。 */
/* Includes ------------------------------------------------------------------ */
#include "main.h"
#include "bsp.h"

/* Private typedef ----------------------------------------------------------- */
/* Private define ------------------------------------------------------------ */
/* Private macro ------------------------------------------------------------- */
/* Private variables --------------------------------------------------------- */
/* Private function prototypes ----------------------------------------------- */
/* External functions -------------------------------------------------------- */

/* @brief  全局 MSP 初始化（HAL_Init() 内部回调，整个程序最早的外设动作之一） 【参数】无 【返回】无 【为什么开这两个时钟】 SYSCFG（系统配置）：HAL 用它做中断向量重映射、EXTI 线映射等； */
void HAL_MspInit(void)
{
	__HAL_RCC_SYSCFG_CLK_ENABLE();
	__HAL_RCC_PWR_CLK_ENABLE();
}



/* @brief  SPI1 的 MSP：开时钟 + 配置 SCK/MISO/MOSI 与两个片选脚 【参数】hspi：SPI 句柄（靠 hspi->Instance 判断是哪个 SPI） 【返回】无 【本工程的引脚分配（以代码为准）】 */
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    GPIO_InitTypeDef  GPIO_InitStruct;

                /* SPI1 初始化 */
    if (hspi->Instance == SPI1)
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();                                   /* GPIOB时钟使能 */
        __HAL_RCC_GPIOA_CLK_ENABLE();                                   /* GPIOA时钟使能 */
        __HAL_RCC_SPI1_CLK_ENABLE();                                    /* SPI1时钟使能 */
                                /* PA5-SCK  (AF0) PA6-MISO(AF0) PA7-MOSI(AF0) PA4-NSS1_ICM42688(AF0) */
                                /* SCK */
                                /* 注意这里与下面两段的模式：AF_PP（复用推挽）是 SPI 主机的标准配置； MISO 也配成 AF_PP（由外设接管方向），不要手工改成输入模式 */
        GPIO_InitStruct.Pin       = GPIO_PIN_5;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF0_SPI1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
	        	        	        	        /* GPIO配置为SPI（串行外设接口）：MISO（SPI 主入从出数据线）/MOSI（SPI 主出从入数据线） */
        GPIO_InitStruct.Pin       = GPIO_PIN_7;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF0_SPI1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		
				GPIO_InitStruct.Pin       = GPIO_PIN_6 ;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF0_SPI1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


	
                                /* SPI ICM42688NSS */
                                /* 片选 1：PA8，普通推挽输出 + 上拉 + 最高速度。 上拉的作用：MCU 复位/未初始化期间引脚悬空时，片选保持高电平（不选中）， 避免上电瞬间陀螺仪被误选中而在总线上乱输出 */
        GPIO_InitStruct.Pin = GPIO_PIN_8;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
																/* SPI BMP390NSS */
                                /* 片选 2：PB7（注意端口是 GPIOB，与上面的 PA8 不同端口）。 与 spi.h 里的 SPI_BMP390_CS_LOW/HIGH 宏必须一致 */
        GPIO_InitStruct.Pin = GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
}



/* @brief       定时器底层驱动，开启时钟，设置中断优先级 此函数会被HAL_TIM_PWM_Init函数调用 @param       htim:定时器句柄 @retval      无 */

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
if (htim->Instance == TIM1)
	{
		GPIO_InitTypeDef	Advanced_TIM1_PWM_Channal_GPIO_Config;
		
								/* 使能TIM时钟 */
		__HAL_RCC_TIM1_CLK_ENABLE();	
								/* PWM Output Channal GPIO时钟使能 */	

    Advanced_TIM1_PWM_CH1_GPIO_CLK_ENABLE();
		Advanced_TIM1_PWM_CH2_GPIO_CLK_ENABLE();
		Advanced_TIM1_PWM_CH3_GPIO_CLK_ENABLE();




		
		Advanced_TIM1_PWM_Channal_GPIO_Config.Mode = GPIO_MODE_AF_PP;
		Advanced_TIM1_PWM_Channal_GPIO_Config.Speed = GPIO_SPEED_FREQ_HIGH;
		Advanced_TIM1_PWM_Channal_GPIO_Config.Pull = GPIO_NOPULL;
		
                /* PWM Output Channal 1 */
		Advanced_TIM1_PWM_Channal_GPIO_Config.Pin = Advanced_TIM1_PWM_CH1_GPIO_PIN;
		Advanced_TIM1_PWM_Channal_GPIO_Config.Alternate = Advanced_TIM1_PWM_CH1_GPIO_AF;
		HAL_GPIO_Init(Advanced_TIM1_PWM_CH1_GPIO_PORT, &Advanced_TIM1_PWM_Channal_GPIO_Config);
        
								/* PWM Output Channal 2 */
		Advanced_TIM1_PWM_Channal_GPIO_Config.Pin = Advanced_TIM1_PWM_CH2_GPIO_PIN;
		Advanced_TIM1_PWM_Channal_GPIO_Config.Alternate = Advanced_TIM1_PWM_CH2_GPIO_AF;
		HAL_GPIO_Init(Advanced_TIM1_PWM_CH2_GPIO_PORT, &Advanced_TIM1_PWM_Channal_GPIO_Config);

								/* PWM Output Channal 3 */
		Advanced_TIM1_PWM_Channal_GPIO_Config.Pin = Advanced_TIM1_PWM_CH3_GPIO_PIN;
		Advanced_TIM1_PWM_Channal_GPIO_Config.Alternate = Advanced_TIM1_PWM_CH3_GPIO_AF;
		HAL_GPIO_Init(Advanced_TIM1_PWM_CH3_GPIO_PORT, &Advanced_TIM1_PWM_Channal_GPIO_Config);
		
	}	

	if (htim->Instance == TIM3)
	{
		GPIO_InitTypeDef	Advanced_TIM3_PWM_Channal_GPIO_Config;
								/* 使能TIM时钟 */
		__HAL_RCC_TIM3_CLK_ENABLE();	
								/* PWM Output Channal GPIO时钟使能 */
		Advanced_TIM3_PWM_CH1_GPIO_CLK_ENABLE();
		
		Advanced_TIM3_PWM_Channal_GPIO_Config.Mode = GPIO_MODE_AF_PP;
		Advanced_TIM3_PWM_Channal_GPIO_Config.Speed = GPIO_SPEED_FREQ_HIGH;
		Advanced_TIM3_PWM_Channal_GPIO_Config.Pull = GPIO_NOPULL;
		
								/* PWM Output Channal 1 */
		Advanced_TIM3_PWM_Channal_GPIO_Config.Pin = Advanced_TIM3_PWM_CH1_GPIO_PIN;
		Advanced_TIM3_PWM_Channal_GPIO_Config.Alternate = Advanced_TIM3_PWM_CH1_GPIO_AF;
		HAL_GPIO_Init(Advanced_TIM3_PWM_CH1_GPIO_PORT, &Advanced_TIM3_PWM_Channal_GPIO_Config);
		
								/* PWM Output Channal 2 */
		Advanced_TIM3_PWM_Channal_GPIO_Config.Pin = Advanced_TIM3_PWM_CH2_GPIO_PIN;
		Advanced_TIM3_PWM_Channal_GPIO_Config.Alternate = Advanced_TIM3_PWM_CH2_GPIO_AF;
		HAL_GPIO_Init(Advanced_TIM3_PWM_CH2_GPIO_PORT, &Advanced_TIM3_PWM_Channal_GPIO_Config);
		
								/* PWM Output Channal 3 */
		Advanced_TIM3_PWM_Channal_GPIO_Config.Pin = Advanced_TIM3_PWM_CH3_GPIO_PIN;
		Advanced_TIM3_PWM_Channal_GPIO_Config.Alternate = Advanced_TIM3_PWM_CH3_GPIO_AF;
		HAL_GPIO_Init(Advanced_TIM3_PWM_CH3_GPIO_PORT, &Advanced_TIM3_PWM_Channal_GPIO_Config);
		
								/* PWM Output Channal 4 */
		Advanced_TIM3_PWM_Channal_GPIO_Config.Pin = Advanced_TIM3_PWM_CH4_GPIO_PIN;
		Advanced_TIM3_PWM_Channal_GPIO_Config.Alternate = Advanced_TIM3_PWM_CH4_GPIO_AF;
		HAL_GPIO_Init(Advanced_TIM3_PWM_CH4_GPIO_PORT, &Advanced_TIM3_PWM_Channal_GPIO_Config);
	}
}

/* (C) COPYRIGHT Puya *****END OF FILE */
