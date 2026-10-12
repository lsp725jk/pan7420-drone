/**
 * ch02_ex8_extern_b.c —— ch02_ex8 的第二个编译单元
 */
#include <stdio.h>

/**
 * ---------- 定义（分配空间，只此一份）----------
 */
int g_counter = 0;
const char *g_build_tag = "ch02_ex8 / C17 / MSVC";

/**
 * a.c 里定义了这个变量，这里只是声明它
 */
extern int g_shared_from_a;

/* a.c 里定义了 read_a_shared()，这里声明后才敢调 */
int read_a_shared(void);

int bump_counter(int delta)
{
    g_counter += delta;
                /* 顺便读一下 a.c 的变量，证明两个文件真的连在一起了 */
    if (g_shared_from_a == 100) {
        return g_counter;
    }
    return -1;
}
