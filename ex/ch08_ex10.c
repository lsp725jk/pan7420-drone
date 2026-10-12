/**
 * ch08_ex10.c —— 多行宏与续行符（line continuation, 反斜杠 \）
 */
#include <stdio.h>

/**
 * 多行宏的规矩：
 */
#define LOG_INT(tag, v)                          \
    do {                                         \
        printf("[%s] %s = %d\n", (tag), #v, (v)); \
    } while (0)

/**
 * 变式：宏体跨很多行时，用 \ 对齐反而不好看，
 */
#define CHECK_RANGE(v, lo, hi)                   \
    do {                                         \
        int _v = (v);                            \
        if (_v < (lo) || _v > (hi)) {            \
            printf("  越界: %d 不在 [%d, %d]\n", \
                   _v, (lo), (hi));              \
        } else {                                 \
            printf("  正常: %d 在 [%d, %d]\n",   \
                   _v, (lo), (hi));              \
        }                                        \
    } while (0)

int main(void)
{
    LOG_INT("陀螺", 123);
    LOG_INT("加速度", -45);

    CHECK_RANGE(50, 0, 100);
    CHECK_RANGE(150, 0, 100);

        /* 坑 1：\ 后面多一个空格就断了。编译器会把宏体截断在那一行， 后面几行变成普通代码 —— 报错信息往往指向"最后一行"或"文件结尾"， 见章末"实测报错"。 坑 2：宏体某行末尾如果是 // 行注释，这个 // 会"吃掉"续行符， */
    printf("多行宏里的注释请一律用块注释\n");
    return 0;
}
