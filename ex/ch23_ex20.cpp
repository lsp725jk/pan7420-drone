// ch23_ex20.cpp —— 例 23-20：过早优化是什么样子的
// "过早优化是万恶之源"（Knuth）不是让你别优化，而是说：
// **没测量就动手**，往往把代码改复杂了，性能却没变甚至更差。
// 这里有一个真实的"想当然优化"，实测反而更慢。
#include <chrono>
#include <cstdint>
#include <cstdio>

// ---- 版本 A：直白写法 ----
long long sum_if(const std::uint8_t* p, int n) {
    long long s = 0;
    for (int i = 0; i < n; ++i) {
        if (p[i] > 128) {
            s += p[i];
        }
    }
    return s;
}

// ---- 版本 B："优化"成无分支：用掩码代替 if ----
// 想法：分支预测会失败，那就别分支。
long long sum_mask(const std::uint8_t* p, int n) {
    long long s = 0;
    for (int i = 0; i < n; ++i) {
        const std::uint32_t v = p[i];
                // v > 128 时 mask = 0xFFFFFFFF，否则 0
        const std::uint32_t mask = (std::uint32_t) - (std::int32_t)(v > 128u);
        s += (long long)(v & mask);
    }
    return s;
}

// ---- 版本 C：把乘法换成移位（"乘法很慢"是几十年前的常识了）----
long long scale_mul(int x) {
    long long s = 0;
    for (int i = 0; i < 1000; ++i) {
        s += (long long)x * 7;
    }
    return s;
}
long long scale_shift(int x) {
    long long s = 0;
    for (int i = 0; i < 1000; ++i) {
        s += (long long)((x << 3) - x);  // x*8 - x
    }
    return s;
}

int main() {
    constexpr int N = 1 << 20;
    static std::uint8_t data[N];
    std::uint32_t seed = 12345;
    for (int i = 0; i < N; ++i) {
        seed = seed * 1664525u + 1013904223u;
        data[i] = static_cast<std::uint8_t>(seed >> 24);
    }

    const int ROUNDS = 100;

    const auto t0 = std::chrono::steady_clock::now();
    long long a = 0;
    for (int r = 0; r < ROUNDS; ++r) {
        a ^= sum_if(data, N);
    }
    const auto t1 = std::chrono::steady_clock::now();
    long long b = 0;
    for (int r = 0; r < ROUNDS; ++r) {
        b ^= sum_mask(data, N);
    }
    const auto t2 = std::chrono::steady_clock::now();

    const double ms_if = std::chrono::duration<double, std::milli>(t1 - t0).count();
    const double ms_mask = std::chrono::duration<double, std::milli>(t2 - t1).count();
    std::printf("数据是随机的（分支完全无法预测，一半一半）\n");
    std::printf("  if 版本    : %7.2f ms\n", ms_if);
    std::printf("  掩码版本   : %7.2f ms   相对 %+.1f%%\n", ms_mask, (ms_mask / ms_if - 1) * 100);
    std::printf("  结果一致   : %d\n", (int)(a == b));

        // 再来一组"可预测分支"的数据
    for (int i = 0; i < N; ++i) {
        data[i] = static_cast<std::uint8_t>(i < N / 2 ? 10 : 250);
    }
    const auto t3 = std::chrono::steady_clock::now();
    long long c = 0;
    for (int r = 0; r < ROUNDS; ++r) {
        c ^= sum_if(data, N);
    }
    const auto t4 = std::chrono::steady_clock::now();
    long long d = 0;
    for (int r = 0; r < ROUNDS; ++r) {
        d ^= sum_mask(data, N);
    }
    const auto t5 = std::chrono::steady_clock::now();
    const double ms_if2 = std::chrono::duration<double, std::milli>(t4 - t3).count();
    const double ms_mask2 = std::chrono::duration<double, std::milli>(t5 - t4).count();
    std::printf("\n数据是规整的（分支几乎每次预测都成功）\n");
    std::printf("  if 版本    : %7.2f ms\n", ms_if2);
    std::printf("  掩码版本   : %7.2f ms   相对 %+.1f%%\n", ms_mask2, (ms_mask2 / ms_if2 - 1) * 100);
    std::printf("  结果一致   : %d\n", (int)(c == d));

        // 乘法 vs 移位
    const int ITER = 300000;
    const auto t6 = std::chrono::steady_clock::now();
    long long e = 0;
    for (int i = 0; i < ITER; ++i) {
        e += scale_mul(i);
    }
    const auto t7 = std::chrono::steady_clock::now();
    long long f = 0;
    for (int i = 0; i < ITER; ++i) {
        f += scale_shift(i);
    }
    const auto t8 = std::chrono::steady_clock::now();
    std::printf("\n乘 7 vs 移位（%d 次调用）：\n", ITER);
    std::printf("  x * 7         : %7.2f ms\n",
                std::chrono::duration<double, std::milli>(t7 - t6).count());
    std::printf("  (x<<3) - x    : %7.2f ms\n",
                std::chrono::duration<double, std::milli>(t8 - t7).count());
    std::printf("  结果一致      : %d\n", (int)(e == f));

    std::printf("\n判断【该不该优化】的四条标准：\n");
    std::printf("  1) 它真的慢吗？—— 有测量数据，不是感觉\n");
    std::printf("  2) 它占总时间的多少？—— 优化 5%% 的部分没有意义\n");
    std::printf("  3) 优化后**测得**更快吗？—— 必须复测，不能想当然\n");
    std::printf("  4) 代价是什么？—— 可读性、可维护性、Flash 占用、可移植性\n");
    std::printf("\n反过来说，**该**做的事（这些是免费的）：\n");
    std::printf("  - 选对算法和数据结构（O(n^2) -> O(n log n) 比什么微优化都管用）\n");
    std::printf("  - 选对内存布局（AoS/SoA）\n");
    std::printf("  - 避免不必要的拷贝（用 const&、用移动）\n");
    std::printf("  - 打开优化开关（很多人忘了 Release 构建）\n");
    std::printf("\n（本机 /Od 实测，具体数字随编译选项变化，重点看【结论是否成立】而不是绝对数）\n");
    return 0;
}
