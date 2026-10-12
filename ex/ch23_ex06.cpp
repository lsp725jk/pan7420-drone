// ch23_ex06.cpp —— 例 23-6：查表的代价——表越大越准，Flash 也越吃紧
// 编译期查表不是免费的：**表本身要占 Flash**。64KB 的 PY32F030 上，
// 表大一点点就可能从"刚好放得下"变成"放不下"。
// 这个例子打印三张表的大小和精度，用来做取舍。
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>

constexpr double kPi = 3.14159265358979323846;

// 简易 constexpr sin（同例 23-3）
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

template <int Bits>
constexpr std::array<std::int16_t, (1 << Bits)> make_table() {
    constexpr int N = 1 << Bits;
    std::array<std::int16_t, N> t{};
    for (int i = 0; i < N; ++i) {
        const double v = my_sin(2.0 * kPi * i / N);
        t[static_cast<std::size_t>(i)] = static_cast<std::int16_t>(v * 32767.0 + (v >= 0 ? 0.5 : -0.5));
    }
    return t;
}

constexpr auto kTab4 = make_table<4>();    // 16 点
constexpr auto kTab6 = make_table<6>();    // 64 点
constexpr auto kTab8 = make_table<8>();    // 256 点
constexpr auto kTab10 = make_table<10>();  // 1024 点

// 用整数索引查表，返回 Q15
template <typename Tab>
int lookup_q15(const Tab& tab, unsigned deg) {
    constexpr unsigned N = (unsigned)Tab{}.size();
    return tab[(deg * N / 360u) % N];
}

// 理论最大误差：相邻两点之间角度差一半时最差
template <int Bits>
double max_err_deg() {
    constexpr unsigned N = 1u << Bits;
    return 1.0 - std::cos(kPi / N);  // 弦到弧的最大偏差
}

int main() {
    std::printf("表点数   字节数   0..360度最大量化误差\n");
    std::printf("----------------------------------------\n");
    std::printf("%6d %8zu   %.6f\n", 16, sizeof(kTab4), max_err_deg<4>());
    std::printf("%6d %8zu   %.6f\n", 64, sizeof(kTab6), max_err_deg<6>());
    std::printf("%6d %8zu   %.6f\n", 256, sizeof(kTab8), max_err_deg<8>());
    std::printf("%6d %8zu   %.6f\n", 1024, sizeof(kTab10), max_err_deg<10>());

    std::printf("\n实测几个角度的误差（相对标准 sin）：\n");
    std::printf("角度   16点      64点      256点     1024点\n");
    for (unsigned deg : {0u, 17u, 45u, 89u, 123u}) {
        const double want = std::sin(deg * kPi / 180.0);
        const double e4 = std::fabs(lookup_q15(kTab4, deg) / 32768.0 - want);
        const double e6 = std::fabs(lookup_q15(kTab6, deg) / 32768.0 - want);
        const double e8 = std::fabs(lookup_q15(kTab8, deg) / 32768.0 - want);
        const double e10 = std::fabs(lookup_q15(kTab10, deg) / 32768.0 - want);
        std::printf("%4u  %.2e  %.2e  %.2e  %.2e\n", deg, e4, e6, e8, e10);
    }

    std::printf("\n怎么选：\n");
    std::printf("  - 只是要点亮 LED、做蜂鸣器音调：16~32 点足够\n");
    std::printf("  - 做电机 FOC 的 sin/cos：256 点 + 线性插值（误差可再降两个数量级）\n");
    std::printf("  - 做姿态解算的三角函数：宁可用 256 点表 + 插值，也别调 sinf()\n");

        // 插值版：把相邻两点线性插上，256 点能顶 4096 点用
    std::printf("\n线性插值的效果（256 点表）：\n");
    for (unsigned deg : {17u, 45u, 89u}) {
        constexpr unsigned N = 256;
        const double want = std::sin(deg * kPi / 180.0);
        const double idx_f = deg * N / 360.0;
        const unsigned i0 = (unsigned)idx_f % N;
        const unsigned i1 = (i0 + 1) % N;
        const double frac = idx_f - (unsigned)idx_f;
        const double a = kTab8[i0] / 32768.0;
        const double b = kTab8[i1] / 32768.0;
        const double interp = a + (b - a) * frac;
        std::printf("  %3u 度: 直接查表误差 %.2e，插值后 %.2e\n", deg,
                    std::fabs(a - want), std::fabs(interp - want));
    }
    return 0;
}
