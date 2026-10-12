// ch19_ex01_thread_basics.cpp —— std::thread 的创建、join、detach、joinable
// 编译: check.bat ch19_ex01_thread_basics.cpp c++latest
#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <vector>
#include <atomic>
#include <chrono>

std::mutex g_coutMutex;                       // 保证多线程打印不交错

// 需要加锁的打印：一次 << 链条整体持锁，输出才不会互相插队
void say(const std::string& s)
{
    std::lock_guard<std::mutex> lk(g_coutMutex);
    std::cout << s << "\n";
}

// ---------- 普通函数也能当线程入口 ----------
void workerWithArgs(int id, std::string tag)
{
    say("  [线程" + std::to_string(id) + "] 收到 tag=" + tag);
}

// ---------- 演示 detach 的入口：睡一会儿再打印 ----------
void slowWorker()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    say("  [detach 出去的线程] 我还在跑，主线程已经不管我了");
}

// ---------- 最小 RAII 守卫：不管怎么退出都 join ----------
class ThreadGuard {
public:
    explicit ThreadGuard(std::thread&& t) : t_(std::move(t)) {}
    ~ThreadGuard() {
        if (t_.joinable()) {
            t_.join();
            say("  [ThreadGuard] 析构时自动 join 了");
        }
    }
    ThreadGuard(const ThreadGuard&) = delete;
    ThreadGuard& operator=(const ThreadGuard&) = delete;
private:
    std::thread t_;
};

int main()
{
    say("=== A. 最基本的创建与 join ===");
    std::atomic<int> counter{0};
    std::thread t1([&counter] {
        for (int i = 0; i < 1000; ++i) counter.fetch_add(1);
        say("  [线程1] 数完了");
    });
    say(std::string("  joinable() 在 join 之前 = ") + (t1.joinable() ? "true" : "false"));
    t1.join();                                   // 必须：等它结束并回收
    say(std::string("  joinable() 在 join 之后 = ") + (t1.joinable() ? "true" : "false"));
    say("  counter = " + std::to_string(counter.load()));

    say("");
    say("=== B. 线程入口的三种写法 ===");
        // B1: 普通函数 + 参数
    std::thread t2(workerWithArgs, 2, std::string("函数指针"));
    t2.join();
        // B2: lambda
    std::thread t3([] { say("  [线程3] lambda 入口"); });
    t3.join();
        // B3: 函数对象（带 operator() 的类）
    struct Functor {
        void operator()() const { say("  [线程4] 函数对象入口"); }
    };
    std::thread t4(Functor{});
    t4.join();

    say("");
    say("=== C. 多个线程：注意谁是并发、谁要等谁 ===");
    std::vector<std::thread> pool;
    for (int i = 0; i < 3; ++i) {
        pool.emplace_back([i] { say("  [并发线程 " + std::to_string(i) + "] 运行中"); });
    }
    say("  main: 3 个线程已启动（上面几行的顺序每次运行可能不同）");
    for (auto& th : pool) th.join();
    say("  main: 全部 join 完毕");

    say("");
    say("=== D. detach：把线程交出去，不再 join ===");
    std::thread t5(slowWorker);
    t5.detach();
    say(std::string("  detach 之后 joinable() = ") + (t5.joinable() ? "true" : "false"));
    say("  main: 我先睡 400ms，等 detach 的线程有机会打印");
    std::this_thread::sleep_for(std::chrono::milliseconds(400));
    say("  main: 睡醒了");

    say("");
    say("=== E. RAII 守卫：忘 join 的解药（C++20 的 jthread 内置了这个）===");
    {
        ThreadGuard g(std::thread([] {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            say("  [守卫里的线程] 干完活了");
        }));
        say("  main: 出了这个作用域，守卫会替我 join");
    }
    say("  main: 守卫已析构，线程已回收");

    say("");
    say("main 正常结束");
    return 0;
}
