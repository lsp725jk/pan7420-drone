/**
 * ch09_ex02_bad.c —— 故意犯错：格式串里的占位符比实参多
 */
#include <stdio.h>

int main(void)
{
                /**
         * 想显示"说明文字"，却把 %.5d 直接写进了格式串：
         */
    printf("整数精度 %.5d : [%.5d]\n", 42);
    return 0;
}
