/**
 * 文件：user/bsp_pwm.h                     【角色】四路电机 PWM 输出的引脚表与接口
 */
#ifndef __BSP_PWM_H
#define __BSP_PWM_H

/**
 * 包含其他头文件
 */
#include "py32f0xx_hal.h"

/**
 * 四路电机占空比数组（0~1000，千分比）：定义在 bsp_pwm.c。 它是“姿态控制 → 电机”之间唯一的交接缓冲区。
 */
extern uint16_t motorPWMValue[4];

/* 宏定义 */ 
#define		Advanced_TIM3_PWM_CH4_GPIO_PORT         	GPIOB					//PWM1
/* 电机1：TIM3_CH4 → PB1 */
#define		Advanced_TIM3_PWM_CH4_GPIO_PIN          	GPIO_PIN_1
#define 	Advanced_TIM3_PWM_CH4_GPIO_AF    			GPIO_AF1_TIM3			/* 端口复用到TIM1 */  
#define		Advanced_TIM3_PWM_CH4_GPIO_CLK_ENABLE 		__HAL_RCC_GPIOB_CLK_ENABLE

#define		Advanced_TIM3_PWM_CH3_GPIO_PORT         	GPIOB					//PWM2
/* 电机2：TIM3_CH3 → PB0 */
#define		Advanced_TIM3_PWM_CH3_GPIO_PIN          	GPIO_PIN_0
#define 	Advanced_TIM3_PWM_CH3_GPIO_AF    			GPIO_AF1_TIM3			/* 端口复用到TIM1 */  
#define		Advanced_TIM3_PWM_CH3_GPIO_CLK_ENABLE 		__HAL_RCC_GPIOB_CLK_ENABLE
					
#define		Advanced_TIM3_PWM_CH1_GPIO_PORT         	GPIOB					//PWM3
/* 电机3：TIM3_CH1 → PB4 */
#define		Advanced_TIM3_PWM_CH1_GPIO_PIN          	GPIO_PIN_4
#define 	Advanced_TIM3_PWM_CH1_GPIO_AF    			GPIO_AF1_TIM3			/* 端口复用到TIM1 */  
#define		Advanced_TIM3_PWM_CH1_GPIO_CLK_ENABLE 		__HAL_RCC_GPIOB_CLK_ENABLE
					
#define		Advanced_TIM3_PWM_CH2_GPIO_PORT         	GPIOB					//PWM4
/* 电机4：TIM3_CH2 → PB5 */
#define		Advanced_TIM3_PWM_CH2_GPIO_PIN          	GPIO_PIN_5
#define 	Advanced_TIM3_PWM_CH2_GPIO_AF    			GPIO_AF1_TIM3			/* 端口复用到TIM1 */  
#define		Advanced_TIM3_PWM_CH2_GPIO_CLK_ENABLE 		__HAL_RCC_GPIOB_CLK_ENABLE

/* 全局变量声明 */



/* 函数声明 */
/* 下面两个声明在本工程里没有对应的实现/调用，属于历史遗留（未改动） */
void SetGeneralTIMxPWMOutputChannalDuty(int arg_num, ...);
void Advanced_Timer3_PWM_Init(uint16_t Prescaler, uint16_t Period);
void HAL_Advanced_Timer1_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim);



/* 四路占空比单独设置（入参 0~1000 千分比，内部会限幅）。 注意函数名与硬件通道是错位的：PWM1 操作的是 TIM3_CH4……以函数体内写的 CCRx 为准 */
void PWM1_ChangeDuty(uint16_t NewDuty);
void PWM2_ChangeDuty(uint16_t NewDuty);
void PWM3_ChangeDuty(uint16_t NewDuty);
void PWM4_ChangeDuty(uint16_t NewDuty);

/* 把 motorPWMValue[4] 一次性刷到四路 CCR（捕获/比较寄存器，改占空比就是改它） —— 电调“同时”收到新占空比，避免帧内不同步 */
void motor_pwm_output(void);

#endif /* __BSP_TIM1_PWM_H */

