/**
 * 文件：user/deal_pack.h                    【角色】通信数据包的结构定义（按键位域、空翻方向）
 */
#ifndef __DEAL_PACK_H__
#define __DEAL_PACK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "py32f0xx_hal.h"
#include "bsp.h"
/* 收发共用的 12 字节缓冲：定义在 deal_pack.c。发送（回传状态）与接收（解析遥控）都用它 */
extern uint8_t temp_buff[__PN298_PAYLOAD];	//数据接收或者发送缓冲区

/* 按键位域联合体：一个字节 = 8 个开关量。 【语法】位域 a:1 的作用是“只用 1 个 bit 存这个成员”，属于紧凑存储布尔标志的做法； union 让 key_bits（按位）与 key_byte（整字节）映射到同一块内存。 */
// 使用联合体(union)方便按位操作和整体访问 位域
typedef union {
    struct {
        uint8_t is_rolling       : 1;  // key5: 对频/对频解锁后是翻滚
        uint8_t is_armed         : 1;  // key6: 解锁/一键起飞
        uint8_t is_althold       : 1;  // key7: 定高
        uint8_t is_steeringon    : 1;  // key9: 舵机开关
        uint8_t speed_level      : 1;  // key10: 速度档位
				uint8_t light_mode       : 3;  // key8: 切换灯光(3位支持8种模式)
    } key_bits;
    uint8_t key_byte;             // 整个字节值
} KeyBitStatus_t;

/* 一键翻滚方向：与遥控器发来的“第 12 字节低 4 位”一一对应。 所以枚举值就是协议值：0=不翻、1=前、2=后、3=左、4=右。 改这里的顺序会同时影响通信协议与状态机。 */
typedef enum{
	NONE,
	FORWARD,
	BACK,
	LEFT,
	RIGHT,
}Flip_Dirs;



void decode_pack(void);
void get_now_addr(void);

#ifdef __cplusplus
}
#endif

#endif 

