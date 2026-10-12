// ch18_ex13_uncaught.cpp —— 异常无人接住会怎样（主线程 / 子线程两种）
// 编译: check.bat ch18_ex13_uncaught.cpp c++latest
// 本程序故意不接异常，退出码非 0 —— 这正是"异常没人管"的症状。
#include <iostream>
#include <stdexcept>
#include <thread>

struct Loud {
    const char* n;
    explicit Loud(const char* s) : n(s) { std::cout << "    + 构造 " << n << std::endl; }
    ~Loud() { std::cout << "    - 析构 " << n << std::endl; }
};

void thrower(const char* who)
{
    Loud here(who);
    std::cout << "  " << who << ": 即将 throw（无人接）" << std::endl;
    throw std::runtime_error(std::string(who) + " 抛出的异常");
}

int main(int argc, char**)
{
    std::cout << "=== A. 异常从 main 飞出去：不展开栈，直接 terminate ===" << std::endl;
    std::cout << "  对比看：Loud 的析构函数一行都不会打印" << std::endl;
    Loud inMain("main 里的对象");
    thrower("main");                 // 一路飞出 main，没有任何 catch

    std::cout << "  这行不会执行" << std::endl;
    (void)argc;
    return 0;
}
