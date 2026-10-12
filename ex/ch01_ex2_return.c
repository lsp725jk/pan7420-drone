/**
 * ch01_ex2_return.c —— main 的返回值到底给了谁
 */
#include <stdio.h>

#ifndef RETURN_MODE
#define RETURN_MODE 1        /**
 * 默认演示第 1 种
 */
#endif

int main(void)
{
    printf("RETURN_MODE = %d\n", RETURN_MODE);

#if RETURN_MODE == 1
    printf("即将执行: return 0;\n");
    return 0;                               /* 0 表示"一切正常"，这是全世界的约定 */
#elif RETURN_MODE == 2
    printf("即将执行: return 3;  (自定义错误码)\n");
    return 3;                               /* 非 0 表示出错，具体数字由程序员自定 */
#else
    printf("本分支没有 return 语句（main 里等价于 return 0）\n");
                /* 故意不写 return */
#endif
}
