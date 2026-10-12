// ch23_ex03.cpp —— 例 23-3：编译期查表（嵌入式最有价值的一招）
// 场景：PY32F030（Cortex-M0+，无 FPU、无硬件除法）要算 sin。
// - 运行期调 sinf()：软件浮点，几百到上千个周期，还要几 KB 库代码；
// - 编译期算好一张 256 点 Q15 表：运行期就是一次数组读取 + 一次乘法。
// 本机实测：MSVC 19.51 的 std::sin 还不是 constexpr（P0533 没实现），
// 所以这里自己写一个 constexpr 版本的 sin——这本身就是嵌入式常见做法。
#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>

constexpr double kPi = 3.14159265358979323846;

// ---- 自己实现的 constexpr sin（泰勒级数 + 区间归约到 [-pi, pi]）----
// 展开到 x^17/17! 项，在 [-pi, pi] 上误差约 2e-8，
// 远小于 Q15 定点数的最小刻度 1/32768 ≈ 3e-5，够用了。
constexpr double my_sin(double x) {
    while (x > kPi) {
        x -= 2.0 * kPi;
    }
    while (x < -kPi) {
        x += 2.0 * kPi;
    }
    const double x2 = x * x;
    const double p = 1.0 +
                     x2 * (-1.0 / 6.0 +
                           x2 * (1.0 / 120.0 +
                                 x2 * (-1.0 / 5040.0 +
                                       x2 * (1.0 / 362880.0 +
                                             x2 * (-1.0 / 39916800.0 +
                                                   x2 * (1.0 / 6227020800.0 +
                                                         x2 * (-1.0 / 355687428096000.0)))))));
    return x * p;
}

// ---- 编译期生成 Q15 正弦表 ----
constexpr int kTableBits = 8;                 // 256 点
constexpr int kTableSize = 1 << kTableBits;   // 一个周期分 256 份

constexpr std::array<std::int16_t, kTableSize> make_sin_table_q15() {
    std::array<std::int16_t, kTableSize> t{};
    for (int i = 0; i < kTableSize; ++i) {
        const double angle = 2.0 * kPi * i / kTableSize;
        const double v = my_sin(angle);
                // Q15：把 [-1,1] 映射到 [-32767,32767]
        t[i] = static_cast<std::int16_t>(v * 32767.0 > 0 ? v * 32767.0 + 0.5
                                                         : v * 32767.0 - 0.5);
    }
    return t;
}

constexpr auto kSinTable = make_sin_table_q15();

// 编译期就验证：0 度是 0，90 度是满量程
static_assert(kSinTable[0] == 0, "");
static_assert(kSinTable[kTableSize / 4] == 32767, "");

// ---- 运行期：按角度索引查表 ----
int sin_q15_lookup(unsigned angle_deg) {
    const unsigned idx = (angle_deg * kTableSize / 360u) & (kTableSize - 1);
    return kSinTable[idx];
}

int main() {
        // 1) 表本身有多大？
    std::printf("表大小 = %d 项 x %zu 字节 = %zu 字节（在 .rdata 里，不占 RAM）\n", kTableSize,
                sizeof(std::int16_t), sizeof(kSinTable));

        // 2) 抽查几个点，和标准库对一下
    std::printf("\n角度   查表(Q15)   查表/32768   标准 sin   误差\n");
    for (unsigned deg : {0u, 30u, 45u, 90u, 180u, 270u, 359u}) {
        const int q = sin_q15_lookup(deg);
        const double got = q / 32768.0;
        const double want = std::sin(deg * kPi / 180.0);
        std::printf("%4u   %8d   %10.6f   %9.6f   %.2e\n", deg, q, got, want,
                    got > want ? got - want : want - got);
    }

        // 3) 查表 vs 实时计算，谁快？
    const int N = 2000000;
    volatile unsigned seed = 7;

    const auto t0 = std::chrono::steady_clock::now();
    long long acc1 = 0;
    for (int i = 0; i < N; ++i) {
        acc1 += sin_q15_lookup((unsigned)((i * 37 + seed) % 360));
    }
    const auto t1 = std::chrono::steady_clock::now();

    long long acc2 = 0;
    for (int i = 0; i < N; ++i) {
        const double a = ((i * 37 + seed) % 360) * kPi / 180.0;
        acc2 += (long long)(std::sin(a) * 32767.0);
    }
    const auto t2 = std::chrono::steady_clock::now();

    const double ms_lookup = std::chrono::duration<double, std::milli>(t1 - t0).count();
    const double ms_sin = std::chrono::duration<double, std::milli>(t2 - t1).count();

    std::printf("\n%d 次调用：\n", N);
    std::printf("  查表      : %7.2f ms  (%.2f ns/次)\n", ms_lookup, ms_lookup * 1e6 / N);
    std::printf("  实时 sin  : %7.2f ms  (%.2f ns/次)\n", ms_sin, ms_sin * 1e6 / N);
    std::printf("  校验和 %lld / %lld\n", acc1, acc2);

        // 说明：这是 PC 上的 x64 结果，PC 有硬件浮点和 sin 的查表指令；
        // 在 M0+ 上差距会大得多（软件浮点 + 无 FPU），这是查表最典型的收益场景。
    return 0;
}
