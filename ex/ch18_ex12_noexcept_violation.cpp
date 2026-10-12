// ch18_ex12_noexcept_violation.cpp —— 当 noexcept 函数真的抛了异常：std::terminate
// 编译: check.bat ch18_ex12_noexcept_violation.cpp c++latest
// 注意：本程序的退出码是 3（Windows 下 abort 的结果），不是 0 —— 这正是要演示的症状。
#include <iostream>
#include <stdexcept>
#include <exception>
#include <cstdio>

void say(const char* s)
{
    std::cout << s << std::endl;      // 用 endl 强刷，保证 terminate 之前输出已经落地
}

void willThrow()
{
    say("    进入 willThrow(), 即将抛出");
    throw std::runtime_error("我不该抛出来的");
        // 这里本来可以再写一句 say("这行不会执行");
        // 但编译器会报 warning C4702（无法访问的代码），
        // 而且这句话**永远不会出现在运行结果里**，
        // 所以直接用注释代替它，保持示例零警告。
}

void promisesNoThrow() noexcept        // 承诺"绝不抛"
{
    say("    进入 promisesNoThrow() [noexcept]");
    willThrow();                       // 承诺被打破
    say("    这行也不会执行");
}

void promisesNoThrowCaught() noexcept  // 正确姿势：在 noexcept 函数内部自己吃掉
{
    try {
        willThrow();
    }
    catch (const std::exception& e) {
        say("    [noexcept 函数内部] 捕获并吞掉异常，承诺没被打破");
        std::cout << "      内容: " << e.what() << std::endl;
    }
}

int main()
{
        // 注册 terminate 处理器：进程死之前先说一句话
    std::set_terminate([]() {
        std::cout << "  >>> std::terminate 被调用了！" << std::endl;
        std::cout << "  >>> 有正在飞的异常吗？ "
                  << (std::current_exception() ? "有" : "没有（可能是别的原因）") << std::endl;
        std::cout << "  >>> 注意：栈不会继续展开，noexcept 函数里已经构造好的局部对象\n"
                     "      在 MSVC 上不保证析构（未实测其它编译器）" << std::endl;
        std::cout.flush();
        std::abort();                  // 让退出码保持非 0
    });

    say("=== A. noexcept 函数内部自己消化异常：程序正常继续 ===");
    promisesNoThrowCaught();
    say("  回到 main，程序还活着");

    say("");
    say("=== B. noexcept 函数让异常飞出去：立即 terminate ===");
    say("  调用 promisesNoThrow() ... （下一行之后程序就会死）");
    promisesNoThrow();

    say("这行永远打印不出来");
    return 0;
}
