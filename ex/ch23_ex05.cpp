// ch23_ex05.cpp —— 例 23-5：把除法换成乘法（M0+ 没有硬件除法指令）
// Cortex-M0+ 没有 SDIV/UDIV 指令，a / b 会调用 __aeabi_uidiv 之类的软件例程，
// 一次除法几十个周期。两条路可以避开它：
// 1) 除数是编译期常数 -> 编译器自动换成"乘法 + 移位"；
// 2) 除数是运行期变量 -> 编译期先做一张倒数表。
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdio>

// ---- 1) 除数是常数：编译器自己就会优化 ----
std::uint32_t div_by_256(std::uint32_t x) {
    return x / 256u;  // 编译器会变成 >> 8
}
std::uint32_t div_by_10(std::uint32_t x) {
    return x / 10u;  // 编译器会变成"乘 0xCCCCCCCD 再右移 35 位"
}

// ---- 2) 除数是变量：只能真除 ----
std::uint32_t div_by_var(std::uint32_t x, std::uint32_t d) {
    return x / d;
}

// ---- 3) 编译期倒数表（Q16 定点）：适合"除数是一小组固定值"的场合 ----
// 比如：把 ADC 读数换算成物理量，分母是几个固定的量程。
constexpr int kRecipCount = 64;

constexpr std::array<std::uint32_t, kRecipCount> make_recip_q16() {
    std::array<std::uint32_t, kRecipCount> t{};
    for (int i = 0; i < kRecipCount; ++i) {
        const std::uint32_t d = static_cast<std::uint32_t>(i + 1);
        t[static_cast<std::size_t>(i)] = (1u << 16) / d;  // 编译期算，不是运行期
    }
    return t;
}

constexpr auto kRecip = make_recip_q16();
static_assert(kRecip[0] == 65536u, "1/1 = 1.0");
static_assert(kRecip[1] == 32768u, "1/2 = 0.5");
static_assert(kRecip[9] == 6553u, "1/10 ≈ 0.1");

// 查表除法：一次乘法 + 一次移位，M0+ 上都是单周期指令
inline std::uint32_t div_by_small(std::uint32_t x, int d) {
    return static_cast<std::uint32_t>((static_cast<std::uint64_t>(x) * kRecip[d - 1]) >> 16);
}

int main() {
    std::printf("表大小 = %d 项 x 4 字节 = %zu 字节\n", kRecipCount, sizeof(kRecip));
    std::printf("\n除数  真除法   查表法   差\n");
    for (int d : {1, 3, 7, 10, 17, 33, 64}) {
        const std::uint32_t x = 1000000u;
        const std::uint32_t exact = x / static_cast<std::uint32_t>(d);
        const std::uint32_t fast = div_by_small(x, d);
        std::printf("%4d  %7u  %7u  %+d\n", d, exact, fast, (int)(fast - exact));
    }

        // ---- 速度对比：变量除法 vs 查表乘法 ----
    const int N = 20000000;
    volatile std::uint32_t seed = 12345;
    volatile int dv = 10;

    const auto t0 = std::chrono::steady_clock::now();
    std::uint64_t acc1 = 0;
    for (int i = 0; i < N; ++i) {
        acc1 += div_by_var(static_cast<std::uint32_t>(i) + seed, static_cast<std::uint32_t>(dv));
    }
    const auto t1 = std::chrono::steady_clock::now();

    std::uint64_t acc2 = 0;
    for (int i = 0; i < N; ++i) {
        acc2 += div_by_small(static_cast<std::uint32_t>(i) + seed, 10);
    }
    const auto t2 = std::chrono::steady_clock::now();

    const double ms_div = std::chrono::duration<double, std::milli>(t1 - t0).count();
    const double ms_tab = std::chrono::duration<double, std::milli>(t2 - t1).count();
    std::printf("\n%d 次运算：\n", N);
    std::printf("  变量除法  : %7.2f ms  (%.2f ns/次)\n", ms_div, ms_div * 1e6 / N);
    std::printf("  查表乘法  : %7.2f ms  (%.2f ns/次)\n", ms_tab, ms_tab * 1e6 / N);
    std::printf("  校验和低 32 位 %u / %u（查表是近似，尾数会有偏差）\n",
                (unsigned)acc1, (unsigned)acc2);
    std::printf("  常数除法示例 div_by_256(100000)=%u div_by_10(100000)=%u\n", div_by_256(100000),
                div_by_10(100000));

        // 坑：查表除法是**近似**的。
        // Q16 只有 16 位小数精度，x 很大时误差会累积。
        // 用在"传感器换算"上没问题，用在"算钱"上会出事。
        // 精度不够就把 Q16 换成 Q32（除数表用 uint64 存），代价是每次乘法要 64 位。
    return 0;
}
