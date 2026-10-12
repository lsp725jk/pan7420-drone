// ch11_ex19_cpp.cpp —— C++ 调用 C 模块：头文件要用 extern "C" 包起来
// 编译：_runmulti.bat ch11_ex19_cpp.cpp ch11_ex19_pure_c.c
#include <cstdio>

// 在 C++ 里，头文件必须这样写，才能和 C 编译出来的符号对上：
extern "C" {
    int c_module_version(void);
    int c_module_scale(int v);
}
// 工程惯例：头文件里写成
// #ifdef __cplusplus
// extern "C" {
// #endif
// ... 声明 ...
// #ifdef __cplusplus
// }
// #endif
// 这样同一份头文件 C 和 C++ 都能包含。

int main()
{
    std::printf("C 模块版本 = %d\n", c_module_version());
    std::printf("c_module_scale(21) = %d\n", c_module_scale(21));
    return 0;
}
