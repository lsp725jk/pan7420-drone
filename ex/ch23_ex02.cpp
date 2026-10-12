// ch23_ex02.cpp —— 例 23-2：常量折叠（constant folding）长什么样
// 同一段计算，写在 constexpr 上下文里 -> 编译完只剩下一个立即数；
// 写在运行期 -> 真的一个循环一个循环地跑。
#include <chrono>
#include <cstdio>

// 故意写得"不像能被优化掉"：有循环、有异或、有依赖
constexpr long long work(int n) {
    long long s = 0;
    for (int i = 1; i <= n; ++i) {
        s += static_cast<long long>(i) * i;
        s ^= (s >> 3);
    }
    return s;
}

int main() {
    const int N = 20000;

        // ---- 编译期：编译完这里只有一个数字 ----
    constexpr long long kCompileTime = work(N);
    static_assert(kCompileTime != 0, "编译期真的算出来了");

        // ---- 运行期：n 来自 volatile，编译器没法提前算 ----
    volatile int n = N;
    const auto t0 = std::chrono::steady_clock::now();
    long long runtime = work(n);
    const auto t1 = std::chrono::steady_clock::now();
    const double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();

    std::printf("编译期结果 kCompileTime = %lld\n", kCompileTime);
    std::printf("运行期结果 runtime      = %lld\n", runtime);
    std::printf("两者一致: %d\n", (int)(kCompileTime == runtime));
    std::printf("运行期算了 %d 次，耗时 %.3f ms\n", N, ms);
    std::printf("编译期那份的运行期耗时 = 0（就是一条 mov 立即数）\n");

        // 怎么亲眼确认？
        // 把本文件用 /c 编译成 .obj，再用 dumpbin /disasm 反汇编 main，
        // 会看到 kCompileTime 直接变成一个立即数，循环不见了。
        // 注意：编译期求值不是免费的，它花的是**编译时间**，
        // 而且编译器有步数上限（MSVC: /constexpr:steps，默认 1048576），
        // 算得太多会报 "error C2131" 或编译变得很慢。
    return 0;
}
