// ch22_ex05.cpp —— 例 22-5：模板元编程的起点——用"类模板递归"在编译期算数
// 思路：把"计算"表达成"类型的嵌套"。Fact<5> 里套着 Fact<4>，一路套到 Fact<0>，
// 而 Fact<0> 用**全特化**给出终止值。编译器展开类型的过程，就是求值的过程。
#include <cstdio>

// 主模板（primary template）：N! = N * (N-1)!
template <unsigned N>
struct Fact {
    static constexpr unsigned long long value =
        static_cast<unsigned long long>(N) * Fact<N - 1>::value;
};

// 全特化（explicit specialization）：递归的出口，必须有，否则永远展开不完
template <>
struct Fact<0> {
    static constexpr unsigned long long value = 1ULL;
};

// 变式：求和 1+2+...+N，出口放在 N==1
template <unsigned N>
struct SumTo {
    static constexpr unsigned long long value =
        static_cast<unsigned long long>(N) + SumTo<N - 1>::value;
};
template <>
struct SumTo<1> {
    static constexpr unsigned long long value = 1ULL;
};

// 对照写法：C++11 起，同样的事用 constexpr 函数做，可读性好得多
constexpr unsigned long long fact_fn(unsigned n) {
    unsigned long long r = 1;
    for (unsigned i = 2; i <= n; ++i) {
        r *= i;
    }
    return r;
}

int main() {
        // static_assert 里能用，说明它真的是编译期常量
    static_assert(Fact<5>::value == 120ULL, "5! 应该是 120");

    std::printf("Fact<5>::value  = %llu   (编译期)\n", Fact<5>::value);
    std::printf("Fact<10>::value = %llu   (编译期)\n", Fact<10>::value);
    std::printf("Fact<20>::value = %llu   (编译期)\n", Fact<20>::value);
    std::printf("SumTo<100>::value = %llu  (编译期)\n", SumTo<100>::value);
    std::printf("fact_fn(20)     = %llu   (也是编译期，见 static_assert)\n", fact_fn(20));
    static_assert(fact_fn(20) == Fact<20>::value, "两种写法结果必须一样");

        // 注意：模板递归的深度是编译期开销。Fact<100> 会因为 unsigned long long 溢出
        // 而算错（不会报错），SumTo<1000> 可能触发模板深度限制。
    return 0;
}
