// ch13_ex10.cpp —— override 与 final
// 编译：check.bat ch13_ex10.cpp c++latest
#include <cstdio>

class Base {
public:
    virtual void f(int)      { std::printf("  Base::f(int)\n"); }
    virtual void g()         { std::printf("  Base::g()\n"); }
    virtual ~Base() = default;
};

class Mid : public Base {
public:
        // 1) override：让编译器检查"这真的是在覆盖虚函数"
    void f(int) override { std::printf("  Mid::f(int)  [加了 override，会检查签名]\n"); }

        // 2) 写错签名时 override 会立刻报错（这就是它最大的价值）
        // void f(double) override;    // 错误：没有可覆盖的虚函数（参数类型不匹配）
        // void g() const override;    // 错误：const 不匹配
        // void h() override;          // 错误：Base 里没有 h

        // 3) final 用在虚函数上：禁止后续派生类再覆盖
    void g() override final { std::printf("  Mid::g()  [final，后面不能再覆盖]\n"); }
};

class Leaf : public Mid {
public:
    void f(int) override { std::printf("  Leaf::f(int)\n"); }
        // void g() override;          // 错误：Mid::g 是 final，不能覆盖
};

// 4) final 用在类上：整个类不能被继承
class Sealed final {
public:
    void run() { std::printf("  Sealed::run()\n"); }
};
// class TryInherit : public Sealed {};   // 错误：Sealed 是 final

int main()
{
    Base* b = new Mid();
    b->f(1);
    b->g();

    Base* c = new Leaf();
    c->f(1);
    c->g();                 // 仍然调用 Mid::g（final 只是禁止再覆盖）

    Sealed s;
    s.run();

    delete b;
    delete c;

    std::printf("5) 实践建议：\n");
    std::printf("   - 派生类里覆盖虚函数一律写 override（C++11 起）\n");
    std::printf("   - 不想被继承的类写 final；不想被覆盖的虚函数写 final\n");
    std::printf("   - 有了 override/final，这类错误从运行期莫名调用错函数变成编译期报错\n");
    return 0;
}
