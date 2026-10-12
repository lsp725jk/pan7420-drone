// ch19_ex08_call_once.cpp —— std::call_once / std::once_flag：只初始化一次
// 编译: check.bat ch19_ex08_call_once.cpp c++latest
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <string>
#include <chrono>
#include <atomic>

std::mutex g_ioMutex;
void say(const std::string& s)
{
    std::lock_guard<std::mutex> lk(g_ioMutex);
    std::cout << s << "\n";
}

// ============================================================
// A. 单例：用 call_once 保证"只构造一次"
// ============================================================
class Config {
public:
    static Config& instance()
    {
        std::call_once(initFlag_, [] {
            instance_.reset(new Config());          // 只有第一个到达的线程会执行
            say("    >>> Config 真正被构造了（只会出现一次）");
        });
        return *instance_;
    }
    int value() const { return value_; }
    void setValue(int v) { value_ = v; }

private:
    Config() : value_(42) {}
    static std::once_flag initFlag_;
    static std::unique_ptr<Config> instance_;
    int value_;
};
std::once_flag Config::initFlag_;
std::unique_ptr<Config> Config::instance_;

// ============================================================
// B. 反例：用"检查-然后-初始化"自己实现（double-checked locking）
// ============================================================
std::atomic<int> g_ctorCount{0};
struct NaiveSingleton {
    static NaiveSingleton* instance_;
    static NaiveSingleton* get()
    {
        if (!instance_) {                        // ← 两个线程可能同时通过这个判断
            std::this_thread::sleep_for(std::chrono::milliseconds(1));   // 放大竞态窗口
            instance_ = new NaiveSingleton();     // 于是构造两次，还泄漏一个
        }
        return instance_;
    }
    NaiveSingleton() { g_ctorCount.fetch_add(1); }
};
NaiveSingleton* NaiveSingleton::instance_ = nullptr;

// ============================================================
// C. 最省事的替代：函数内静态局部变量（C++11 起保证线程安全）
// ============================================================
std::atomic<int> g_meyersCount{0};
struct MeyersSingleton {
    static MeyersSingleton& get()
    {
        static MeyersSingleton inst;              // C++11 起：magic static，编译器负责加锁
        return inst;
    }
    MeyersSingleton() { g_meyersCount.fetch_add(1); }
};

// ============================================================
// D. call_once 保证"恰好一次"，即使前一次抛了异常
// ============================================================
std::once_flag g_retryFlag;
std::atomic<int> g_attempt{0};

void mayFailInit()
{
    int n = g_attempt.fetch_add(1) + 1;
    say("    尝试初始化第 " + std::to_string(n) + " 次");
    if (n == 1) throw std::runtime_error("第一次初始化故意失败");
    say("    初始化成功");
}

int main()
{
    say("=== A. call_once：8 个线程同时抢，只构造一次 ===");
    {
        std::vector<std::thread> ts;
        for (int i = 0; i < 8; ++i) {
            ts.emplace_back([] {
                int v = Config::instance().value();
                (void)v;
            });
        }
        for (auto& t : ts) t.join();
        say("  Config::instance().value() = " + std::to_string(Config::instance().value()));
    }

    say("");
    say("=== B. 反例：自己写 if (!instance_) new ... 会构造多次 ===");
    {
        std::vector<std::thread> ts;
        for (int i = 0; i < 8; ++i) ts.emplace_back([] { NaiveSingleton::get(); });
        for (auto& t : ts) t.join();
        say("  构造函数被调用了 " + std::to_string(g_ctorCount.load())
            + " 次（期望 1 次）   <- 多出来的对象全部泄漏");
        say("  这就是经典的 double-checked locking 陷阱：");
        say("    `if (!p) p = new T;` 里\"判断\"和\"赋值\"不是一个原子操作。");
        say("    修法：① call_once（本例 A）；② 函数内 static（本例 C）；");
        say("          ③ 用 std::atomic + CAS，但要配合正确的内存序，很难写对。");
    }

    say("");
    say("=== C. Meyers singleton：一行搞定，还线程安全 ===");
    {
        std::vector<std::thread> ts;
        for (int i = 0; i < 8; ++i) ts.emplace_back([] { MeyersSingleton::get(); });
        for (auto& t : ts) t.join();
        say("  构造函数被调用了 " + std::to_string(g_meyersCount.load())
            + " 次（期望 1 次）   <- C++11 保证");
        say("  代价：如果构造抛异常，下次调用会重试；这点和 call_once 一样。");
        say("        嵌入式注意：magic static 需要编译器插入 guard 变量和加锁调用，");
        say("        在 -fno-threadsafe-statics 或裸机上可能退化，详见第 21 章。");
    }

    say("");
    say("=== D. call_once 与异常：抛异常的这次算\"没成功\"，下次会重试 ===");
    {
        for (int round = 1; round <= 2; ++round) {
            try {
                std::call_once(g_retryFlag, mayFailInit);
                say("  第 " + std::to_string(round) + " 轮 call_once 返回（说明已成功）");
            }
            catch (const std::exception& e) {
                say(std::string("  第 ") + std::to_string(round) + " 轮抛出: " + e.what());
                say("          注意：一旦 call_once 成功返回过，后面再调用就**直接跳过**，" );
                say("                不会再执行函数体，也不会再抛异常。");
            }
        }
        say("");
        say("  第 3 轮再调一次，看是否还执行：");
        std::call_once(g_retryFlag, mayFailInit);
        say("  （上面没有打印\"尝试初始化\"，说明函数体被跳过了）");
    }

    say("");
    say("=== E. 什么时候真的需要 call_once ===");
    say("  1. 全局资源的一次性初始化（日志、配置、硬件外设）；");
    say("  2. 懒惰初始化，且初始化过程可能失败需要重试；");
    say("  3. 想避免\"函数内 static\"带来的析构顺序问题（call_once 不管析构）。");
    say("  如果只是想要一个单例，优先用函数内 static，代码最短。");

    say("");
    say("main 正常结束");
    return 0;
}
