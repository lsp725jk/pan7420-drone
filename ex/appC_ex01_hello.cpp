// 附录C 例 1：C++ 版 hello（UTF-8 源码 + 中文输出）
// 编译（cmd）：
// cmd /c "C:\Users\y\Desktop\pan飞控代码zts(1)\build-gcc\tools\check.bat appC_ex01_hello.cpp c++latest"
// 或手工：
// call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"
// cl /nologo /utf-8 /std:c++latest /EHsc /W4 appC_ex01_hello.cpp
#include <cstdio>
#include <string>
#include <vector>
#include <print>

int main() {
    std::puts("你好，C++。");
    std::printf("本机 sizeof(void*) = %zu\n", sizeof(void*));

    std::vector<std::string> names = {"飞控", "遥控器", "上位机"};
    for (const auto& n : names) {
        std::printf("  子系统: %s\n", n.c_str());
    }

    std::print("C++23 的 std::print 也能直接输出中文：{}\n", 42);
    return 0;
}
