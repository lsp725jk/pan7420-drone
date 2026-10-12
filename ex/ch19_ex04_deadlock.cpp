// ch19_ex04_deadlock.cpp —— 死锁：两个 mutex 反序加锁；用超时演示"卡住"
// 编译: check.bat ch19_ex04_deadlock.cpp c++latest
// 说明：本程序不会挂死 —— 死锁发生在一个 detach 出去的线程里，
// 主线程用 try_lock_for 观察到"锁拿不到"，然后正常退出。
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <string>
#include <atomic>

std::mutex g_ioMutex;
void say(const std::string& s)
{
    std::lock_guard<std::mutex> lk(g_ioMutex);
    std::cout << s << "\n";
}

// ================= 案例 1：两把锁，加锁顺序相反 -> 死锁 =================
std::mutex mA, mB;
std::atomic<bool> g_shouldStop{false};

void threadAB()
{
    say("  [线程AB] 先锁 A");
    std::lock_guard<std::mutex> la(mA);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));   // 给对手留出插队时间
    say("  [线程AB] 再锁 B ... 如果卡住，就说明死锁了");
    std::lock_guard<std::mutex> lb(mB);                            // ← 可能永远拿不到
    say("  [线程AB] 两把锁都拿到了（正常时才会打印）");
}

void threadBA()
{
    say("  [线程BA] 先锁 B");
    std::lock_guard<std::mutex> lb(mB);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    say("  [线程BA] 再锁 A ... 如果卡住，就说明死锁了");
    std::lock_guard<std::mutex> la(mA);                            // ← 可能永远拿不到
    say("  [线程BA] 两把锁都拿到了（正常时才会打印）");
}

// ================= 案例 2：用 try_lock_for 观察死锁（不阻塞自己）=================
// 注意：普通的 std::mutex 只有 try_lock()，没有超时版本；
// 想要"最多等 300ms"，必须用 std::timed_mutex。
std::timed_mutex mC, mD;
std::atomic<bool> g_deadlockReady{false};

void holdCThenD()
{
    std::unique_lock<std::timed_mutex> lc(mC);
    g_deadlockReady = true;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    std::unique_lock<std::timed_mutex> ld(mD);    // 拿不到就永远等
    say("  [持有C的线程] 拿到了 D（不会打印）");
}

void holdDThenC()
{
    std::unique_lock<std::timed_mutex> ld(mD);
    g_deadlockReady = true;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    std::unique_lock<std::timed_mutex> lc(mC);    // 拿不到就永远等
    say("  [持有D的线程] 拿到了 C（不会打印）");
}

// ================= 案例 3：std::scoped_lock 自动避免死锁 =================
std::mutex mE, mF;
void safeAB()
{
    for (int i = 0; i < 3; ++i) {
        std::scoped_lock lk(mE, mF);          // 一次锁两把，内部用死锁避免算法
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    say("  [安全线程AB] 3 轮都成功");
}
void safeBA()
{
    for (int i = 0; i < 3; ++i) {
        std::scoped_lock lk(mF, mE);          // 顺序相反也没关系
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    say("  [安全线程BA] 3 轮都成功");
}

// ================= 案例 4：std::lock + 手动管理（C++11 的老写法）=================
std::mutex mG, mH;
long long g_pairSum = 0;
void lockedPair()
{
    for (int i = 0; i < 3; ++i) {
        std::unique_lock<std::mutex> lg(mG, std::defer_lock);   // 先不锁
        std::unique_lock<std::mutex> lh(mH, std::defer_lock);
        std::lock(lg, lh);                                      // 一次性全锁上，内部避免死锁
        g_pairSum += i;
    }
    say("  [std::lock 线程] 完成");
}

int main()
{
    say("=== A. 死锁现场：两个线程反序加锁 ===");
    say("  把两个都 detach 出去，让它们在后台互相卡住（本进程不受影响）");
    std::thread(threadAB).detach();
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    std::thread(threadBA).detach();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    say("  main: 500ms 过去了。上面两个线程都停在\"再锁 X\"那一行，");
    say("        AB 等 B、BA 等 A —— 死锁成立，而且永远不会自己解开。");
    say("        （这两个线程被 detach 了，进程退出时直接抛弃，所以程序不会挂死）");

    say("");
    say("=== B. 用 try_lock_for 从外部\"探测\"死锁，而不是自己也卡死 ===");
    std::thread(holdCThenD).detach();
    std::thread(holdDThenC).detach();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));   // 等它们各自拿到第一把锁
    {
                // 主线程尝试拿 C，超时 300ms
        std::unique_lock<std::timed_mutex> probeC(mC, std::defer_lock);
        bool gotC = probeC.try_lock_for(std::chrono::milliseconds(300));
        say(std::string("  探测 mC: ") + (gotC ? "拿到了（没死锁）" : "300ms 拿不到 -> 疑似死锁"));
        if (gotC) probeC.unlock();

        std::unique_lock<std::timed_mutex> probeD(mD, std::defer_lock);
        bool gotD = probeD.try_lock_for(std::chrono::milliseconds(300));
        say(std::string("  探测 mD: ") + (gotD ? "拿到了（没死锁）" : "300ms 拿不到 -> 疑似死锁"));
        if (gotD) probeD.unlock();
    }

    say("");
    say("=== C. 修法 1：std::scoped_lock 一次锁多把（推荐）===");
    {
        std::thread a(safeAB), b(safeBA);
        a.join(); b.join();
    }
    say("  两边加锁顺序相反也没死锁 —— 因为 scoped_lock 内部避免了循环等待");

    say("");
    say("=== D. 修法 2：std::lock + defer_lock（C++11 写法）===");
    {
        std::thread t(lockedPair);
        t.join();
    }
    say("  g_pairSum = " + std::to_string(g_pairSum));

    say("");
    say("=== E. 死锁的四个必要条件（背下来，破一个就不会死锁）===");
    say("  1. 互斥      : 资源同一时刻只能被一个线程持有（mutex 天生满足）");
    say("  2. 持有并等待: 拿着 A 的同时去等 B");
    say("  3. 不可抢占  : 别人拿着 B，你只能等，不能抢");
    say("  4. 循环等待  : A 等 B，B 等 A");
    say("  工程上的破法：全局规定加锁顺序（破 4）、用 scoped_lock/std::lock（破 2+4）、");
    say("                用 try_lock + 退避（破 3）、把临界区缩小到不需要第二把锁（破 2）。");

    say("");
    say("main 正常结束（能走到这里，说明死锁被隔离在后台线程里）");
    return 0;
}
