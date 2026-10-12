// ch13_ex12.cpp —— 菱形继承与二义性
// 编译：check.bat ch13_ex12.cpp c++latest

// 经典的"菱形"：Base 派生出 A 和 B，C 同时继承 A 和 B。
// 不做特殊处理时，C 里会有两份 Base 子对象。
#include <cstdio>

class Base {
public:
    int value = 1;
    void hello() { std::printf("  Base::hello()\n"); }
};

class A : public Base { };
class B : public Base { };

class C : public A, public B { };      // 菱形底部

int main()
{
    C c;

        // 1) 访问继承来的成员：二义性，编译不过
        // std::printf("%d", c.value);        // 错误：不明确的符号（A::Base::value 还是 B::Base::value）
        // c.hello();                         // 错误：同上

        // 2) 必须显式指明走哪条路
    std::printf("1) c.A::value = %d\n", c.A::value);
    std::printf("   c.B::value = %d\n", c.B::value);
    c.A::hello();
    c.B::hello();

        // 3) 可以修改其中一份，另一份不受影响 -> 证明真的有两份
    c.A::value = 100;
    std::printf("2) 改 c.A::value 之后: A::value=%d  B::value=%d\n",
                c.A::value, c.B::value);
    std::printf("   同一个对象里存了两份 Base -> 数据不一致，这是菱形继承最大的问题\n");

        // 4) 尺寸对比（下一节的虚继承会变大）
    std::printf("3) sizeof(Base)=%llu sizeof(A)=%llu sizeof(C)=%llu\n",
                (unsigned long long)sizeof(Base),
                (unsigned long long)sizeof(A),
                (unsigned long long)sizeof(C));

    std::printf("4) 症状 -> 原因 -> 修法：\n");
    std::printf("   症状：访问继承成员时报不明确的符号；同一份数据有两个副本\n");
    std::printf("   原因：普通继承为每条路径各生成一个基类子对象\n");
    std::printf("   修法：让 A、B 用虚继承（virtual public Base），见 ch13_ex13.cpp\n");
    return 0;
}
