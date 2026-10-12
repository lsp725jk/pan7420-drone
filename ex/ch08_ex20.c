/**
 * ch08_ex20.c —— 【变式/踩坑】无参宏、带分号的宏、宏与函数的"看起来一样"
 */
#include <stdio.h>

static unsigned int g_port = 0;

/**
 * 变式 1：带括号的无参宏 —— 调用时必须写 ()
 */
#define LED_ON()   (g_port |=  (1u << 0))
#define LED_OFF()  (g_port &= ~(1u << 0))

/**
 * 变式 2：不带括号的无参宏 —— 用法像常量，但其实是"一段语句"
 */
#define LED_TOGGLE (g_port ^= (1u << 0))

int main(void)
{
                /* 1) 两种无参宏都能用 */
    LED_ON();
    printf("LED_ON() 之后    g_port = 0x%02X\n", g_port);
    LED_TOGGLE;
    printf("LED_TOGGLE 之后  g_port = 0x%02X\n", g_port);
    LED_OFF();
    printf("LED_OFF() 之后   g_port = 0x%02X\n", g_port);

                /* 2) 坑：函数式宏 LED_ON 忘了写括号，【编译直接失败】 */
        /* g_port = 0; LED_ON;                    <-- 少了 () 】 实测报错：error C2065: "LED_ON": 未声明的标识符 因为 LED_ON 是"函数式宏"，不带括号时预处理器【不展开】它， */
    printf("\n2) 写 LED_ON（漏了括号）会直接报 C2065 未声明的标识符 —— 见章末实测报错\n");

                /* 3) 变式：不带括号的对象式宏就不会有这个坑（代价是用法不像函数） */
    g_port = 0;
    LED_TOGGLE;                            /* 不需要括号，因为它是对象式宏 */
    printf("3) 对象式宏 LED_TOGGLE 不需要括号：g_port = 0x%02X\n", g_port);

                /* 4) 坑：函数式宏的名字和左括号之间【不能有空格】 */

    printf("\n4) 函数式宏的名字和左括号之间不能有空格，否则它会变成对象式宏\n");

                /* 5) 坑：宏体末尾多写一个分号 */
#define LED_SET_BAD()  do { g_port |= (1u << 1); } while (0);
    g_port = 0;
    LED_SET_BAD();
    printf("\n5) 带分号的宏 LED_SET_BAD() 展开后是 do{...}while(0);;  —— 多一个空语句\n");
    printf("   在 if/else 里会报「没有匹配 if 的非法 else」（同例 8-8 的坑）\n");
    printf("   g_port = 0x%02X\n", g_port);
    return 0;
}
