// ch23_ex21.cpp —— 例 23-21：std::memcpy 为什么比手写循环快
// 不是因为 memcpy "更底层"，而是因为：
// 1) 它被编译器当成内建函数（intrinsic），能直接生成 SIMD 指令（SSE/AVX/NEON）；
// 2) 它知道长度，可以做 8/16/32 字节的对齐搬运；
// 3) 库里的实现是按目标 CPU 手写汇编调优过的。
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>

constexpr int N = 1 << 22;  // 4 MB
static std::uint8_t src[N];
static std::uint8_t dst[N];

// 手写循环（逐字节）
void copy_bytes(std::uint8_t* d, const std::uint8_t* s, std::size_t n) {
    for (std::size_t i = 0; i < n; ++i) {
        d[i] = s[i];
    }
}

// 手写循环（一次 8 字节，靠 uint64_t）
void copy_words(std::uint8_t* d, const std::uint8_t* s, std::size_t n) {
    std::size_t i = 0;
    for (; i + 8 <= n; i += 8) {
        std::uint64_t w = 0;
        std::memcpy(&w, s + i, 8);
        std::memcpy(d + i, &w, 8);
    }
    for (; i < n; ++i) {
        d[i] = s[i];
    }
}

int main() {
    for (int i = 0; i < N; ++i) {
        src[i] = static_cast<std::uint8_t>(i * 7);
    }

    const int ROUNDS = 20;

    const auto t0 = std::chrono::steady_clock::now();
    for (int r = 0; r < ROUNDS; ++r) {
        copy_bytes(dst, src, N);
    }
    const auto t1 = std::chrono::steady_clock::now();

    for (int r = 0; r < ROUNDS; ++r) {
        copy_words(dst, src, N);
    }
    const auto t2 = std::chrono::steady_clock::now();

    for (int r = 0; r < ROUNDS; ++r) {
        std::memcpy(dst, src, N);
    }
    const auto t3 = std::chrono::steady_clock::now();

    const double total_mb = (double)N * ROUNDS / 1048576.0;
    const double ms_b = std::chrono::duration<double, std::milli>(t1 - t0).count();
    const double ms_w = std::chrono::duration<double, std::milli>(t2 - t1).count();
    const double ms_m = std::chrono::duration<double, std::milli>(t3 - t2).count();

    std::printf("搬运 %.0f MB：\n", total_mb);
    std::printf("  逐字节循环 : %8.2f ms  (%6.1f MB/s)\n", ms_b, total_mb / (ms_b / 1000));
    std::printf("  8 字节循环 : %8.2f ms  (%6.1f MB/s)\n", ms_w, total_mb / (ms_w / 1000));
    std::printf("  std::memcpy: %8.2f ms  (%6.1f MB/s)   比逐字节快 %.1f 倍\n", ms_m,
                total_mb / (ms_m / 1000), ms_b / ms_m);
    std::printf("  内容一致   : %d\n", (int)(std::memcmp(dst, src, N) == 0));

    std::printf("\n为什么手写的逐字节循环不行：\n");
    std::printf("  - 每次只搬 1 字节，还要做一次循环判断 + 一次下标计算\n");
    std::printf("  - 编译器**有时**能自动向量化，但只要有对齐疑虑或别名疑虑就会放弃\n");
    std::printf("  - memcpy 是编译器内建，能直接用 16/32 字节的 SIMD 寄存器搬\n");

    std::printf("\n嵌入式上要注意的：\n");
    std::printf("  - M0+ 没有 SIMD，但 memcpy 仍然会用 LDM/STM 一次搬多个寄存器\n");
    std::printf("  - 编译器自带的 memcpy 可能比你自己写的更快，也可能更大（体积敏感时\n");
    std::printf("    可以加 -fno-builtin-memcpy 或自己写一个小版本）\n");
    std::printf("  - 大块搬运优先用 DMA，CPU 直接不参与\n");

        // 坑 1：memcpy 的参数是**字节数**，不是元素个数。
        // memcpy(dst, src, n) 搬 n 个字节；想搬 n 个 int 要写 n * sizeof(int)。
        // 坑 2：源和目标**重叠**时必须用 std::memmove，memcpy 重叠是 UB。
        // 典型场景：把缓冲区往后挪 1 个字节。
    char ov[16] = "abcdefgh";
    std::memmove(ov + 1, ov, 8);  // 正确：memmove
    std::printf("\n重叠搬移用 memmove: %s\n", ov);
    std::printf("（同样的调用换成 memcpy，结果未定义——本机可能对，换个编译器就可能错）\n");
    return 0;
}
