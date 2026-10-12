/**
 * ch08_ex13_err.c —— 故意触发 #error（这个文件编译不过，是故意的）
 */
#include <stdio.h>

#define BOARD_NONE   0
#define BOARD        BOARD_NONE

#if BOARD == BOARD_NONE
#error "没有选择板型：请定义 BOARD_PY32F030 或 BOARD_PY32F040"
#endif

int main(void) { printf("永远不会运行到这里\n"); return 0; }
