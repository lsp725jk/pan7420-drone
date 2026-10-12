/**
 * ch08_ex11.c —— 条件编译（conditional compilation）基础：
 */
#include <stdio.h>

#define BOARD_PY32F030      1     /**
 * 当前板型
 */
#define USE_BARO            1     /**
 * 是否启用气压计
 */
#define DEBUG_LEVEL         2     /**
 * 0=关 1=基础 2=详细
 */

int main(void)
{
                /**
         * 1) #ifdef / #ifndef：只关心"有没有定义过"
         */
#ifdef BOARD_PY32F030
    printf("1) #ifdef  BOARD_PY32F030 成立\n");
#else
    printf("1) #ifdef  BOARD_PY32F030 不成立\n");
#endif

#ifndef BOARD_TEST
    printf("   #ifndef BOARD_TEST 成立（没定义过也算「成立」）\n");
#endif

                /* 2) #if + defined()：可以组合多个条件，是最灵活的写法 */
#if defined(BOARD_PY32F030) && !defined(BOARD_TEST)
    printf("2) #if defined(...) && !defined(...) 成立\n");
#endif

                /* 3) #if 比的是"数值"，所以能写区间和比较 */
#if DEBUG_LEVEL == 0
    printf("3) DEBUG_LEVEL == 0 ：不打印调试信息\n");
#elif DEBUG_LEVEL == 1
    printf("3) DEBUG_LEVEL == 1 ：基础调试\n");
#elif DEBUG_LEVEL >= 2
    printf("3) DEBUG_LEVEL >= 2 ：详细调试（当前值 %d）\n", DEBUG_LEVEL);
#else
    printf("3) 其它值\n");
#endif

                /* 4) 变式：#if 0 大段屏蔽代码 —— 比块注释安全，因为它可以嵌套 */
#if 0
    printf("这一整段不会被编译，写错了语法也不会报错 %d\n", "甚至可以不是合法 C");
#endif

                /* 5) 变式：整段函数按开关取舍，不占 Flash */
#if USE_BARO
    printf("5) USE_BARO 打开：编译进了气压计相关代码\n");
#else
    printf("5) USE_BARO 关闭：气压计代码根本不存在\n");
#endif
    return 0;
}
