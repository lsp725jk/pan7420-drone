// ch22_ex06.cpp —— 例 22-6：变参模板与折叠表达式（C++17）
// "参数个数也不定"的场景：printf 靠 ... 和 va_list，代价是没有类型检查。
// 变参模板把这件事变成类型安全的。
#include <cstdio>
#include <iostream>
#include <string>

// sizeof... 运算符：数一数包里有几个参数（编译期常量）
template <typename... Ts>
constexpr std::size_t count_args(const Ts&...) {
    return sizeof...(Ts);
}

// 折叠表达式（fold expression）：把一包参数用同一个运算符"摊平"
// (vs + ... + 0) 展开成 0 + v1 + v2 + v3 ...
template <typename... Ts>
auto total(const Ts&... vs) {
    return (vs + ... + 0LL);  // 加初值 0LL，空参数包也合法
}

// 变式：C++17 之前只能靠"递归 + 一个同名非模板函数兜底"
void print_rec() {
    std::cout << '\n';
}
template <typename T, typename... Rest>
void print_rec(const T& first, const Rest&... rest) {
    std::cout << first << ' ';
    print_rec(rest...);  // 每次剥掉第一个参数
}

// 折叠表达式版：不需要兜底函数
template <typename... Ts>
void print_fold(const Ts&... vs) {
    ((std::cout << vs << ' '), ...);  // 逗号折叠：从左到右依次求值
    std::cout << '\n';
}

int main() {
    std::printf("count_args()          = %zu\n", count_args());
    std::printf("count_args(1,2,3)     = %zu\n", count_args(1, 2, 3));
    std::printf("count_args(1,'a',2.5) = %zu\n", count_args(1, 'a', 2.5));

    std::printf("total(1,2,3,4)        = %lld\n", total(1, 2, 3, 4));
    std::printf("total(1,2.5)          = %g  (返回类型也被推导成 double)\n", total(1, 2.5));

        // 返回类型是推导出来的：拿 static_assert 一看就清楚
    static_assert(std::is_same<decltype(total(1, 2, 3, 4)), long long>::value,
                  "整数包 + 0LL 初值 -> long long");
    static_assert(std::is_same<decltype(total(1, 2.5)), double>::value,
                  "包里有 double -> double");

    std::cout << "递归版 : ";
    print_rec(1, 2.5, "three", std::string("four"));

    std::cout << "折叠版 : ";
    print_fold(1, 2.5, "three", std::string("four"));
    return 0;
}
