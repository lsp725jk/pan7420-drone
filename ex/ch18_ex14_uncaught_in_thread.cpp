// ch18_ex14_uncaught_in_thread.cpp —— 子线程里的异常逃出线程函数 = 整个进程 terminate
// 编译: check.bat ch18_ex14_uncaught_in_thread.cpp c++latest
// 退出码非 0：线程里没人接的异常会把整个程序带走（详见第 19 章）。
#include <iostream>
#include <stdexcept>
#include <thread>
#include <chrono>

int main()
{
    std::cout << "=== 主线程活得好好的，子线程抛异常没人接 ===" << std::endl;
    std::cout << "  主线程: 启动 worker" << std::endl;

    std::thread worker([] {
        std::cout << "  worker: 即将 throw" << std::endl;
                // 下面这个 if 是为了防止编译器警告"无条件抛"；永远为真
        if (std::chrono::steady_clock::now().time_since_epoch().count() != 0) {
            throw std::runtime_error("worker 内部错误，没人接");
        }
        std::cout << "  worker: 这行不会执行" << std::endl;
    });

    worker.join();                    // join 会等 worker 结束，等来的是 terminate

    std::cout << "  主线程: 这行永远不会执行" << std::endl;
    return 0;
}
