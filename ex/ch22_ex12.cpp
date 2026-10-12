// ch22_ex12.cpp —— 例 22-12：if constexpr 替代"模板特化"当递归出口
// 例 22-5 里用全特化当出口；有了 if constexpr，同样的递归可以写成一个函数，
// 不用再单独写一个特化版本。
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <string>
#include <type_traits>

// 1) 编译期求和：1+2+...+N
template <unsigned N>
constexpr unsigned long long sum_to() {
    if constexpr (N == 0) {
        return 0ULL;  // 出口：这一支在 N>0 时被丢弃，不会造成无限递归
    } else {
        return static_cast<unsigned long long>(N) + sum_to<N - 1>();
    }
}

// 2) 打印任意个参数：不再需要写一个"零参数的兜底重载"
template <typename T, typename... Rest>
void print_cx(const T& first, const Rest&... rest) {
    std::cout << first;
    if constexpr (sizeof...(rest) > 0) {
        std::cout << ", ";
        print_cx(rest...);  // 只有在还有参数时这一行才被实例化
    }
}

// 3) 同一份代码按类型分支：转成字符串
template <typename T>
std::string to_text(const T& v) {
    if constexpr (std::is_same_v<T, bool>) {
        return v ? "true" : "false";
    } else if constexpr (std::is_integral_v<T>) {
        return std::to_string(v);
    } else if constexpr (std::is_floating_point_v<T>) {
        return std::to_string(v);
    } else {
        return std::string(v);  // 假定是字符串类
    }
}

int main() {
    static_assert(sum_to<0>() == 0ULL);
    static_assert(sum_to<10>() == 55ULL);
    static_assert(sum_to<100>() == 5050ULL);

    std::printf("sum_to<10>()  = %llu\n", sum_to<10>());
    std::printf("sum_to<100>() = %llu（全部在编译期算完）\n", sum_to<100>());

    std::cout << "print_cx : ";
    print_cx(1, 2.5, "three");
    std::cout << '\n';

    std::cout << "print_cx(单个) : ";
    print_cx("only one");  // sizeof...(rest) == 0，递归那一支被丢弃
    std::cout << '\n';

    std::printf("to_text(true)  = %s\n", to_text(true).c_str());
    std::printf("to_text(255)   = %s\n", to_text(255).c_str());
    std::printf("to_text(1.5)   = %s\n", to_text(1.5).c_str());
    std::printf("to_text(\"hi\") = %s\n", to_text(std::string("hi")).c_str());
    return 0;
}
