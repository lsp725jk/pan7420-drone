// ch13_ex05.cpp —— 虚析构函数实验（二）：加了 virtual 的对照
// 编译：check.bat ch13_ex05.cpp c++latest

// 和 ch13_ex04.cpp 只差一个 virtual，其余完全相同。
// 用例子里有"真的资源"：构造函数 new，析构函数 delete，泄漏能直接看出来。
#include <cstdio>

static int g_live = 0;              // 记录"还没释放的缓冲块数"

class Base {
public:
    Base() { std::printf("  Base 构造\n"); }
    virtual ~Base() { std::printf("  Base 析构\n"); }      // ★ 这一个 virtual 是关键
};

class Derived : public Base {
public:
    Derived()
    {
        buf_ = new char[1024];       // 申请资源
        ++g_live;
        std::printf("  Derived 构造（申请 1KB，当前存活 %d 块）\n", g_live);
    }
    ~Derived() override
    {
        delete[] buf_;               // 释放资源
        --g_live;
        std::printf("  Derived 析构（释放 1KB，当前存活 %d 块）\n", g_live);
    }
private:
    char* buf_ = nullptr;
};

Base* create() { return new Derived(); }

int main()
{
    std::printf("1) 加 virtual 之后，通过基类指针 delete：\n");
    delete create();                 // 两个析构都会执行，且顺序是 派生 -> 基类
    std::printf("   存活块数 = %d\n", g_live);

    std::printf("\n2) 数组也不行？delete[] 基类指针：\n");
        // delete[] p2; 配 new[] 才是合法的，这里只是提醒：
        // 通过基类指针删除"派生类数组"是有问题的（数组的步长按基类大小算），
        // 标准做法是别用裸数组，改用 std::vector<std::unique_ptr<Base>>（详见第 14 章）
    std::printf("   结论：多态删除要用 delete 单个对象，数组请用容器管理\n");

    std::printf("\n3) 最终存活块数 = %d（等于 0 表示没有泄漏）\n", g_live);

    std::printf("\n4) 规则总结：\n");
    std::printf("   - 只要一个类可能被继承并且通过基类指针删除，基类析构函数就必须是 virtual\n");
    std::printf("   - 只要类里有 virtual 函数，析构函数就应该 virtual（否则 /W4 会给 warning C4265）\n");
    std::printf("   - 反过来说：不打算被继承的类，可以把析构函数写成非虚（甚至 final）以省一层间接\n");
    return 0;
}
