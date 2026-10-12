/**
 * @file    py32f030xx_Start_Kit.h
 * @author  MCU Application Team
 * @brief   This file provides set of firmware functions to manage Leds,
 * @attention
 */

/* Define to prevent recursive inclusion ------------------------------------- */
#ifndef PY32F030_START_KIT_H
#define PY32F030_START_KIT_H

/* 开发板的板级支持包（BSP）头文件（HAL 版） 一、这个文件在项目里干什么 这是普冉半导体（Puya）官方例程自带的“开发板 BSP”，作用是把开发板上 现成的东西（1 个 LED、1 个用户按键、1 路调试串口）封装成几个好用的函数 */

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------ */
#include "py32f0xx_hal.h"

/* @addtogroup BSP @{ */

/* @defgroup py32f0xx_Start_Kit @brief This section contains the exported types, contants and functions required to use the Nucleo 32 board. @{ */

/* @defgroup py32f0xx_Start_Kit_Exported_Types Exported Types @{ */

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



#define LEDn                               1

#define LED3_PIN                           GPIO_PIN_11
#define LED3_GPIO_PORT                     GPIOA
#define LED3_GPIO_CLK_ENABLE()             __HAL_RCC_GPIOA_CLK_ENABLE()
#define LED3_GPIO_CLK_DISABLE()            __HAL_RCC_GPIOA_CLK_DISABLE()

#define LEDx_GPIO_CLK_ENABLE(__INDEX__)    do {LED3_GPIO_CLK_ENABLE(); } while(0U)
#define LEDx_GPIO_CLK_DISABLE(__INDEX__)   LED3_GPIO_CLK_DISABLE()

/* 板上有几个用户按键（当前 1 个：PA12）。作用同 LEDn。 */
#define BUTTONn                            1

/* @brief User push-button */
#define USER_BUTTON_PIN                         GPIO_PIN_12
#define USER_BUTTON_GPIO_PORT                   GPIOA
#define USER_BUTTON_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOA_CLK_ENABLE()
#define USER_BUTTON_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOA_CLK_DISABLE()
#define USER_BUTTON_EXTI_IRQn                   EXTI4_15_IRQn

/* Aliases */
#define KEY_BUTTON_PIN                          USER_BUTTON_PIN
#define KEY_BUTTON_GPIO_PORT                    USER_BUTTON_GPIO_PORT
#define KEY_BUTTON_GPIO_CLK_ENABLE()            USER_BUTTON_GPIO_CLK_ENABLE()
#define KEY_BUTTON_GPIO_CLK_DISABLE()           USER_BUTTON_GPIO_CLK_DISABLE()
#define KEY_BUTTON_EXTI_IRQn                    USER_BUTTON_EXTI_IRQn

#define BUTTONx_GPIO_CLK_ENABLE(__INDEX__)      do { if((__INDEX__) == 0) USER_BUTTON_GPIO_CLK_ENABLE();} while(0)
#define BUTTONx_GPIO_CLK_DISABLE(__INDEX__)     (((__INDEX__) == 0) ? USER_BUTTON_GPIO_CLK_DISABLE() : 0)



#ifdef HAL_UART_MODULE_ENABLED
// debug printf redirect config
#define DEBUG_USART_BAUDRATE                    115200

#define DEBUG_USART                             USART2
#define DEBUG_USART_CLK_ENABLE()                do { \
                                                     __IO uint32_t tmpreg = 0x00U; \
                                                     SET_BIT(RCC->APBENR1, RCC_APBENR1_USART2EN);\
                                                                                                                                                                                                                    /* Delay after an RCC peripheral clock enabling */ \
                                                     tmpreg = READ_BIT(RCC->APBENR1, RCC_APBENR1_USART2EN);\
                                                     UNUSED(tmpreg); \
                                                   } while(0U)

#define __GPIOA_CLK_ENABLE()                    do { \
                                                     __IO uint32_t tmpreg = 0x00U; \
                                                     SET_BIT(RCC->IOPENR, RCC_IOPENR_GPIOAEN);\
                                                                                                                                                                                                                    /* Delay after an RCC peripheral clock enabling */ \
                                                     tmpreg = READ_BIT(RCC->IOPENR, RCC_IOPENR_GPIOAEN);\
                                                     UNUSED(tmpreg); \
                                                   } while(0U)

#define DEBUG_USART_RX_GPIO_PORT                GPIOA
#define DEBUG_USART_RX_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()
#define DEBUG_USART_RX_PIN                      GPIO_PIN_3
#define DEBUG_USART_RX_AF                       GPIO_AF4_USART2

#define DEBUG_USART_TX_GPIO_PORT                GPIOA
#define DEBUG_USART_TX_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()
#define DEBUG_USART_TX_PIN                      GPIO_PIN_2
#define DEBUG_USART_TX_AF                       GPIO_AF4_USART2

#define DEBUG_USART_IRQHandler                  USART2_IRQHandler
#define DEBUG_USART_IRQ                         USART2_IRQn
#endif



#ifdef HAL_UART_MODULE_ENABLED
/* 调试串口句柄：定义在本文件的 .c 里，这里只做 extern 声明供外部引用。 */
extern UART_HandleTypeDef DebugUartHandle;
#endif

/* @defgroup Functions @{ */
/* ---- 对外函数声明（实现在同名 .c 里） ---- */
uint32_t         BSP_GetVersion(void);          /* 返回 BSP 驱动版本号（0x01000000） */

void             BSP_LED_Init(Led_TypeDef Led); /* 初始化 LED 引脚为推挽输出并熄灭 */
void             BSP_LED_DeInit(Led_TypeDef Led);/* 反初始化 LED 引脚（不关 GPIO 时钟） */
void             BSP_LED_On(Led_TypeDef Led);   /* 点亮：引脚输出低（板载 LED 低电平点亮） */
void             BSP_LED_Off(Led_TypeDef Led);  /* 熄灭：引脚输出高 */
void             BSP_LED_Toggle(Led_TypeDef Led);/* 翻转 LED 状态 */

void             BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode);
                                                                                                                                                                                                /* 初始化按键：GPIO 输入 或 EXTI 下降沿中断 */
void             BSP_PB_DeInit(Button_TypeDef Button);   /* 关闭按键中断并释放引脚 */
uint32_t         BSP_PB_GetState(Button_TypeDef Button); /* 读按键电平（1=按下） */

void             BSP_USART_Config(void);        /* 配置调试串口 USART2：115200 8N1 + printf 重定向 */
#define          DEBUG_USART_Config       BSP_USART_Config
/* @} */

/* @} */

#ifdef __cplusplus
}
#endif

#endif /* PY32F030xx_START_KIT_H */

/* (C) COPYRIGHT Puya *****END OF FILE */
