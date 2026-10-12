// ch12_ex06.cpp —— 析构函数：对象"下班"时自动执行的收尾
// 编译：check.bat ch12_ex06.cpp c++latest
#include <cstdio>

class Tracer {
public:
    explicit Tracer(const char* name) : name_(name)
    {
        std::printf("  + 构造 %s\n", name_);
    }
    ~Tracer()
    {
        std::printf("  - 析构 %s\n", name_);
    }
private:
    const char* name_;
};

class Scope {
public:
    Scope()  { std::printf("  Scope 构造\n"); }
    ~Scope() { std::printf("  Scope 析构\n"); }
};

void inner()
{
    Tracer t("inner 里的局部对象");
    std::printf("    inner 函数体执行中\n");
}   // 离开作用域 -> t 自动析构

int main()
{
    std::printf("1) 局部对象：函数结束自动析构\n");
    inner();

    std::printf("2) 块作用域：离开 {} 就析构\n");
    {
        Tracer t("块里的对象");
        std::printf("    块内\n");
    }
    std::printf("    块外\n");

    std::printf("3) 多个对象：后构造的先析构（栈式，LIFO）\n");
    {
        Tracer a("先构造 A");
        Tracer b("后构造 B");
    }

    std::printf("4) main 里的对象在 main 结束时析构\n");
    Scope s;
    std::printf("   main 准备返回\n");
    return 0;
}
