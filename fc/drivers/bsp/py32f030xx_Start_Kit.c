/**
 * @file    py32f030xx_Start_Kit.c
 * @author  MCU Application Team
 * @brief   This file provides set of firmware functions to manage Leds,
 * @attention
 */

/* Includes ------------------------------------------------------------------ */
#include "py32f030xx_Start_Kit.h"



/**
 * @brief PY32F030xx STK BSP Driver version number
 */
#define __PY32F030xx_STK_BSP_VERSION_MAIN   (0x01U) /* !< [31:24] main version */
#define __PY32F030xx_STK_BSP_VERSION_SUB1   (0x00U) /* !< [23:16] sub1 version */
#define __PY32F030xx_STK_BSP_VERSION_SUB2   (0x00U) /* !< [15:8]  sub2 version */
#define __PY32F030xx_STK_BSP_VERSION_RC     (0x00U) /* !< [7:0]  release candidate */
#define __PY32F030xx_STK_BSP_VERSION        ((__PY32F030xx_STK_BSP_VERSION_MAIN << 24) \
                                            |(__PY32F030xx_STK_BSP_VERSION_SUB1 << 16) \
                                            |(__PY32F030xx_STK_BSP_VERSION_SUB2 << 8 ) \
                                            |(__PY32F030xx_STK_BSP_VERSION_RC))

#ifdef HAL_UART_MODULE_ENABLED
UART_HandleTypeDef DebugUartHandle;
#endif

GPIO_TypeDef* LED_PORT[LEDn] = {LED3_GPIO_PORT};
const uint16_t LED_PIN[LEDn] = {LED3_PIN};

GPIO_TypeDef* BUTTON_PORT[BUTTONn] = {USER_BUTTON_GPIO_PORT };
const uint16_t BUTTON_PIN[BUTTONn] = {USER_BUTTON_PIN };
const uint8_t BUTTON_IRQn[BUTTONn] = {USER_BUTTON_EXTI_IRQn };

/* @addtogroup PY32F030xx_STK_Exported_Functions @{ */



/* 返回 BSP 驱动版本号（宏拼出来的 0x01000000，仅作标识用）。 */
uint32_t BSP_GetVersion(void)
{
  return __PY32F030xx_STK_BSP_VERSION;
}

/* @addtogroup LED_Functions @{ */

/* @brief  Configures LED GPIO. @param  Led Specifies the Led to be configured. This parameter can be one of following parameters: @arg  LED3 */
/* 把 LED 引脚配成推挽输出并立即输出高电平（熄灭）；会打开 GPIO 时钟。 */
void BSP_LED_Init(Led_TypeDef Led)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

        /* Enable the GPIO_LED Clock */
  LEDx_GPIO_CLK_ENABLE(Led);

        /* Configure the GPIO_LED pin */
  GPIO_InitStruct.Pin = LED_PIN[Led];
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

  HAL_GPIO_Init(LED_PORT[Led], &GPIO_InitStruct);
  HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_SET);
}



/* 先熄灭再反初始化 LED 引脚（HAL_GPIO_DeInit）；注意不关 GPIO 时钟。 */
void BSP_LED_DeInit(Led_TypeDef Led)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

        /* Turn off LED */
  HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_RESET);
        /* DeInit the GPIO_LED pin */
  GPIO_InitStruct.Pin = LED_PIN[Led];
  HAL_GPIO_DeInit(LED_PORT[Led], GPIO_InitStruct.Pin);
}

/* @brief  Turns selected LED On. @param  Led Specifies the Led to be set on. This parameter can be one of following parameters: @arg  LED3 */
/* 点亮 LED：写 RESET（输出低）—— 板载 LED 是低电平点亮。 */
void BSP_LED_On(Led_TypeDef Led)
{
  HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_RESET);
}

/* @brief  Turns selected LED Off. @param  Led Specifies the Led to be set off. This parameter can be one of following parameters: @arg  LED3 */
/* 熄灭 LED：写 SET（输出高）。 */
void BSP_LED_Off(Led_TypeDef Led)
{
  HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_SET);
}

/* @brief  Toggles the selected LED. @param  Led Specifies the Led to be toggled. This parameter can be one of following parameters: @arg  LED3 */
/* 翻转 LED 电平（常用作心跳灯/程序存活指示）。 */
void BSP_LED_Toggle(Led_TypeDef Led)
{
  HAL_GPIO_TogglePin(LED_PORT[Led], LED_PIN[Led]);
}



/* 初始化用户按键：BUTTON_MODE_GPIO=普通输入；BUTTON_MODE_EXTI=下降沿中断， 并把该中断设为最低优先级（0x0F），避免影响飞控的定时器节拍。 */
void BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode)
{
  GPIO_InitTypeDef gpioinitstruct;

        /* Enable the BUTTON Clock */
  BUTTONx_GPIO_CLK_ENABLE(Button);

  gpioinitstruct.Pin = BUTTON_PIN[Button];
  gpioinitstruct.Pull = GPIO_NOPULL;
  gpioinitstruct.Speed = GPIO_SPEED_FREQ_MEDIUM;

  if(ButtonMode == BUTTON_MODE_GPIO)
  {
                /* Configure Button pin as input */
    gpioinitstruct.Mode = GPIO_MODE_INPUT;

    HAL_GPIO_Init(BUTTON_PORT[Button], &gpioinitstruct);
  }

  if(ButtonMode == BUTTON_MODE_EXTI)
  {
                /* Configure Button pin as input with External interrupt */
    gpioinitstruct.Mode   = GPIO_MODE_IT_FALLING;
    HAL_GPIO_Init(BUTTON_PORT[Button], &gpioinitstruct);

                /* Enable and set Button EXTI Interrupt to the lowest priority */
    HAL_NVIC_SetPriority((IRQn_Type)(BUTTON_IRQn[Button]), 0x0F, 0);
    HAL_NVIC_EnableIRQ((IRQn_Type)(BUTTON_IRQn[Button]));
  }
}



/* 关掉按键中断并反初始化引脚（同样不关 GPIO 时钟）。 */
void BSP_PB_DeInit(Button_TypeDef Button)
{
  GPIO_InitTypeDef gpio_init_structure;

  gpio_init_structure.Pin = BUTTON_PIN[Button];
  HAL_NVIC_DisableIRQ((IRQn_Type)(BUTTON_IRQn[Button]));
  HAL_GPIO_DeInit(BUTTON_PORT[Button], gpio_init_structure.Pin);
}



/* 读按键当前电平：返回 GPIO_PIN_SET(=1) 表示按下（上拉输入，按下接地）。 */
uint32_t BSP_PB_GetState(Button_TypeDef Button)
{
  return HAL_GPIO_ReadPin(BUTTON_PORT[Button], BUTTON_PIN[Button]);
}

#ifdef HAL_UART_MODULE_ENABLED
/* @brief  DEBUG_USART GPIO Config,Mode Config,115200 8-N-1 @param  None @retval None */

void BSP_USART_Config(void)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

#if  defined(__GNUC__)
  setvbuf(stdout, NULL, _IONBF, 0 );
#endif

  DEBUG_USART_CLK_ENABLE();

  DebugUartHandle.Instance          = DEBUG_USART;

  DebugUartHandle.Init.BaudRate     = DEBUG_USART_BAUDRATE;
  DebugUartHandle.Init.WordLength   = UART_WORDLENGTH_8B;
  DebugUartHandle.Init.StopBits     = UART_STOPBITS_1;
  DebugUartHandle.Init.Parity       = UART_PARITY_NONE;
  DebugUartHandle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
  DebugUartHandle.Init.Mode         = UART_MODE_TX_RX;
  DebugUartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
  DebugUartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

  HAL_UART_Init(&DebugUartHandle);

  DEBUG_USART_RX_GPIO_CLK_ENABLE();
  DEBUG_USART_TX_GPIO_CLK_ENABLE();

        /* USART GPIO Configuration PA2     ------> USART1_TX PA3     ------> USART1_RX */
  GPIO_InitStruct.Pin = DEBUG_USART_TX_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = DEBUG_USART_TX_AF;
  HAL_GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = DEBUG_USART_RX_PIN;
  GPIO_InitStruct.Alternate = DEBUG_USART_RX_AF;

  HAL_GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStruct);

        /* ENABLE NVIC */
  HAL_NVIC_SetPriority(DEBUG_USART_IRQ,0,1);
  HAL_NVIC_EnableIRQ(DEBUG_USART_IRQ );
}

#if (defined (__CC_ARM)) || (defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
/* @brief  writes a character to the usart @param  ch f @retval the character */
/* ARMCC/AC6 的 printf 重定向钩子：每输出一个字符就发一字节到调试串口。 */
int fputc(int ch, FILE *f)
{
        /* Send a byte to USART */
  HAL_UART_Transmit(&DebugUartHandle, (uint8_t *)&ch, 1, 1000);

  return (ch);
}

/* @brief  get a character from the usart @param  *f @retval a character */
/* ARMCC/AC6 的 scanf 重定向钩子：从调试串口读一个字符（阻塞等待）。 */
int fgetc(FILE *f)
{
  int ch;
  HAL_UART_Receive(&DebugUartHandle, (uint8_t *)&ch, 1, 1000);
  return (ch);
}

#elif defined(__ICCARM__)
/* @brief  writes a character to the usart @param  ch f @retval the character */
/* IAR 编译器（__ICCARM__）的 printf 重定向钩子，作用同 fputc。 */
int putchar(int ch)
{
        /* Send a byte to USART */
  HAL_UART_Transmit(&DebugUartHandle, (uint8_t *)&ch, 1, 1000);

  return (ch);
}
#elif  defined(__GNUC__)
/* @brief  writes a character to the usart @param  ch @retval the character */
/* GCC（STM32CubeIDE 风格）的 printf 重定向钩子。 */
int __io_putchar(int ch)
{
        /* Send a byte to USART */
  HAL_UART_Transmit(&DebugUartHandle, (uint8_t *)&ch, 1, 1000);

  return ch;
}

/* GCC + newlib 真正会调用的钩子：printf 的输出最终汇到这里， 逐个字符转给 __io_putchar() 发出去。 */
int _write(int file, char *ptr, int len)
{
  int DataIdx;
  for (DataIdx=0;DataIdx<len;DataIdx++)
  {
    __io_putchar(*ptr++);
  }
  return len;
}
#endif

#endif
/* @} */

/* @} */

/* @} */

/* (C) COPYRIGHT Puya *****END OF FILE */
