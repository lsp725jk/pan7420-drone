/**
 * ch08_ex04.c —— 【参数括号铁律】之一：宏体整体必须加最外层括号
 */
#include <stdio.h>

#define HALF_BAD(x) (x) / 2      /**
 * 错：宏体是 (x) / 2，没有最外层括号
 */
#define HALF_OK(x)  ((x) / 2)    /**
 * 对：宏体整体被一对括号包住
 */

int main(void)
{
                /**
         * 宏是"文本替换"：100 / HALF_BAD(20) 会变成 100 / (20) / 2
         */
    printf("100 / HALF_BAD(20) = %d   <-- 错！心里想要的是 10\n", 100 / HALF_BAD(20));
    printf("100 / HALF_OK (20) = %d   <-- 对\n", 100 / HALF_OK(20));

                /* 同样的错误在减法/加法里也存在，只是有时"碰巧"对，更难发现 */
    printf("30 - HALF_OK(10) - 5 = %d\n", 30 - HALF_OK(10) - 5);

                /* 变式：宏体是"表达式"就一定加括号；如果宏体本身就是单个记号， 例如 #define PI 3.14159f，就不存在这个问题 */
#define PI 3.14159f
    printf("2 * PI = %f\n", 2 * PI);
    return 0;
}
