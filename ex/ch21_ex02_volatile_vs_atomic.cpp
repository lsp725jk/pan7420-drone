// ch21_ex02_volatile_vs_atomic.cpp —— volatile 在 C++ 里更容易写错
// 编译: check.bat ch21_ex02_volatile_vs_atomic.cpp c++latest
// 说明：PC 上没有真中断，这里用线程模拟"ISR 随时改这个变量"。
#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>
#include <vector>
#include <mutex>

std::mutex g_ioMutex;
void say(const std::string& s)
{
    std::lock_guard<std::mutex> lk(g_ioMutex);
    std::cout << s << "\n";
}

// ============================================================
// volatile 的"正确用途"：告诉编译器"这个值可能被程序之外的东西改"
// 典型场景 = 硬件寄存器 / ISR 修改的、主循环只读不共享写 的标志
// ============================================================
volatile int  g_volatileFlag = 0;      // volatile：每次访问都真的去读内存
int           g_plainFlag    = 0;      // 普通：编译器可以把它缓存进寄存器

// volatile 在"编译器优化掉重复读"这件事上的效果，用汇编角度解释：
int loopVolatile(int n)
{
    int sum = 0;
    for (int i = 0; i < n; ++i) sum += g_volatileFlag;   // 每次循环都重新读内存
    return sum;
}
int loopPlain(int n)
{
    int sum = 0;
    for (int i = 0; i < n; ++i) sum += g_plainFlag;      // 编译器可能只读一次
    return sum;
}

// ============================================================
// volatile 不保证原子性：经典反例
// ============================================================
volatile long long g_volatileCounter = 0;       // ✘ 用它做多线程计数
std::atomic<long long> g_atomicCounter{0};      // ✔ 正确工具

// ============================================================
// volatile 也不提供内存屏障（memory barrier）
// ============================================================
volatile int g_data = 0;
volatile int g_ready = 0;

int main()
{
    std::cout << "=== A. volatile 到底做了什么 ===\n";
    std::cout << "  loopVolatile(5) = " << loopVolatile(5) << "\n";
    std::cout << "  loopPlain(5)    = " << loopPlain(5) << "\n";
    std::cout << "  值一样，但生成的指令不同：\n";
    std::cout << "    volatile 版：循环体里每次都 LDR 从内存读（因为值可能被改）；\n";
    std::cout << "    普通版    ：编译器可能只读一次到寄存器，后面复用。\n";
    std::cout << "  这就是 volatile 的**唯一**语义：不许优化掉这次访问。\n";

    std::cout << "\n=== B. volatile 不保证原子性（最重要的坑）===\n";
    std::cout << "  用 volatile long long 让 4 个线程各加 200000 次：\n";
    {
        std::vector<std::thread> ts;
        for (int i = 0; i < 4; ++i) {
            ts.emplace_back([] {
                for (int k = 0; k < 200000; ++k) ++g_volatileCounter;
            });
        }
        for (auto& t : ts) t.join();
    }
    say("  volatile 版结果 = " + std::to_string(g_volatileCounter)
        + "（期望 800000）");
    say("    ^^^ 错了！volatile 只保证\"不优化掉访问\"，不保证\"读-改-写不可分割\"。");
    say("        long long 在 32 位机上要两条指令读写，中间可能被别的线程插进来。");
    say("        更糟的是：这是 **数据竞争 = UB（未定义行为）**，");
    say("        编译器甚至可以把它优化成别的样子。");

    std::cout << "\n";
    {
        std::vector<std::thread> ts;
        for (int i = 0; i < 4; ++i) {
            ts.emplace_back([] {
                for (int k = 0; k < 200000; ++k) g_atomicCounter.fetch_add(1, std::memory_order_relaxed);
            });
        }
        for (auto& t : ts) t.join();
    }
    say("  atomic 版结果   = " + std::to_string(g_atomicCounter.load())
        + "（期望 800000）  <- 正确");

    std::cout << "\n=== C. volatile 不提供内存屏障 ===\n";
    std::cout << "  常见错误写法（ISR 里）：\n";
    std::cout << "      g_data = 42;                 // volatile int\n";
    std::cout << "      g_ready = 1;                 // volatile int 当作\"数据已就绪\"\n";
    std::cout << "  问题：编译器/CPU 可以把这两句换序，主循环可能先看到 ready=1\n";
    std::cout << "        再看到 data 还是 0。volatile 阻止不了这种重排。\n";
    std::cout << "  正确写法（C++11 起）：\n";
    std::cout << "      g_data = 42;\n";
    std::cout << "      g_ready.store(1, std::memory_order_release);   // 屏障\n";
    std::cout << "     （另一个线程用 memory_order_acquire 读 g_ready）\n";
    std::cout << "  详见第 19 章 ex06 的 release/acquire 实测。\n";

    std::cout << "\n=== D. 那什么时候真的该用 volatile ===\n";
    std::cout << "  ✔ 硬件寄存器映射：\n";
    std::cout << "       #define GPIOA_ODR (*(volatile uint32_t*)0x50000014u)\n";
    std::cout << "       这里的 volatile 是给编译器的必要信息（读寄存器有副作用），\n";
    std::cout << "       跟线程安全无关，永远要写。\n";
    std::cout << "  ✔ 被 ISR 写、主循环只读的简单标志：\n";
    std::cout << "       volatile uint8_t g_tickFlag;   // ISR 置 1，主循环读到就清 0\n";
    std::cout << "       单写者单读者、且是单字节（M0+ 上 8/16/32 位对齐访问本身原子），\n";
    std::cout << "       这时 volatile 够用。\n";
    std::cout << "  ✘ 多写者计数器、多字段一致性 -> 必须原子的场景，用 std::atomic\n";
    std::cout << "      或关中断/临界区。\n";

    std::cout << "\n=== E. volatile 和 std::atomic 的区别（背这张表）===\n";
    std::cout << "  问题                     volatile    std::atomic\n";
    std::cout << "  ------------------------------------------------\n";
    std::cout << "  禁止优化掉访问             ✔           ✔\n";
    std::cout << "  读-改-写不可分割           ✘           ✔\n";
    std::cout << "  提供内存屏障               ✘           ✔（可指定 memory_order）\n";
    std::cout << "  保证不产生数据竞争         ✘           ✔\n";
    std::cout << "  能修饰硬件寄存器           ✔           ✘（atomic 可能被优化成非访存）\n";
    std::cout << "  零运行时代价               ✔           ✔（无锁时就是普通指令）\n";
    std::cout << "  结论：硬件寄存器用 volatile；线程/ISR 共享变量用 atomic；\n";
    std::cout << "        不要用 volatile 假装 atomic。\n";

    std::cout << "\n=== F. 嵌入式专用：volatile + atomic 同时用 ===\n";
    std::cout << "  有些驱动要两者兼得（既不能被优化掉，又要原子）：\n";
    std::cout << "      volatile std::atomic<uint32_t> reg;\n";
    std::cout << "  标准允许，但要清楚：volatile 在这里只影响\"不要省略访问\"，\n";
    std::cout << "  原子性由 atomic 提供。（细节行为随编译器实现，未实测其它平台）\n";

    std::cout << "\nmain 正常结束\n";
    return 0;
}
