// ch22_ex20.cpp —— 例 22-20：concepts 的起点——requires 表达式
// requires 表达式就是"写一段看起来像代码的东西，编译器只回答：它能不能编译"。
// 注意：requires 表达式**不求值**，只是检查合法性。
#include <concepts>
#include <cstddef>
#include <cstdio>
#include <string>
#include <type_traits>
#include <vector>

// 1) 简单要求（simple requirement）：只要 t.size() 能编译就行
template <typename T>
concept HasSize = requires(const T& t) { t.size(); };

// 2) 类型要求（type requirement）：T 里必须有个叫 value_type 的类型
template <typename T>
concept HasValueType = requires { typename T::value_type; };

// 3) 复合要求（compound requirement）：还要检查返回类型
template <typename T>
concept Addable = requires(const T& a, const T& b) {
    { a + b } -> std::convertible_to<T>;
};

// 4) 嵌套要求（nested requirement）：在 requires 里再放一个编译期条件
template <typename T>
concept SmallInt = requires {
    requires std::is_integral_v<T> && (sizeof(T) <= 4);
};

struct Point {
    int x, y;
};
Point operator+(const Point& a, const Point& b) { return Point{a.x + b.x, a.y + b.y}; }

struct NoSize {
    int a;
};

template <typename T>
void report(const char* name) {
    std::printf("%-18s HasSize=%d HasValueType=%d Addable=%d SmallInt=%d\n", name,
                (int)HasSize<T>, (int)HasValueType<T>, (int)Addable<T>, (int)SmallInt<T>);
}

int main() {
    report<int>("int");
    report<long long>("long long");
    report<double>("double");
    report<std::string>("std::string");
    report<std::vector<int>>("std::vector<int>");
    report<Point>("Point");
    report<NoSize>("NoSize");

        // concept 可以直接用在 if constexpr 里（比 is_xxx::value 读起来顺）
    if constexpr (HasSize<std::string>) {
        std::printf("HasSize<std::string> 为真\n");
    }
    std::printf("sizeof(int)=%zu, SmallInt<int> 为真；SmallInt<long long> 为 %d\n", sizeof(int),
                (int)SmallInt<long long>);
    return 0;
}
