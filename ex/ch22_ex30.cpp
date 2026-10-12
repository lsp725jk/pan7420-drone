// ch22_ex30.cpp —— 例 22-30：CRTP 静态多态 vs 虚函数，实测差多少
// 虚函数：调用要走 vtable，编译器不知道最终调哪个函数，没法内联。
// CRTP  ：类型在编译期就确定，调用点能直接内联，编译器还能顺着手算下去。
#include <chrono>
#include <cstdio>

// -------- 虚函数版 --------
struct ShapeV {
    virtual ~ShapeV() = default;
    virtual int area(int x) const = 0;
};
struct SquareV final : ShapeV {
    int area(int x) const override { return x * x; }
};

// -------- CRTP 版 --------
template <typename D>
struct ShapeC {
    int area(int x) const { return static_cast<const D*>(this)->area_impl(x); }
};
struct SquareC final : ShapeC<SquareC> {
    int area_impl(int x) const { return x * x; }
};

// noinline 是为了不让编译器看穿"这里 new 的就是 SquareV"，
// 否则它会把虚调用去虚化（devirtualize），对比就不公平了。
__declspec(noinline) ShapeV* make_shape() {
    return new SquareV();
}

int main() {
    const int N = 20000000;
    volatile int seed_in = 3;  // volatile：编译器不知道它的值
    const int seed = seed_in;

        // ---- 虚函数 ----
    ShapeV* p = make_shape();
    long long acc1 = 0;
    const auto t0 = std::chrono::steady_clock::now();
    for (int i = 0; i < N; ++i) {
        acc1 ^= p->area(i + seed);
    }
    const auto t1 = std::chrono::steady_clock::now();

        // ---- CRTP ----
    SquareC s;
    long long acc2 = 0;
    const auto t2 = std::chrono::steady_clock::now();
    for (int i = 0; i < N; ++i) {
        acc2 ^= s.area(i + seed);
    }
    const auto t3 = std::chrono::steady_clock::now();

    const double ms_v = std::chrono::duration<double, std::milli>(t1 - t0).count();
    const double ms_c = std::chrono::duration<double, std::milli>(t3 - t2).count();

    std::printf("调用次数            : %d\n", N);
    std::printf("虚函数版            : %.1f ms  (%.2f ns/次)  校验和 %lld\n", ms_v,
                ms_v * 1e6 / N, acc1);
    std::printf("CRTP 版             : %.1f ms  (%.2f ns/次)  校验和 %lld\n", ms_c,
                ms_c * 1e6 / N, acc2);
    std::printf("sizeof(SquareV)     : %zu 字节（多一个 vptr）\n", sizeof(SquareV));
    std::printf("sizeof(SquareC)     : %zu 字节（空基类优化，一个字节都不多占）\n",
                sizeof(SquareC));
    std::printf("两个校验和必须一样: %d\n", (int)(acc1 == acc2));

        // 注意：这里用的是 check.bat 的默认优化等级（/Od，相当于 GCC 的 -O0）。
        // /Od 下两者都是"没优化"的笨代码，差距被放大了。
        // /O2 下的对比见下一章的实测表。
    delete p;
    return 0;
}
