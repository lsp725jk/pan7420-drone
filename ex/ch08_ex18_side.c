/**
 * ch08_ex18_side.c —— 【坑】assert 里放带副作用的表达式
 */
#include <assert.h>
#include <stdio.h>

static int g_inited = 0;

static int init_sensor(void)
{
    g_inited = 1;
    printf("    （init_sensor 真的执行了）\n");
    return 1;
}

int main(void)
{
    assert(init_sensor());                  /* 危险写法：把"干活"塞进了断言 */
    printf("g_inited = %d , 传感器%s\n",
           g_inited, g_inited ? "已初始化" : "【没初始化！】");

                /* 正确写法：先调用，再对结果断言 */
    g_inited = 0;
    int ok = init_sensor();
    assert(ok);
    printf("正确写法：g_inited = %d , init 返回值 = %d\n", g_inited, ok);
    return 0;
}
