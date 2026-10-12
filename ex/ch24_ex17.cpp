// ch24_ex17.cpp —— 例 24-17：调试技巧（断点 + 打印 + 断言）
// 调试器能看"此刻的真相"，打印能看"历史的过程"，断言能看"不该发生的事"。
// 三者配合用，比只用一种快得多。
#include <cstdio>
#include <cstdint>

// ==================================================================
// 技巧 1：用 __debugbreak() / 断言宏在"出事的那一刻"停下来
// ==================================================================
#if defined(_MSC_VER)
#define DEBUG_BREAK() __debugbreak()
#else
#define DEBUG_BREAK() __builtin_trap()
#endif

// 带上下文的断言：比 assert 多打印实际值，调试时省一次重新运行
// TRAP() 默认是空操作，让例子能跑完并打印出来。
// 想真的在断言处停下来（需要调试器），编译时加 /DENABLE_BREAK_ON_ASSERT
#ifdef ENABLE_BREAK_ON_ASSERT
#define TRAP() DEBUG_BREAK()
#else
#define TRAP() ((void)0)
#endif

#define CHECK_RANGE(v, lo, hi)                                                          \
    do {                                                                                \
        const auto _v = (v);                                                            \
        if (_v < (lo) || _v > (hi)) {                                                   \
            std::printf("[断言失败] %s:%d  %s = %d 不在 [%d, %d] 内\n", __FILE__,       \
                        __LINE__, #v, (int)_v, (int)(lo), (int)(hi));                   \
            std::fflush(stdout);                                                        \
            TRAP();                                                                     \
        }                                                                               \
    } while (0)

// ==================================================================
// 技巧 2：条件断点最好用的场景——循环第 N 次才出问题
// ==================================================================
int find_bad_sample(const std::int16_t* p, int n) {
    for (int i = 0; i < n; ++i) {
                // 在调试器里把断点设在这里，条件是 i == 37 或 p[i] == -32768
        if (p[i] == -32768) {
            return i;  // 传感器饱和/异常值的典型表现
        }
    }
    return -1;
}

// ==================================================================
// 技巧 3：分级日志（嵌入式上比调试器更实用）
// ==================================================================
enum class LogLevel { Error = 0, Warn = 1, Info = 2, Debug = 3 };
constexpr LogLevel kLogLevel = LogLevel::Info;  // 编译期决定，低的会被整个优化掉

#define LOG(level, ...)                                                     \
    do {                                                                    \
        if (static_cast<int>(level) <= static_cast<int>(kLogLevel)) {       \
            std::printf("[%s] ", #level);                                   \
            std::printf(__VA_ARGS__);                                       \
            std::printf("\n");                                              \
        }                                                                   \
    } while (0)

int main() {
    std::printf("=== 分级日志 ===\n");
    LOG(LogLevel::Error, "电机 %d 过流 %d mA", 2, 12000);
    LOG(LogLevel::Warn, "电池电压偏低: %d mV", 3400);
    LOG(LogLevel::Info, "已解锁, 模式 = %d", 3);
    LOG(LogLevel::Debug, "这条不会打印（等级不够）: %d", 42);

    std::printf("\n=== 断言带上下文 ===\n");
    const int throttle = 105;
    CHECK_RANGE(throttle, 0, 100);  // 会打印实际值
    std::printf("（上面这条触发了断言；本例默认只打印不停下，\n");
    std::printf("  加 /DENABLE_BREAK_ON_ASSERT 编译才会真的 __debugbreak() 停下来）\n");

    std::printf("\n=== 找异常样本 ===\n");
    const std::int16_t samples[8] = {100, 200, 300, 400, 500, 600, -32768, 800};
    const int bad = find_bad_sample(samples, 8);
    std::printf("异常样本在索引 %d，值 %d（-32768 是 int16 的最小值，\n", bad, samples[bad]);
    std::printf("通常是【sensor 没接/读了全 0/溢出】，不是真实数据）\n");

    std::printf("\n调试的一般顺序（先假设，再验证，别瞎改）：\n");
    std::printf("  1) 复现：找到 100%% 能重现的步骤。不能复现就没有调试，只有猜。\n");
    std::printf("  2) 缩小范围：二分法。是这一版才有的吗？注释掉一半代码还在吗？\n");
    std::printf("  3) 观察：在**嫌疑最大**的位置打断点/加打印，看实际值和预期差在哪。\n");
    std::printf("  4) 形成假设并验证：别一次改五处，改一处测一次。\n");
    std::printf("\nC/C++ 里最常见的几类 bug 和它们的症状：\n");
    std::printf("  - 未初始化变量         -> 结果是【垃圾值】，/Od 下正常 /O2 下出错\n");
    std::printf("  - 数组越界/野指针      -> 时好时坏，改一行无关代码就变了\n");
    std::printf("  - 栈溢出（递归/大局部）-> 进函数就 HardFault，或随机跑飞\n");
    std::printf("  - 有符号溢出           -> -O2 下结果诡异（UB 被优化器利用了）\n");
    std::printf("  - 中断和主循环共享变量 -> 没加 volatile/临界区，偶发\n");
    std::printf("\n工具：\n");
    std::printf("  - VS 调试器：断点、条件断点、数据断点（某个变量被写就停，抓野指针神器）\n");
    std::printf("  - printf / RTT / SWO：嵌入式上比调试器更不影响时序\n");
    std::printf("  - 逻辑分析仪 + GPIO 翻转：测中断响应时间的唯一可靠办法\n");
    return 0;
}
