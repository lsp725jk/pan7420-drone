// ch23_ex23.cpp —— 例 23-23：综合实战——把一段"运行期算"的嵌入式代码改成"编译期算"
// 需求（照搬真实飞控场景）：
// ADC 读到一个 12 位原始值（0..4095），要
// 1) 换算成电压（参考电压 3.3V，但 MCU 没有 FPU、没有硬件除法）
// 2) 用一阶低通滤波平滑（系数固定）
// 3) 查表换算成"电池百分比"
// 全部在控制循环（1kHz）里跑，一秒 1000 次。
// 做法：把能搬到编译期的全部搬走 —— 表、系数、倒数。
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdio>

// ================= 编译期准备的三样东西 =================

// 1) 电压换算：mV = raw * 3300 / 4095。
// 3300/4095 是常数，编译期先算成 Q16 定点系数，运行期只剩乘 + 移。
constexpr std::uint32_t kVoltScaleQ16 = (3300u << 16) / 4095u;  // ≈ 52800

// 2) 电池百分比表：12 位 ADC -> 0..100%。
// 真实的电池放电曲线是非线性的，正好用表来表达。
constexpr int kPctLevels = 33;  // 表越小 Flash 越省
constexpr std::array<std::uint8_t, kPctLevels> make_pct_table() {
    std::array<std::uint8_t, kPctLevels> t{};
    for (int i = 0; i < kPctLevels; ++i) {
                // 假装是一条放电曲线：下标 0 是满电(100)，最后一个下标是空电(0)
        const int v = i * 100 / (kPctLevels - 1);
        t[static_cast<std::size_t>(i)] = static_cast<std::uint8_t>(100 - v);
    }
    return t;
}
constexpr auto kPctTable = make_pct_table();
static_assert(kPctTable[0] == 100, "");
static_assert(kPctTable[kPctLevels - 1] == 0, "");

// 3) 滤波系数：alpha = 0.1 的 Q16 定点版本，编译期算好
constexpr std::uint32_t kAlphaQ16 = 6554;  // 0.100006... * 65536

// ================= 版本 A：全运行期算（浮点 + 除法）=================
float filter_a = 0.0f;
float process_float(std::uint16_t raw) {
    const float v = raw * 3.3f / 4095.0f;             // 浮点乘 + 浮点除
    filter_a = filter_a + 0.1f * (v - filter_a);      // 浮点乘加
    const int pct_idx = (int)(filter_a / 3.3f * (kPctLevels - 1));
    const int idx = pct_idx < 0 ? 0 : (pct_idx >= kPctLevels ? kPctLevels - 1 : pct_idx);
    return kPctTable[idx];
}

// ================= 版本 B：全定点（整数 + 编译期常数）=================
std::uint32_t filter_b_q16 = 0;  // 滤波状态，Q16 定点
std::uint8_t process_fixed(std::uint16_t raw) {
        // 1) raw -> mV：一次 32 位乘法 + 一次右移，没有除法
    const std::uint32_t mv = (static_cast<std::uint32_t>(raw) * kVoltScaleQ16) >> 16;
        // 2) 一阶低通：y += alpha * (x - y)，全是整数
    const std::uint32_t mv_q16 = mv << 16;
    filter_b_q16 += (std::uint32_t)(((std::int64_t)(mv_q16 - filter_b_q16) * kAlphaQ16) >> 16);
        // 3) 查表：把 mV 映射到表下标（mV 范围 0..3300 -> 0..32）
    const std::uint32_t idx = ((filter_b_q16 >> 16) * (kPctLevels - 1)) / 3300u;
    const std::uint32_t i = idx < (kPctLevels - 1) ? idx : (kPctLevels - 1);
    return kPctTable[i];
}

int main() {
    std::printf("编译期算好的常数：\n");
    std::printf("  电压换算系数 Q16 = %u   (= %.6f)\n", kVoltScaleQ16, kVoltScaleQ16 / 65536.0);
    std::printf("  滤波系数     Q16 = %u   (= %.6f)\n", kAlphaQ16, kAlphaQ16 / 65536.0);
    std::printf("  电池表大小       = %zu 字节（%d 项 x uint8_t）\n", sizeof(kPctTable), kPctLevels);

        // ---- 结果对照：定点版本应该非常接近浮点版本 ----
    std::printf("\nraw    浮点版%%   定点版%%   差\n");
    std::printf("--------------------------------\n");
    for (std::uint16_t raw : {(std::uint16_t)0, (std::uint16_t)1000, (std::uint16_t)2048,
                              (std::uint16_t)3000, (std::uint16_t)4095}) {
        filter_a = 0.0f;
        filter_b_q16 = 0;
        int fa = 0;
        int fb = 0;
        for (int i = 0; i < 50; ++i) {  // 让滤波器收敛
            fa = (int)process_float(raw);
            fb = process_fixed(raw);
        }
        std::printf("%4u   %5d     %5d     %+d\n", raw, fa, fb, fb - fa);
    }

        // ---- 速度对比 ----
    const int N = 2000000;
    const auto t0 = std::chrono::steady_clock::now();
    int s1 = 0;
    for (int i = 0; i < N; ++i) {
        s1 += (int)process_float(static_cast<std::uint16_t>(i & 0xFFF));
    }
    const auto t1 = std::chrono::steady_clock::now();
    int s2 = 0;
    for (int i = 0; i < N; ++i) {
        s2 += process_fixed(static_cast<std::uint16_t>(i & 0xFFF));
    }
    const auto t2 = std::chrono::steady_clock::now();

    const double ms_f = std::chrono::duration<double, std::milli>(t1 - t0).count();
    const double ms_i = std::chrono::duration<double, std::milli>(t2 - t1).count();
    std::printf("\n%d 次调用：\n", N);
    std::printf("  浮点版 : %7.2f ms  (%.1f ns/次)\n", ms_f, ms_f * 1e6 / N);
    std::printf("  定点版 : %7.2f ms  (%.1f ns/次)   快 %.2f 倍\n", ms_i, ms_i * 1e6 / N,
                ms_f / ms_i);
    std::printf("  校验和 %d / %d\n", s1, s2);

        // 在 PY32F030（M0+，无 FPU、无硬件除法）上，这个差距会比 PC 大得多：
        // 浮点版：软件浮点库（__aeabi_fmul / fdiv / f2iz...），每次几百个周期
        // 定点版：全是 32 位乘、加、移位，每次几十个周期
        // 代价：可读性下降、精度要自己想清楚、状态变量的量纲要写在注释里。
    std::printf("\n编译期能搬走的东西：\n");
    std::printf("  - 常数表达式（3300/4095、0.1 的定点表示）        -> 编译期\n");
    std::printf("  - 查表（电池曲线、sin/cos、CRC）                 -> 编译期\n");
    std::printf("  - 数组大小、缓冲长度                             -> 编译期\n");
    std::printf("  - 分支的判断条件（如果依赖编译期常量）           -> 编译期（if constexpr）\n");
    std::printf("搬不走的：输入数据本身、以及依赖输入的计算。\n");
    return 0;
}
