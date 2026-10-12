/**
 * 文件：user/led.h                        【角色】指示灯（RGB 三色灯 + LED4）的接口与控制结构
 */
#ifndef __LED_H__
#define __LED_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include "py32f0xx_hal.h"

/**
 * 注意：LEDNUM 定义为 4，但本工程实际只用两个灯（RGB_LED 与 LED4）， 此宏没有使用点，属历史遗留，未改动
 */
#define LEDNUM	4

/* RGB 灯的颜色编号（0~7）：由遥控器按键经 deal_pack.c 写进这个全局量， 再由 led.c 的 Led_Process() 映射到 RGB_xxx() 预设色 */
extern uint8_t color;

/* 定义LED类型枚举 */
/* 灯的编号：作为 Led_SetState()/Led_SetRGBColor() 的第一个参数， 决定这次操作的是三色灯还是单色灯 */
typedef enum {
    RGB_LED = 0,
    LED4 = 1,
} Led_TypeDef;

/* 定义LED状态枚举 */
/* 三种状态：灭、常亮、闪烁。 闪烁是“非阻塞”的：只记录周期和上次翻转时刻，由 Led_Process() 到点翻转， 绝不在 Led_SetState() 里 HAL_Delay —— 那样会拖死整个主循环 */
typedef enum {
    LED_STATE_OFF,
    LED_STATE_ON,
    LED_STATE_BLINKING,
} LedState_t;

/* 定义LED报警优先级 */
/* 枚举值本身就代表“优先级高低”：数值越大越优先。 ★ 调整这里的顺序就等于调整灯语优先级，led.c 的 Led_EvaluateState() 是按“低电压 → 失联 → 校准 → 对码”的 if-else 链条实现的，两处必须一致 */
typedef enum {
    LED_PRIORITY_NORMAL = 0,    // 正常状态
		LED_PRIORITY_PAIRING,       // 对码状态
    LED_PRIORITY_CALIBRATING,   // 校准状态
    LED_PRIORITY_RC_LOST,       // 遥控器失联
    LED_PRIORITY_LOW_VOLTAGE,   // 低电压
} LedPriority_t;

/* 定义LED控制结构体 */

typedef struct {
    LedState_t state;
    uint32_t blinkPeriod;
    uint32_t lastToggleTime;
    uint16_t red;
    uint16_t green;
    uint16_t blue;
    LedPriority_t currentPriority; // 当前优先级
    LedState_t normalState;        // 正常状态备份
    uint32_t normalBlinkPeriod;    // 正常闪烁周期备份
} LedControl_t;







/* 宏定义 */ 
/* 下面三组宏是 RGB 三色灯的引脚与复用功能定义， 这里只是“引脚是谁”的唯一事实来源。三色与通道/引脚的对应关系已在行尾标出 */
#define		Advanced_TIM1_PWM_CH2_GPIO_PORT         	GPIOB					//LED1
#define		Advanced_TIM1_PWM_CH2_GPIO_PIN          	GPIO_PIN_3
#define 	Advanced_TIM1_PWM_CH2_GPIO_AF    			GPIO_AF1_TIM1			/* 端口复用到TIM1 */  
#define		Advanced_TIM1_PWM_CH2_GPIO_CLK_ENABLE() 		__HAL_RCC_GPIOB_CLK_ENABLE()

#define		Advanced_TIM1_PWM_CH3_GPIO_PORT         	GPIOB					//LED2
#define		Advanced_TIM1_PWM_CH3_GPIO_PIN          	GPIO_PIN_6
#define 	Advanced_TIM1_PWM_CH3_GPIO_AF    			GPIO_AF1_TIM1			/* 端口复用到TIM1 */  
#define		Advanced_TIM1_PWM_CH3_GPIO_CLK_ENABLE() 		__HAL_RCC_GPIOB_CLK_ENABLE()
					
#define		Advanced_TIM1_PWM_CH1_GPIO_PORT         	GPIOA					//LED3
#define		Advanced_TIM1_PWM_CH1_GPIO_PIN          	GPIO_PIN_3
#define 	Advanced_TIM1_PWM_CH1_GPIO_AF    			GPIO_AF13_TIM1			/* 端口复用到TIM1 */  
#define		Advanced_TIM1_PWM_CH1_GPIO_CLK_ENABLE() 		__HAL_RCC_GPIOA_CLK_ENABLE()

#define	LED4_GPIO_CLK_ENABLE()	__HAL_RCC_GPIOF_CLK_ENABLE()
/* LED4 是普通 GPIO 驱动的单色灯：PF2。 注意它没有用 TIM1 的第四个通道，因此不能调亮度，只能亮/灭/闪 */
#define	LED4_GPIO_PORT			GPIOF
#define	LED4_GPIO_PIN			GPIO_PIN_2

/* 硬件是“低电平点亮”（共阳接法/灌电流）： 引脚输出 0（低）→ 灯亮；输出 1（高）→ 灯灭。 所以 led.c 里“点亮 LED4”用的是写 BRR（复位引脚）、 “熄灭”用的是写 BSRR（置位引脚）—— 看代码时别被 BSRR/BRR 搞反。 */
#define	LED_ON	0
#define	LED_OFF 1



void Advanced_Timer1_PWM_Init(uint16_t Prescaler, uint16_t Period);

/* 函数声明 */
/* 初始化 LED4 引脚（GPIO 输出），并复位灯语状态机。main.c 调用 */
void Led_Init(void);
/* 只复位两盏灯的状态机初值（不碰硬件）。被 Led_Init() 调用 */
void Led_ControlInit(void);
/* 直接设置某盏灯的状态与闪烁周期（立即写硬件）。周期传 0 表示“不改周期” */
void Led_SetState(Led_TypeDef led, LedState_t state, uint32_t period);
/* 灯语状态机“心跳”：需在主循环里定期调用（约 100ms 一次），负责闪烁翻转与选色 */
void Led_Process(void);
/* 设置 RGB 三色亮度（0~1000 会被限幅）。若当前是常亮会立即刷新 CCR（捕获/比较寄存器，改占空比就是改它） */
void Led_SetRGBColor(Led_TypeDef led, uint16_t red, uint16_t green, uint16_t blue);

/* RGB颜色预设函数 */
void RGB_Red(void);
void RGB_Green(void);
void RGB_Blue(void);
void RGB_Yellow(void);
void RGB_Cyan(void);
void RGB_Magenta(void);
void RGB_White(void);
void RGB_Pink(void);



/* 登记/清除一条报警（state 传 LED_STATE_OFF 等于清除）。内部会重算优先级并刷新灯 */
void Led_SetAlert(LedPriority_t priority, LedState_t state, uint32_t period);
/* 清除某条报警（等价于 Led_SetAlert(priority, LED_STATE_OFF, 0)）。本工程未直接调用，未改动 */
void Led_ClearAlert(LedPriority_t priority);
/* 按优先级链重新决定两盏灯“应该是什么状态”，内部由 Led_SetAlert() 调用 */
void Led_EvaluateState(void);

#ifdef __cplusplus
}
#endif

#endif



