// ch22_ex21.cpp —— 例 22-21：把 concept 用到函数和类上（requires 子句 / 简写模板）
#include <concepts>
#include <cstddef>
#include <cstdio>
#include <string>
#include <type_traits>

// ---------------- 写法 1：requires 子句（写在模板头后面） ----------------
template <typename T>
    requires std::integral<T>
T twice(T v) {
    return static_cast<T>(v + v);
}

// ---------------- 写法 2：尾置 requires 子句 ----------------
template <typename T>
T twice2(T v)
    requires std::integral<T>
{
    return static_cast<T>(v + v);
}

// ---------------- 写法 3：简写函数模板（abbreviated function template） ----------------
// 最像普通函数的写法，C++20 起可用
std::integral auto twice3(std::integral auto v) {
    return v + v;
}

// ---------------- 写法 4：约束类模板 ----------------
template <std::integral T>
struct Counter {
    T v{};
    void bump() { ++v; }
};

// ---------------- 写法 5：requires requires（子句 + 表达式连着写） ----------------
template <typename T>
    requires requires(const T& t) { t.size(); }
std::size_t len(const T& t) {
    return t.size();
}

// ---------------- 组合：自己定义一个新 concept ----------------
template <typename T>
concept SmallIntegral = std::integral<T> && (sizeof(T) <= 4);

template <SmallIntegral T>
const char* width_name() {
    return "不超过 4 字节的整数";
}

int main() {
    std::printf("twice(21)   = %d\n", twice(21));
    std::printf("twice2(21)  = %d\n", twice2(21));
    std::printf("twice3(21)  = %d\n", twice3(21));

    Counter<int> c;
    c.bump();
    c.bump();
    std::printf("Counter<int>::v = %d\n", c.v);

    std::printf("len(std::string) = %zu\n", len(std::string("hello")));

    std::printf("width_name<int>()       -> %s\n", width_name<int>());
    std::printf("width_name<short>()     -> %s\n", width_name<short>());

        // 下面这些一旦取消注释，报错信息会直接说"约束不满足"，比 SFINAE 好读得多：
        // twice(1.5);            // 约束不满足: std::integral<double>
        // Counter<double> cd;    // 约束不满足: std::integral<double>
        // width_name<long long>(); // 约束不满足: SmallIntegral<long long>
    std::printf("sizeof(long long)=%zu\n", sizeof(long long));
    return 0;
}
