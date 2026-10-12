#ifndef __KEY_H__
#define __KEY_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include "board.h"

#define KEY_PORT GPIOB
#define KEY_33_OUT_PIN GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8
#define KEY_33_IN_PIN GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5
#define KEY_PIN GPIO_PIN_2

/**
 * key10							key9
 */
// key10速度 key9舵机开关 key5对频/对频解锁后是翻滚 key6解锁/一键起飞 key7定高 key8切换灯光



// 按键定义
typedef enum {
    KEY_NONE = 0,
    KEY_1, KEY_2, KEY_3,
    KEY_4, KEY_5, KEY_6,
    KEY_7, KEY_8, KEY_9,
	KEY_10
} KeyCode_t;

// 按键状态机状态
typedef enum {
    KEY_STATE_IDLE,
    KEY_STATE_DEBOUNCE_PRESS,
    KEY_STATE_PRESSED,
    KEY_STATE_DEBOUNCE_RELEASE
} KeyState_t;

// 按键结构体
typedef struct {
    KeyState_t state;       // 当前状态
    uint8_t count;        // 消抖计数器
    GPIO_TypeDef* port;   // GPIO端口
    uint16_t pin;         // GPIO引脚
    KeyCode_t key_code;     // 按键代码
	GPIO_TypeDef* row_port; // 行端口
    uint16_t row_pin;      // 行引脚(用于矩阵键盘)
   
} Key_t;



// 使用联合体(union)方便按位操作和整体访问 位域
typedef union {
    struct {
        uint8_t key5    : 1;  // 第0位：按键1状态
        uint8_t key6    : 1;  // 第1位：按键2状态
        uint8_t key7    : 1;  // 第2位：按键3状态
        uint8_t key9    : 1;  // 第3位：按键4状态
        uint8_t key10   : 1;  // 第4位：按键5状态
        uint8_t key8    : 3;  // 第5位：按键6状态
    } key_bits;
    uint8_t key_byte;             // 整个字节值
} KeyBitStatus_t;



void Keypad_Init(void);
void Keypad_Scan(void);
KeyCode_t Keypad_GetKey(void);
uint8_t is_pair_key(void);
// 按键处理回调函数类型定义
typedef void (*KeyHandlerFunc)(KeyCode_t key);

// 设置按键处理回调函数
void Keypad_SetKeyHandler(KeyHandlerFunc handler);

uint8_t is_crsf_en(void);
uint8_t key_crsf_scankey(void) ;
uint8_t checkCalibrationTrigger(void);
#ifdef __cplusplus
}
#endif

#endif
