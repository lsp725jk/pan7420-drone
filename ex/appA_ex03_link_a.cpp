// 附录A 探针 3A：链接性验证 —— 编译单元 1（与 p_a03_link_b.cpp 一起编译）
// C++ 结论：命名空间作用域的 const 变量默认是"内部链接"，别的编译单元链接不到。
// C 里同名写法是"外部链接"，能链接到。见 p_a03_link_c1.c / _c2.c。
#include <cstdio>

int readPlain();    // 定义在 p_a03_link_b.cpp
int readInline();

const     int gCppConst     = 111;   // C++：内部链接
constexpr int gCppConstexpr = 222;   // C++：内部链接（因为隐含 const）
int           gCppPlain     = 333;   // 外部链接
inline        int gCppInline = 444;  // 外部链接，但可多编译单元重复定义（C++17）
static        int gCppStatic  = 555; // 内部链接

int getCppConst()     { return gCppConst; }
int getCppConstexpr() { return gCppConstexpr; }

int main() {
    std::printf("本单元: gCppConst=%d gCppConstexpr=%d gCppPlain=%d gCppInline=%d gCppStatic=%d\n",
                gCppConst, gCppConstexpr, gCppPlain, gCppInline, gCppStatic);
    std::printf("从另一编译单元读到: gCppPlain=%d gCppInline=%d\n", readPlain(), readInline());
    return 0;
}
