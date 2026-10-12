// ch22_ex23.cpp —— 例 22-23：把 SFINAE 改写成 concepts，看看能省多少事
// 同一个需求写两遍，直接对比可读性。
#include <concepts>
#include <cstdio>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

// ==================================================================
// 需求 1：只接受整数
// ==================================================================
// 旧写法（SFINAE）：一行的条件被拆成三处——template、typename、::type = 0
template <typename T, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
void print_old(T v) {
    std::printf("旧写法: %lld\n", static_cast<long long>(v));
}

// 新写法（concept）：条件就是条件
template <std::integral T>
void print_new(T v) {
    std::printf("新写法: %lld\n", static_cast<long long>(v));
}

// ==================================================================
// 需求 2：探测"有没有 size()"
// ==================================================================
// 旧写法：主模板 + 偏特化 + void_t + declval，四段代码
template <typename T, typename = void>
struct has_size_old : std::false_type {};
template <typename T>
struct has_size_old<T, std::void_t<decltype(std::declval<const T&>().size())>> : std::true_type {};

// 新写法：一个 concept，三行
template <typename T>
concept HasSize = requires(const T& t) { t.size(); };

// ==================================================================
// 需求 3：既要是算术类型，又要能打印出来
// ==================================================================
// 旧写法（条件组合）
template <typename T,
          typename std::enable_if<std::is_arithmetic<T>::value, int>::type = 0,
          typename = decltype(std::declval<std::FILE*>() , void())>
void show_old(T v) {
    std::printf("旧: %g\n", static_cast<double>(v));
}
// 新写法（concept 组合）
template <typename T>
concept PrintableArithmetic = std::is_arithmetic_v<T>;

template <PrintableArithmetic T>
void show_new(T v) {
    std::printf("新: %g\n", static_cast<double>(v));
}

int main() {
    print_old(1);
    print_new(2);

    std::printf("has_size_old<std::string> = %d\n", (int)has_size_old<std::string>::value);
    std::printf("has_size_old<int>         = %d\n", (int)has_size_old<int>::value);
    std::printf("HasSize<std::string>      = %d\n", (int)HasSize<std::string>);
    std::printf("HasSize<int>              = %d\n", (int)HasSize<int>);

    show_old(1.5);
    show_new(2.5);

        // 真正值钱的地方在报错信息上（下一节贴实测对比）：
        // 旧写法报的是 "error C2672: 找不到匹配的重载函数"，
        // 新写法报的是 "约束不满足: std::integral<double>"，直接点名原因。
    std::printf("两种写法生成的目标代码完全一样，区别只在人读起来累不累\n");
    static_assert(HasSize<std::vector<int>>);
    static_assert(!HasSize<int>);
    return 0;
}
