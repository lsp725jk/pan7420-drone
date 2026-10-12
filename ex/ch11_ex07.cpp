// ch11_ex07.cpp —— 引用(&)是什么：C 指针的"别名"版本
// 编译：check.bat ch11_ex07.cpp c++latest
#include <cstdio>

int main()
{
    int value = 10;

        // ---- C 的写法：指针 ----
    int* p = &value;      // 取地址
    *p = 20;              // 解引用后赋值
    std::printf("C 指针写法 : value=%d, *p=%d, p=%p\n", value, *p, (void*)p);

        // ---- C++ 的写法：引用 ----
    int& r = value;       // r 是 value 的"别名"，& 在这里是"声明引用"
    r = 30;               // 直接当作 value 来用，不写 *
    std::printf("C++ 引用写法: value=%d, r=%d\n", value, r);

        // ---- 三个关键区别 ----
        // 1) 引用必须初始化，指针可以先空着
        // int& bad;                  // 编译错误：引用必须初始化
    int* can_be_null = nullptr;  // 指针可以先不指向任何东西
    std::printf("指针可以为空: %s\n", can_be_null == nullptr ? "是" : "否");

        // 2) 引用一旦绑定不能改绑；给引用赋值 = 给原变量赋值
    int other = 100;
    r = other;                   // 这不是"让 r 指向 other"，而是 value = 100
    std::printf("r = other 之后: value=%d, other=%d（value 被改了，r 没改绑）\n",
                value, other);

        // 指针可以改指向
    p = &other;
    std::printf("p = &other 之后: *p=%d（p 改指向了）\n", *p);

        // 3) sizeof 的含义不同
    std::printf("sizeof(r)=%zu（是 int 的大小）, sizeof(p)=%zu（是指针的大小）\n",
                sizeof(r), sizeof(p));

    return 0;
}
