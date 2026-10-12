/**
 * ch08_ex06.c —— 【副作用陷阱】宏参数会被求值多次
 */
#include <stdio.h>

#define SQ(x)     ((x) * (x))       /**
 * 参数出现 2 次
 */
#define MAX(a, b) ((a) > (b) ? (a) : (b))  /**
 * 参数各出现 2 次
 */

int main(void)
{
                /**
         * 1) 最经典的坑：SQ(i++)
         */
    int i = 3;
    int r = SQ(i++);                /* 展开成 ((i++) * (i++)) —— i 被加了两次 */
    printf("SQ(i++) 之后 i = %d ，返回值 = %d   <-- i 少了 1\n", i, r);

                /* 2) 两个参数都有副作用：谁先谁后没有规定 */
    int a = 1, b = 1;
    int m = MAX(a++, b++);
    printf("MAX(a++, b++) = %d ，之后 a = %d , b = %d\n", m, a, b);

                /* 3) 对照：同样的表达式传给真函数，参数只求值一次 */
    i = 3;
    printf("对照：先把 i 存起来再调用 —— i = %d\n", i);
    i = 3;
    int t = i++;
    printf("手动展开后 SQ 的返回值 = %d ，i = %d\n", SQ(t), i);

                /* 4) 变式：读-改-写混在一起时更糟，i = i++ 是彻底的 UB */
    i = 5;
    i = i++;                        /* UB：同一个 i 上既有读又有写，还没规定顺序 */
    printf("执行 i = i++ 之后 i = %d （UB，不可依赖）\n", i);

                /* 结论：宏参数里绝不要放 ++ -- 赋值 或函数调用 */
    return 0;
}
