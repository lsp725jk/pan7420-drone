#ifndef __LED_H__
#define __LED_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include "py32f0xx_hal.h"

#define LEDNUM	2
	 
/**
 * 定义LED类型枚举
 */
typedef enum {
    LED1 = 0,
    LED2 = 1,
} Led_TypeDef;



/**
 * 定义LED状态枚举
 */
typedef enum {
    LED_STATE_OFF,       // 关闭状态
    LED_STATE_ON,        // 常亮状态
    LED_STATE_BLINKING,  // 闪烁状态
} LedState_t;

/**
 * 定义LED控制结构体
 */
typedef struct {
    LedState_t state;       // 当前状态
    uint32_t blinkPeriod;   // 闪烁周期(ms)
    uint32_t lastToggleTime;// 上次切换时间
} LedControl_t;


	 
// #define	LED1_GPIO_CLK_ENABLE()	__HAL_RCC_GPIOB_CLK_ENABLE()
// #define	LED1_GPIO_PORT			GPIOB
// #define	LED1_GPIO_PIN			GPIO_PIN_1

#define	LED2_GPIO_CLK_ENABLE()	__HAL_RCC_GPIOA_CLK_ENABLE()
#define	LED2_GPIO_PORT			GPIOA
#define	LED2_GPIO_PIN			GPIO_PIN_3

#define	LED_ON	0
#define	LED_OFF 1



// #define LED1(x)	x ? WRITE_REG(LED1_GPIO_PORT->BSRR,LED1_GPIO_PIN): \
// WRITE_REG(LED1_GPIO_PORT->BRR,LED1_GPIO_PIN); \

// #define LED1_TOGGLE()	HAL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_GPIO_PIN)



// #define LED2(x) x ? WRITE_REG(LED2_GPIO_PORT->BSRR,LED2_GPIO_PIN): \
// WRITE_REG(LED2_GPIO_PORT->BRR,LED2_GPIO_PIN); \

// #define	LED2_TOGGLE()	HAL_GPIO_TogglePin(LED2_GPIO_PORT, LED2_GPIO_PIN)



void Led_Init(void);
void Led_ControlInit(void);
void Led_SetState(Led_TypeDef led, LedState_t state, uint32_t period);
void Led_Process(void);



#ifdef __cplusplus
}
#endif

#endif
