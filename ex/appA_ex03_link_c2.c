/**
 * 附录A 探针 3D：编译单元 2 —— 在 C 里能成功链接到 gCConst
 */
#include <stdio.h>

extern const int gCConst;   /**
 * C 里可以这样跨编译单元引用 const 变量
 */

int main(void) {
    printf("C 跨编译单元读到 gCConst = %d\n", gCConst);
    return 0;
}
