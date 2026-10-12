// ch19_ex09_thread_pool.cpp —— 线程池概念：一个 40 行的最小实现
// 编译: check.bat ch19_ex09_thread_pool.cpp c++latest
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <vector>
#include <string>
#include <atomic>
#include <chrono>

std::mutex g_ioMutex;
void say(const std::string& s)
{
    std::lock_guard<std::mutex> lk(g_ioMutex);
    std::cout << s << "\n";
}

// ============================================================
// 最小线程池：N 个工人 + 1 个任务队列
// 核心只有 4 样东西：任务队列、队列锁、条件变量、"该收工了"标志
// ============================================================
class ThreadPool {
public:
    explicit ThreadPool(unsigned n)
    {
        for (unsigned i = 0; i < n; ++i) {
            workers_.emplace_back([this, i] { workerLoop(i); });
        }
    }

    ~ThreadPool()
    {
        {
            std::lock_guard<std::mutex> lk(m_);
            stopping_ = true;
        }
        cv_.notify_all();                     // 叫醒所有睡着的工人
        for (auto& w : workers_) {
            if (w.joinable()) w.join();
        }
    }

    void submit(std::function<void()> job)
    {
        {
            std::lock_guard<std::mutex> lk(m_);
            jobs_.push(std::move(job));
        }
        cv_.notify_one();
    }

private:
    void workerLoop(unsigned id)
    {
        for (;;) {
            std::function<void()> job;
            {
                std::unique_lock<std::mutex> lk(m_);
                                // 标准写法：谓词 = "有活干 或者 该收工了"
                cv_.wait(lk, [this] { return stopping_ || !jobs_.empty(); });
                if (stopping_ && jobs_.empty()) return;    // 收工（先把队列里的活干完）
                job = std::move(jobs_.front());
                jobs_.pop();
            }
                        // 注意：解锁之后再执行任务。否则一个长任务会挡住所有工人取活。
            job();
            g_done.fetch_add(1);
            (void)id;
        }
    }

    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> jobs_;
    std::mutex m_;
    std::condition_variable cv_;
    bool stopping_ = false;
    static std::atomic<int> g_done;
};
std::atomic<int> ThreadPool::g_done{0};

int main()
{
    say("=== A. 提交 12 个任务给 3 个工人 ===");
    auto t0 = std::chrono::steady_clock::now();
    {
        ThreadPool pool(3);
        for (int i = 1; i <= 12; ++i) {
            pool.submit([i] {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                say("  [任务 " + std::to_string(i) + "] 完成，线程 id="
                    + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()) % 1000));
            });
        }
        say("  main: 12 个任务已提交，等 pool 析构时自动收工");
    }   // pool 析构：通知 + join 所有工人
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - t0).count();
    say("  12 个任务每个 50ms，3 个工人 -> 约 4 批 = 200ms；实测 " + std::to_string(ms) + " ms");

    say("");
    say("=== B. 为什么不能每个任务都 std::thread ===");
    say("  如果上面改成 12 个 std::thread：");
    say("    - 每个线程默认栈 1MB（Windows），12 个就是 12MB 虚拟内存；");
    say("    - 线程创建/销毁本身要几十微秒，短任务下开销比干活还大；");
    say("    - 线程数超过 CPU 核数后，只是让操作系统更频繁地切换，不会更快；");
    say("    - 极端情况：任务里再创建线程 -> 线程耗尽，创建失败抛 std::system_error。");
    say("  线程池把这四件事一次性解决：线程复用、数量可控、队列削峰、避免爆炸。");

    say("");
    say("=== C. 线程池的三个经典坑 ===");
    say("  坑 1：任务里再 submit 并等待结果 -> 可能自己把自己饿死（死锁）。");
    say("        工人全都在等子任务，而队列里的子任务没人执行。");
    say("  坑 2：析构时直接退出，不等队列里的活干完 -> 任务丢失。");
    say("        本例的 workerLoop 是\"先把队列清空再退出\"。");
    say("  坑 3：任务抛异常没接住 -> 异常逃出 workerLoop -> 整个进程 terminate。");
    say("        生产代码里要在 job() 外面套 try/catch（见第 18 章 ex14）。");

    say("");
    say("=== D. 相关设施 ===");
    say("  std::packaged_task : 把\"函数+参数\"打包成能被队列存的对象，结果通过 future 取。");
    say("  std::function<void()> : 上面用的类型擦除容器，能装任意可调用对象（有轻微开销）。");
    say("  嵌入式：Cortex-M0+ 没有 OS、没有 MMU、RAM 8KB，");
    say("          线程池/多线程整体不可用，通常改用\"主循环 + 状态机 + 中断\"。");

    say("");
    say("main 正常结束");
    return 0;
}
