/**
 * ch09_ex12.c —— 从 main 返回 与 atexit：正常结束的两条路
 */
#include <stdio.h>
#include <stdlib.h>

static void on_exit_a(void) { printf("  [atexit] 清理函数 A（先注册）\n"); }
static void on_exit_b(void) { printf("  [atexit] 清理函数 B（后注册，先执行）\n"); }

int main(void)
{
    atexit(on_exit_a);                /**
     * atexit 是"栈"结构：后注册的先跑
     */
    atexit(on_exit_b);

    printf("main 正常走到结尾\n");
    printf("  main 的 return 等价于 exit(返回值)\n");
    printf("  atexit 注册的函数会在 return/exit 之后、进程结束之前运行\n");
    return EXIT_SUCCESS;              /* EXIT_SUCCESS 在标准里就是 0 */
}
