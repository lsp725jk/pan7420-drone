// ch13_ex19.cpp —— 两个高级坑：构造/析构里调用虚函数、纯虚函数被误调
// 编译：check.bat ch13_ex19.cpp c++latest
#include <cstdio>

class Base {
public:
    Base()
    {
        std::printf("  Base 构造里调用 who(): %s\n", who());   // 只会得到 "Base"
    }
    virtual ~Base()
    {
        std::printf("  Base 析构里调用 who(): %s\n", who());   // 也只会得到 "Base"
    }
    virtual const char* who() const { return "Base"; }
};

class Derived : public Base {
public:
    Derived() { std::printf("  Derived 构造\n"); }
    ~Derived() override { std::printf("  Derived 析构\n"); }
    const char* who() const override { return "Derived"; }
};

// 反面教材：在基类构造函数里调用"自己的纯虚函数"
class BadBase {
public:
    BadBase() { print(); }        // 这里调用纯虚函数
    virtual void print() = 0;
    virtual ~BadBase() = default;
};

class BadDerived : public BadBase {
public:
    void print() override { std::printf("  BadDerived::print()\n"); }
};

int main()
{
    std::printf("1) 构造/析构期间调用虚函数：\n");
    {
        Derived d;
        std::printf("  对象构造完毕，正常调用 who(): %s\n", d.who());
    }
    std::printf("  结论：构造/析构期间，对象的动态类型是当前正在构造或析构的那一层，\n");
    std::printf("        所以虚函数退化成静态绑定。\n");

    std::printf("\n2) 在基类构造函数里调用纯虚函数 —— 不要这么做：\n");
    std::printf("   （本程序不实际执行，因为它是 UB，MSVC 会直接报链接错误）\n");
    std::printf("   如果 BadBase 的构造函数调用 print()，而 print 是纯虚的，\n");
    std::printf("   编译能过，但链接时会报无法解析的外部符号 BadBase::print\n");

    std::printf("\n3) 实用结论：\n");
    std::printf("   - 构造函数/析构函数里只做自己这一层的初始化/清理\n");
    std::printf("   - 需要多态行为时，提供一个 init()/start() 之类的成员函数，构造完再调\n");
    std::printf("   - 想在析构里打印类型名，就像 ch13_ex18 那样把名字存成成员\n");
    return 0;
}
