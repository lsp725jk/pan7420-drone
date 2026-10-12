// ch23_ex07.cpp —— 例 23-7：C++20 <bit> ——位运算不用再手写循环
// 这些函数都是 constexpr，也能在编译期用；底层会编译成 CPU 的对应指令。
#include <bit>
#include <cstdint>
#include <cstdio>
#include <initializer_list>

// ---- 1) 整数 log2：不用循环移位 ----
constexpr int ilog2(std::uint32_t x) {
    return x == 0 ? -1 : 31 - std::countl_zero(x);
}

// ---- 2) 判断是不是 2 的幂：一句搞定 ----
constexpr bool is_pow2(std::uint32_t x) {
    return std::has_single_bit(x);
}

// ---- 3) 找最低置位的下标（RTOS 里选最高优先级任务就靠这个）----
constexpr int lowest_bit(std::uint32_t mask) {
    return mask == 0 ? -1 : std::countr_zero(mask);
}

// ---- 4) 位图占用率 ----
constexpr int bits_set(std::uint32_t map) {
    return std::popcount(map);
}

static_assert(ilog2(1) == 0, "");
static_assert(ilog2(1024) == 10, "");
static_assert(is_pow2(64) && !is_pow2(100), "");
static_assert(lowest_bit(0b10001000u) == 3, "");
static_assert(bits_set(0xF0F0u) == 8, "");

int main() {
    std::printf("x            popcount  countl_zero  countr_zero  bit_width  bit_ceil\n");
    std::printf("------------------------------------------------------------------------\n");
    for (std::uint32_t x : {0u, 1u, 2u, 6u, 100u, 4096u, 0x80000000u}) {
        std::printf("%-12u %-9d %-12d %-12d %-10u %u\n", x, std::popcount(x),
                    std::countl_zero(x), std::countr_zero(x), std::bit_width(x), std::bit_ceil(x));
    }

    std::printf("\n实用换算：\n");
    for (std::uint32_t x : {1u, 255u, 256u, 1000u, 12345u}) {
        std::printf("  ilog2(%u) = %d   is_pow2=%d   bit_floor=%u\n", x, ilog2(x), (int)is_pow2(x),
                    std::bit_floor(x));
    }

    std::printf("\n环形缓冲大小取 2 的幂，就能用 & 代替 %%：\n");
    for (std::uint32_t want : {10u, 100u, 300u, 1000u}) {
        const std::uint32_t size = std::bit_ceil(want);
        std::printf("  想存 %4u 字节 -> 用 %4u 字节的缓冲，索引 (i & %u) 就是 i %% %u\n", want, size,
                    size - 1, size);
    }

    std::printf("\nrotl/rotr 循环移位（CRC、加密、显示驱动常用）：\n");
    std::printf("  rotl(0x80000001, 1) = 0x%08X\n", std::rotl(0x80000001u, 1));
    std::printf("  rotr(0x00000003, 1) = 0x%08X\n", std::rotr(0x00000003u, 1));

        // 坑：countl_zero / popcount 的参数是**无符号**类型。
        // 传 int 会被隐式转成 unsigned，负数会变成很大的数，结果就完全不对了。
        // 比如 popcount(-1) 是 32，不是 1。
        // 坑：std::bit_ceil(0) 返回 1；对超过一半位宽的输入会返回 0（不满足 2 的幂）。
        // 想安全用，先判断 x <= 最大可表示的一半。
    std::printf("\npopcount((unsigned)-1) = %d（不是 1！）\n", std::popcount((unsigned)-1));
    std::printf("sizeof(unsigned)=%zu\n", sizeof(unsigned));
    return 0;
}
