/**
 * 文件：user/flip.h                        【角色】翻滚特技（一键空翻）的状态定义与接口
 */
#ifndef __FLIP_H__
#define __FLIP_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include "py32f0xx_hal.h"
	 
/* 一键翻滚过程的计数与“接管标志”。 【字段含义】 qhqh       —— 前后翻（PITCH 轴）过程中，是否让角速度环接管该轴（1=接管） zyzy       —— 左右翻（ROLL 轴）过程中，是否让角速度环接管该轴（1=接管） */
typedef struct          //一键翻滚时间计数
{
	uint8_t qhqh;		
	uint8_t zyzy;
	uint16_t flip_count;
}sum_sum;

// 新增空翻状态标记（全局变量，需在头文件或全局定义）

typedef enum {
    FLIP_IDLE,       // 非空翻状态
    FLIP_FORWARD,    // 前空翻中
    FLIP_BACK,       // 后空翻中
    FLIP_LEFT,       // 左空翻中
    FLIP_RIGHT       // 右空翻中
} FlipState;

extern FlipState flip_state;

/* 空翻状态机主函数：实现在 flip.c。main.c 主循环里约每 4ms 调用一次 */
void Mode_Controler(void);

#ifdef __cplusplus
}
#endif

#endif
