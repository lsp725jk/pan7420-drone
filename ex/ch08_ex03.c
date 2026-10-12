/**
 * ch08_ex03.c —— 宏常量的三个坑：没有类型、没有作用域、会被"就地展开"
 */
#include <stdio.h>

#define SIZE    4 + 1          /**
 * 坑 1：整体没加括号
 */
#define SIZE_OK (4 + 1)

#define OFFSET  8              /**
 * 坑 2：宏没有作用域，任何地方都能看见
 */

int main(void)
{
                /**
         * 坑 1 现场：SIZE 出现在乘法里，"4 + 1" 只把 4 带进来了
         */
    printf("2 * SIZE     = %d  （想要 10，实际 9）\n", 2 * SIZE);
    printf("2 * SIZE_OK  = %d  （加了括号才对）\n", 2 * SIZE_OK);
    printf("sizeof(int) * SIZE = %d  （同一个宏在 sizeof 上也出错）\n",
           (int)sizeof(int) * SIZE);

                /* 坑 2 现场：同名宏可以在"看起来局部的地方"被重定义， 而且一旦定义，从这一行往后所有代码都受影响 —— 包括别人的头文件 */
#define OFFSET 16
    printf("重定义后 OFFSET = %d ，sizeof 也能用：%d\n",
           OFFSET, (int)sizeof(char[OFFSET]));

                /* 坑 3：宏是文本，所以能被用在"看起来完全不像常量"的地方 */
    printf("把宏当类型用也没人拦你 : %s\n",
           sizeof(SIZE_OK) > 0 ? "sizeof(SIZE_OK) 合法" : "?");

                /* 变式：想让常量"只在本文件可见"，用 static const 或 enum， 它们有作用域、有类型，调试器里也能看见名字 */
    {
        static const int local_only = 100;
        printf("static const 有作用域 : %d\n", local_only);
    }
    return 0;
}
