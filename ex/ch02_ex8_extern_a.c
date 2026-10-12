/**
 * ch02_ex8_extern_a.c —— 跨文件共享变量与函数（extern / 链接）
 */
#include <stdio.h>

/**
 * ---------- 声明（不是定义）----------
 */
extern int g_counter;               /**
 * 在 b.c 里定义
 */
extern const char *g_build_tag;     /**
 * 在 b.c 里定义
 */
int bump_counter(int delta);        /* 在 b.c 里定义 */
int read_a_shared(void);            /* 在 b.c 里声明、在本文件底部定义 */

/* ---------- 本文件自己的定义 ---------- */
int g_shared_from_a = 100;          /* b.c 会用 extern 声明它 */

int main(void)
{
    printf("---- 读另一个文件里的变量 ----\n");
    printf("初始 g_counter = %d\n", g_counter);
    printf("g_build_tag    = %s\n", g_build_tag);

    printf("---- 调另一个文件里的函数 ----\n");
    for (int i = 0; i < 3; i++) {
        printf("bump_counter(5) = %d\n", bump_counter(5));
    }

                /* 本文件的全局变量被 b.c 读走了 */
    printf("---- 本文件的全局变量被 b.c 使用 ----\n");
    printf("b.c 看到的 g_shared_from_a = %d\n", read_a_shared());

    printf("---- 全局变量默认可以被任何文件访问，所以要克制 ----\n");
    printf("本文件的 static 函数 read_a_shared 只在 b.c 里声明过；\n");
    printf("而 g_shared_from_a 没有 static，任何一个 .c 都能改它 ——\n");
    printf("工程上更推荐：能 static 就 static，只把必要的接口暴露出去。\n");
    return 0;
}

/* 这个函数没有 static，b.c 才能声明并调用它 */
int read_a_shared(void)
{
    return g_shared_from_a;
}
