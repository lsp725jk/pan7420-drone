// ch11_ex05.cpp —— 正确版本：头文件放声明，inline 放小函数
// 编译：_runmulti.bat ch11_ex05.cpp ch11_ex05_src.cpp
#include "ch11_ex05_math.h"
#include <cstdio>

int main()
{
    std::printf("square(5)=%d\n", square(5));   // 定义在 ch11_ex05_src.cpp
    std::printf("cube(5)=%d\n", cube(5));       // 定义在头文件里，但是 inline
    return 0;
}
