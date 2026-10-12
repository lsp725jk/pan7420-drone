/**
 * ch08_ex20_bad.c —— 故意犯错：函数式宏忘了写括号（这个文件编译不过，是故意的）
 */
#include <stdio.h>

static unsigned int g_port = 0;
#define LED_ON()   (g_port |= (1u << 0))

int main(void)
{
    LED_ON;                            /**
     * 少了 () —— 函数式宏不带括号时【不展开】
     */
    printf("g_port = 0x%02X\n", g_port);
    return 0;
}
