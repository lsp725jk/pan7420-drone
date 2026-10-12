// ch13_ex03.cpp —— 名字隐藏（name hiding）vs 重载 vs 覆盖
// 编译：check.bat ch13_ex03.cpp c++latest
#include <cstdio>

class Base {
public:
    void f()         { std::printf("  Base::f()\n"); }
    void f(int)      { std::printf("  Base::f(int)\n"); }        // 重载
    void g()         { std::printf("  Base::g()\n"); }
    virtual void v() { std::printf("  Base::v()\n"); }           // 虚函数
};

class Derived : public Base {
public:
        // 只声明了 f()，它把基类里所有叫 f 的版本**全部隐藏**了（不是覆盖）
    void f() { std::printf("  Derived::f()\n"); }

        // 想保留基类版本：用 using 声明把它们引进来
    using Base::g;

        // 覆盖虚函数（同名、同参数、同返回类型）
    void v() override { std::printf("  Derived::v()\n"); }
};

class DerivedG : public Base {
public:
    using Base::f;                 // 把基类所有 f 引进来
    void f() { std::printf("  DerivedG::f()\n"); }   // 再加重载一个新版本
};

int main()
{
    Derived d;
    std::printf("1) 名字隐藏的效果：\n");
    d.f();                          // Derived::f()
        // d.f(1);                      // 错误：Base::f(int) 被隐藏了，看不见
    std::printf("   d.f(1) 会编译失败，因为 Base::f(int) 被隐藏了\n");

    std::printf("2) using 可以把基类的重载集合引回来：\n");
    d.g();                          // 通过 using Base::g 可见
    DerivedG dg;
    dg.f();                         // DerivedG::f()
    dg.f(1);                        // Base::f(int)：被 using 引进来了

    std::printf("3) 覆盖（override）与隐藏的区别：\n");
    Base* p = &d;
    p->f();                         // Base::f()：f 不是虚函数，静态绑定
    p->v();                         // Derived::v()：v 是虚函数，动态绑定

    std::printf("4) 三种情况对照：\n");
    std::printf("   重载(overload) : 同一个作用域内，同名不同参\n");
    std::printf("   隐藏(hiding)   : 派生类里的同名成员把基类的全部同名成员挡住（不看参数）\n");
    std::printf("   覆盖(override) : 派生类用同名同参函数替换基类的虚函数\n");
    return 0;
}
