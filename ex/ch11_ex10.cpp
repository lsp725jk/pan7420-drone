// ch11_ex10.cpp —— 左值/右值、const 引用、&& 简介
// 编译：check.bat ch11_ex10.cpp c++latest
// 本章只需要知道"引用能绑什么"，移动语义与完美转发详见第 14 章。
#include <cstdio>
#include <string>

void take_ref(int& v)        { std::printf("  绑到非常量左值: %d\n", v); }
void take_const_ref(const int& v) { std::printf("  绑到 const 引用 : %d\n", v); }

int make_value() { return 7; }

int main()
{
    int a = 1;

        // ---- 左值(lvalue)：有名字、有地址，能出现在 = 左边 ----
    take_ref(a);
    take_const_ref(a);          // const 引用可以绑左值

        // ---- 右值(rvalue)：临时结果，没有名字 ----
        // take_ref(2);             // 错误：非常量引用不能绑右值
    take_const_ref(2);          // 正确：const 引用可以绑右值，会延长其生命周期
    take_const_ref(a + 3);
    take_const_ref(make_value());
    std::printf("  const 引用延长临时量的生命期，绑完还能用\n");

        // ---- const 引用绑临时 string：C 里得手动 malloc/free 的场景 ----
    const std::string& s = std::string("temporary");   // 临时对象被延长到 s 离开作用域
    std::printf("  const string& 绑临时量，长度 = %zu\n", s.size());

        // ---- 右值引用 && ：C++11 起，专门用来绑右值（本章只需认识符号）----
    int&& rr = 5;               // rr 绑定到一个临时 int，生命期被延长
    std::printf("  右值引用 int&& rr = %d（详见第 14 章 移动语义）\n", rr);

        // ---- auto&& ：能接住左值和右值两种 ----
    auto&& f1 = a;              // a 是左值 -> f1 是 int&
    auto&& f2 = 5;              // 5 是右值 -> f2 是 int&&，生命期被延长
    std::printf("  auto&& 两个都能接: %d %d\n", f1, f2);

    return 0;
}
