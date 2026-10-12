/**
 * @file    main.h
 * @author  MCU Application Team
 * @brief   Header for main.c file.
 * @attention
 */

/* Define to prevent recursive inclusion ------------------------------------- */
#ifndef __MAIN_H
#define __MAIN_H

/* 一、这个文件在项目里干什么 它只干三件事： ② 声明两个“出错就停机”的函数原型：Error_Handler() / APP_ErrorHandler()； ③ 提供 include guard 与 extern "C" 包裹，保证重复包含和 C++ 编译都不出错。 */

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------ */
/* HAL 总头：真正的“模块裁剪表”在 user/py32f0xx_hal_conf.h 里 */
#include "py32f0xx_hal.h"
// #include "py32f030xx_Start_Kit.h"

/* Private includes ---------------------------------------------------------- */
/* Private defines ----------------------------------------------------------- */
/* Exported variables prototypes --------------------------------------------- */
/* Exported functions prototypes --------------------------------------------- */
/* 注意：APP_ErrorHandler() 在本工程里没有实现（只有这一个声明），也没有任何调用点， 属于历史遗留声明，未改动。 */
void APP_ErrorHandler(void);
/* 初始化失败时的“终点”：实现在 main.c 末尾，函数体是 while(1) 死循环。 被 SystemClock_Config()（时钟配置失败）等 HAL 初始化流程调用。 */
void Error_Handler(void);
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/* (C) COPYRIGHT Puya *****END OF FILE */
