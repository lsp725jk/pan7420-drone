// ch11_ex13.cpp —— auto 与类型推导基础
// 编译：check.bat ch11_ex13.cpp c++latest
#include <cstdio>
#include <string>
#include <vector>

// 函数返回类型也能用 auto（C++14 起），由 return 语句推导
auto add(int a, int b) { return a + b; }          // 推导为 int
auto add(double a, double b) { return a + b; }    // 推导为 double

int main()
{
        // 1) 最基本的用处：让编译器去写那些又长又啰嗦的类型
    auto    i  = 42;                  // int
    auto    d  = 3.14;                // double
    auto    c  = 'x';                 // char
    auto    s  = std::string("abc");  // std::string

    std::printf("1) i=%d d=%.2f c=%c s=%s\n", i, d, c, s.c_str());

        // 2) 迭代器：没有 auto 时类型名长得没法看
    std::vector<int> v{ 1, 2, 3 };
    auto it = v.begin();              // 类型是 std::vector<int>::iterator
    std::printf("2) *it=%d, v.size()=%llu\n", *it, (unsigned long long)v.size());

        // 3) auto 配合函数返回值
    std::printf("3) add(1,2)=%d, add(1.5,2.5)=%.2f\n", add(1, 2), add(1.5, 2.5));

        // 4) auto 的类型完全由 = 右边决定
    auto u = 10;                      // int
    auto w = 10u;                     // unsigned int
    auto z = 10.0f;                   // float（不是 double！）
    std::printf("4) sizeof(u)=%llu, sizeof(w)=%llu, sizeof(z)=%llu\n",
                (unsigned long long)sizeof(u), (unsigned long long)sizeof(w),
                (unsigned long long)sizeof(z));

    return 0;
}
