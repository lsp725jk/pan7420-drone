// ch22_ex07.cpp —— 例 22-7：std::index_sequence——"编译期 for 循环"
// 模板里没有 for。想对"一包东西"逐个处理，标准库给了 index_sequence：
// 它把 0,1,2,...,N-1 变成一包编译期常量，用折叠表达式一展开就是循环。
#include <array>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <tuple>
#include <utility>

// 用法 1：把 tuple 里的每个元素都打印出来
template <typename Tuple, std::size_t... I>
void print_tuple_impl(const Tuple& t, std::index_sequence<I...>) {
    ((std::cout << (I == 0 ? "" : ", ") << std::get<I>(t)), ...);
    std::cout << '\n';
}

template <typename Tuple>
void print_tuple(const Tuple& t) {
        // make_index_sequence<N> 生成 0..N-1；N 由 tuple 的元素个数在编译期给出
    print_tuple_impl(t, std::make_index_sequence<std::tuple_size<Tuple>::value>{});
}

// 用法 2：用 index_sequence 在编译期生成一张表（下一章的主角）
template <typename F, std::size_t... I>
constexpr auto make_table_impl(F f, std::index_sequence<I...>) {
    return std::array<int, sizeof...(I)>{f(static_cast<int>(I))...};
}

template <std::size_t N, typename F>
constexpr auto make_table(F f) {
    return make_table_impl(f, std::make_index_sequence<N>{});
}

int main() {
    auto t = std::make_tuple(42, 3.5, "hello", 'x');
    std::cout << "tuple = ";
    print_tuple(t);

        // 表在编译期就算好了，运行期只是一次数组读取
    constexpr auto squares = make_table<8>([](int i) { return i * i; });
    static_assert(squares[5] == 25, "5*5 应该等于 25");

    std::printf("squares =");
    for (int v : squares) {
        std::printf(" %d", v);
    }
    std::printf("\n");
    std::printf("sizeof(squares) = %zu 字节，全部在 .rdata/.rodata 里\n", sizeof(squares));
    static_assert(std::tuple_size<decltype(t)>::value == 4, "元组有 4 个元素");
    return 0;
}
