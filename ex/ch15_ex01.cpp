// ch15_ex01.cpp —— 左值/右值，以及编译器怎么"看出"一个表达式的值类别
#include <cstdio>
#include <type_traits>
#include <utility>

// 两个重载：一个只接左值，一个只接右值
void which(int&)  { std::printf("  调用 which(int&)      —— 收到左值\n"); }
void which(int&&) { std::printf("  调用 which(int&&)     —— 收到右值\n"); }

int  make_int() { return 42; }          // 返回的是纯右值（prvalue）
int& ref_to(int& r) { return r; }       // 返回的是左值引用 -> 表达式是左值

int main()
{
    std::printf("--- 1) 字面量和临时对象是右值（prvalue）---\n");
    which(10);                 // 10 是纯右值
    which(1 + 2);              // 表达式的计算结果也是纯右值
    which(make_int());         // 函数返回的临时值也是纯右值

    std::printf("--- 2) 有名字的变量是左值（lvalue）---\n");
    int a = 10;
    which(a);
    which(ref_to(a));          // 返回左值引用 -> 表达式还是左值

    std::printf("--- 3) 关键点：右值引用变量本身是左值！---\n");
    int&& rref = 100;          // 给右值起了个名字 -> 它变成一个"左值"了
    std::printf("  rref = %d\n", rref);
    which(rref);               // 这里调用的是 which(int&)！
    which(std::move(rref));    // 想当成右值用，必须再 move 一次

    std::printf("--- 4) 用类型特性在编译期确认 ---\n");
    int  x = 1;
    int& lr = x;
    int&& rr = std::move(x);

    std::printf("  decltype(x)   绑定 int&  ? %d\n", std::is_same_v<decltype(x), int&>);
    std::printf("  decltype((x)) 绑定 int&  ? %d   （多一层括号 -> 左值引用）\n",
                std::is_same_v<decltype((x)), int&>);
    std::printf("  decltype(lr)  绑定 int&  ? %d\n", std::is_same_v<decltype(lr), int&>);
    std::printf("  decltype(rr)  绑定 int&& ? %d   （右值引用变量，类型是 int&&）\n",
                std::is_same_v<decltype(rr), int&&>);
    std::printf("  decltype((rr))绑定 int&  ? %d   （作为表达式用，它是左值）\n",
                std::is_same_v<decltype((rr)), int&>);
    std::printf("  decltype(10)  绑定 int   ? %d   （纯右值的类型不带引用）\n",
                std::is_same_v<decltype(10), int>);

    std::printf("--- 5) 赋值号左边必须是左值 ---\n");
    std::printf("  10 = 1;  // 编译错误：左操作数必须为左值\n");
    std::printf("  x = 1;   // 合法，x 是左值\n");
    x = 1;
    std::printf("  x = %d\n", x);

    std::printf("--- 6) 函数返回值的类别 ---\n");
    std::printf("  make_int() 返回 int（prvalue）\n");
    std::printf("  ref_to(x)  返回 int&（lvalue）\n");
    std::printf("  注意：C++ 里返回 int& 和返回 int 是完全不同的语义\n");

    return 0;
}
