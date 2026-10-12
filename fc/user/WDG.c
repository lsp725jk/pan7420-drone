/**
 * 一、这个文件在项目里干什么
 */
#include "bsp.h"



/* 看门狗句柄（Handle）：HAL 用这个结构体记住“IWDG 的配置和状态”。 定义为全局非 static，是为了 main.c 里能用 extern 拿到它去喂狗。 = {0} 是 C 的“聚合初始化”，把结构体所有成员清零，避免用到未初始化的野值。 */
IWDG_HandleTypeDef   IwdgHandle = {0};

/**
 * @brief  初始化独立看门狗（IWDG），并立即开始计时 【参数】无 【返回】无 【算法/为什么这么写】
 */
void WDG_init(void)
{
	IwdgHandle.Instance = IWDG;                     		/* Select IWDG */
				/* 指定要操作的是 IWDG 这个外设（HAL 靠 Instance 指针区分 IWDG / WWDG（窗口看门狗） / TIM 等） */
  IwdgHandle.Init.Prescaler = IWDG_PRESCALER_32;      /* Configure prescaler to 32 */
        /* 预分频 32：把 LSI 的 32768Hz 分频成 1024Hz，即计数器每约 0.977ms 减 1， 这是本项目“计数器单位 = 毫秒”的换算基础（魔法数字的来历） */
  IwdgHandle.Init.Reload = (512);                    /* Set IWDG（独立看门狗） counter reload value to 1024, 1s */

        /* Initialize IWDG */
  if (HAL_IWDG_Init(&IwdgHandle) != HAL_OK)       
  {
    printf("看门狗初始化失败");
  }
}



