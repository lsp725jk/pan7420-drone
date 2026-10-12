/**
 * ch08_ex08_bad.c —— 故意犯错：裸花括号的宏放进 if/else（这个文件编译不过，是故意的）
 */
#include <stdio.h>

#define SWAP_A(a, b) { int t = (a); (a) = (b); (b) = t; }

int main(void)
{
    int x = 1, y = 2;
    if (x < y)
        SWAP_A(x, y);                              /**
         * 宏体后面的分号成了"空语句"，else 失去配对的 if
         */
    else
        printf("x >= y\n");
    printf("x = %d , y = %d\n", x, y);
    return 0;
}
