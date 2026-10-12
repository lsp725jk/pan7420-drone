// ch23_ex11.cpp —— 例 23-11：inline 到底是什么意思
// 很多人以为 inline 是"叫编译器内联"，其实它在 C++ 里的正式含义是：
// "这个函数允许在多个 .cpp 文件里各有一份定义，链接器把它们合成一个"（放宽 ODR 规则）
// 至于"要不要真的内联展开"，是编译器自己决定的，跟你写不写 inline 无关。
#include <chrono>
#include <cstdio>

// 1) 定义在类体里的成员函数**隐式**是 inline
struct Tiny {
    int add(int a, int b) { return a + b; }  // 隐式 inline
};

// 2) 自由函数要 inline 就得自己写
inline int add_inline(int a, int b) {
    return a + b;
}

// 3) 明确要求内联：编译器一般会照做，但也不是绝对保证
__forceinline int add_forced(int a, int b) {
    return a + b;
}

// 4) 明确禁止内联：用来测量"真函数调用"的代价
__declspec(noinline) int add_noinline(int a, int b) {
    return a + b;
}

int main() {
    Tiny t;
    std::printf("Tiny::add(1,2)        = %d\n", t.add(1, 2));
    std::printf("add_inline(1,2)       = %d\n", add_inline(1, 2));
    std::printf("add_forced(1,2)       = %d\n", add_forced(1, 2));
    std::printf("add_noinline(1,2)     = %d\n", add_noinline(1, 2));

    const int N = 50000000;
    volatile int seed = 1;
    const int s = seed;

        // ---- 能内联的调用 ----
    const auto t0 = std::chrono::steady_clock::now();
    long long a1 = 0;
    for (int i = 0; i < N; ++i) {
        a1 += add_inline(i, s);
    }
    const auto t1 = std::chrono::steady_clock::now();

        // ---- 不能内联的调用 ----
    long long a2 = 0;
    for (int i = 0; i < N; ++i) {
        a2 += add_noinline(i, s);
    }
    const auto t2 = std::chrono::steady_clock::now();

    const double ms_in = std::chrono::duration<double, std::milli>(t1 - t0).count();
    const double ms_no = std::chrono::duration<double, std::milli>(t2 - t1).count();
    std::printf("\n%d 次 add：\n", N);
    std::printf("  可内联    : %7.2f ms  (%.3f ns/次)\n", ms_in, ms_in * 1e6 / N);
    std::printf("  禁止内联  : %7.2f ms  (%.3f ns/次)\n", ms_no, ms_no * 1e6 / N);
    std::printf("  校验和 %lld / %lld\n", a1, a2);

        // 编译器凭什么决定内联？
        // - 函数体可见（定义在同一个编译单元里，或者开了 LTCG/IPO）
        // - 函数足够小，或者只被调用一处
        // - 内联后不会把代码撑爆（-Os / /O1 下会更保守）
        // - 是虚函数且调用点能确定类型时，C++ 也能"去虚化"后内联
        // 坑：为了内联把大函数塞进头文件，会让**每个** .cpp 都编一遍，编译变慢；
        // 而且 /Os 下编译器可能拒绝内联，反而代码更大。

        // 头文件里写普通函数必须加 inline，否则多个 .cpp 包含时
        // 链接会报 LNK2005（重复定义）——这才是 inline 最常见的用途。
    std::printf("\n(头文件里的自由函数请写 inline，这是它最实际的用途)\n");
    return 0;
}
