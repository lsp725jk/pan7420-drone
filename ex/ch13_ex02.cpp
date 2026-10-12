// ch13_ex02.cpp —— 构造与析构顺序：基类先构造，派生类先析构
// 编译：check.bat ch13_ex02.cpp c++latest
#include <cstdio>

class Base {
public:
    Base()  { std::printf("  1 Base 构造\n"); }
    ~Base() { std::printf("  5 Base 析构\n"); }
};

class Member {                       // 派生类的成员对象
public:
    Member()  { std::printf("  2 Member 构造（基类之后、派生类体之前）\n"); }
    ~Member() { std::printf("  4 Member 析构\n"); }
};

class Derived : public Base {
public:
    Derived()                        // 成员 m_ 不需要写进初始化列表：它会被默认构造
    {
        std::printf("  3 Derived 构造函数体\n");
    }
    ~Derived()
    {
        std::printf("  4' Derived 析构函数体（先执行体，再析构成员和基类）\n");
    }
private:
    Member m_;
};

int main()
{
    std::printf("1) 构造一个派生类对象，观察顺序：\n");
    {
        Derived d;
        std::printf("   （对象已构造完毕）\n");
    }

    std::printf("2) 顺序总结：\n");
    std::printf("   构造: 基类 -> 成员对象（按声明顺序）-> 派生类构造函数体\n");
    std::printf("   析构: 派生类析构体 -> 成员对象（逆序）-> 基类\n");

        // 3) 为什么必须这样？
    std::printf("3) 原因：派生类的构造函数体可能用到基类和成员，\n");
    std::printf("   所以它们必须先就绪；析构时反过来，先拆自己再拆地基。\n");

        // 4) 常见坑：在基类构造函数里调用虚函数
    std::printf("4) 坑：基类构造期间，对象的派生部分还没构造，\n");
    std::printf("   此时调用虚函数只会调用基类版本（不是派生类版本）。\n");
    return 0;
}
