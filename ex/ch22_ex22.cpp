// ch22_ex22.cpp —— 例 22-22：标准库自带的 concept（不用自己写 SFINAE 了）
// <concepts> 里的这些概念，等价于一大堆 enable_if 组合，但写起来是一个词。
#include <concepts>
#include <cstdio>
#include <functional>
#include <string>
#include <type_traits>
#include <vector>

// 1) 数值类概念
template <std::integral T>
const char* numeric_kind() {
    return "整数";
}
template <std::floating_point T>
const char* numeric_kind() {
    return "浮点";
}
template <typename T>
    requires(!std::integral<T> && !std::floating_point<T>)
const char* numeric_kind() {
    return "既不是整数也不是浮点";
}

// 2) std::same_as / std::convertible_to
template <typename T>
    requires std::same_as<T, bool>
const char* bool_text(T v) {
    return v ? "true" : "false";
}

// 3) std::totally_ordered：能比大小就能排序
template <std::totally_ordered T>
T max_of(const std::vector<T>& v) {
    T best = v[0];
    for (const auto& x : v) {
        if (x > best) {
            best = x;
        }
    }
    return best;
}

// 4) std::invocable / std::predicate：回调是否可调用、返回值能否当条件
template <typename F>
    requires std::invocable<F, int>
int apply_twice(F f, int x) {
    return f(f(x));
}

// 5) std::regular：像 int 一样"能拷贝、能比较、能默认构造"
template <std::regular T>
void describe_regular() {
    T a{};
    T b = a;
    b = a;  // 用一下 b，避免"未引用变量"警告
    std::printf("   regular: 可默认构造、可拷贝、可相等比较，%zu 字节\n", sizeof(T));
}

int main() {
    std::printf("numeric_kind<int>()      = %s\n", numeric_kind<int>());
    std::printf("numeric_kind<double>()   = %s\n", numeric_kind<double>());
    std::printf("numeric_kind<std::string>() = %s\n", numeric_kind<std::string>());

    std::printf("bool_text(true)          = %s\n", bool_text(true));

    std::printf("max_of({3,9,4,1,7})      = %d\n", max_of(std::vector<int>{3, 9, 4, 1, 7}));
    std::printf("max_of({2.5,1.5,9.25})   = %g\n", max_of(std::vector<double>{2.5, 1.5, 9.25}));

    std::printf("apply_twice(x*x, 3)      = %d\n", apply_twice([](int v) { return v * v; }, 3));

    std::printf("describe_regular<int>():\n");
    describe_regular<int>();
    std::printf("describe_regular<double>():\n");
    describe_regular<double>();

        // 这些概念的"老写法"分别是：
        // std::integral<T>          <-> std::enable_if_t<std::is_integral_v<T>, int> = 0
        // std::same_as<T, bool>     <-> std::enable_if_t<std::is_same_v<T, bool>, int> = 0
        // std::invocable<F, int>    <-> 旧写法要靠 SFINAE 探测 std::declval<F>()(std::declval<int>())
    std::printf("sizeof(std::string)=%zu（std::string 不满足 std::regular 吗？满足，只是本例没调）\n",
                sizeof(std::string));
    return 0;
}
