/**
 * ch08_ex02.c —— 用宏定义常量的 4 种变式：什么时候用哪个
 */
#include <stdio.h>

#define MAX_PACKET   64          /**
 * 变式 1：宏常量（文本替换，没有类型）
 */
#define BUFFER_COUNT (2 * 4)     /**
 * 宏常量里可以放表达式，但整体要加括号
 */

const int g_baud = 115200;       /**
 * 变式 2：const 变量（有类型，占内存/ROM）
 */

enum { LED_OFF = 0, LED_ON = 1, LED_BLINK = 2 };  /**
 * 变式 3：枚举常量（整数，有类型）
 */

int main(void)
{
                /**
         * 变式 4：把"尺寸"交给编译器算，用 sizeof 表达，永远不会和实际不符
         */
    static unsigned char buf[MAX_PACKET];
    static int slots[BUFFER_COUNT];

    printf("MAX_PACKET   = %d , sizeof(buf)   = %d\n", MAX_PACKET, (int)sizeof(buf));
    printf("BUFFER_COUNT = %d , sizeof(slots) = %d\n", BUFFER_COUNT, (int)sizeof(slots));
    printf("g_baud = %d , LED_BLINK = %d\n", g_baud, LED_BLINK);

                /* 宏和 const 的最大区别：宏能用在需要"编译期常量"的地方 */
    int array_a[MAX_PACKET];                      /* 可以：宏是编译期整数常量表达式 */
    int array_b[BUFFER_COUNT];
                /* int array_c[g_baud]; */                                    /* C 里不行：const 变量不是常量表达式 */
    printf("array_a 有 %d 个元素，array_b 有 %d 个元素\n",
           (int)(sizeof(array_a) / sizeof(array_a[0])),
           (int)(sizeof(array_b) / sizeof(array_b[0])));
    return 0;
}
