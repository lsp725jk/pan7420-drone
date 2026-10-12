/**
 * ch08_ex17_bad.c —— 故意犯错：直接使用 MSVC 没有定义的 __STDC__
 */
#include <stdio.h>

int main(void)
{
    printf("__STDC__ = %d\n", __STDC__);           /**
     * MSVC /std:c17 下 __STDC__ 不存在
     */
    return 0;
}
