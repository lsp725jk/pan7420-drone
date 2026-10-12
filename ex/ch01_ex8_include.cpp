/**
 * ch01_ex8_include.c —— #include 到底做了什么：头文件、尖括号 vs 双引号、卫士
 */
#include <stdio.h>          /**
 * 尖括号：去系统头文件目录里找
 */
#include "ch01_ex8_include.h"   /**
 * 双引号：先在本文件所在目录找，再找系统目录
 */
#include "ch01_ex8_include.h"   /* 故意包含第二次：卫士会挡住，不会重复定义 */

/* 头文件里声明过的函数，在这里定义 */
int led_state_to_text_len(void)
{
    return 3;                       /* "ON" / "OFF" 里最长的长度 */
}

const char *led_state_to_text(int state)
{
    return state ? "ON" : "OFF";
}

int main(void)
{
    printf("---- 1. 用头文件里的宏和函数 ----\n");
    printf("LED_ON = %d, LED_OFF = %d\n", LED_ON, LED_OFF);
    printf("led_state_to_text(LED_ON)  = %s\n", led_state_to_text(LED_ON));
    printf("led_state_to_text(LED_OFF) = %s\n", led_state_to_text(LED_OFF));
    printf("led_state_to_text_len()    = %d\n", led_state_to_text_len());

    printf("---- 2. 为什么不加卫士会出事 ----\n");
    printf("  第一次 #include 把内容插进来，定义了 LED_ON 等名字；\n");
    printf("  第二次 #include 又插一遍，同样的宏被重复定义 ——\n");
    printf("  宏重复定义如果内容不一致会报 warning C4005；\n");
    printf("  结构体/枚举重复定义会直接报 error C2011。\n");
    printf("  加了三行卫士（#ifndef / #define / #endif）之后，第二次就整个跳过了。\n");

    printf("---- 3. 尖括号和双引号的搜索顺序 ----\n");
    printf("  #include <stdio.h>         先找系统目录\n");
    printf("  #include \"my.h\"           先找当前源文件所在目录，再找系统目录\n");
    printf("  所以自己写的头文件用双引号，标准库/第三方库用尖括号。\n");
    printf("  把自定义头文件写成 <my.h> 也能编过（只要加了 /I 包含路径），\n");
    printf("  但那是误导后人 —— 别这么写。\n");
    printf("---- 4. 现代替代方案 ----\n");
    printf("  很多编译器支持 #pragma once，比三行卫士短；\n");
    printf("  但它不是 C 标准的一部分（C23 也没有纳入），\n");
    printf("  要兼容性就用卫士，要简洁就用 pragma once。\n");
    return 0;
}
