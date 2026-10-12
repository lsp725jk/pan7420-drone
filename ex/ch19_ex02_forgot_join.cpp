// ch19_ex02_forgot_join.cpp —— 忘 join / 忘 detach 会 std::terminate（本程序故意崩）
// 编译: check.bat ch19_ex02_forgot_join.cpp c++latest
// 退出码非 0 —— 这正是"忘了 join"的症状。
#include <iostream>
#include <thread>
#include <chrono>

int main()
{
    std::cout << "=== 故意制造：一个 joinable 的 thread 被析构 ===" << std::endl;
    std::cout << "  main: 启动一个要跑 3 秒的线程" << std::endl;

    {
        std::thread t([] {
            std::this_thread::sleep_for(std::chrono::seconds(3));
            std::cout << "  子线程: 我睡醒了（这行永远打印不到）" << std::endl;
        });
        std::cout << "  t.joinable() = " << (t.joinable() ? "true" : "false") << std::endl;
        std::cout << "  main: 马上离开作用域，但既没有 join 也没有 detach" << std::endl;
                // ← t 在这里被析构。因为 joinable() == true，标准要求调用 std::terminate
    }

    std::cout << "  main: 这行永远不会执行" << std::endl;
    return 0;
}
