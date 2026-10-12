/**
 * ch00_evolution_gets_c2065.c —— 《技术演进总纲》实测脚本 3b/4
 */

#include <stdio.h>
#include <string.h>

int main(void)
{
                /**
         * 第一次使用 gets，且要求它"必须是一个函数"（函数指针初始化）。 预期：error C2065: "gets": 未声明的标识符
         */
    char *(*fp)(char *) = gets;
    (void)fp;

                /* 对照：C11 同时提供的"边界检查版本"是【可选】接口， 名字叫 gets_s，很多实现根本没有它 —— 所以也不能依赖。 */
    printf("如果这一行能编过，说明本机根本没有按 C11 移除 gets。\n");
    return 0;
}
