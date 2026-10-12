// ch21_ex06_isr_externC.cpp —— 中断与 C++：extern "C"、ISR 不能抛异常、ISR 安全的数据传递
// 编译: check.bat ch21_ex06_isr_externC.cpp c++latest
// 说明：PC 上没有真中断，这里用线程模拟"随时打断主流程的 ISR"。
// 结论对着 PY32F030（Cortex-M0+，无 OS）说。
#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <mutex>

std::mutex g_ioMutex;
void say(const std::string& s)
{
    std::lock_guard<std::mutex> lk(g_ioMutex);
    std::cout << s << "\n";
}

// ============================================================
// 1. extern "C"：给中断向量表 / C 代码用的函数必须去掉名字修饰
// ============================================================
extern "C" {

// 这些函数会被启动文件里的中断向量表引用，名字必须是原样的 C 名字。
// C++ 有名字修饰（name mangling）：void SysTick_Handler() 编译后可能变成
// ?SysTick_Handler@@YAXXZ 之类，链接器就找不到它了。
volatile std::uint32_t g_tickCount = 0;     // ISR 里累加，主循环读

void SysTick_Handler(void)                  // 名字必须和启动文件里一致
{
    ++g_tickCount;
}

}   // extern "C"

// C++ 内部函数：正常 C++ 链接，可以重载、可以放命名空间
namespace fc {
    int readTick() { return static_cast<int>(g_tickCount); }
}

// ============================================================
// 2. ISR 安全的"单向标志"：ISR 只写，主循环只读
// ============================================================
// 注意：C++ 标准里 std::atomic 的成员函数不保证信号/中断安全，
// 但在 Cortex-M 这种单核、无重入的中断模型下，
// 无锁（lock-free）的原子读写实际就是一条 LDR/STR，是安全且常用的做法。
// 这里用 C 风格的 volatile 标志，避免标准措辞上的争议。
volatile std::uint8_t  g_rxByte = 0;        // ISR 写入的字节
volatile std::uint8_t  g_rxReady = 0;       // ISR 置 1，主循环读后清 0

extern "C" void USART1_IRQHandler_Callback(std::uint8_t byte)   // 模拟 ISR
{
    g_rxByte = byte;
    g_rxReady = 1;                          // 顺序不能反！先写数据再置标志
}

// ============================================================
// 3. 环形缓冲区：ISR 与主循环之间最可靠的通道
// 规则：ISR 只写 head，主循环只写 tail -> 无锁、无阻塞
// ============================================================
template <std::size_t N>
class RingBuffer {
public:
    static_assert((N & (N - 1)) == 0, "N 必须是 2 的幂，这样取模可以用位与");
    static_assert(N <= 256, "嵌入式环形缓冲别开太大");

        // 只能由 ISR 调用
    void pushFromIsr(std::uint8_t b)
    {
        const std::size_t next = (head_ + 1) & (N - 1);
        if (next == tail_) return;          // 满了：丢数据（ISR 里绝不能等！）
        buf_[head_] = b;
        head_ = next;                       // 最后才更新 head（发布）
    }

        // 只能由主循环调用
    bool popFromMain(std::uint8_t& out)
    {
        if (tail_ == head_) return false;   // 空
        out = buf_[tail_];
        tail_ = (tail_ + 1) & (N - 1);
        return true;
    }

    std::size_t sizeFromMain() const { return (head_ - tail_) & (N - 1); }

private:
    std::uint8_t  buf_[N]{};
    volatile std::size_t head_ = 0;         // ISR 写
    volatile std::size_t tail_ = 0;         // 主循环写
};

RingBuffer<8> g_rxRing;

// ============================================================
// 4. ISR 里绝对不能做的事（清单）
// ============================================================
void printIsrRules()
{
    std::cout << "  ISR 里**绝对不能**做的事：\n";
    std::cout << "    ✘ 抛异常：没有 catch 上下文，直接 terminate / HardFault；\n";
    std::cout << "       而且异常展开需要栈和查表，M0+ 上根本来不及。\n";
    std::cout << "    ✘ new / delete、malloc / free：堆操作不可重入，会破坏堆结构；\n";
    std::cout << "    ✘ printf / std::cout：内部有锁和缓冲，非重入，会死锁或乱码；\n";
    std::cout << "    ✘ 任何阻塞等待（delay、等信号量、等互斥量）：会拖死整个系统；\n";
    std::cout << "    ✘ 浮点运算（M0+ 无 FPU，软件浮点会保存一堆寄存器，慢且占栈）；\n";
    std::cout << "    ✘ 长循环：中断响应延迟（latency）会被拉长，破坏控制周期。\n";
    std::cout << "  ISR 里**应该**做的：\n";
    std::cout << "    ✔ 读/写硬件寄存器（用 volatile）\n";
    std::cout << "    ✔ 把数据塞进环形缓冲、置一个标志\n";
    std::cout << "    ✔ 计数（用无符号，注意回绕）\n";
    std::cout << "    ✔ 尽快返回\n";
}

int main()
{
    std::cout << "=== A. extern \"C\" 为什么必须写 ===\n";
    std::cout << "  中断向量表（startup_py32f0xx.s）里写的是：\n";
    std::cout << "      .word SysTick_Handler\n";
    std::cout << "  汇编器/链接器看到的是**纯 C 名字** SysTick_Handler。\n";
    std::cout << "  如果 C++ 文件里不写 extern \"C\"，编译器会做名字修饰\n";
    std::cout << "  （name mangling），生成类似 ?SysTick_Handler@@YAXXZ 的符号，\n";
    std::cout << "  于是链接器报 undefined reference to `SysTick_Handler'，\n";
    std::cout << "  或者更糟：向量表里指向 0，一进中断就 HardFault。\n";
    std::cout << "  规则：凡是\"被 C 代码或启动文件按名字引用\"的函数，都加 extern \"C\"。\n";
    std::cout << "  同理，调用 HAL 库（C 写的）也要在头文件外裹 extern \"C\" { }。\n";

    std::cout << "\n=== B. 模拟 SysTick 中断：ISR 只累加，主循环只读 ===\n";
    {
        std::thread isr([] {
            for (int i = 0; i < 5; ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
                SysTick_Handler();
            }
        });
        for (int i = 0; i < 5; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(25));
            say("  主循环读到 g_tickCount = " + std::to_string(fc::readTick()));
        }
        isr.join();
        say("  最终 tick = " + std::to_string(fc::readTick()) + "（期望 5）");
        say("  单向读写 + volatile：不需要加锁，也不会撕裂（32 位对齐访问原子）。");
    }

    std::cout << "\n=== C. 单向标志：ISR 写数据、置标志；主循环见标志才读 ===\n";
    {
        std::thread isr([] {
            for (std::uint8_t b = 'A'; b <= 'E'; ++b) {
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
                USART1_IRQHandler_Callback(b);
            }
        });
        int got = 0;
        while (got < 5) {
            if (g_rxReady) {
                std::uint8_t b = g_rxByte;      // 先读数据
                g_rxReady = 0;                  // 再清标志
                say(std::string("  主循环收到字节: ") + static_cast<char>(b));
                ++got;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        isr.join();
        say("  关键：ISR 里\"先写数据，最后置标志\"；主循环\"先读数据，最后清标志\"。");
        say("        顺序写反会读到半个数据 —— 这是最难查的偶发 bug 之一。");
    }

    std::cout << "\n=== D. 环形缓冲区：多字节也不丢（代价 = N 字节 RAM）===\n";
    {
        for (std::uint8_t b : {std::uint8_t(1), std::uint8_t(2), std::uint8_t(3),
                               std::uint8_t(4), std::uint8_t(5)}) {
            g_rxRing.pushFromIsr(b);
        }
        say("  ISR 塞进 5 个字节，主循环取出：");
        std::uint8_t b;
        std::string line = "    ";
        while (g_rxRing.popFromMain(b)) line += std::to_string(b) + " ";
        say(line);
        say("  sizeof(RingBuffer<8>) = " + std::to_string(sizeof(g_rxRing))
            + " 字节（8 数据 + 2 个 volatile size_t 索引）");
        say("  在 8KB RAM 的 PY32F030 上，这样一条 64 字节的串口缓冲完全划算。");
        say("  如果缓冲区满了，pushFromIsr 选择\"丢最新的\"而不是等待 ——");
        say("  在 ISR 里等待会直接毁掉实时性。丢弃策略要写进注释/文档。");
    }

    std::cout << "\n=== E. ISR 里不能做的事 ===\n";
    printIsrRules();

    std::cout << "\n=== F. C 和 C++ 混编的三个实用要点 ===\n";
    std::cout << "  1) C 头文件要用条件编译包起来，两边都能用：\n";
    std::cout << "       #ifdef __cplusplus\n";
    std::cout << "       extern \"C\" {\n";
    std::cout << "       #endif\n";
    std::cout << "       void HAL_GPIO_WritePin(...);\n";
    std::cout << "       #ifdef __cplusplus\n";
    std::cout << "       }\n";
    std::cout << "       #endif\n";
    std::cout << "  2) 中断服务函数：加 extern \"C\"，并且**不要**在里面用任何\n";
    std::cout << "     会抛异常的 C++ 设施（字符串、容器、iostream）。\n";
    std::cout << "  3) 单例/全局对象：裸机上静态对象的构造函数会在 main 之前跑，\n";
    std::cout << "     而那时时钟/外设可能还没初始化。用 constexpr/constinit\n";
    std::cout << "     或者显式的 init() 函数，不要依赖全局构造函数。\n";

    std::cout << "\nmain 正常结束\n";
    return 0;
}
