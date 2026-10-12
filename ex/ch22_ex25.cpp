// ch22_ex25.cpp —— 例 22-25：用 std::true_type / std::false_type 当标签
// 除了迭代器标签，标准库还给了两个"布尔标签"类型，
// 把"是/不是"从值变成类型，就能用重载来分发。
#include <cstdio>
#include <string>
#include <type_traits>

// -------- 两个实现，靠标签类型区分 --------
template <typename T>
T half_impl(T v, std::true_type) {  // 整数：整除
    return static_cast<T>(v / 2);
}

template <typename T>
T half_impl(T v, std::false_type) {  // 浮点：乘 0.5
    return static_cast<T>(v * 0.5);
}

// -------- 分发器 --------
template <typename T>
T half(T v) {
    return half_impl(v, std::is_integral<T>{});
        // ^^^^^^^^^^^^^^^^^^^ 注意：传的是**类型**，
        // 不是 std::is_integral<T>::value（那是 bool 值）
}

// -------- 变式：同样的事用 if constexpr 写 --------
template <typename T>
T half_cx(T v) {
    if constexpr (std::is_integral<T>::value) {
        return static_cast<T>(v / 2);
    } else {
        return static_cast<T>(v * 0.5);
    }
}

// -------- 变式：用 concept 写 --------
template <typename T>
    requires std::is_integral_v<T>
T half_cpt(T v) {
    return static_cast<T>(v / 2);
}
template <typename T>
    requires(!std::is_integral_v<T>)
T half_cpt(T v) {
    return static_cast<T>(v * 0.5);
}

int main() {
    std::printf("half(7)     = %d   （整除，丢掉余数）\n", half(7));
    std::printf("half(7.0)   = %g\n", half(7.0));
    std::printf("half(-7)    = %d\n", half(-7));

    std::printf("half_cx(7)  = %d\n", half_cx(7));
    std::printf("half_cx(7.0)= %g\n", half_cx(7.0));

    std::printf("half_cpt(7)   = %d\n", half_cpt(7));
    std::printf("half_cpt(7.0) = %g\n", half_cpt(7.0));

        // 坑：标签必须是**类型**。
        // 写 half_impl(v, std::is_integral<T>::value) 传进去的是 bool，
        // 两个重载都收不下（或者更糟：被隐式转成 int 后产生歧义），
        // 典型报错是 "error C2668: 对重载函数的调用不明确" 或 "无法推导模板参数"。
    std::printf("三种写法的取舍：\n");
    std::printf("  标签分发  —— C++11 起可用，老代码/库代码里最常见\n");
    std::printf("  if constexpr —— C++17 起，逻辑集中在一个函数里，最好读\n");
    std::printf("  concept   —— C++20 起，能同时约束接口和给出好报错\n");
    return 0;
}
