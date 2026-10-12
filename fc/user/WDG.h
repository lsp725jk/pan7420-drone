/**
 * 文件：user/WDG.h                        【角色】看门狗模块的对外声明
 */
#ifndef __WDG_H__
#define __WDG_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include "py32f0xx_hal.h"
#include "bsp.h"

/**
 * 看门狗句柄：定义在 WDG.c。加 extern 表示这里只是“引用声明”，不是定义。
 */
extern IWDG_HandleTypeDef   IwdgHandle;

/* 初始化看门狗（配置预分频/重装值并启动）。实现在 WDG.c。 */
void WDG_init(void);

#ifdef __cplusplus
}
#endif

#endif
/* 头文件卫士就此结束；配套实现见 user/WDG.c */

