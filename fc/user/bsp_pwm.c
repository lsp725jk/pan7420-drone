/**
 * @file     bsp_tim1_PWM.c
 * @author   embedfire
 * @version  V1.0
 * @date     2024
 * @brief    TIM1 PWM模式
 */
/* 一、这个文件在项目里干什么 把姿态控制算出来的四个 0~1000 的占空比（千分比）写进 TIM3 的四个比较寄存器 CCR， 产生四路 8kHz PWM 去驱动四个电调/电机。它是“软件算出来的姿态修正量” 变成“电机转速”的最后一站，也是整个飞控唯一的执行器输出。 */
#include "bsp_PWM.h"

/**
 * 四路电机占空比（0~1000 千分比）：软件与硬件之间的交接缓冲区。 定义成全局数组：混控算法写、motor_pwm_output() 读
 */
uint16_t motorPWMValue[4];



/* 定时器x句柄 */
// TIM_HandleTypeDef Advanced_Timer1_PWM_Handle;
/* TIM3 句柄：只在本文件使用，但没加 static（原作者风格）。 注意：LED 用的是 TIM1，它的句柄定义在 led.c，两者不要混。 */
TIM_HandleTypeDef Advanced_Timer3_PWM_Handle;        



/* @brief       初始化 TIM3 的四路 PWM 输出（PWM 模式 1），用于驱动四个电机 @note TIM3 挂在 APB1 上，PCLK1 = **48MHz**（main.c 的 RCC 配置里 APB1 不分频）， 所以 TIM3 计数时钟 = 24MHz。 */
void Advanced_Timer3_PWM_Init(uint16_t Prescaler, uint16_t Period)
{
	
	TIM_OC_InitTypeDef Advanced_Timer3_OC_PWM_Config;    		/* 定时器PWM输出配置 */




		
	Advanced_Timer3_PWM_Handle.Instance = TIM3;																
                /* 定时器x */
    Advanced_Timer3_PWM_Handle.Init.Period = Period;                                            /* 自动重装载值 */	
    Advanced_Timer3_PWM_Handle.Init.Prescaler = Prescaler;                                      /* 定时器分频 */
    Advanced_Timer3_PWM_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;                           /* 向上计数模式 */
    Advanced_Timer3_PWM_Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;                     /* 时钟分频因子 */
    Advanced_Timer3_PWM_Handle.Init.RepetitionCounter = 0;                                      /* 不重复计数 */
                /* 自动重装载寄存器没有缓冲 */
    Advanced_Timer3_PWM_Handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE; 
    HAL_TIM_PWM_Init(&Advanced_Timer3_PWM_Handle);                                              /* 初始化PWM（脉宽调制） */

    Advanced_Timer3_OC_PWM_Config.OCMode = TIM_OCMODE_PWM1;                                     /* 模式选择PWM1 */

    Advanced_Timer3_OC_PWM_Config.Pulse = 0;                                       /* ( Period + 1 ) / 2设置比较值,此值用来确定占空比,默认50% */
    Advanced_Timer3_OC_PWM_Config.OCPolarity = TIM_OCPOLARITY_HIGH;                             /* 输出比较极性为高 */
    Advanced_Timer3_OC_PWM_Config.OCFastMode = TIM_OCFAST_DISABLE;
    Advanced_Timer3_OC_PWM_Config.OCIdleState = TIM_OCIDLESTATE_SET;
    Advanced_Timer3_OC_PWM_Config.OCNIdleState = TIM_OCNIDLESTATE_RESET;                        /* 空闲状态OC1N输出低电平 */             
    Advanced_Timer3_OC_PWM_Config.OCIdleState  = TIM_OCIDLESTATE_RESET; 				        /* 空闲状态OC1输出低电平 */
	
																/* 配置TIM3通道1 */
    HAL_TIM_PWM_ConfigChannel(&Advanced_Timer3_PWM_Handle, &Advanced_Timer3_OC_PWM_Config, TIM_CHANNEL_1); 
                /* 使能定时器x和定时器x更新中断 */
    HAL_TIM_PWM_Start(&Advanced_Timer3_PWM_Handle, TIM_CHANNEL_1);
		
																/* 配置TIM3通道2 */
    HAL_TIM_PWM_ConfigChannel(&Advanced_Timer3_PWM_Handle, &Advanced_Timer3_OC_PWM_Config, TIM_CHANNEL_2); 
                /* 使能定时器x和定时器x更新中断 */
    HAL_TIM_PWM_Start(&Advanced_Timer3_PWM_Handle, TIM_CHANNEL_2);

								/* 配置TIM3通道3 */
    HAL_TIM_PWM_ConfigChannel(&Advanced_Timer3_PWM_Handle, &Advanced_Timer3_OC_PWM_Config, TIM_CHANNEL_3); 
                /* 使能定时器x和定时器x更新中断 */
    HAL_TIM_PWM_Start(&Advanced_Timer3_PWM_Handle, TIM_CHANNEL_3);

								/* 配置TIM3通道4 */
    HAL_TIM_PWM_ConfigChannel(&Advanced_Timer3_PWM_Handle, &Advanced_Timer3_OC_PWM_Config, TIM_CHANNEL_4); 
                /* 使能定时器x和定时器x更新中断 */
    HAL_TIM_PWM_Start(&Advanced_Timer3_PWM_Handle, TIM_CHANNEL_4);

}



/* @brief 设置电机1的占空比（TIM3_CH4 → PB1） 【参数】NewDuty：占空比千分比 0~1000（>1000 会被限幅到 1000，即 100%） 【返回】无 【算法】 */
void PWM1_ChangeDuty(uint16_t NewDuty)
{
		if (NewDuty > 1000)
    {
        NewDuty = 1000;
    }
		Advanced_Timer3_PWM_Handle.Instance->CCR4 =NewDuty*1000/1000;
// printf("%d\r\n",Advanced_Timer3_PWM_Handle.Instance->CCR4);
}
/* @brief 设置电机2的占空比（TIM3_CH3 → PB0），入参 0~1000，>1000 限幅。调用关系同 PWM1_ChangeDuty() */
void PWM2_ChangeDuty(uint16_t NewDuty)
{
		if (NewDuty > 1000)
    {
        NewDuty =1000;
    }
		Advanced_Timer3_PWM_Handle.Instance->CCR3 =NewDuty*1000/1000;
}
/* @brief 设置电机3的占空比（TIM3_CH1 → PB4），入参 0~1000，>1000 限幅。调用关系同 PWM1_ChangeDuty() */
void PWM3_ChangeDuty(uint16_t NewDuty)
{
		if (NewDuty > 1000)
    {
        NewDuty = 1000;
    }
		Advanced_Timer3_PWM_Handle.Instance->CCR1 =NewDuty*1000/1000;
}
/* @brief 设置电机4的占空比（TIM3_CH2 → PB5），入参 0~1000，>1000 限幅。调用关系同 PWM1_ChangeDuty() */
void PWM4_ChangeDuty(uint16_t NewDuty)
{
		if (NewDuty > 1000)
    {
        NewDuty = 1000;
    }
		Advanced_Timer3_PWM_Handle.Instance->CCR2 =NewDuty*1000/1000;
}

/* @brief  把 motorPWMValue[4] 的四路占空比一次性刷进 TIM3 的四个 CCR 【参数】无（数据从全局 motorPWMValue 取） 【返回】无 【为什么要单独抽一个“统一输出”函数】 */

void motor_pwm_output(void)
{
	PWM1_ChangeDuty(motorPWMValue[0]);
	PWM2_ChangeDuty(motorPWMValue[1]);
	PWM3_ChangeDuty(motorPWMValue[2]);
	PWM4_ChangeDuty(motorPWMValue[3]);
}



