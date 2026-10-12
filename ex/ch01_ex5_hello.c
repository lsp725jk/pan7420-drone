/**
 * ch01_ex5_hello.c —— 最小的 Hello World，以及 main 的三种写法
 */
#include <stdio.h>

/**
 * main 的定义。注意这里写的是 (void) 而不是 ()。
 */
int main(void)
{
    printf("Hello, World!\n");
    return 0;
}

/**
 * 【变式一】最短的写法：连 return 都可以不写。
 */
