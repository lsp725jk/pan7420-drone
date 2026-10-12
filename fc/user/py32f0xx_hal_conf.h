/**
 * @file    py32f0xx_hal_conf.h
 * @author  MCU Application Team
 * @brief   HAL configuration file.
 * @attention
 */

/* ★说明：本次只按约定补充“文件头说明”，不改动文件内任何内容。 一、这个文件在项目里干什么 它是 PY32F0xx HAL 驱动的“总开关面板”，由两大部分组成： 真正参与编译、哪些头文件被包含。被注释掉的模块 = 不编译 = 省 Flash。 */
/* Define to prevent recursive inclusion ------------------------------------- */
#ifndef __PY32F0xx_HAL_CONF_H
#define __PY32F0xx_HAL_CONF_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Exported types ------------------------------------------------------------ */
/* Exported constants -------------------------------------------------------- */

/* ########################## Module Selection ############################## */

#define HAL_MODULE_ENABLED  
#define HAL_RCC_MODULE_ENABLED
#define HAL_ADC_MODULE_ENABLED
/* #define HAL_CRC_MODULE_ENABLED */   
/* #define HAL_COMP_MODULE_ENABLED */  
#define HAL_FLASH_MODULE_ENABLED   
#define HAL_GPIO_MODULE_ENABLED    
#define HAL_IWDG_MODULE_ENABLED   /* 独立看门狗：user/WDG.c 使用（喂狗在 main.c 主循环） */
/* #define HAL_WWDG_MODULE_ENABLED */  /* 窗口看门狗：本工程不用（IWDG 更适合飞控，见 WDG.c 说明） */
#define HAL_TIM_MODULE_ENABLED    /* 定时器：TIM1/TIM3 的 PWM（灯与电机）、TIM16/TIM17 的定时 */
#define HAL_DMA_MODULE_ENABLED    /* DMA（直接存储器访问）：本工程没有直接用，但 HAL 内部部分模块会引用 */
/* #define HAL_LPTIM_MODULE_ENABLED */  
#define HAL_PWR_MODULE_ENABLED
#define HAL_I2C_MODULE_ENABLED    /* 硬件 I2C（两线串行总线）：为气压计预留的备用总线（当前走 SPI（串行外设接口）） */
#define HAL_UART_MODULE_ENABLED   /* 串口：printf 调试输出（bsp_uart.c / bsp_usart.c） */
#define HAL_SPI_MODULE_ENABLED    /* SPI：ICM42688 陀螺仪 + BMP390 气压计共用 SPI1 */
/* #define HAL_RTC_MODULE_ENABLED */   
/* #define HAL_LED_MODULE_ENABLED */ 
/* #define HAL_EXTI_MODULE_ENABLED */
#define HAL_CORTEX_MODULE_ENABLED
  
/* ########################## Oscillator Values adaptation #################### */

#if !defined  (HSI_VALUE) 
  #define HSI_VALUE              ((uint32_t)8000000)         /* !< Value of the Internal oscillator in Hz */
#endif /* HSI_VALUE */



#if !defined  (HSE_VALUE) 
  #define HSE_VALUE              ((uint32_t)24000000)     /* !< Value of the External oscillator in Hz */
#endif /* HSE_VALUE */

#if !defined  (HSE_STARTUP_TIMEOUT)
  #define HSE_STARTUP_TIMEOUT    ((uint32_t)200)       /* !< Time out for HSE start up, in ms */
#endif /* HSE_STARTUP_TIMEOUT */

/* @brief Internal Low Speed Internal oscillator (LSI) value. */
#if !defined  (LSI_VALUE) 
  /* LSI（内部低速 RC）标称 32768Hz：独立看门狗 IWDG 的时钟源。 注意它只是“典型值”，实际会随电压/温度漂移（手册有说明），所以看门狗超时只是近似值， 这正是不把喂狗周期贴着超时时间设的原因（WDG.c 里留了 5 倍余量） */
 #define LSI_VALUE               ((uint32_t)32768)      /* !< LSI Typical Value in Hz */
#endif /* LSI_VALUE */                                /* !< Value of the Internal Low Speed oscillator in Hz The real value may vary depending on the variations in voltage and temperature. */



#if !defined  (LSE_VALUE)
  #define LSE_VALUE              ((uint32_t)32768)     /* !< Value of the External oscillator in Hz */
#endif /* LSE_VALUE */

#if !defined  (LSE_STARTUP_TIMEOUT)
  #define LSE_STARTUP_TIMEOUT    ((uint32_t)5000)       /* !< Time out for LSE start up, in ms */
#endif /* LSE_STARTUP_TIMEOUT */



/* ########################### System Configuration ######################### */
/* @brief This is the HAL system configuration section */     
#define  VDD_VALUE               ((uint32_t)3300) /* !< Value of VDD in mv */           

#define  PRIORITY_HIGHEST        0
#define  PRIORITY_HIGH           1
#define  PRIORITY_LOW            2
#define  PRIORITY_LOWEST         3
/* SysTick 中断优先级设为最低：让外设中断（TIM17 等）能抢占它， 避免节拍中断拖慢控制相关的中断响应 */
#define  TICK_INT_PRIORITY       ((uint32_t)PRIORITY_LOWEST)    /* !< tick interrupt priority (lowest by default) */            
#define  USE_RTOS                0   /* 裸机，不使用 RTOS */
#define  PREFETCH_ENABLE         0   /* Flash 预取关闭；追求性能时可开启 */

/* ########################## Assert Selection ############################## */

/* #define USE_FULL_ASSERT       1U */
/* 断言（assert）默认关闭：开启后 assert_param() 会真正检查 HAL 函数入参， 参数非法时调用 assert_failed()（实现在 main.c 底部）。 调试阶段开启能早发现配置错误，量产固件应保持关闭以省 Flash */



/* Includes ------------------------------------------------------------------ */
/* @brief Include module's header file */
#ifdef HAL_MODULE_ENABLED
 #include "py32f0xx_hal.h"
#endif /* HAL_MODULE_ENABLED */

#ifdef HAL_RCC_MODULE_ENABLED
 #include "py32f0xx_hal_rcc.h"
#endif /* HAL_RCC_MODULE_ENABLED */

#ifdef HAL_EXTI_MODULE_ENABLED
 #include "py32f0xx_hal_exti.h"
#endif /* HAL_EXTI_MODULE_ENABLED */

#ifdef HAL_GPIO_MODULE_ENABLED
 #include "py32f0xx_hal_gpio.h"
#endif /* HAL_GPIO_MODULE_ENABLED */

#ifdef HAL_CORTEX_MODULE_ENABLED
 #include "py32f0xx_hal_cortex.h"
#endif /* HAL_CORTEX_MODULE_ENABLED */

#ifdef HAL_DMA_MODULE_ENABLED
  #include "py32f0xx_hal_dma.h"
#endif /* HAL_DMA_MODULE_ENABLED */

#ifdef HAL_ADC_MODULE_ENABLED
 #include "py32f0xx_hal_adc.h"
#endif /* HAL_ADC_MODULE_ENABLED */

#ifdef HAL_CRC_MODULE_ENABLED
 #include "py32f0xx_hal_crc.h"
#endif /* HAL_CRC_MODULE_ENABLED */

#ifdef HAL_COMP_MODULE_ENABLED
#include "py32f0xx_hal_comp.h"
#endif /* HAL_COMP_MODULE_ENABLED */

#ifdef HAL_FLASH_MODULE_ENABLED
 #include "py32f0xx_hal_flash.h"
#endif /* HAL_FLASH_MODULE_ENABLED */

#ifdef HAL_I2C_MODULE_ENABLED
 #include "py32f0xx_hal_i2c.h"
#endif /* HAL_I2C_MODULE_ENABLED */

#ifdef HAL_IWDG_MODULE_ENABLED
 #include "py32f0xx_hal_iwdg.h"
#endif /* HAL_IWDG_MODULE_ENABLED */

#ifdef HAL_PWR_MODULE_ENABLED
 #include "py32f0xx_hal_pwr.h"
#endif /* HAL_PWR_MODULE_ENABLED */

#ifdef HAL_RTC_MODULE_ENABLED
 #include "py32f0xx_hal_rtc.h"
#endif /* HAL_RTC_MODULE_ENABLED */    

#ifdef HAL_SPI_MODULE_ENABLED
 #include "py32f0xx_hal_spi.h"
#endif /* HAL_SPI_MODULE_ENABLED */

#ifdef HAL_TIM_MODULE_ENABLED
 #include "py32f0xx_hal_tim.h"
#endif /* HAL_TIM_MODULE_ENABLED */

#ifdef HAL_LPTIM_MODULE_ENABLED
 #include "py32f0xx_hal_lptim.h" 
#endif /* HAL_LPTIM_MODULE_ENABLED */

#ifdef HAL_UART_MODULE_ENABLED
 #include "py32f0xx_hal_uart.h"
#endif /* HAL_UART_MODULE_ENABLED */

#ifdef HAL_WWDG_MODULE_ENABLED
 #include "py32f0xx_hal_wwdg.h"
#endif /* HAL_WWDG_MODULE_ENABLED */

#ifdef HAL_LED_MODULE_ENABLED
 #include "py32f0xx_hal_led.h"
#endif /* HAL_LED_MODULE_ENABLED */

#ifdef HAL_USART_MODULE_ENABLED
 #include "py32f0xx_hal_usart.h"
#endif /* HAL_USART_MODULE_ENABLED */

/* Exported macro ------------------------------------------------------------ */
#ifdef  USE_FULL_ASSERT

  #define assert_param(expr) ((expr) ? (void)0U : assert_failed((uint8_t *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
  void assert_failed(uint8_t* file, uint32_t line);
#else
  #define assert_param(expr) ((void)0U)
        /* 关闭断言时，assert_param(x) 被替换成“什么都不做”， 于是 HAL 源码里满地的参数检查在编译期就消失了 —— 这是 HAL 省 Flash 的主要手段之一 */
#endif /* USE_FULL_ASSERT */

#ifdef __cplusplus
}
#endif

#endif /* __PY32F0xx_HAL_CONF_H */

/* (C) COPYRIGHT Puya *****END OF FILE */
