// ch11_ex11.cpp —— const 在 C++ 里比 C 多了什么
// 编译：check.bat ch11_ex11.cpp c++latest
#include <cstdio>

// C++ 里 const 修饰的整型变量是"编译期常量"，可以用来定数组长度
const int kMax = 8;
// C 里这行在 C99 之前是编译错误，C99 起是变长数组（VLA）；
// C++ 从来没有 VLA（C++ 标准里它是扩展），所以 const 常量必须能编译期求值。
int table[kMax];

// 函数参数：const 引用 = "我不拷贝，也不改你的东西"
void print(const int& v) { std::printf("  print: %d\n", v); }

int main()
{
        // 1) const 变量的初始化：C++ 里 const 变量必须初始化
    const int a = 10;
        // const int b;              // 错误：const 对象必须有初始化器
    std::printf("1) const int a = %d, 数组长度 kMax = %d\n", a, kMax);

        // 2) 指向 const 的指针 vs const 指针（两种都叫 const 指针，容易混）
    int x = 1, y = 2;
    const int* p1 = &x;          // 不能通过 p1 改 x，但 p1 可以改指向
    int* const p2 = &x;          // 可以通过 p2 改 x，但 p2 不能改指向
    const int* const p3 = &x;    // 都不能改
    p1 = &y;                     // 合法：改的是指针自己
    *p2 = 5;                     // 合法：改的是指向的对象
    std::printf("2) *p1=%d, *p2=%d, *p3=%d\n", *p1, *p2, *p3);
        // *p1 = 9;                  // 错误：表达式必须是可修改的左值
        // p2 = &y;                  // 错误：p2 是只读的

        // 3) constexpr：C++11 起，明确表示"编译期就能算出来"
    constexpr int sq = 6 * 6;
    constexpr double pi = 3.14159265358979323846;
    std::printf("3) constexpr sq=%d, pi=%.4f\n", sq, pi);
    static_assert(sq == 36, "sq 必须是编译期常量");   // 编译期断言
    std::printf("   static_assert 在编译期通过\n");

        // 4) const 引用做参数：不拷贝、不改动
    int big = 123456;
    print(big);
    print(42);                   // 也能绑右值
    std::printf("4) const 引用参数既能省拷贝，又保证只读\n");

    return 0;
}
