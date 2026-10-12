// ch19_ex03_mutex_races.cpp —— mutex / lock_guard / unique_lock / scoped_lock，以及数据竞争
// 编译: check.bat ch19_ex03_mutex_races.cpp c++latest
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <string>
#include <atomic>

std::mutex g_ioMutex;
void say(const std::string& s)
{
    std::lock_guard<std::mutex> lk(g_ioMutex);
    std::cout << s << "\n";
}

// ================= 反例 1：完全不加锁 -> 数据竞争 =================
long long g_unlocked = 0;
void addUnlocked(int n)
{
    for (int i = 0; i < n; ++i) ++g_unlocked;     // 读-改-写，三步不是原子的
}

// ================= 正例 1：mutex + lock_guard =================
long long g_locked = 0;
std::mutex g_mtx;
void addLocked(int n)
{
    for (int i = 0; i < n; ++i) {
        std::lock_guard<std::mutex> lk(g_mtx);    // 构造即加锁，析构即解锁
        ++g_locked;
    }
}

// ================= 正例 2：锁粒度大一点（性能更好，仍然正确）=================
long long g_locked2 = 0;
std::mutex g_mtx2;
void addLockedBatch(int n)
{
    std::lock_guard<std::mutex> lk(g_mtx2);       // 整段循环只加一次锁
    for (int i = 0; i < n; ++i) ++g_locked2;
}

// ================= 正例 3：atomic，连锁都不需要 =================
std::atomic<long long> g_atomic{0};
void addAtomic(int n)
{
    for (int i = 0; i < n; ++i) g_atomic.fetch_add(1, std::memory_order_relaxed);
}

// ================= unique_lock：可以中途解锁 =================
std::mutex g_mtx3;
long long g_shared = 0;
void addWithUniqueLock(int n)
{
    for (int i = 0; i < n; ++i) {
        std::unique_lock<std::mutex> lk(g_mtx3);
        ++g_shared;
        lk.unlock();                              // lock_guard 做不到这一点
                // 这里可以做不需要保护的计算
    }
}

int main()
{
    const int perThread = 200000;
    const int threadCount = 4;

    say("=== A. 反例：不加锁累加（数据竞争 = UB）===");
    say("  每个线程加 " + std::to_string(perThread) + " 次，4 个线程，期望 = "
        + std::to_string((long long)perThread * threadCount));
    {
        std::vector<std::thread> ts;
        for (int i = 0; i < threadCount; ++i) ts.emplace_back(addUnlocked, perThread);
        for (auto& t : ts) t.join();
    }
    say("  实际 g_unlocked = " + std::to_string(g_unlocked)
        + "   <- 少了 " + std::to_string((long long)perThread * threadCount - g_unlocked)
        + " 次！这就是数据竞争");

    say("");
    say("=== B. 正例：lock_guard 每次循环都加解锁 ===");
    {
        std::vector<std::thread> ts;
        for (int i = 0; i < threadCount; ++i) ts.emplace_back(addLocked, perThread);
        for (auto& t : ts) t.join();
    }
    say("  g_locked = " + std::to_string(g_locked) + "   <- 每次都对");

    say("");
    say("=== C. 正例：把锁拿到循环外面（粒度更大，快得多）===");
    {
        std::vector<std::thread> ts;
        for (int i = 0; i < threadCount; ++i) ts.emplace_back(addLockedBatch, perThread);
        for (auto& t : ts) t.join();
    }
    say("  g_locked2 = " + std::to_string(g_locked2) + "   <- 结果一样，但锁竞争少了 20 万倍");

    say("");
    say("=== D. 正例：std::atomic，不用锁也能正确 ===");
    {
        std::vector<std::thread> ts;
        for (int i = 0; i < threadCount; ++i) ts.emplace_back(addAtomic, perThread);
        for (auto& t : ts) t.join();
    }
    say("  g_atomic = " + std::to_string(g_atomic.load()) + "   <- 同样正确");
    say("  注意：atomic 只保证\"这一步操作\"不可分割，不等于\"整段代码\"安全。");

    say("");
    say("=== E. unique_lock：同一把锁，中途解锁再加锁 ===");
    {
        std::thread t(addWithUniqueLock, 100000);
        t.join();
    }
    say("  g_shared = " + std::to_string(g_shared));

    say("");
    say("=== F. lock_guard vs unique_lock vs scoped_lock 选型 ===");
    say("  lock_guard  : 最小最省；构造加锁、析构解锁，不能中途解锁。默认选它。");
    say("  unique_lock : 更重（多一个 bool 成员），但能 unlock/lock、能配条件变量、能移动。");
    say("  scoped_lock : C++17 起，一次锁多个 mutex 且内部用死锁避免算法（见下一例）。");
    say("  三者都是 RAII：离开作用域自动解锁，所以不要手写 mtx.lock()/unlock()。");

    say("");
    say("main 正常结束");
    return 0;
}
