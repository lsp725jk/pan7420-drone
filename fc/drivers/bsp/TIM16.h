#ifndef __TIM16_H
#define __TIM16_H
/**
 * 文件：drivers/bsp/TIM16.h               【角色】定时器模块的对外接口声明
 */

/**
 * 包含其他头文件
 */
#include "py32f0xx_hal.h"



void tim16_init(void);				/**
 * 配置并启动 TIM16：1us 一步的自由计数器（不开中断）
 */
uint16_t micros16(void);			/**
 * 读 TIM16 计数值当微秒时间戳（16 位，约 65.5ms 回绕）
 */
uint32_t micros32(void);			/* 软件扩展成 32 位微秒时间戳（本工程未使用） */
void Timer17_UpdateIT_Init(void);	/* 配置并启动 TIM17：1ms 周期中断（系统节拍源） */

#endif 
 
