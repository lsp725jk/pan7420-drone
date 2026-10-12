#include "key.h"
#include "buzzer.h"

// 引脚定义 - 根据实际硬件连接修改
#define ROW1_PIN GPIO_PIN_8
#define ROW1_PORT GPIOB
#define ROW2_PIN GPIO_PIN_7
#define ROW2_PORT GPIOB
#define ROW3_PIN GPIO_PIN_6
#define ROW3_PORT GPIOB

#define COL1_PIN GPIO_PIN_5
#define COL1_PORT GPIOB
#define COL2_PIN GPIO_PIN_4
#define COL2_PORT GPIOB
#define COL3_PIN GPIO_PIN_3
#define COL3_PORT GPIOB

static KeyState_t keyState = KEY_STATE_IDLE;
static uint32_t debounceStartTime = 0;
static KeyCode_t currentKey = KEY_NONE;
static KeyHandlerFunc keyHandler = NULL;
uint8_t key10_count = 0;	//用来记录挡位的

uint8_t key8_count = 0;	//用来灯光的

// uint8_t key9_count = 0;	//用来crsf某个功能切换用的
extern uint8_t crsf_send_en;

KeyBitStatus_t key_bit_status;



extern int16_t joystick_ch_offset[]; //这是两个摇杆的微调
extern uint8_t fly_status;
// 两个按键的实例
// 两个按键的实例
Key_t keys[] = {
    {
        .state = KEY_STATE_IDLE,
        .count = 0,
        .port = COL3_PORT,
        .pin = COL3_PIN,
        .key_code = KEY_9,
        .row_pin = ROW3_PIN,
        .row_port = ROW3_PORT
    },
    {
        .state = KEY_STATE_IDLE,
        .count = 0,
        .port = KEY_PORT,
        .pin = KEY_PIN,
        .key_code = KEY_10,
        .row_pin = 0,      // 独立按键不需要行引脚
        .row_port = NULL
    }
};

// 延时函数 - 非阻塞式
static uint8_t DelayElapsed(uint32_t startTime, uint32_t delayMs) {
    return (HAL_GetTick() - startTime) >= delayMs;
}

void Keypad_Init(void) {
        // 初始化行引脚为输出
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    __HAL_RCC_GPIOB_CLK_ENABLE();  // 根据实际端口修改
    
    GPIO_InitStruct.Pin = ROW1_PIN | ROW2_PIN | ROW3_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
        // 初始化列引脚为输入，上拉
    GPIO_InitStruct.Pin = COL1_PIN | COL2_PIN | COL3_PIN | KEY_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
        // 初始行输出为高电平
    HAL_GPIO_WritePin(ROW1_PORT, ROW1_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ROW2_PORT, ROW2_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ROW3_PORT, ROW3_PIN, GPIO_PIN_SET);
}

void Keypad_SetKeyHandler(KeyHandlerFunc handler) {
    keyHandler = handler;
}

static KeyCode_t ScanKeys(void) {
        // 逐行扫描
    HAL_GPIO_WritePin(ROW1_PORT, ROW1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(ROW2_PORT, ROW2_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ROW3_PORT, ROW3_PIN, GPIO_PIN_SET);
    HAL_Delay(1);  // 短暂延时稳定电平
    
    if (HAL_GPIO_ReadPin(COL1_PORT, COL1_PIN) == GPIO_PIN_RESET) return KEY_1;
    if (HAL_GPIO_ReadPin(COL2_PORT, COL2_PIN) == GPIO_PIN_RESET) return KEY_2;
    if (HAL_GPIO_ReadPin(COL3_PORT, COL3_PIN) == GPIO_PIN_RESET) return KEY_3;
    
    HAL_GPIO_WritePin(ROW1_PORT, ROW1_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ROW2_PORT, ROW2_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(ROW3_PORT, ROW3_PIN, GPIO_PIN_SET);
    HAL_Delay(1);  // 短暂延时稳定电平
    
    if (HAL_GPIO_ReadPin(COL1_PORT, COL1_PIN) == GPIO_PIN_RESET) return KEY_4;
    if (HAL_GPIO_ReadPin(COL2_PORT, COL2_PIN) == GPIO_PIN_RESET) return KEY_5;
    if (HAL_GPIO_ReadPin(COL3_PORT, COL3_PIN) == GPIO_PIN_RESET) return KEY_6;
    
    HAL_GPIO_WritePin(ROW1_PORT, ROW1_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ROW2_PORT, ROW2_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ROW3_PORT, ROW3_PIN, GPIO_PIN_RESET);
    HAL_Delay(1);  // 短暂延时稳定电平
    
    if (HAL_GPIO_ReadPin(COL1_PORT, COL1_PIN) == GPIO_PIN_RESET) return KEY_7;
	if(crsf_send_en == 0){
		if (HAL_GPIO_ReadPin(COL2_PORT, COL2_PIN) == GPIO_PIN_RESET) return KEY_8;
		if (HAL_GPIO_ReadPin(COL3_PORT, COL3_PIN) == GPIO_PIN_RESET) return KEY_9;
		
		if(HAL_GPIO_ReadPin(KEY_PORT,KEY_PIN)==0) return KEY_10;
	}


	
    return KEY_NONE;
}

// 处理单个按键
static uint8_t ProcessKey(Key_t* key) {
    uint8_t key_level;
    uint8_t result = KEY_NONE;
    
        // 如果有行引脚，先设置行引脚状态
    if (key->row_port != NULL && key->row_pin != 0) {
                // 先将所有行设置为高电平
        HAL_GPIO_WritePin(ROW1_PORT, ROW1_PIN, GPIO_PIN_SET);
        HAL_GPIO_WritePin(ROW2_PORT, ROW2_PIN, GPIO_PIN_SET);
        HAL_GPIO_WritePin(ROW3_PORT, ROW3_PIN, GPIO_PIN_SET);
        
                // 再将当前行设置为低电平
        HAL_GPIO_WritePin(key->row_port, key->row_pin, GPIO_PIN_RESET);
        HAL_Delay(1);  // 短暂延时稳定电平
    }
    
        // 读取按键电平
    key_level = HAL_GPIO_ReadPin(key->port, key->pin);
    
        // 状态机处理
    switch (key->state) {
        case KEY_STATE_IDLE:
            if (key_level == GPIO_PIN_RESET) {
                key->state = KEY_STATE_PRESSED;
				debounceStartTime = HAL_GetTick();
            }
            break;
            
        case KEY_STATE_DEBOUNCE_PRESS:
			if (DelayElapsed(debounceStartTime, 20)){
				if (key_level == GPIO_PIN_RESET) {
					key->state = KEY_STATE_PRESSED;
					result = key->key_code;  // 返回按键值
				} else {
					key->state = KEY_STATE_IDLE;
				}
			}
            break;
            
        case KEY_STATE_PRESSED:
            if (key_level == GPIO_PIN_SET) {
                key->state = KEY_STATE_DEBOUNCE_RELEASE;
                key->count ^=0x01;
				if(key->count & 0x01){
					Buzzer_Start(BUZZER_MODE_DOUBLE);
				}
				else
					Buzzer_Start(BUZZER_MODE_SHORT);
				debounceStartTime = HAL_GetTick();
            }
            break;
            
        case KEY_STATE_DEBOUNCE_RELEASE:
			if (DelayElapsed(debounceStartTime, 20)){
				if (key_level == GPIO_PIN_SET) {
					key->state = KEY_STATE_IDLE;
				} else {
					key->state = KEY_STATE_PRESSED;
				}
			}
            break;
    }
    
    return result;
}



// 按键扫描函数（每次主循环调用）
uint8_t key_crsf_scankey(void) {
    uint8_t key = KEY_NONE;
    
        // 遍历处理所有按键
    for (int i = 0; i < sizeof(keys) / sizeof(keys[0]); i++) {
        uint8_t result = ProcessKey(&keys[i]);
        if (result != KEY_NONE) {
            key = result;  // 返回触发的按键代码
            break;
        }
    }
    return key;
}

void Keypad_Scan(void) {
    static uint32_t lastScanTime = 0;
    
        // 限制扫描频率，避免过于频繁
    if (HAL_GetTick() - lastScanTime < 5) return;
    lastScanTime = HAL_GetTick();
    
    KeyCode_t key = ScanKeys();
    
    switch (keyState) {
        case KEY_STATE_IDLE:
            if (key != KEY_NONE) {
                currentKey = key;
                keyState = KEY_STATE_DEBOUNCE_PRESS;
                debounceStartTime = HAL_GetTick();
            }
            break;
            
        case KEY_STATE_DEBOUNCE_PRESS:
            if (DelayElapsed(debounceStartTime, 20)) {
                if (key == currentKey) {
                    keyState = KEY_STATE_PRESSED;
                                        // 调用按键处理函数
                    if (keyHandler) keyHandler(currentKey);
										// printf("key_bit_status.key_byte = %x\r\n",key_bit_status.key_byte);
					Buzzer_Start(BUZZER_MODE_SHORT);
					if(key == KEY_10){
						key10_count = !key10_count;
						if(key10_count){
							Buzzer_Start(BUZZER_MODE_DOUBLE);
						}
					}
					else if (key == KEY_6){
						if(fly_status){
							Buzzer_Start(BUZZER_MODE_DOUBLE);
						}
					}
					else if (key == KEY_1||key == KEY_3){
						if(joystick_ch_offset[0] == 0 ){
							Buzzer_Start(BUZZER_MODE_DOUBLE);
						}
					}
					else if (key == KEY_2||key == KEY_4){
						if(joystick_ch_offset[1]==0){
							Buzzer_Start(BUZZER_MODE_DOUBLE);
						}
					}
					
                } else {
                    keyState = KEY_STATE_IDLE;
                }
            }
            break;
            
        case KEY_STATE_PRESSED:
            if (key == KEY_NONE) {
                keyState = KEY_STATE_DEBOUNCE_RELEASE;
                debounceStartTime = HAL_GetTick();
            }
            break;
            
        case KEY_STATE_DEBOUNCE_RELEASE:
            if (DelayElapsed(debounceStartTime, 20)) {
                if (key == KEY_NONE) {
                    keyState = KEY_STATE_IDLE;
                    currentKey = KEY_NONE;
                } else {
                    keyState = KEY_STATE_PRESSED;
                }
            }
            break;
    }
}

KeyCode_t Keypad_GetKey(void) {
    return currentKey;
}
   
uint8_t is_pair_key(void){
    HAL_GPIO_WritePin(ROW1_PORT, ROW1_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ROW2_PORT, ROW2_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(ROW3_PORT, ROW3_PIN, GPIO_PIN_SET);
    HAL_Delay(20);  // 短暂延时稳定电平
    
    if (HAL_GPIO_ReadPin(COL2_PORT, COL2_PIN) == GPIO_PIN_RESET) return KEY_5;
	return 0;
}



uint8_t is_crsf_en(void){
    HAL_GPIO_WritePin(ROW1_PORT, ROW1_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ROW2_PORT, ROW2_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ROW3_PORT, ROW3_PIN, GPIO_PIN_RESET);
    HAL_Delay(20);  // 短暂延时稳定电平
   
	if (HAL_GPIO_ReadPin(COL2_PORT, COL2_PIN) == GPIO_PIN_RESET) return KEY_8;
	return 0;
}



/* @brief 检查是否触发校准模式 @retval 触发成功返回true，否则返回false */
uint8_t checkCalibrationTrigger(void) {
        // 这里使用特定按键组合触发(例如:长按校准键上电)
        // 实际应用中应根据硬件设计修改
    if(HAL_GPIO_ReadPin(KEY_PORT,KEY_PIN) == GPIO_PIN_RESET) {
        HAL_Delay(20); // 延时消抖
        if(HAL_GPIO_ReadPin(KEY_PORT,KEY_PIN) == GPIO_PIN_RESET) {
            return KEY_10;
        }
    }
    return 0;
}
