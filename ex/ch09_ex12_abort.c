/**
 * ch09_ex12_abort.c —— abort()：异常终止。不执行 atexit，退出码是"异常码"
 */
#include <stdio.h>
#include <stdlib.h>

static void on_exit(void) { printf("  [atexit] 这一句不会打印，因为 abort 跳过了 atexit\n"); }

int main(void)
{
    atexit(on_exit);

    printf("准备调用 abort()\n");
    printf("  与 exit 的区别：abort 不刷新 stdio 缓冲区、不跑 atexit、\n");
    printf("  退出码在 Windows 上是 0xC0000409（MSVC 的 __fastfail），不是普通的 1/2/3\n");
    fflush(stdout);                    /* 必须自己刷，否则上面的内容可能丢 */
    abort();
                /* abort 不会返回，这后面的代码编译器会标成"无法访问" */
    return 0;
}
