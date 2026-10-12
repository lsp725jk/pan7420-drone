/**
 * 文件：user/led.c                        【角色】LED 灯语：优先级报警 + 非阻塞闪烁状态机
 */
#include "led.h"

/**
 * 声明LED控制对象
 */
/**
 * static：只在本文件可见。两盏灯各一份“状态机寄存器”， 所以灯语的状态是“每盏灯独立”的（例如失联只闪 RGB、LED4 保持原样）
 */
static LedControl_t rgbLedControl;  // RGB七色灯控制
static LedControl_t led4Control;    // LED4控制



/* 定时器x句柄 */
/* TIM1 句柄：RGB 三色灯的 PWM 由它驱动。 为什么定义在 led.c 而不是 bsp_pwm.c：TIM1 在本工程专门给灯用（电机用 TIM3）， 谁用谁定义，避免两个文件抢同一个句柄。 注意它没加 static（原作者风格，其它文件也能 extern 到） */
TIM_HandleTypeDef Advanced_Timer1_PWM_Handle;        
       
// 报警状态记录

static struct {
    uint8_t pairing : 1;        // 对码状态（新增）
    uint8_t calibrating : 1;
    uint8_t rc_lost : 1;
    uint8_t low_voltage : 1;
} alertStatus = {0, 0, 0, 0};



/* 设置LED报警状态 */
void Led_SetAlert(LedPriority_t priority, LedState_t state, uint32_t period)
{
    switch(priority) {
        case LED_PRIORITY_PAIRING:        // 新增对码状态
                                                /* 把“三态”压成“一位”：只要不是 OFF 就算正在报警 */
            alertStatus.pairing = (state != LED_STATE_OFF);
            break;
        case LED_PRIORITY_CALIBRATING:
            alertStatus.calibrating = (state != LED_STATE_OFF);
            break;
        case LED_PRIORITY_RC_LOST:
            alertStatus.rc_lost = (state != LED_STATE_OFF);
            break;
        case LED_PRIORITY_LOW_VOLTAGE:
            alertStatus.low_voltage = (state != LED_STATE_OFF);
            break;
        default:
            break;
    }
    
        // 重新评估LED状态
                /* 每次登记/清除都重算一次：保证“灯的表现”永远跟着 alertStatus 走， 不需要调用者自己记得去刷新灯 */
    Led_EvaluateState();
}

/* 清除LED报警状态 */

void Led_ClearAlert(LedPriority_t priority)
{
    Led_SetAlert(priority, LED_STATE_OFF, 0);
}

/* @brief  按优先级链仲裁两盏灯的最终表现（本文件的核心算法） 【参数】无（读 alertStatus 位域与两盏灯的备份字段） 【返回】无（必要时调用 Led_SetState() 真正改硬件） 【算法】 */
/* 评估并设置LED最终状态 */
void Led_EvaluateState(void)
{
                /* 四个“候选值”：先按正常状态填好，后面哪个报警成立就覆盖哪个 */
    LedPriority_t rgbPriority = LED_PRIORITY_NORMAL;
    LedPriority_t led4Priority = LED_PRIORITY_NORMAL;
    LedState_t rgbState = rgbLedControl.normalState;
    LedState_t led4State = led4Control.normalState;
    uint32_t rgbPeriod = rgbLedControl.normalBlinkPeriod;
    uint32_t led4Period = led4Control.normalBlinkPeriod;
    
        // 根据优先级确定最终状态（从高到低）
                /* 优先级第 1 档：低电压（最高）—— 两盏灯都 500ms 闪， 因为这是“马上要掉下来”的告警，必须比失联更显眼 */
    if (alertStatus.low_voltage) {
                // 低电压优先级最高 - RGB和LED4都以500ms闪烁
        rgbPriority = LED_PRIORITY_LOW_VOLTAGE;
        led4Priority = LED_PRIORITY_LOW_VOLTAGE;
        rgbState = LED_STATE_BLINKING;
        led4State = LED_STATE_BLINKING;
        rgbPeriod = 500;
        led4Period = 500;
    } 
                /* 优先级第 2 档：遥控失联 —— 200ms 快闪（比 500ms 更急促，暗示“快回来”） */
    else if (alertStatus.rc_lost) {
                // 遥控器失联优先级次之 - 只有RGB灯以200ms闪烁
        rgbPriority = LED_PRIORITY_RC_LOST;
        rgbState = LED_STATE_BLINKING;
        rgbPeriod = 200;
                // LED4保持正常状态
        led4State = led4Control.normalState;
    }
                /* 优先级第 3 档：传感器校准中（本工程主流程当前没有登记这条报警的地方， 是给校准流程预留的档位），表现与失联相同 */
    else if (alertStatus.calibrating) {
                // 校准优先级 - RGB和LED4都以200ms闪烁
        rgbPriority = LED_PRIORITY_CALIBRATING;
        led4Priority = LED_PRIORITY_CALIBRATING;
        rgbState = LED_STATE_BLINKING;
        led4State = LED_STATE_BLINKING;
        rgbPeriod = 200;
        led4Period = 200;
    }
        /* 优先级第 4 档：对码中 —— 500ms 慢闪（“我在等你配对”）。 注意原作者注释写“LED4 保持常亮”，但下面的代码把 led4State/led4Period 也 改成了闪烁 —— 注释与代码不一致；对功能无害（对码阶段飞机在地上）。未改动代码 */
    else if (alertStatus.pairing) {
                // 对码状态优先级 - 只有RGB灯以500ms闪烁，LED4保持常亮
        rgbPriority = LED_PRIORITY_PAIRING;
				led4Priority =LED_PRIORITY_PAIRING;
        rgbState = LED_STATE_BLINKING;
        led4State = LED_STATE_BLINKING;
        rgbPeriod = 500;
        led4Period = 500;
    }
    
        // 只有当优先级变化时才更新状态
                /* ★关键：先在两边都判断“优先级变了没有”，变了才动手。 原因见函数头说明：本函数会被每个控制周期反复调用，无条件刷新会不断重置 闪烁基准时间 lastToggleTime，导致灯不闪（卡在亮或灭） */
    if (rgbLedControl.currentPriority != rgbPriority) {
        rgbLedControl.currentPriority = rgbPriority;
        if (rgbPriority == LED_PRIORITY_NORMAL) {
                        // 恢复正常状态
                                                /* 走这里说明报警解除了：用备份的“正常状态/周期”恢复（而不是把报警状态留下） */
            Led_SetState(RGB_LED, rgbLedControl.normalState, rgbLedControl.normalBlinkPeriod);
        } else {
                        // 设置报警状态 - 只改变闪烁模式，不改变颜色
                                                /* 注意这里只改“状态/周期”，颜色分量 red/green/blue 不动， 于是报警期间颜色仍是遥控器选的颜色（Led_Process 会按当前颜色刷新 CCR） */
            Led_SetState(RGB_LED, rgbState, rgbPeriod);
        }
    }
    
                /* LED4 同理，独立判断：它的优先级可能与 RGB 不同（例如失联时 LED4 仍是 NORMAL） */
    if (led4Control.currentPriority != led4Priority) {
        led4Control.currentPriority = led4Priority;
        if (led4Priority == LED_PRIORITY_NORMAL) {
            Led_SetState(LED4, led4Control.normalState, led4Control.normalBlinkPeriod);
        } else {
            Led_SetState(LED4, led4State, led4Period);
        }
    }
}

/* @brief  初始化 TIM1 的三路 PWM，驱动 RGB 三色灯 【参数】Prescaler：预分频（main.c 传 4-1，实际分频 = 4） Period   ：自动重装值（main.c 传 600-1，实际周期 = 600 个计数） 【返回】无 */
void Advanced_Timer1_PWM_Init(uint16_t Prescaler, uint16_t Period)
{
    TIM_OC_InitTypeDef Advanced_Timer1_OC_PWM_Config;            /* 定时器PWM输出配置 */



    Advanced_Timer1_PWM_Handle.Instance = TIM1;																
                /* 定时器x */
    Advanced_Timer1_PWM_Handle.Init.Period = Period;                                            /* 自动重装载值 */	

    Advanced_Timer1_PWM_Handle.Init.Prescaler = Prescaler;                                      /* 定时器分频 */
    Advanced_Timer1_PWM_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;                           /* 向上计数模式 */
    Advanced_Timer1_PWM_Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;                     /* 时钟分频因子 */
    Advanced_Timer1_PWM_Handle.Init.RepetitionCounter = 0;                                      /* 不重复计数 */
                /* 自动重装载寄存器没有缓冲 */
    Advanced_Timer1_PWM_Handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE; 
    HAL_TIM_PWM_Init(&Advanced_Timer1_PWM_Handle);                                              /* 初始化PWM（脉宽调制） */

    Advanced_Timer1_OC_PWM_Config.OCMode = TIM_OCMODE_PWM1;                                     /* 模式选择PWM1 */
    Advanced_Timer1_OC_PWM_Config.Pulse = Period;                                       /* ( Period + 1 ) / 2设置比较值,此值用来确定占空比,默认50% */
    Advanced_Timer1_OC_PWM_Config.OCPolarity = TIM_OCPOLARITY_HIGH;                             /* 输出比较极性为高 */
    Advanced_Timer1_OC_PWM_Config.OCFastMode = TIM_OCFAST_DISABLE;
    Advanced_Timer1_OC_PWM_Config.OCIdleState = TIM_OCIDLESTATE_SET;
    Advanced_Timer1_OC_PWM_Config.OCNIdleState = TIM_OCNIDLESTATE_RESET;                        /* 空闲状态OC1N输出低电平 */             
    Advanced_Timer1_OC_PWM_Config.OCIdleState  = TIM_OCIDLESTATE_RESET; 				        /* 空闲状态OC1输出低电平 */

                /* 配置TIMx通道1 */

    HAL_TIM_PWM_ConfigChannel(&Advanced_Timer1_PWM_Handle, &Advanced_Timer1_OC_PWM_Config, TIM_CHANNEL_1); 
                /* 使能定时器x和定时器x更新中断 */
    HAL_TIM_PWM_Start(&Advanced_Timer1_PWM_Handle, TIM_CHANNEL_1);
    
                /* 配置TIMx通道2 */
    HAL_TIM_PWM_ConfigChannel(&Advanced_Timer1_PWM_Handle, &Advanced_Timer1_OC_PWM_Config, TIM_CHANNEL_2); 
                /* 使能定时器x和定时器x更新中断 */
    HAL_TIM_PWM_Start(&Advanced_Timer1_PWM_Handle, TIM_CHANNEL_2);

                /* 配置TIMx通道3 */
    HAL_TIM_PWM_ConfigChannel(&Advanced_Timer1_PWM_Handle, &Advanced_Timer1_OC_PWM_Config, TIM_CHANNEL_3); 
                /* 使能定时器x和定时器x更新中断 */
    HAL_TIM_PWM_Start(&Advanced_Timer1_PWM_Handle, TIM_CHANNEL_3);

}

/* @brief  设置 RGB 三色亮度（0~1000 千分比，超出会被限幅） 【参数】led：要设置的灯（只有 RGB_LED 有效）；red/green/blue：三色亮度 0~1000 【返回】无 【算法】 */
/* 设置RGB灯颜色 */
/* 设置RGB灯颜色 - 遥控器调用此函数设置颜色 */
void Led_SetRGBColor(Led_TypeDef led, uint16_t red, uint16_t green, uint16_t blue)
{
    if (led == RGB_LED) {
        rgbLedControl.red = (red > 1000) ? 1000 : red;
        rgbLedControl.green = (green > 1000) ? 1000 : green;
        rgbLedControl.blue = (blue > 1000) ? 1000 : blue;
        
                // 立即更新PWM输出（如果当前状态是ON或闪烁的亮起阶段）
        if (rgbLedControl.state == LED_STATE_ON) {
            Advanced_Timer1_PWM_Handle.Instance->CCR2 = (1000 - rgbLedControl.red) * 600 / 1000;
            Advanced_Timer1_PWM_Handle.Instance->CCR3 = (1000 - rgbLedControl.green) * 600 / 1000;
            Advanced_Timer1_PWM_Handle.Instance->CCR1 = (1000 - rgbLedControl.blue) * 600 / 1000;
        }
                // 注意：在闪烁状态下，颜色会在Led_Process的下一个闪烁周期生效
    }
}

/* 初始化LED控制对象 */
/* 初始化LED控制对象 */
/* 只复位“软件状态机的初值”，不碰硬件寄存器（硬件在 Led_Init() 里配）。 两盏灯的初值都是：OFF、周期 500ms、优先级 NORMAL、颜色全 0（即不亮） */
void Led_ControlInit(void)
{
        // 初始化RGB灯控制参数
    rgbLedControl.state = LED_STATE_OFF;
    rgbLedControl.blinkPeriod = 500;
    rgbLedControl.normalBlinkPeriod = 500;
    rgbLedControl.lastToggleTime = 0;
    rgbLedControl.red = 0;
    rgbLedControl.green = 0;
    rgbLedControl.blue = 0;
    rgbLedControl.currentPriority = LED_PRIORITY_NORMAL;
    rgbLedControl.normalState = LED_STATE_OFF;
    
        // 初始化LED4控制参数
    led4Control.state = LED_STATE_OFF;
    led4Control.blinkPeriod = 500;
    led4Control.normalBlinkPeriod = 500;
    led4Control.lastToggleTime = 0;
    led4Control.currentPriority = LED_PRIORITY_NORMAL;
    led4Control.normalState = LED_STATE_OFF;
}



void Led_Init(void)
{
	GPIO_InitTypeDef	GPIO_Led_InitConfig;
	
				/* GPIOA时钟使能 */
	LED4_GPIO_CLK_ENABLE();
 	
	GPIO_Led_InitConfig.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Led_InitConfig.Pull = GPIO_PULLUP;
	GPIO_Led_InitConfig.Speed = GPIO_SPEED_FREQ_HIGH;
	
								/* LED4 GPIO初始化 */
	GPIO_Led_InitConfig.Pin = LED4_GPIO_PIN;
	HAL_GPIO_Init(LED4_GPIO_PORT, &GPIO_Led_InitConfig);

				/* LED 关闭 */
				/* BSRR（GPIO 置位寄存器） = 置位寄存器（写这个寄存器对应位 = 引脚输出高电平）。 硬件低电平点亮，所以“输出高”就是灭 —— 上电先灭灯，避免亮着让人误判状态 */
	WRITE_REG(LED4_GPIO_PORT->BSRR,LED4_GPIO_PIN);
	Led_ControlInit();
	
}



/* @brief  设置某盏灯的状态（灭/常亮/闪烁）并立即更新硬件 【参数】led   ：哪盏灯 state ：LED_STATE_OFF / ON / BLINKING period：闪烁周期（ms）。传 0 表示“沿用上一周期”（见下面 if (period > 0)） */
/* 设置LED状态 - 非阻塞方式 */
void Led_SetState(Led_TypeDef led, LedState_t state, uint32_t period)
{
    LedControl_t *ledControl;


    
    if (led == RGB_LED) {
        ledControl = &rgbLedControl;
    } else {
        ledControl = &led4Control;
    }
    
        // 备份正常状态（只在非报警状态下备份）
                /* 只有在“没有报警接管”时才备份：否则报警期间设的闪烁状态会被当成正常状态存下来， 报警解除后灯就回不到原来的样子了（见文件头取舍说明） */
    if (ledControl->currentPriority == LED_PRIORITY_NORMAL) {
        ledControl->normalState = state;
        if (period > 0) {
            ledControl->normalBlinkPeriod = period;
        }
    }
    
        // 更新当前状态
    ledControl->state = state;
    
    if (period > 0) {
        ledControl->blinkPeriod = period;
    }
    
        // 根据新状态设置LED初始状态
    switch (state) {
        case LED_STATE_OFF:
            if (led == RGB_LED) {
                Advanced_Timer1_PWM_Handle.Instance->CCR2 = 600;
                Advanced_Timer1_PWM_Handle.Instance->CCR3 = 600;
                Advanced_Timer1_PWM_Handle.Instance->CCR1 = 600;
            } else {
                WRITE_REG(LED4_GPIO_PORT->BSRR, LED4_GPIO_PIN); 
            }
            break;
        case LED_STATE_ON:
            if (led == RGB_LED) {
                                // 使用当前设置的颜色（由遥控器设置）
                Advanced_Timer1_PWM_Handle.Instance->CCR2 = (1000 - rgbLedControl.red) * 600 / 1000;
                Advanced_Timer1_PWM_Handle.Instance->CCR3 = (1000 - rgbLedControl.green) * 600 / 1000;
                Advanced_Timer1_PWM_Handle.Instance->CCR1 = (1000 - rgbLedControl.blue) * 600 / 1000;
            } else {
                WRITE_REG(LED4_GPIO_PORT->BRR, LED4_GPIO_PIN); 
            }
            break;
        case LED_STATE_BLINKING:
            if (led == RGB_LED) {
                                // 闪烁开始时先关闭
                Advanced_Timer1_PWM_Handle.Instance->CCR2 = 600;
                Advanced_Timer1_PWM_Handle.Instance->CCR3 = 600;
                Advanced_Timer1_PWM_Handle.Instance->CCR1 = 600;
            } else {
                WRITE_REG(LED4_GPIO_PORT->BSRR, LED4_GPIO_PIN); 
            }
            ledControl->lastToggleTime = HAL_GetTick();
            break;
        default:
            break;
    }
}



/* @brief  LED 灯语心跳：选颜色 + 非阻塞翻转（必须定期在主循环里调用） 【参数】无（读全局 color 与两盏灯的结构体） 【返回】无 【算法/为什么这么写】 */
/* LED状态机处理函数 - 需在主循环中定期调用 */
void Led_Process(void)
{
    uint32_t currentTime = HAL_GetTick();           /* 毫秒时基：所有时间比较都用它（无符号相减，回绕也安全） */

    static uint8_t rgbLedBlinkState = 0;  // RGB灯闪烁状态：0-熄灭，1-亮起
                /* 颜色编号 → 预设色。编号来自遥控器按键（3 位，共 8 种）， 映射关系与 deal_pack.h 里 light_mode 的 3 位编码一一对应 */
        /* 【关键字·switch/case】三个要点： ① 每个 case 末尾必须 break，漏了会“贯穿”执行下一个 case（本 switch 的每个 case 都是单语句 + break，没有有意贯穿）； ② 末尾的 default 建议写：颜色编号非法时保持上次颜色，行为确定； */
    switch(color) {
        case 0: RGB_Cyan(); break;
        case 1: RGB_Green(); break;
        case 2: RGB_Blue(); break;
        case 3: RGB_Yellow(); break;
        case 4: RGB_Red(); break;
        case 5: RGB_Magenta(); break;
        case 6: RGB_White(); break;
        case 7: RGB_Pink(); break;
        default: 
            break;                           /* color 越界（理论上不会）时保持上次颜色 */
    }
        // 处理RGB灯状态
                /* 只有“闪烁”状态才需要本函数干活：常亮/常灭在 Led_SetState() 时就写好了硬件 */
    if (rgbLedControl.state == LED_STATE_BLINKING) {
                                /* 半周期判定：用无符号减法，lastToggleTime 是上一次翻转的时刻 */
        if ((currentTime - rgbLedControl.lastToggleTime) >= (rgbLedControl.blinkPeriod / 2)) {
                        // 切换RGB灯状态
            if (rgbLedBlinkState == 0) {
                                // 亮起状态：显示设置的颜色
                Advanced_Timer1_PWM_Handle.Instance->CCR2 = (1000 - rgbLedControl.red) * 600 / 1000;
                Advanced_Timer1_PWM_Handle.Instance->CCR3 = (1000 - rgbLedControl.green) * 600 / 1000;
                Advanced_Timer1_PWM_Handle.Instance->CCR1 = (1000 - rgbLedControl.blue) * 600 / 1000;
                rgbLedBlinkState = 1;
            } else {
                                // 熄灭状态
                Advanced_Timer1_PWM_Handle.Instance->CCR2 = 600;
                Advanced_Timer1_PWM_Handle.Instance->CCR3 = 600;
                Advanced_Timer1_PWM_Handle.Instance->CCR1 = 600;
                rgbLedBlinkState = 0;
            }
            rgbLedControl.lastToggleTime = currentTime;
        }
    }
    
        // 处理LED4状态
                /* LED4 用 HAL_GPIO_TogglePin() 翻转：把引脚电平取反（亮↔灭）。 与 RGB 不同，它不需要“记住当前是亮是灭”——引脚电平本身就是记忆 */
    if (led4Control.state == LED_STATE_BLINKING) {
        if ((currentTime - led4Control.lastToggleTime) >= (led4Control.blinkPeriod / 2)) {
            HAL_GPIO_TogglePin(LED4_GPIO_PORT, LED4_GPIO_PIN);
            led4Control.lastToggleTime = currentTime;
        }
    }
}

/* 下面 8 个预设色：入参是 RGB 三色亮度（0~1000）。 它们只负责“调色”，不负责亮灭；亮灭由状态机（ON/BLINKING）决定， 所以同一颜色在常亮和闪烁下都能用 */
void RGB_Red(void)       { Led_SetRGBColor(RGB_LED, 1000, 0, 0); }
void RGB_Green(void)     { Led_SetRGBColor(RGB_LED, 0, 1000, 0); }
void RGB_Blue(void)      { Led_SetRGBColor(RGB_LED, 0, 0, 1000); }
void RGB_Yellow(void)    { Led_SetRGBColor(RGB_LED, 1000, 1000, 0); }
void RGB_Cyan(void)      { Led_SetRGBColor(RGB_LED, 0, 1000, 1000); }
void RGB_Magenta(void)   { Led_SetRGBColor(RGB_LED, 1000, 0, 1000); }
void RGB_White(void)     { Led_SetRGBColor(RGB_LED, 1000, 1000, 1000); }

void RGB_Pink(void)      { Led_SetRGBColor(RGB_LED, 1000, 400, 0); }

