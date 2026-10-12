/**
 * ch02_ex35_other.c —— ch02_ex35 的第二个编译单元（配套示例）
 */
#include <stdio.h>

/**
 * 同名但 static：和另一个文件里的 helper 完全不冲突
 */
static int helper(int x)
{
    return x * 100;                             /**
     * 故意实现得不一样，用来证明"互不干扰"
     */
}

static int g_local_state = 0;

/**
 * 对外暴露：包装一下本文件的 helper
 */
int other_helper(int x)
{
    return helper(x);
}

/* 对外暴露：读本文件私有的 g_local_state */
int other_state_value(void)
{
    g_local_state = 222;
    return g_local_state;
}
