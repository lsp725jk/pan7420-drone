/**
 * ch04_ex5_proto.c —— 函数的声明与定义：头文件 + 实现 + 使用者
 */
#include <stdio.h>
#include "ch04_ex5_proto.h"     /**
 * 双引号：先在本文件所在目录找
 */

/**
 * ---------- 定义：分配了真正的代码 ----------
 */
int g_sample_calls = 0;         /**
 * 头文件里 extern 声明过的那个变量
 */

int sample_add(int a, int b)
{
    g_sample_calls++;
    return a + b;
}

int sample_max(const int *data, int count)
{
    g_sample_calls++;
    if (count <= 0) {
        return 0;
    }
    int best = data[0];
    for (int i = 1; i < count; i++) {
        if (data[i] > best) {
            best = data[i];
        }
    }
    return best;
}

void sample_reset(void)
{
    g_sample_calls = 0;
}
