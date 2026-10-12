// ch19_ex07_async_future.cpp —— std::async / std::future / std::promise / std::shared_future
// 编译: check.bat ch19_ex07_async_future.cpp c++latest
#include <iostream>
#include <future>
#include <thread>
#include <vector>
#include <string>
#include <mutex>
#include <numeric>
#include <stdexcept>
#include <chrono>

std::mutex g_ioMutex;
void say(const std::string& s)
{
    std::lock_guard<std::mutex> lk(g_ioMutex);
    std::cout << s << "\n";
}

int slowSquare(int x)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    return x * x;
}

int mayThrow(bool bad)
{
    if (bad) throw std::runtime_error("async 任务内部失败");
    return 7;
}

// ---------- 用 promise 手工传值（比 async 更底层）----------
void producerThread(std::promise<std::string> p, int n)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    if (n < 0) {
                // 通过 promise 传异常：future.get() 会把它抛出来
        p.set_exception(std::make_exception_ptr(std::runtime_error("输入不能为负")));
        return;
    }
    p.set_value("计算完成，n=" + std::to_string(n));
}

int main()
{
    say("=== A. std::async 最简用法：拿到 future，get() 时阻塞取结果 ===");
    {
        std::future<int> f = std::async(std::launch::async, slowSquare, 12);
        say("  main: 已提交任务，先去干别的");
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        say("  main: 现在取结果");
        int r = f.get();                 // 阻塞直到算完；get() 只能调用一次
        say("  结果 = " + std::to_string(r));
        say(std::string("  get() 之后 valid() = ") + (f.valid() ? "true" : "false")
            + "   <- future 被取空，不能再 get");
    }

    say("");
    say("=== B. launch::async vs launch::deferred（很重要）===");
    {
        say("  ① launch::async —— 真的开一个线程立刻算");
        auto t0 = std::chrono::steady_clock::now();
        std::future<int> fa = std::async(std::launch::async, slowSquare, 3);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        int ra = fa.get();
        auto d1 = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - t0).count();
        say("     结果 = " + std::to_string(ra) + "，从提交到取到共 " + std::to_string(d1)
            + " ms（>=100ms 说明计算早就并发完成了）");

        say("  ② launch::deferred —— 不开线程，get() 那一刻才在**当前线程**同步算");
        t0 = std::chrono::steady_clock::now();
        std::future<int> fd = std::async(std::launch::deferred, slowSquare, 3);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        auto dBefore = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - t0).count();
        say("     提交后等了 100ms，此时已经过了 " + std::to_string(dBefore) + " ms，但任务还没开始");
        int rd = fd.get();               // 这一句才真正执行 slowSquare
        auto d2 = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - t0).count();
        say("     结果 = " + std::to_string(rd) + "，get() 返回时共 " + std::to_string(d2)
            + " ms   <- 比 ① 多了约 50ms，就是被同步执行的那 50ms");
        say("  坑：std::async 的默认策略是 async|deferred，究竟走哪条由实现决定；");
        say("      想并发就必须显式写 std::launch::async。");
    }

    say("");
    say("=== C. 多任务并行：把所有 future 收起来，最后一起 get ===");
    {
        std::vector<std::future<int>> fs;
        auto t0 = std::chrono::steady_clock::now();
        for (int i = 1; i <= 4; ++i) fs.push_back(std::async(std::launch::async, slowSquare, i));
                // 4 个任务各睡 50ms。如果串行要 200ms；并行只需约 50ms。
        int sum = 0;
        for (auto& f : fs) sum += f.get();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - t0).count();
        say("  1^2+2^2+3^2+4^2 = " + std::to_string(sum));
        say("  总耗时 " + std::to_string(ms) + " ms（串行需要约 200ms）");
    }

    say("");
    say("=== D. 异常会被 future 保存，在 get() 处重新抛出 ===");
    {
        std::future<int> f1 = std::async(std::launch::async, mayThrow, true);
        try {
            f1.get();
        }
        catch (const std::exception& e) {
            say(std::string("  捕获到: ") + e.what());
        }
        std::future<int> f2 = std::async(std::launch::async, mayThrow, false);
        say("  正常任务 get() = " + std::to_string(f2.get()));
    }
    say("  这就是为什么用 async 比手搓 thread 舒服：返回值、异常都能带回来。");
    say("  但注意：如果 future 被析构时任务还没跑完，且是 launch::async，");
    say("        future 的析构会**阻塞等待**（相当于隐式 join）。");

    say("");
    say("=== E. std::promise：手工把一个值/异常交给另一个线程 ===");
    {
        for (int n : {5, -1}) {
            std::promise<std::string> p;
            std::future<std::string> f = p.get_future();
            std::thread t(producerThread, std::move(p), n);
            try {
                say("  n=" + std::to_string(n) + " -> " + f.get());
            }
            catch (const std::exception& e) {
                say("  n=" + std::to_string(n) + " -> 异常: " + e.what());
            }
            t.join();
        }
    }

    say("");
    say("=== F. std::shared_future：多个消费者取同一个结果 ===");
    {
        std::shared_future<int> sf =
            std::async(std::launch::async, slowSquare, 9).share();   // share() 转成可复制的
        std::vector<std::thread> ts;
        for (int i = 0; i < 3; ++i) {
            ts.emplace_back([sf, i] {                       // 按值捕获（shared_future 可拷贝）
                say("  [消费者 " + std::to_string(i) + "] 读到 " + std::to_string(sf.get()));
            });
        }
        for (auto& t : ts) t.join();
    }
    say("  future 只能 get 一次；shared_future 可以多线程各 get 一次。");

    say("");
    say("main 正常结束");
    return 0;
}
