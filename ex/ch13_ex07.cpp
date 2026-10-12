// ch13_ex07.cpp —— 虚函数与虚函数表（vtable）原理
// 编译：check.bat ch13_ex07.cpp c++latest

// 这个程序用 sizeof 与"通过基类指针调用"的实验，把 vtable 的存在"量"出来。
#include <cstdio>

class NoVirt {                       // 没有虚函数
public:
    int a = 1;
    int b = 2;
    int sum() const { return a + b; }
};

class OneVirt {                      // 一个虚函数
public:
    int a = 1;
    int b = 2;
    virtual int sum() const { return a + b; }
};

class TwoVirt {                      // 两个虚函数：还是只多一个指针
public:
    int a = 1;
    int b = 2;
    virtual int sum() const { return a + b; }
    virtual int diff() const { return a - b; }
};

class Base {
public:
    virtual const char* name() const { return "Base"; }
    virtual ~Base() = default;
};

class Derived : public Base {
public:
    const char* name() const override { return "Derived"; }
};

int main()
{
        // 1) 有虚函数的对象会多一个"虚函数表指针"（vptr）
    std::printf("1) sizeof(NoVirt)=%llu（两个 int）\n", (unsigned long long)sizeof(NoVirt));
    std::printf("   sizeof(OneVirt)=%llu（多了一个 vptr，64 位下 8 字节）\n",
                (unsigned long long)sizeof(OneVirt));
    std::printf("   sizeof(TwoVirt)=%llu（虚函数再多，对象里也只多一个 vptr）\n",
                (unsigned long long)sizeof(TwoVirt));

        // 2) 通过基类指针调用虚函数：运行期查表决定调哪个
    Base  b;
    Derived d;
    Base* p1 = &b;
    Base* p2 = &d;
    std::printf("2) Base* p1=&b; p1->name() = %s\n", p1->name());
    std::printf("   Base* p2=&d; p2->name() = %s（同一个指针类型，不同结果）\n", p2->name());

        // 3) 非虚函数没有这个效果
    std::printf("3) 机制说明：\n");
    std::printf("   每个有多态的类有一张虚函数表 vtable（编译期生成，只读数据）\n");
    std::printf("   每个对象里存一个 vptr 指向它所属类的 vtable\n");
    std::printf("   调用 p->name() 时实际执行：(*p->vptr[偏移])(p)\n");
    std::printf("   代价：每个对象多一个指针，每次调用多一次间接寻址（通常无法内联）\n");

        // 4) 三条实践结论
    std::printf("4) 实践结论：\n");
    std::printf("   - 不是所有函数都要 virtual：多一次间接 + 对象变大 + 难内联\n");
    std::printf("   - 基类只要有虚函数，析构函数就必须 virtual\n");
    std::printf("   - 具体内存布局是实现定义的，不同编译器可能不同，不要依赖\n");
    return 0;
}
