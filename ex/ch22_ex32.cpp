// ch22_ex32.cpp —— 例 22-32：constexpr 函数——同一个函数，两种求值时机
// 关键认识：constexpr 不是"必须在编译期算"，而是"**允许**在编译期算"。
// 它到底在哪算，取决于调用它的上下文。
#include <cstddef>
#include <cstdio>
#include <type_traits>

// C++14 起，constexpr 函数里可以有循环、局部变量、if
constexpr unsigned long long fib(unsigned n) {
    if (n < 2) {
        return n;
    }
    unsigned long long a = 0, b = 1;
    for (unsigned i = 2; i <= n; ++i) {
        const unsigned long long t = a + b;
        a = b;
        b = t;
    }
    return b;
}

// C++20 的 std::is_constant_evaluated()：在函数里问"我这次是编译期调用吗"
// 注意：参数必须能让"运行期"这件事真的落地，所以这里带一个参数。
constexpr int which_phase(int n) {
    if (std::is_constant_evaluated()) {
        return 1;  // 编译期
    }
    return n;  // 运行期
}

// 经典用途：编译期用简单算法（不许 UB），运行期用快算法
constexpr int clamp(int v, int lo, int hi) {
    if (std::is_constant_evaluated()) {
        return v < lo ? lo : (v > hi ? hi : v);
    }
    return v;  // 假装运行期有个更快的实现
}

int main() {
        // ---- 编译期求值 ----
    static_assert(fib(10) == 55ULL, "");
    static_assert(fib(30) == 832040ULL, "");
    constexpr auto kFib20 = fib(20);
    static_assert(kFib20 == 6765ULL, "");
    int arr[fib(5)];  // 数组长度必须是编译期常量：fib(5)=5
    std::printf("fib(5)=%llu 用作数组长度，sizeof(arr)=%zu\n", fib(5), sizeof(arr));

        // ---- 运行期求值 ----
    volatile int n = 20;  // volatile 让编译器无法把它当常量
    const unsigned long long r = fib(n);
    std::printf("运行期 fib(%d) = %llu\n", (int)n, r);

        // ---- 同一个函数，两条路 ----
    constexpr int a = which_phase(0);  // 编译期：返回 1
    volatile int v = 2;                // volatile：挡住常量传播
    const int b = which_phase(v);      // 运行期：返回 v
    std::printf("constexpr 上下文 = %d，运行期上下文 = %d\n", a, b);
    static_assert(which_phase(0) == 1, "在 static_assert 里一定是编译期");

    std::printf("clamp(15,0,10) = %d\n", clamp(15, 0, 10));

        // 坑 1：constexpr 函数不保证编译期求值。
        // volatile int n = 20; fib(n) 就是运行期算的，一点没省。
        // 想让"必须编译期"成立，用 consteval（下一个例子）。
        // 坑 2：constexpr 函数里不能有 try/catch（C++20）、不能有 static 局部变量、
        // 不能调用非 constexpr 函数——这些错误在**实例化/求值**时才报，
        // 有时直到你写 static_assert 才暴露。
    std::printf("sizeof(arr)=%zu\n", sizeof(arr));
    return 0;
}
