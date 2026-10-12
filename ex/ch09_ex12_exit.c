/**
 * ch09_ex12_exit.c —— exit()：在程序的任何地方"立刻结束"
 */
#include <stdio.h>
#include <stdlib.h>

static void on_exit(void) { printf("  [atexit] 清理函数仍然会执行\n"); }

static int check_config(int v)
{
    if (v <= 0) {
        printf("配置非法（%d），直接退出\n", v);
        fflush(stdout);                           /**
         * exit 会刷新缓冲区，这里写出来只为对照 abort
         */
        exit(EXIT_FAILURE);                       /**
         * 不会返回到调用者
         */
    }
    return v;
}

int main(void)
{
    atexit(on_exit);
    printf("第一次调用：\n");
    printf("  check_config(100) = %d\n", check_config(100));

    printf("第二次调用（会退出）：\n");
    printf("  check_config(-1) = %d\n", check_config(-1));

    printf("这一行永远不会执行\n");
    return 0;
}
