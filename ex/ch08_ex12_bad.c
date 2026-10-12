/**
 * ch08_ex12_bad.c —— 故意犯错：#if 里用了 sizeof（预处理器不认识类型）
 */
#include <stdio.h>

#if sizeof(int) > 2
#define BIG_INT 1
#endif

int main(void)
{
    printf("BIG_INT = %d\n", BIG_INT);
    return 0;
}
