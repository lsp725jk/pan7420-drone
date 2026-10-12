/**
 * ch08_ex10_bad2.c —— 故意犯错：多行宏里的行注释把续行符"吃掉"了
 */
#include <stdio.h>

#define SET_BOTH(a, b)  do {                      \
    (a) = 1;                                      \
    (b) = 2;           /**
     * 这里是块注释，没事
     */            \
    (a) = (a) + (b);                              \
} while (0)

/**
 * 下面这个版本的第三行用了 // 行注释，而那一行末尾还有一个续行符：
 */
#define SET_BOTH_BAD(a, b)  do {                  \
    (a) = 1;                                      \
    (b) = 2;   // 行注释 + 行尾续行符 = 灾难 \
    (a) = (a) + (b);                              \
} while (0)

int main(void)
{
    int p = 0, q = 0;
    SET_BOTH(p, q);
    printf("p = %d , q = %d\n", p, q);
    SET_BOTH_BAD(p, q);
    printf("p = %d , q = %d\n", p, q);
    return 0;
}
