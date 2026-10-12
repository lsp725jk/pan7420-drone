/**
 * ch08_ex13.c —— 用"版本/平台检查"在编译期挡住错误配置
 */
#include <stdio.h>

#define HAL_VERSION_MAJOR  1
#define HAL_VERSION_MINOR  4
#define BOARD_PY32F030     1
#define BOARD_PY32F040     2

#define BOARD  BOARD_PY32F030

/**
 * 用法 1：把"必须满足的条件"写成 #error，配错了根本编不出固件
 */
#if !defined(BOARD)
#error "没有选择板型：请定义 BOARD_PY32F030 或 BOARD_PY32F040"
#endif

/* 用法 2：版本下限检查（真实工程里常用来挡住过旧的 HAL） */
#if HAL_VERSION_MAJOR < 1 || (HAL_VERSION_MAJOR == 1 && HAL_VERSION_MINOR < 2)
#error "HAL 版本过低，至少需要 1.2"
#endif



#ifdef __STDC_VERSION__
#if __STDC_VERSION__ >= 202311L
#ifdef BOARD_PY32F040
#warning "PY32F040 分支尚未在硬件上验证"
#endif
#endif
#endif

/* 用法 4：把检查结果变成宏，主程序可以再判断一次 */
#if BOARD == BOARD_PY32F030
#define BOARD_NAME "PY32F030"
#elif BOARD == BOARD_PY32F040
#define BOARD_NAME "PY32F040"
#else
#error "未知板型"
#endif

int main(void)
{
    printf("板型        : %s\n", BOARD_NAME);
    printf("HAL 版本    : %d.%d\n", HAL_VERSION_MAJOR, HAL_VERSION_MINOR);
    printf("编译通过了 —— 说明上面的 #error 条件都不成立\n");
    return 0;
}
