// ch23_ex14.cpp —— 例 23-14：优化等级到底改变什么
// 同一份代码，用 /Od（不优化）、/O1（省空间）、/O2（求速度）、/Ox（全开）各编一遍，
// 运行时间和目标文件大小都不一样。这个例子自己测运行时；
// 目标文件大小用 dumpbin 量（见本章正文的实测表）。
#include <chrono>
#include <cstdint>
#include <cstdio>

// 一段"有优化空间"的代码：循环 + 分支 + 位运算 + 数组访问
std::uint32_t crunch(const std::uint32_t* data, int n) {
    std::uint32_t acc = 0;
    for (int i = 0; i < n; ++i) {
        const std::uint32_t v = data[i];
        if (v & 1u) {
            acc += v * 3u;
        } else {
            acc ^= v >> 1;
        }
        acc = (acc << 1) | (acc >> 31);  // 循环左移
    }
    return acc;
}

int main() {
    constexpr int N = 4096;
    static std::uint32_t data[N];
    for (int i = 0; i < N; ++i) {
        data[i] = static_cast<std::uint32_t>(i * 2654435761u);
    }

    const int ROUNDS = 2000;
    const auto t0 = std::chrono::steady_clock::now();
    std::uint32_t acc = 0;
    for (int r = 0; r < ROUNDS; ++r) {
        acc ^= crunch(data, N);
    }
    const auto t1 = std::chrono::steady_clock::now();
    const double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();

    std::printf("crunch x %d 轮（每轮 %d 个元素），耗时 %.2f ms，校验和 0x%08X\n", ROUNDS, N, ms,
                acc);
    std::printf("编译时用的优化选项决定了上面这个数字。\n");

#if defined(_MSC_VER)
        // _M_IX86_FP / _MSC_VER 不是优化等级标志，MSVC 没有提供查询优化等级的宏。
        // 想确认优化等级，只能看编译命令行或者反汇编。
    std::printf("(MSVC 没有查询优化等级的宏，反汇编才能看出来)\n");
#endif

    std::printf("\n各等级的典型特征：\n");
    std::printf("  /Od  不优化：每条语句一套模板代码，所有局部变量都在栈上，便于单步调试\n");
    std::printf("  /O1  偏向省空间：等价于 /Os，会拒绝【变大】的内联和展开\n");
    std::printf("  /O2  偏向速度：等价于 /Ot，会做循环展开、函数内联、向量化\n");
    std::printf("  /Ox  全开（/O2 加更多激进优化），有时反而更大更慢\n");
    std::printf("\n嵌入式建议：\n");
    std::printf("  Flash 用到 96%% 时用 -Os（GCC）/ /O1（MSVC）；\n");
    std::printf("  还有余量时用 -O2；-O3 经常让代码体积翻倍，收益却很小。\n");
    return 0;
}
