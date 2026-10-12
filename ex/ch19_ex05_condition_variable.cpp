// ch19_ex05_condition_variable.cpp —— 条件变量：标准写法、虚假唤醒、丢通知
// 编译: check.bat ch19_ex05_condition_variable.cpp c++latest
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>
#include <vector>
#include <chrono>
#include <atomic>

std::mutex g_ioMutex;
void say(const std::string& s)
{
    std::lock_guard<std::mutex> lk(g_ioMutex);
    std::cout << s << "\n";
}

// ============================================================
// A. 正确写法：condition_variable + 谓词（有界缓冲区）
// ============================================================
class BoundedBuffer {
public:
    explicit BoundedBuffer(std::size_t cap) : cap_(cap) {}

    void push(int v)
    {
        std::unique_lock<std::mutex> lk(m_);
                // 关键：wait 的第二个参数是"谓词"。它替你把 while 循环写好了。
                // 语义 = while (!pred()) wait(lk);
        notFull_.wait(lk, [this] { return buf_.size() < cap_; });
        buf_.push(v);
        lk.unlock();                 // 通知前解锁，减少被唤醒线程"又立刻阻塞"的概率
        notEmpty_.notify_one();
    }

    int pop()
    {
        std::unique_lock<std::mutex> lk(m_);
        notEmpty_.wait(lk, [this] { return !buf_.empty(); });
        int v = buf_.front();
        buf_.pop();
        lk.unlock();
        notFull_.notify_one();
        return v;
    }

private:
    std::mutex m_;
    std::condition_variable notFull_, notEmpty_;
    std::queue<int> buf_;
    std::size_t cap_;
};

// ============================================================
// B. 错误写法 1：只 wait，不判断谓词 —— 虚假唤醒 (spurious wakeup)
// ============================================================
std::mutex mWrong;
std::condition_variable cvWrong;
bool readyWrong = false;
std::atomic<int> g_wrongWakeCount{0};

void wrongWaiter()
{
    std::unique_lock<std::mutex> lk(mWrong);
    cvWrong.wait(lk);                     // ✘ 没有谓词
    g_wrongWakeCount.fetch_add(1);
        // 这里会直接使用 readyWrong，但如果上面是虚假唤醒，readyWrong 还是 false
    say(std::string("  [错误写法] 被唤醒，此刻 readyWrong = ")
        + (readyWrong ? "true" : "false")
        + "   <- 如果是 false，说明被虚假唤醒骗了，接下来读数据就是错的");
}

// ============================================================
// C. 正确写法：用谓词挡住虚假唤醒
// ============================================================
std::mutex mRight;
std::condition_variable cvRight;
bool readyRight = false;

void rightWaiter()
{
    std::unique_lock<std::mutex> lk(mRight);
    cvRight.wait(lk, [] { return readyRight; });   // ✔ 醒来自动复查
    say("  [谓词写法] 醒来了，readyRight = true，可以安全取数据");
}

// ============================================================
// D. 丢通知：notify 发生在 wait 之前
// ============================================================
std::mutex mLost;
std::condition_variable cvLost;
bool readyLost = false;

void lostWaiter()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(400));   // 故意晚一点才 wait
    std::unique_lock<std::mutex> lk(mLost);
        // 用带超时的 wait 演示"通知已经飞走了，我还在等"
    bool ok = cvLost.wait_for(lk, std::chrono::milliseconds(500), [] { return readyLost; });
    say(std::string("  [丢通知] 等到超时才醒，等到值了吗？ ") + (ok ? "等到了" : "没有"));
    if (!ok) {
        say("          原因：通知在 wait 之前就发出去了，没人接收，信号直接丢失。");
        say("          修法：把\"条件是否成立\"放进共享变量（readyLost），");
        say("                并且 wait 必须带谓词 —— 这样即使是先通知后 wait 也不会丢。");
    }
}

int main()
{
    say("=== A. 正确写法：生产者-消费者 + 谓词 ===");
    {
        BoundedBuffer buf(2);              // 容量只有 2，生产者一定会被阻塞住
        std::vector<std::thread> ts;
        ts.emplace_back([&buf] {
            for (int i = 1; i <= 5; ++i) {
                buf.push(i * 10);
                say("  [生产者] push " + std::to_string(i * 10));
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
            }
        });
        ts.emplace_back([&buf] {
            for (int i = 0; i < 5; ++i) {
                int v = buf.pop();
                say("  [消费者] pop  " + std::to_string(v));
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        });
        for (auto& t : ts) t.join();
    }
    say("  两边都正常结束：生产者被容量 2 挡住过，消费者也等过数据，");
    say("  全程没有忙等（busy-wait），靠条件变量睡眠+唤醒。");

    say("");
    say("=== B. 错误写法：cv.wait(lk) 不带谓词 ===");
    say("  这里人为模拟一次虚假唤醒：不设 ready，直接 notify 一次");
    {
        std::thread w(wrongWaiter);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        {
            std::lock_guard<std::mutex> lk(mWrong);
                        // 注意：故意不改 readyWrong，只 notify —— 等价于一次"虚假唤醒"
        }
        cvWrong.notify_one();
        w.join();
    }
    say("  结论：没有谓词的 wait，醒来时**不能假设条件成立**。");
    say("  C++ 标准明确允许 condition_variable 在没有 notify 的情况下自己醒来。");

    say("");
    say("=== C. 正确写法：wait 带谓词，醒来后自动复查 ===");
    {
        std::thread w(rightWaiter);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        { std::lock_guard<std::mutex> lk(mRight); readyRight = true; }
        cvRight.notify_one();
        w.join();
    }

    say("");
    say("=== D. 丢通知：notify 早于 wait ===");
    {
        { std::lock_guard<std::mutex> lk(mLost); readyLost = false; }
        say("  main: 先 notify 一次（此时还没有人在 wait）");
        cvLost.notify_all();               // 通知飞出去，无人接收
        std::thread w(lostWaiter);         // 它 400ms 之后才开始 wait
        w.join();
    }

    say("");
    say("=== E. 条件变量的三条铁律 ===");
    say("  1. 共享状态必须由同一把 mutex 保护（wait 要传这把锁）");
    say("  2. wait 必须带谓词，或手写 while (!pred()) cv.wait(lk);");
    say("     —— 为了对付虚假唤醒，以及\"醒来时条件又被别人抢走了\"");
    say("  3. 通知前可以先 unlock（性能），但绝不能在没持锁时改共享变量");
    say("  常见错误还有：notify_one 唤醒了错误的线程（该用 notify_all）、");
    say("                持锁时做耗时工作导致 notify 迟迟发不出去。");

    say("");
    say("main 正常结束");
    return 0;
}
