// ch19_ex06_atomic_memory_order.cpp —— std::atomic 与内存序 memory_order
// 编译: check.bat ch19_ex06_atomic_memory_order.cpp c++latest
#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
#include <string>
#include <mutex>
#include <chrono>

std::mutex g_ioMutex;
void say(const std::string& s)
{
    std::lock_guard<std::mutex> lk(g_ioMutex);
    std::cout << s << "\n";
}

// ---------- 1) atomic 基本用法：各种读改写 ----------
void atomicBasics()
{
    std::atomic<int> a{0};
    a.store(10);                          // 写
    say("  store(10) 后 a = " + std::to_string(a.load()));
    say("  a.fetch_add(5) 的返回值 = " + std::to_string(a.fetch_add(5))
        + "（返回旧值）");
    say("  现在 a = " + std::to_string(a.load()));
    say("  ++a 之后 a = " + std::to_string(++a));
    int expected = 16;
    bool ok = a.compare_exchange_strong(expected, 99);   // CAS
    say(std::string("  CAS(16 -> 99) 成功? ") + (ok ? "是" : "否")
        + ", a = " + std::to_string(a.load()));
    expected = 16;
    ok = a.compare_exchange_strong(expected, 0);          // 这次会失败
    say(std::string("  CAS(16 -> 0)  成功? ") + (ok ? "是" : "否")
        + ", a = " + std::to_string(a.load())
        + ", expected 被改成了 " + std::to_string(expected));
}

// ---------- 2) atomic 和"非 atomic 的可见性"：release/acquire ----------
int g_data = 0;                           // 普通变量，不是 atomic
std::atomic<bool> g_ready{false};

void producerRelAcq()
{
    g_data = 42;                                        // ① 先写数据
    g_ready.store(true, std::memory_order_release);      // ② release：保证①在②之前可见
}

void consumerRelAcq()
{
    while (!g_ready.load(std::memory_order_acquire)) {   // ③ acquire
        std::this_thread::yield();
    }
    say("  [release/acquire] g_data = " + std::to_string(g_data)
        + "   <- 一定是 42，不会读到 0");
}

// ---------- 3) relaxed：只保证自身原子，不保证别的变量可见 ----------
std::atomic<int> g_counterRelaxed{0};
std::atomic<int> g_counterSeq{0};

void relaxedCounter(int n)
{
    for (int i = 0; i < n; ++i) g_counterRelaxed.fetch_add(1, std::memory_order_relaxed);
}
void seqCounter(int n)
{
    for (int i = 0; i < n; ++i) g_counterSeq.fetch_add(1, std::memory_order_seq_cst);
}

// ---------- 4) seq_cst 的 total order：两线程互相观察 ----------
std::atomic<bool> g_x{false}, g_y{false};
std::atomic<int> g_reorderSeen{0};

void seqCstDemo()
{
    g_x.store(false);                 // 每次演示前复位，否则计数会累加
    g_y.store(false);
    g_reorderSeen.store(0);
    std::thread t1([] {
        g_x.store(true);                    // 默认 seq_cst
        if (g_y.load()) g_reorderSeen.fetch_add(1);
    });
    std::thread t2([] {
        g_y.store(true);
        if (g_x.load()) g_reorderSeen.fetch_add(1);
    });
    t1.join(); t2.join();
    int seen = g_reorderSeen.load();
    say("  本轮观察到\"读到对方标志\"的次数 = " + std::to_string(seen)
        + (seen >= 1 ? "  (符合预期)" : "  (异常！)"));
}

// ---------- 5) 用 atomic 做无锁自旋锁（看清 atomic 的边界）----------
class SpinLock {
public:
    void lock()
    {
                // 拿不到就一直试。exchange 返回旧值，旧值是 false 说明我抢到了。
        while (flag_.exchange(true, std::memory_order_acquire)) {
                        // 自旋
        }
    }
    void unlock() { flag_.store(false, std::memory_order_release); }
private:
    std::atomic<bool> flag_{false};
};

SpinLock g_spin;
long long g_spinCount = 0;

int main()
{
    say("=== A. std::atomic 的基本操作 ===");
    atomicBasics();

    say("");
    say("=== B. release/acquire：让普通变量也\"安全地传过去\" ===");
    {
        std::thread c(consumerRelAcq), p(producerRelAcq);
        c.join(); p.join();
    }
    say("  要点：atomic 本身只保证\"这一个变量\"不可分割；");
    say("        要让别的普通变量也跟着可见，必须用 release/acquire 配对。");

    say("");
    say("=== C. relaxed vs seq_cst：都正确，只是强弱不同 ===");
    {
        const int n = 200000;
        auto t0 = std::chrono::steady_clock::now();
        {
            std::vector<std::thread> ts;
            for (int i = 0; i < 4; ++i) ts.emplace_back(relaxedCounter, n);
            for (auto& t : ts) t.join();
        }
        auto t1 = std::chrono::steady_clock::now();
        {
            std::vector<std::thread> ts;
            for (int i = 0; i < 4; ++i) ts.emplace_back(seqCounter, n);
            for (auto& t : ts) t.join();
        }
        auto t2 = std::chrono::steady_clock::now();
        say("  relaxed  结果 = " + std::to_string(g_counterRelaxed.load())
            + ", 用时 " + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count()) + " ms");
        say("  seq_cst  结果 = " + std::to_string(g_counterSeq.load())
            + ", 用时 " + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()) + " ms");
        say("  两个结果都等于 " + std::to_string(4 * n) + " —— 单纯计数用哪个都对。");
        say("  （具体谁快取决于机器和缓存争用，本机这次运行见上面的毫秒数；");
        say("    不要凭\"relaxed 一定更快\"下结论，未实测的部分不要照抄结论。）");
    }

    say("");
    say("=== D. seq_cst 提供全局唯一顺序 ===");
    say("  两个线程各自\"先置自己的标志，再读对方的标志\"：");
    say("  在 seq_cst 下，两个 store 之间一定有一个排在前面，");
    say("  所以至少有一个线程能读到对方的 true —— 次数必然是 1 或 2，不可能是 0。");
    for (int i = 0; i < 3; ++i) seqCstDemo();

    say("");
    say("=== E. 用 atomic 手搓自旋锁（理解 atomic 的边界）===");
    {
        std::vector<std::thread> ts;
        for (int i = 0; i < 4; ++i) {
            ts.emplace_back([] {
                for (int k = 0; k < 100000; ++k) {
                    std::lock_guard<SpinLock> lk(g_spin);
                    ++g_spinCount;
                }
            });
        }
        for (auto& t : ts) t.join();
    }
    say("  g_spinCount = " + std::to_string(g_spinCount) + "（期望 400000）");
    say("  spinlock 在单核或线程数>核数时非常浪费 CPU —— 因为它在**烧**时间而不是睡觉。");
    say("  一般用 std::mutex（内部会先自旋再睡眠）。");

    say("");
    say("=== F. 内存序选型速查 ===");
    say("  memory_order_relaxed : 只要原子性，不要顺序。用于纯计数。");
    say("  memory_order_acquire : 读侧。保证这之后的读写不被提到它前面。");
    say("  memory_order_release : 写侧。保证这之前的读写不被挪到它后面。");
    say("  memory_order_acq_rel : 读改写操作（如 fetch_add）用。");
    say("  memory_order_seq_cst : 默认值，全局单一总顺序，最直观也最保守。");
    say("  经验：先用默认 seq_cst 写对；确认性能瓶颈后再降级，并且只降到 release/acquire。");
    say("  在 x86 上 acquire/release 基本没有额外指令（x86 本就强顺序），");
    say("  所以\"降级能提速\"在 x86 上通常看不到效果（未实测其它架构）。");

    say("");
    say("main 正常结束");
    return 0;
}
