#ifndef __BUZZER_H__
#define __BUZZER_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include "board.h"

#define BUZZER_PORT GPIOA
#define BUZZER_PIN  GPIO_PIN_8



/**
 * 蜂鸣器工作模式枚举
 */
typedef enum {
    BUZZER_MODE_OFF = 0,    // 关闭
    BUZZER_MODE_SHORT,      // 短响
    BUZZER_MODE_LONG,       // 长响
    BUZZER_MODE_DOUBLE,     // 双响
    BUZZER_MODE_CUSTOM      // 自定义模式
} BuzzerMode_TypeDef;



/**
 * 函数声明
 */
void Buzzer_Init(void);
void Buzzer_Start(BuzzerMode_TypeDef mode);
void Buzzer_Stop(void);
void Buzzer_Update(void);
void Buzzer_SetCustomPattern(uint16_t *pattern, uint16_t length);



BuzzerMode_TypeDef Get_Buzzer_Statue(void);

#ifdef __cplusplus
}
#endif

#endif
