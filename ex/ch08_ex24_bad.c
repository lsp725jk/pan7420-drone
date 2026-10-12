/**
 * ch08_ex24_bad.c —— 故意犯错：多行宏忘了花括号，临时变量撞名
 */
#include <stdio.h>

#define SUM_WORSE(a, b)  int t = (a) + (b); (a) = t;

int main(void)
{
    int t = 999;                         /**
     * 调用处已经有一个 t
     */
    int x = 10, y = 20;
    SUM_WORSE(x, y);                     /**
     * 展开后又在同一作用域里 int t = ...
     */
    printf("x = %d , t = %d\n", x, t);
    return 0;
}
