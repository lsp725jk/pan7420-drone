/**
 * ch04_ex5_main.c —— 使用 ch04_ex5_proto.h 里声明的东西
 */
#include <stdio.h>
#include "ch04_ex5_proto.h"

int main(void)
{
    printf("---- 1. 调用另一个文件里定义的函数 ----\n");
    printf("sample_add(3,4) = %d\n", sample_add(3, 4));
    printf("sample_add(10,20) = %d\n", sample_add(10, 20));

    printf("---- 2. 传数组给函数 ----\n");
    int data[MAX_SAMPLES] = { 3, 9, 1, 7, 5, 2, 8, 4 };
    int n = (int)(sizeof(data) / sizeof(data[0]));
    printf("数组有 %d 个元素，最大值 = %d\n", n, sample_max(data, n));
    printf("只算前 3 个的最大值 = %d\n", sample_max(data, 3));

    printf("---- 3. 通过头文件共享一个计数器 ----\n");
    printf("目前调用次数 g_sample_calls = %d\n", g_sample_calls);
    sample_reset();
    printf("reset 之后 g_sample_calls = %d\n", g_sample_calls);

    printf("---- 4. 头文件的几条纪律 ----\n");
    printf("a) 头文件里只放声明，不要放函数体（否则每个包含它的 .c 都会有一份，\n");
    printf("   链接时报 LNK2005 xxx 已经在 xxx.obj 中定义）；\n");
    printf("b) 头文件必须能独立编译：它需要什么就自己 #include 什么；\n");
    printf("c) 每个头文件都要有 include guard，防止重复包含；\n");
    printf("d) 宏定义的常量尽量用 enum 或 static const 代替，便于调试器查看。\n");
    printf("---- 5. C 和 C++ 混编时的 extern \"C\" ----\n");
    printf("C++ 会做名字修饰（name mangling），C 不会。\n");
    printf("如果 .h 要给 C++ 用，包一层：\n");
    printf("    #ifdef __cplusplus\n");
    printf("    extern \"C\" {\n");
    printf("    #endif\n");
    printf("    ... 声明 ...\n");
    printf("    #ifdef __cplusplus\n");
    printf("    }\n");
    printf("    #endif\n");
    printf("否则 C++ 侧会报 undefined reference to 'foo(int,int)'（带参数类型的那种）。\n");
    return 0;
}
