// 附录A 探针 3E：反证 —— 在 C++ 里跨编译单元引用命名空间作用域的 const 变量
// 应当链接失败（LNK2001），因为 C++ 给它们内部链接。
// 与 p_a03_link_a.cpp 一起编译：
// cl /nologo /utf-8 /std:c++latest /EHsc /W4 p_a03_link_a.cpp p_a03_link_neg.cpp
#include <cstdio>

extern const int     gCppConst;       // 链接不到
extern int           gCppStatic;      // 链接不到（static = 内部链接）
// 注意：constexpr 变量不能只声明不定义，下面这行会直接编译报错 C2737
// “必须初始化 constexpr 对象”：
// extern constexpr int gCppConstexpr;

int main() {
    std::printf("%d %d\n", gCppConst, gCppStatic);
    return 0;
}
