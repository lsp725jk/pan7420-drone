#include "led.h"

GPIO_TypeDef* LED_PORT[LEDNUM] = {0,LED2_GPIO_PORT};
const uint16_t LED_PIN[LEDNUM] = {0,LED2_GPIO_PIN};

/**
 * 声明LED控制对象
 */
static LedControl_t led1Control;
static LedControl_t led2Control;

/**
 * 初始化LED控制对象
 */
void Led_ControlInit(void)
{
        // 初始化LED1控制参数
    led1Control.state = LED_STATE_OFF;
    led1Control.blinkPeriod = 500; // 默认500ms闪烁周期
    led1Control.lastToggleTime = 0;
    
        // 初始化LED2控制参数
    led2Control = led1Control;
}

void Led_Init(void)
{
	GPIO_InitTypeDef	GPIO_Led_InitConfig;
	
				/* GPIOA时钟使能 */
		// LED1_GPIO_CLK_ENABLE();
	LED2_GPIO_CLK_ENABLE();  
 	
	GPIO_Led_InitConfig.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Led_InitConfig.Pull = GPIO_PULLUP;
	GPIO_Led_InitConfig.Speed = GPIO_SPEED_FREQ_HIGH;
				/* LED1 GPIO初始化 */
// GPIO_Led_InitConfig.Pin = LED1_GPIO_PIN;
// HAL_GPIO_Init(LED1_GPIO_PORT, &GPIO_Led_InitConfig);
				/* LED2 GPIO初始化 */
	GPIO_Led_InitConfig.Pin = LED2_GPIO_PIN;
	HAL_GPIO_Init(LED2_GPIO_PORT, &GPIO_Led_InitConfig);

				/* LED 关闭 */
// WRITE_REG(LED1_GPIO_PORT->BSRR,LED1_GPIO_PIN);
	WRITE_REG(LED2_GPIO_PORT->BSRR,LED2_GPIO_PIN);
	Led_ControlInit();
	
}



/* 设置LED状态 - 非阻塞方式 */
void Led_SetState(Led_TypeDef led, LedState_t state, uint32_t period)
{
    LedControl_t *ledControl;
    
        // 根据LED选择控制对象
    if (led == LED1) {
        ledControl = &led1Control;
    } else {
        ledControl = &led2Control;
    }
    
        // 更新状态和参数
    ledControl->state = state;
    
    if (period > 0) {
        ledControl->blinkPeriod = period;
    }
    
        // 根据新状态设置LED初始状态
    switch (state) {
        case LED_STATE_OFF:
            WRITE_REG(LED_PORT[led]->BSRR,LED_PIN[led]); 
            break;
        case LED_STATE_ON:
            WRITE_REG(LED_PORT[led]->BRR,LED_PIN[led]); 
            break;
        case LED_STATE_BLINKING:
                        // 开始闪烁前先关闭LED
            WRITE_REG(LED_PORT[led]->BSRR,LED_PIN[led]); 
            ledControl->lastToggleTime = HAL_GetTick();
            break;
        default:
            break;
    }
}

/* LED状态机处理函数 - 需在主循环中定期调用 */
void Led_Process(void)
{
    uint32_t currentTime = HAL_GetTick();
    
// // 处理LED1状态
// if (led1Control.state == LED_STATE_BLINKING) {
// if ((currentTime - led1Control.lastToggleTime) >= (led1Control.blinkPeriod / 2)) {
// // 切换LED状态
// HAL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_GPIO_PIN);
// // 更新时间戳
// led1Control.lastToggleTime = currentTime;
// }
// }
    
        // 处理LED2状态
    if (led2Control.state == LED_STATE_BLINKING) {
        if ((currentTime - led2Control.lastToggleTime) >= (led2Control.blinkPeriod / 2)) {
            HAL_GPIO_TogglePin(LED2_GPIO_PORT, LED2_GPIO_PIN);
            led2Control.lastToggleTime = currentTime;
        }
    }
}
