#include "buzzer.h"

/**
 * 私有变量
 */
static BuzzerMode_TypeDef currentMode = BUZZER_MODE_OFF;
static uint32_t startTime = 0;
static uint32_t duration = 0;
static uint8_t phase = 0;
static uint16_t *customPattern = NULL;
static uint16_t customLength = 0;
static uint16_t customIndex = 0;

static uint8_t is_use_beering = 0; //0 no,1 yes
uint8_t is_use_doublebeering = 0;
/**
 * 预定义模式参数
 */
#define SHORT_BEEP_DURATION     100     // 100ms
#define LONG_BEEP_DURATION      1000    // 1000ms
#define DOUBLE_BEEP_PAUSE       30    // 20ms
#define DOUBLE_BEEP_TOTAL       (SHORT_BEEP_DURATION*2 + DOUBLE_BEEP_PAUSE)

/* 功能: 初始化蜂鸣器 参数: config - 蜂鸣器配置结构体指针 */
void Buzzer_Init(void)
{ 
        // 配置GPIO为推挽输出
    GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIOA_CLK_ENABLE();  
    GPIO_InitStruct.Pin = BUZZER_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(BUZZER_PORT, &GPIO_InitStruct);
    
        // 默认关闭蜂鸣器
    Buzzer_Stop();
}

/* 功能: 打开蜂鸣器 */
static void Buzzer_On(void)
{
    BUZZER_PORT->BSRR = (uint32_t)BUZZER_PIN;
}

/* 功能: 关闭蜂鸣器 */
static void Buzzer_Off(void)
{
    BUZZER_PORT->BRR = (uint32_t)BUZZER_PIN;
}



/* 功能: 启动蜂鸣器，设置工作模式 参数: mode - 蜂鸣器工作模式 */
void Buzzer_Start(BuzzerMode_TypeDef mode)
{
	if(is_use_beering)
		return ;
    currentMode = mode;
    startTime = HAL_GetTick();
    phase = 0;
    customIndex = 0;
    
    switch(mode) {
        case BUZZER_MODE_SHORT:
            duration = SHORT_BEEP_DURATION;
            Buzzer_On();
            break;
            
        case BUZZER_MODE_LONG:
            duration = LONG_BEEP_DURATION;
            Buzzer_On();
            break;
            
        case BUZZER_MODE_DOUBLE:
            duration = SHORT_BEEP_DURATION;
            Buzzer_On();
            break;
            
        case BUZZER_MODE_CUSTOM:
            if (customPattern && customLength > 0) {
                duration = customPattern[0];
                Buzzer_On();
            } else {
                currentMode = BUZZER_MODE_OFF;
            }
            break;
            
        case BUZZER_MODE_OFF:
        default:
            Buzzer_Stop();
            break;
    }
}

/* 功能: 停止蜂鸣器 */
void Buzzer_Stop(void)
{
    currentMode = BUZZER_MODE_OFF;
	is_use_beering = 0;
    Buzzer_Off();
}

/* 功能: 蜂鸣器状态机更新函数，需在主循环中定期调用 */
void Buzzer_Update(void)
{
    if (currentMode == BUZZER_MODE_OFF) {
        return;
    }
    
    uint32_t elapsed = HAL_GetTick() - startTime;
    
		// 增加个蜂鸣器正在使用的标志
	is_use_beering = 1;
    if (elapsed >= duration) {
		
        switch(currentMode) {
            case BUZZER_MODE_SHORT:
                Buzzer_Stop();
			
                break;
                
            case BUZZER_MODE_LONG:
                Buzzer_Stop();
                break;
                
            case BUZZER_MODE_DOUBLE:
                if (phase == 0) {
                                        // 第一个短响结束，进入间隔
                    Buzzer_Off();
                    phase = 1;
                    duration = DOUBLE_BEEP_PAUSE;
                    startTime = HAL_GetTick();
                } else if (phase == 1) {
                                        // 间隔结束，开始第二个短响
                    Buzzer_On();
                    phase = 2;
                    duration = SHORT_BEEP_DURATION;
                    startTime = HAL_GetTick();
                } else {
                                        // 第二个短响结束，全部完成
                    Buzzer_Stop();
                }
                break;
                
// case BUZZER_MODE_CUSTOM:
// customIndex++;
// if (customIndex >= customLength) {
// // 自定义模式结束
// Buzzer_Stop();
// } else {
// // 切换到下一阶段
// if (customIndex % 2 == 0) {
// Buzzer_On();  // 奇数索引为开启状态
// } else {
// Buzzer_Off(); // 偶数索引为关闭状态
// }
// duration = customPattern[customIndex];
// startTime = HAL_GetTick();
// }
// break;
                
            default:
                Buzzer_Stop();
			is_use_beering = 0;
                break;
        }
    }
}

/* 功能: 设置自定义蜂鸣器模式 length - 数组长度 */
void Buzzer_SetCustomPattern(uint16_t *pattern, uint16_t length)
{
    customPattern = pattern;
    customLength = length;
}    



BuzzerMode_TypeDef Get_Buzzer_Statue(void){
	return currentMode;
}
