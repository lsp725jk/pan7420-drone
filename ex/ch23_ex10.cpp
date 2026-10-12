// ch23_ex10.cpp —— 例 23-10：std::bit_cast——不违法的"类型双关"
// 需求：把 float 的 4 个字节原封不动当成 uint32 看（发给 CAN、存 Flash、比大小）。
// C 的老办法：union { float f; uint32_t u; } —— 在 C++ 里读"非活跃成员"是 UB；
// memcpy —— 合法，但意图不够明显；
// C++20 的答案：std::bit_cast，编译期也能用。
#include <bit>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <initializer_list>

// 编译期就能做：这是 bit_cast 相对 memcpy 的最大优势
constexpr float kF = 3.5f;
constexpr std::uint32_t kBits = std::bit_cast<std::uint32_t>(kF);
static_assert(kBits == 0x40600000u, "3.5f 的 IEEE754 表示");

// 手工分解 IEEE754：符号 / 指数 / 尾数
constexpr int sign_of(float f) {
    return (std::bit_cast<std::uint32_t>(f) >> 31) != 0;
}
constexpr std::uint32_t exp_of(float f) {
    return (std::bit_cast<std::uint32_t>(f) >> 23) & 0xFFu;
}
constexpr std::uint32_t frac_of(float f) {
    return std::bit_cast<std::uint32_t>(f) & 0x7FFFFFu;
}

// 用位运算判断 NaN / Inf，不用 <cmath>
constexpr bool is_nan_bits(float f) {
    return exp_of(f) == 0xFFu && frac_of(f) != 0;
}
constexpr bool is_inf_bits(float f) {
    return exp_of(f) == 0xFFu && frac_of(f) == 0;
}

int main() {
    std::printf("float       bits       符号 指数  尾数\n");
    std::printf("------------------------------------------------\n");
    for (float f : {0.0f, 1.0f, -1.0f, 3.5f, 0.15625f, 1e30f}) {
        const std::uint32_t b = std::bit_cast<std::uint32_t>(f);
        std::printf("%-10g  0x%08X  %4d  %4u  0x%06X\n", f, b, sign_of(f), exp_of(f), frac_of(f));
    }

        // 特殊值
    const float inf = std::bit_cast<float>(0x7F800000u);
    const float nan = std::bit_cast<float>(0x7FC00000u);
    std::printf("\n构造 Inf  = 0x%08X -> is_inf=%d is_nan=%d\n", std::bit_cast<std::uint32_t>(inf),
                (int)is_inf_bits(inf), (int)is_nan_bits(inf));
    std::printf("构造 NaN  = 0x%08X -> is_inf=%d is_nan=%d\n", std::bit_cast<std::uint32_t>(nan),
                (int)is_inf_bits(nan), (int)is_nan_bits(nan));

        // 往返转换必须一模一样
    const float back = std::bit_cast<float>(kBits);
    std::printf("\n往返: 3.5f -> 0x%08X -> %g  (一致: %d)\n", kBits, back, (int)(back == 3.5f));

        // 和 memcpy 的写法对照
    std::uint32_t via_memcpy = 0;
    std::memcpy(&via_memcpy, &kF, sizeof via_memcpy);
    std::printf("memcpy 写法得到 0x%08X，和 bit_cast 一致: %d\n", via_memcpy,
                (int)(via_memcpy == kBits));

        // 实用场景：把两个 uint16 拼成一个 float 从 CAN 里解析出来
    const std::uint16_t hi = 0x4060, lo = 0x0000;
    const std::uint32_t combined = (static_cast<std::uint32_t>(hi) << 16) | lo;
    std::printf("\nCAN 两帧拼回来: 0x%08X -> %g\n", combined,
                std::bit_cast<float>(combined));

        // 坑 1：bit_cast 要求两个类型大小完全一样，且都是可平凡复制的。
        // std::bit_cast<double>(some_float) -> error C2338: 大小不一致
        // 坑 2：bit_cast 不改变字节，所以**字节序**问题依然存在。
        // 跨 MCU 传浮点，还是得统一成大端或小端。
        // 坑 3：float 的位模式不是标准规定的（IEEE754 只是事实标准），
        // 但 Cortex-M0+ 和 x86 都是 IEEE754，实际没问题。
    std::printf("sizeof(float)=%zu sizeof(uint32_t)=%zu\n", sizeof(float), sizeof(std::uint32_t));
    return 0;
}
