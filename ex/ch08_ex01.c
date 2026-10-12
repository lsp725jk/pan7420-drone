/**
 * ch08_ex01.c —— 预处理（preprocessing）到底做了什么：纯文本替换
 */
#include <stdio.h>

#define N 3                    /**
 * 对象式宏（object-like macro）
 */
#define TWICE(x) (2 * (x))     /**
 * 函数式宏（function-like macro）
 */

/**
 * STR_ 把参数原样变成字符串；STR 先把参数展开再交给 STR_ —— 两级写法是标准技巧
 */
#define STR_(x) #x
#define STR(x)  STR_(x)

int main(void)
{
    printf("源码里写的      : TWICE(N + 1)\n");
    printf("预处理后变成    : %s\n", STR(TWICE(N + 1)));
    printf("它的运行结果    : %d\n", TWICE(N + 1));

                /* 关键点：宏参数先被替换成 N 的文本 3，再去算 */
    printf("STR(N) = %s , STR(TWICE(N)) = %s\n", STR(N), STR(TWICE(N)));

                /* 宏完全没有"作用域"概念：同一个宏在任何地方展开结果一样 */
    printf("花括号里也是同一份文本 : %s\n", STR(TWICE(N + 1)));
    return 0;
}
