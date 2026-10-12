// ch23_ex12.cpp —— 例 23-12：函数指针是内联的"断路开关"
// 同一个函数，直接调用能被内联；通过函数指针调用，编译器不知道会跳到哪，
// 只能老老实实做一次间接跳转。这是 qsort 比 std::sort 慢的根本原因。
#include <chrono>
#include <cstdio>
#include <functional>

__forceinline int cmp_direct(int a, int b) {
    return a < b ? -1 : (a > b ? 1 : 0);
}

// 强制不内联，用来当"真函数"放进指针
__declspec(noinline) int cmp_real(int a, int b) {
    return a < b ? -1 : (a > b ? 1 : 0);
}

// 用一个求最大值的循环来体现差别（比排序更容易控制变量）
__forceinline int pick_direct(int a, int b) {
    return a > b ? a : b;
}
__declspec(noinline) int pick_real(int a, int b) {
    return a > b ? a : b;
}

int main() {
    const int N = 20000000;
    volatile int seed = 3;
    const int s = seed;

        // ---- 1) 直接调用：可内联 ----
    const auto t0 = std::chrono::steady_clock::now();
    long long a1 = 0;
    for (int i = 0; i < N; ++i) {
        a1 = pick_direct(static_cast<int>(a1) + 1, i + s);
    }
    const auto t1 = std::chrono::steady_clock::now();

        // ---- 2) 通过函数指针调用 ----
    int (*fp)(int, int) = &pick_real;
    long long a2 = 0;
    for (int i = 0; i < N; ++i) {
        a2 = fp(static_cast<int>(a2) + 1, i + s);
    }
    const auto t2 = std::chrono::steady_clock::now();

        // ---- 3) 通过 std::function 调用（再包一层）----
    std::function<int(int, int)> f = &pick_real;
    long long a3 = 0;
    for (int i = 0; i < N; ++i) {
        a3 = f(static_cast<int>(a3) + 1, i + s);
    }
    const auto t3 = std::chrono::steady_clock::now();

    const double ms_d = std::chrono::duration<double, std::milli>(t1 - t0).count();
    const double ms_p = std::chrono::duration<double, std::milli>(t2 - t1).count();
    const double ms_f = std::chrono::duration<double, std::milli>(t3 - t2).count();

    std::printf("%d 次调用：\n", N);
    std::printf("  直接调用      : %7.2f ms  (%.3f ns/次)\n", ms_d, ms_d * 1e6 / N);
    std::printf("  函数指针      : %7.2f ms  (%.3f ns/次)   慢 %.1f 倍\n", ms_p,
                ms_p * 1e6 / N, ms_p / ms_d);
    std::printf("  std::function : %7.2f ms  (%.3f ns/次)   慢 %.1f 倍\n", ms_f,
                ms_f * 1e6 / N, ms_f / ms_d);
    std::printf("  校验和 %lld / %lld / %lld\n", a1, a2, a3);
    std::printf("  sizeof(int(*)(int,int))=%zu  sizeof(std::function)=%zu\n",
                sizeof(fp), sizeof(f));

    std::printf("\n和 C 的关系：\n");
    std::printf("  qsort(arr, n, sizeof(int), cmp)   —— cmp 是函数指针，永远内联不了\n");
    std::printf("  std::sort(v.begin(), v.end(), [](int a,int b){...})\n");
    std::printf("    比较器是模板参数，编译期就知道是哪个 lambda，能完整内联\n");
    std::printf("  这就是同样是快排，C++ 的 std::sort 常常比 qsort 快一截的原因\n");

        // 补救办法：
        // 1) 能用模板/泛型就用模板（std::sort、std::find_if 都是这个思路）；
        // 2) 一定要运行时多态时，把"调用点少"的部分用虚函数，把"调用点热"的用模板；
        // 3) 回调注册在初始化阶段做一次，别放在中断/控制循环里。
    return 0;
}
