/**
 * ch08_ex13_warn.c —— 故意触发 #warning
 */
#include <stdio.h>

#define BOARD_PY32F040   1
#define BOARD            BOARD_PY32F040

#ifdef BOARD_PY32F040
#warning "PY32F040 分支尚未在硬件上验证"
#endif

int main(void)
{
    printf("板型 = %d ，程序照常运行\n", BOARD);
    return 0;
}
