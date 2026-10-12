// ch23_ex15.cpp —— 例 23-15：剖析（profiling）的基本方法
// 优化之前必须先测量。这个例子给一个 20 行的计时小工具（RAII 风格），
// 以及"怎么找出真正的热点"的一般流程。
#include <chrono>
#include <cstdint>
#include <cstdio>

// ---- 一个最小的 RAII 计时器 ----
class Stopwatch {
public:
    Stopwatch(const char* name) : name_(name), t0_(std::chrono::steady_clock::now()) {}
    ~Stopwatch() {
        const auto t1 = std::chrono::steady_clock::now();
        const double ms = std::chrono::duration<double, std::milli>(t1 - t0_).count();
        std::printf("[计时] %-24s %8.3f ms\n", name_, ms);
    }

private:
    const char* name_;
    std::chrono::steady_clock::time_point t0_;
};

// ---- 被剖析的三段代码 ----
std::uint64_t sum_naive(const std::uint32_t* p, int n) {
    std::uint64_t s = 0;
    for (int i = 0; i < n; ++i) {
        s += p[i];
    }
    return s;
}

std::uint64_t sum_unrolled(const std::uint32_t* p, int n) {
    std::uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
    int i = 0;
    for (; i + 4 <= n; i += 4) {
        s0 += p[i];
        s1 += p[i + 1];
        s2 += p[i + 2];
        s3 += p[i + 3];
    }
    for (; i < n; ++i) {
        s0 += p[i];
    }
    return s0 + s1 + s2 + s3;
}

std::uint64_t sum_reverse(const std::uint32_t* p, int n) {
    std::uint64_t s = 0;
    for (int i = n - 1; i >= 0; --i) {  // 从后往前，缓存不友好
        s += p[i];
    }
    return s;
}

int main() {
    constexpr int N = 1 << 20;  // 4 MB
    static std::uint32_t data[N];
    for (int i = 0; i < N; ++i) {
        data[i] = static_cast<std::uint32_t>(i);
    }

    std::uint64_t r1 = 0, r2 = 0, r3 = 0;
    {
        Stopwatch sw("朴素顺序求和");
        r1 = sum_naive(data, N);
    }
    {
        Stopwatch sw("四路展开求和");
        r2 = sum_unrolled(data, N);
    }
    {
        Stopwatch sw("倒序求和（缓存不友好）");
        r3 = sum_reverse(data, N);
    }
    std::printf("三个结果一致: %d\n", (int)(r1 == r2 && r2 == r3));

    std::printf("\n剖析的一般流程：\n");
    std::printf("  1) 用**Release**构建 + 真实数据（/Od 或 -O0 下的耗时没有参考价值）\n");
    std::printf("  2) 先整体测（total time），再决定要不要细看\n");
    std::printf("  3) 找热点：\n");
    std::printf("     - 手工计时：像上面这样，按函数/阶段插桩，最直接\n");
    std::printf("     - 采样剖析器：VS 的 Performance Profiler、Linux 的 perf、gprof\n");
    std::printf("       它每隔几毫秒采一次调用栈，统计【哪一行被采到最多】\n");
    std::printf("     - 硬件计数器：cache miss、分支预测失败（perf stat / VTune）\n");
    std::printf("  4) 只优化占比最大的那一两处，改完**再测一次**确认真的快了\n");
    std::printf("  5) 嵌入式上还有两个必看指标：Flash 占用（arm-none-eabi-size）\n");
    std::printf("     和 RAM 占用（.data + .bss）——它们和速度经常互相打架\n");

        // 坑 1：在 /Od（或 -O0）下做性能对比 —— 结论基本无效。
        // 坑 2：只测一次。第一次跑有冷缓存、动态库加载、CPU 降频等因素，
        // 至少要跑 3~5 次取最好值或中位数。
        // 坑 3：编译器把"没人用的计算结果"整个删掉，测出来是 0 ms。
        // 所以结果一定要 printf 出来，或者写进 volatile 变量。
        // 坑 4：把 -O2 换成 -O3 却只看总时间，不看 Flash 增量。
    return 0;
}
