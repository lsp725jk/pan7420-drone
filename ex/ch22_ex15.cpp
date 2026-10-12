// ch22_ex15.cpp —— 例 22-15：std::enable_if 的三种写法与取舍
// enable_if<条件, 类型>::type 的行为：
// 条件为真 -> 就是"类型"
// 条件为假 -> 根本没有 ::type 这个成员 -> 替换失败 -> 候选被剔除（SFINAE）
#include <cstdio>
#include <string>
#include <type_traits>

// ---------------- 写法 A：放在返回类型上（最传统，C++11 常见） ----------------
template <typename T>
typename std::enable_if<std::is_integral<T>::value, void>::type print_a(T v) {
    std::printf("A(返回类型) : 整数 %lld\n", static_cast<long long>(v));
}

// ---------------- 写法 B：放在多出来的模板参数上（推荐） ----------------
// 好处：不动返回类型，函数签名干净；也不会被调用者"手动指定"绕过
template <typename T, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
void print_b(T v) {
    std::printf("B(模板参数) : 整数 %lld\n", static_cast<long long>(v));
}

// ---------------- 写法 C：放在函数参数上（写法最像普通函数） ----------------
// 坑：调用者可以显式写出第二个实参，从而绕过约束
template <typename T>
void print_c(T v, std::enable_if_t<std::is_integral<T>::value, int> = 0) {
    std::printf("C(函数参数) : 整数 %lld\n", static_cast<long long>(v));
}

// C++17 简写：enable_if_t / is_integral_v 省掉一堆 ::type 和 ::value
template <typename T>
std::enable_if_t<std::is_floating_point_v<T>, void> print_f(T v) {
    std::printf("浮点版本    : %g\n", static_cast<double>(v));
}

// 组合条件：是整数且不是 bool
template <typename T>
std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, bool>, void> print_not_bool(T v) {
    std::printf("非 bool 整数: %lld\n", static_cast<long long>(v));
}

int main() {
    print_a(1);
    print_b(2);
    print_c(3);
    print_f(4.5);
    print_not_bool(5);

        // 下面这些一旦取消注释，就会因为"没有可用的重载"而编译失败：
        // print_a(1.5);          // error C2672: 找不到匹配的重载函数
        // print_b(std::string("x"));
        // print_f(3);            // T=int 不满足 is_floating_point

        // 坑：条件不依赖模板参数时，enable_if 不是"剔除候选"而是"直接报错"
        // template <typename T> std::enable_if_t<false, void> never(T) {}
        // -> error C2938: "std::enable_if_t<false,void>": 未能使别名模板专用化

        // 坑：写法 C 能被绕过
    print_c(6, 0);  // 手动传第二个参数，约束等于摆设（但仍然要求 T 是整数）
    std::printf("三种写法都能工作；选 B 最不容易出问题\n");
    return 0;
}
