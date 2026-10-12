// ch13_ex04.cpp —— 虚析构函数实验（一）：不加 virtual 会怎样
// 编译：check.bat ch13_ex04.cpp c++latest

// 实验设计：用工厂函数返回基类指针，指向一个派生类对象，
// 然后 delete 基类指针。观察派生类的析构函数有没有被调用。
// 注意：这个程序的行为在标准里是未定义行为（UB），
// 本机实测的"看起来正常结束"不代表它是正确的。
#include <cstdio>

static int g_live = 0;              // 记录"还没释放的缓冲块数"

class Base {
public:
    Base() { std::printf("  Base 构造\n"); }
    ~Base() { std::printf("  Base 析构\n"); }
        // 注意：这里故意没有写 virtual
};

class Derived : public Base {
public:
    Derived()
    {
        buf_ = new char[1024];
        ++g_live;
        std::printf("  Derived 构造（申请 1KB，存活 %d 块）\n", g_live);
    }
    ~Derived()
    {
        delete[] buf_;              // 这段代码不会被调用 -> 泄漏
        --g_live;
        std::printf("  Derived 析构（释放 1KB，存活 %d 块）\n", g_live);
    }
private:
    char* buf_ = nullptr;
};

Base* create() { return new Derived(); }

int main()
{
    std::printf("=== 实验 A：不加 virtual ===");
    std::printf("\n1) 通过基类指针 delete 派生类对象：\n");
    Base* p = create();
    delete p;
    std::printf("   ← 只有 Base 析构被调用，Derived 析构完全没执行\n");
    std::printf("   ← 存活块数 = %d（本来应该是 0，说明 1KB 泄漏了）\n", g_live);

    std::printf("\n2) 同一份代码，换成先转回派生类指针再 delete：\n");
    Base* q = create();
    Derived* d = static_cast<Derived*>(q);      // 只有你确定真实类型时才能这么转
    delete d;
    std::printf("   ← 这次两个析构都执行了，存活块数 = %d\n", g_live);

    std::printf("\n3) 结论（症状 -> 原因 -> 修法）：\n");
    std::printf("   症状：delete 基类指针时派生类析构不执行，资源泄漏，堆被破坏\n");
    std::printf("   原因：析构函数不是 virtual 时是静态绑定，按指针的静态类型选函数\n");
    std::printf("   修法：给基类析构函数加 virtual（见 ch13_ex05.cpp）\n");
    std::printf("   注意：这是 UB，不同编译器的表现可能不同（未实测其它编译器）\n");
    return 0;
}
