/**
 * @file    py32f030xx_ll_Start_Kit.h
 * @author  MCU Application Team
 * @brief   This file provides set of firmware functions to manage Leds,
 * @attention
 */

/* Define to prevent recursive inclusion ------------------------------------- */
#ifndef PY32F0XX_LL_START_KIT_H
#define PY32F0XX_LL_START_KIT_H

/* 开发板 BSP 头文件（LL 库版本） 一、这个文件在项目里干什么 与 py32f030xx_Start_Kit.h 完全对应的 LL 库版本： 同样封装“板载 LED（PA11）+ 用户按键（PA12）+ 调试串口 USART2（PA2/PA3）”， */

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------ */
#include <stdio.h>
#include "py32f0xx_ll_rcc.h"
#include "py32f0xx_ll_bus.h"
#include "py32f0xx_ll_system.h"
#include "py32f0xx_ll_exti.h"
#include "py32f0xx_ll_cortex.h"
#include "py32f0xx_ll_utils.h"
#include "py32f0xx_ll_pwr.h"
#include "py32f0xx_ll_dma.h"
#include "py32f0xx_ll_gpio.h"
#include "py32f0xx_ll_usart.h"

/* @addtogroup BSP @{ */

/* @defgroup py32f0xx_Start_Kit @brief This section contains the exported types, contants and functions required to use the Nucleo 32 board. @{ */

/* @defgroup py32f0xx_Start_Kit_Exported_Types Exported Types @{ */
/* 板载 LED 编号枚举（LL 版与 HAL 版定义完全相同，只有一个 LED3=PA11）。 */
typedef enum
{
  LED3 = 0,
  LED_GREEN = LED3
} Led_TypeDef;

typedef enum
{
  BUTTON_USER = 0,
        /* Alias */
  BUTTON_KEY  = BUTTON_USER
} Button_TypeDef;

typedef enum
{
  BUTTON_MODE_GPIO = 0,
  BUTTON_MODE_EXTI = 1
} ButtonMode_TypeDef;

/* 板上有几个 LED（决定 LED_PORT[]/LED_PIN[] 数组长度）。 */
#define LEDn                               1

#define LED3_PIN                           LL_GPIO_PIN_11
#define LED3_GPIO_PORT                     GPIOA
#define LED3_GPIO_CLK_ENABLE()             LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA)
#define LED3_GPIO_CLK_DISABLE()            LL_IOP_GRP1_DisableClock(LL_IOP_GRP1_PERIPH_GPIOA)

#define LEDx_GPIO_CLK_ENABLE(__INDEX__)    do {LED3_GPIO_CLK_ENABLE(); } while(0U)
#define LEDx_GPIO_CLK_DISABLE(__INDEX__)   LED3_GPIO_CLK_DISABLE()

/* 板上有几个用户按键（当前 1 个：PA12）。 */
#define BUTTONn                            1

/* @brief User push-button */
#define USER_BUTTON_PIN                         LL_GPIO_PIN_12
#define USER_BUTTON_GPIO_PORT                   GPIOA
#define USER_BUTTON_GPIO_CLK_ENABLE()           LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA)
#define USER_BUTTON_GPIO_CLK_DISABLE()          LL_IOP_GRP1_DisableClock(LL_IOP_GRP1_PERIPH_GPIOA)

#define USER_BUTTON_EXTI_IRQn                   EXTI4_15_IRQn
#define USER_BUTTON_EXTI_LINE                   LL_EXTI_LINE_12
#define USER_BUTTON_EXTI_LINE_ENABLE()          LL_EXTI_EnableIT(USER_BUTTON_EXTI_LINE)
#define USER_BUTTON_EXTI_FALLING_TRIG_ENABLE()  LL_EXTI_EnableFallingTrig(USER_BUTTON_EXTI_LINE)
#define USER_BUTTON_IRQHANDLER                  EXTI4_15_IRQHandler

/* Aliases */
#define KEY_BUTTON_PIN                          USER_BUTTON_PIN
#define KEY_BUTTON_GPIO_PORT                    USER_BUTTON_GPIO_PORT
#define KEY_BUTTON_GPIO_CLK_ENABLE()            USER_BUTTON_GPIO_CLK_ENABLE()
#define KEY_BUTTON_GPIO_CLK_DISABLE()           USER_BUTTON_GPIO_CLK_DISABLE()
#define KEY_BUTTON_EXTI_IRQn                    USER_BUTTON_EXTI_IRQn

#define BUTTONx_GPIO_CLK_ENABLE(__INDEX__)      do { if((__INDEX__) == 0) USER_BUTTON_GPIO_CLK_ENABLE();} while(0)
#define BUTTONx_GPIO_CLK_DISABLE(__INDEX__)     (((__INDEX__) == 0) ? USER_BUTTON_GPIO_CLK_DISABLE() : 0)



/* ---- 调试串口（printf 输出）配置 ---- 与 HAL 版一样是 USART2 + PA2(TX)/PA3(RX)、115200 8N1。 注意飞控实际使用的调试串口是 drivers/bsp/bsp_uart.c 的 USART1 + PB2/PB8。 */
// debug printf redirect config
#define DEBUG_USART_BAUDRATE                    115200

#define DEBUG_USART                             USART2
#define DEBUG_USART_CLK_ENABLE()                LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2)

#define __GPIOA_CLK_ENABLE()                    do { \
                                                     __IO uint32_t tmpreg = 0x00U; \
                                                     SET_BIT(RCC->IOPENR, RCC_IOPENR_GPIOAEN);\
                                                                                                                                                                                                                    /* Delay after an RCC peripheral clock enabling */ \
                                                     tmpreg = READ_BIT(RCC->IOPENR, RCC_IOPENR_GPIOAEN);\
                                                     UNUSED(tmpreg); \
                                                   } while(0U)

#define DEBUG_USART_RX_GPIO_PORT                GPIOA
#define DEBUG_USART_RX_GPIO_CLK_ENABLE()        LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA)
#define DEBUG_USART_RX_PIN                      LL_GPIO_PIN_3
#define DEBUG_USART_RX_AF                       LL_GPIO_AF_4

#define DEBUG_USART_TX_GPIO_PORT                GPIOA
#define DEBUG_USART_TX_GPIO_CLK_ENABLE()        LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA)
#define DEBUG_USART_TX_PIN                      LL_GPIO_PIN_2
#define DEBUG_USART_TX_AF                       LL_GPIO_AF_4

#define DEBUG_USART_IRQHandler                  USART2_IRQHandler
#define DEBUG_USART_IRQ                         USART2_IRQn



/* @defgroup Functions @{ */
/* ---- 对外函数声明（实现在同名 .c 里，全部用 LL 库实现） ---- */
uint32_t         BSP_GetVersion(void);          /* 返回 BSP 驱动版本号（0x01000000） */

void             BSP_LED_Init(Led_TypeDef Led); /* LED 引脚设为推挽输出，并先输出高（熄灭） */
void             BSP_LED_DeInit(Led_TypeDef Led);/* 引脚设为模拟模式（最省电、不抖动） */
void             BSP_LED_On(Led_TypeDef Led);   /* 点亮：输出低（板载 LED 低电平点亮） */
void             BSP_LED_Off(Led_TypeDef Led);  /* 熄灭：输出高 */
void             BSP_LED_Toggle(Led_TypeDef Led);/* 翻转 LED 状态 */

void             BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode);
                                                                                                                                                                                                /* 按键设为输入；EXTI 模式还需开 EXTI 线与下降沿 */
void             BSP_PB_DeInit(Button_TypeDef Button);   /* 关中断并把引脚设为模拟模式 */
uint32_t         BSP_PB_GetState(Button_TypeDef Button); /* 读按键电平（1=按下） */

void             BSP_USART_Config(void);        /* 配置调试串口 USART2 并做 printf 重定向 */
/* @} */

/* @} */

#ifdef __cplusplus
}
#endif

#endif /* PY32F030xx_LL_START_KIT_H */

/* (C) COPYRIGHT Puya *****END OF FILE */
