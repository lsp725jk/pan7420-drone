/**
 * ch01_ex10_main_args.c —— main 的命令行参数，以及 argc/argv 的边界
 */
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    printf("---- 1. 参数个数与内容 ----\n");
    printf("argc = %d   （参数个数，含程序名本身）\n", argc);
    for (int i = 0; i < argc; i++) {
        printf("  argv[%d] = \"%s\"\n", i, argv[i]);
    }
    printf("  argv[argc] 一定是 NULL（标准保证），所以可以用它当循环终止条件：\n");
    for (int i = 0; argv[i] != NULL; i++) {
        printf("    [%d] %s\n", i, argv[i]);
    }

    printf("---- 2. 参数解析：一定要先检查 argc ----\n");
    if (argc < 2) {
        printf("  没有额外参数（这不算错误，只是走默认值）\n");
    } else {
        printf("  第 1 个参数是 \"%s\"\n", argv[1]);

        if (argc > 2) {
                                                /* 把字符串转成整数：用 strtol 而不是 atoi（atoi 无法报错） */
            char *end = NULL;
            long v = strtol(argv[2], &end, 10);
            if (end != NULL && *end == '\0') {
                printf("  第 2 个参数 \"%s\" 解析成整数 %ld\n", argv[2], v);
            } else {
                printf("  第 2 个参数 \"%s\" 不是合法整数\n", argv[2]);
            }
        }
    }

    printf("---- 3. 变式：char **argv 完全等价 ----\n");
    printf("  int main(int argc, char *argv[])  和  int main(int argc, char **argv)\n");
    printf("  在类型上是同一件事（数组形参会退化成指针）。\n");
    printf("  argv[0] 是程序名（含路径），想取它本身的名字要自己找路径分隔符。\n");

    printf("---- 4. 嵌入式里用不到，但概念有用 ----\n");
    printf("  裸机上 main 通常是 int main(void)，参数由启动文件决定；\n");
    printf("  但\"把配置从外部传进来\"这个思路是一样的：\n");
    printf("  PC 上是命令行参数，单片机上可能是 Flash 里的配置区或串口命令。\n");
    return 0;
}
