/**
 * ch01_ex6_exitcode.c —— main 返回值的三种正确写法 + exit() 的用法
 */
#include <stdio.h>
#include <stdlib.h>     /**
 * EXIT_SUCCESS / EXIT_FAILURE / exit 在这里
 */

static void do_work_and_maybe_die(int fail)
{
    if (fail) {
        printf("  某个深层函数决定放弃 → 直接 exit(EXIT_FAILURE)\n");
        exit(EXIT_FAILURE);                     /* 注意：exit 之后的代码不会执行 */
    }
    printf("  深层函数正常返回\n");
}

int main(void)
{
    printf("---- 1. 三个子场景 ----\n");
    printf("  这个程序正常结束时返回 EXIT_SUCCESS (= %d)\n", EXIT_SUCCESS);
    printf("  EXIT_FAILURE = %d（标准只保证它非 0，具体值由实现定）\n",
           EXIT_FAILURE);

    do_work_and_maybe_die(0);               /* 想演示异常退出就把参数改成 1 */

    printf("---- 2. 三种返回风格对照 ----\n");
    printf("  return 0;            最常见，0 = 成功\n");
    printf("  return EXIT_SUCCESS; 语义化，可读性最好\n");
    printf("  （不写 return）       只有 main 允许，等价于 return 0\n");
    printf("  return 3;            自定义错误码，脚本可以按码分支处理\n");
    printf("  exit(2);             任意位置结束整个程序\n");
    return EXIT_SUCCESS;
}
