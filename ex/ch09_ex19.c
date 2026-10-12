/**
 * ch09_ex19.c —— errno 与错误处理：C 标准库怎么"报错"
 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

int main(void)
{
                /**
         * 1) 打开不存在的文件：errno = ENOENT(2)
         */
    errno = 0;
    FILE *fp = fopen("no_such_file_here.txt", "r");
    if (fp == NULL) {
        printf("fopen 失败，errno = %d\n", errno);
        perror("  perror 自动打印");                                 /* perror 会输出 "前缀: 系统描述" */
        printf("  strerror(errno) = %s\n", strerror(errno));
    } else {
        fclose(fp);
    }
        /* 顺带提醒：MSVC 的窄字符 fopen 用的是当前 ANSI 代码页， 传中文文件名时如果编码对不上，errno 会是 EILSEQ(42) 而不是 ENOENT(2)。 跨平台处理中文路径要用宽字符版本 _wfopen，或者把源码/运行环境都统一成 UTF-8。 */

                /* 2) 数值溢出：errno = ERANGE */
    errno = 0;
    long v = strtol("999999999999999999999", NULL, 10);
    printf("\nstrtol 溢出：返回值 = %ld , errno = %d (ERANGE=%d) ? %s\n",
           v, errno, ERANGE, (errno == ERANGE) ? "是" : "否");
    printf("  注意：函数【也】返回了一个值（LONG_MAX），所以不能只看返回值\n");

                /* 3) 数学定义域错误：errno = EDOM */
    errno = 0;
    double d = sqrt(-1.0);
    printf("\nsqrt(-1) = %f , errno = %d (EDOM=%d) ? %s\n",
           d, errno, EDOM, (errno == EDOM) ? "是" : "否");

                /* 4) errno 的常见错误用法：忘记清零 */
    errno = 0;
    double good = sqrt(4.0);                    /* 成功，errno 不变 */
    printf("\nsqrt(4) = %f , errno = %d  （成功时 errno 不会被清零，这是标准允许的）\n",
           good, errno);
    printf("  所以判断失败一定要先看\"函数返回了什么\"，再看 errno\n");

                /* 5) 变式：错误码表。嵌入式里通常自己定义一套，比 errno 更省事 */
    static const struct { int code; const char *text; } table[] = {
        { ENOENT, "文件不存在" },
        { ERANGE, "数值超出范围" },
        { EDOM,   "数学定义域错误" },
        { EILSEQ, "非法字符序列" },
    };
    printf("\n常用 errno 值：\n");
    for (int k = 0; k < 4; k++)
        printf("  %-8d %-6s %s\n", table[k].code, strerror(table[k].code), table[k].text);

        /* 6) C11 起 errno 是"线程局部"的（thread-local）， 所以中断里也能安全地用它 —— 但嵌入式更常见的做法是 "函数返回错误码"而不是依赖全局 errno，因为中断/主循环共享全局量很容易出错。 */
    printf("\nC11 起 errno 是宏，展开成线程局部的左值；直接取它的地址是不可移植的\n");
    return 0;
}
