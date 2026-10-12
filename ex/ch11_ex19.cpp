// ch11_ex19.cpp —— 名称修饰（name mangling）与 extern "C"
// 编译：check.bat ch11_ex19.cpp c++latest

// 为什么需要名称修饰：C++ 允许同名函数（重载）、同名类里的同名方法，
// 链接器只认"符号名"这个字符串，所以编译器把参数信息编码进符号名里。
// 想看真实的符号名，用 dumpbin /symbols 或者链接错误信息（见本节讲解）。
#include <cstdio>

// ---- 1) C++ 函数：符号名会被修饰 ----
int add(int a, int b) { return a + b; }
double add(double a, double b) { return a + b; }        // 重载版本

namespace nav {
    int align(int v) { return (v + 3) & ~3; }           // 命名空间也进符号名
}

// ---- 2) extern "C"：告诉编译器"这个函数按 C 的规则起名，不要修饰" ----
// 典型场景 A：自己的 .c 文件（或者单片机工程里的 C 库）要给 C++ 调用
extern "C" int  c_style_add(int a, int b);
extern "C" void c_style_report(void);

extern "C" int c_style_add(int a, int b) { return a + b; }
extern "C" void c_style_report(void) { std::printf("  C 链接方式的函数被调用了\n"); }

// 典型场景 B：整个头文件用 extern "C" 包起来（这样 C 和 C++ 都能包含它）
extern "C" {
    int wrapped_mul(int a, int b) { return a * b; }
}

// 典型场景 C：用 C++ 的名字去调 C 库函数（标准库已经帮你做好了）
// <cstdio> 里的 printf 在 C++ 里就是 extern "C" 的
// 而 <cmath> 通常额外提供 C++ 重载版本（sin(float)/sin(double)）

int main()
{
    std::printf("1) C++ 重载 add(1,2)      = %d\n", add(1, 2));
    std::printf("1) C++ 重载 add(1.5,2.5)  = %.2f\n", add(1.5, 2.5));
    std::printf("1) nav::align(13)         = %d\n", nav::align(13));

    std::printf("2) extern \"C\" c_style_add = %d\n", c_style_add(3, 4));
    c_style_report();
    std::printf("3) extern \"C\" 块 wrapped_mul = %d\n", wrapped_mul(3, 4));

    return 0;
}
