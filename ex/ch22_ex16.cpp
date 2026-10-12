// ch22_ex16.cpp —— 例 22-16：用 SFINAE 写"类型探测器"（traits）
// 需求：程序里想根据"这个类型有没有某个成员/某个操作"走不同的路。
// 语言本身不提供这种查询，SFINAE 提供：写一个偏特化版本，
// 让它"只有在替换成功时才存在"。
#include <cstdio>
#include <string>
#include <type_traits>
#include <vector>

// 探测 1：有没有成员函数 size()
template <typename T, typename = void>
struct has_size : std::false_type {};
template <typename T>
struct has_size<T, std::void_t<decltype(std::declval<const T&>().size())>> : std::true_type {};

// 探测 2：能不能做 a + b
template <typename T, typename = void>
struct has_plus : std::false_type {};
template <typename T>
struct has_plus<T, std::void_t<decltype(std::declval<const T&>() + std::declval<const T&>())>>
    : std::true_type {};

// 探测 3：有没有内嵌类型 value_type
template <typename T, typename = void>
struct has_value_type : std::false_type {};
template <typename T>
struct has_value_type<T, std::void_t<typename T::value_type>> : std::true_type {};

struct Point {
    int x, y;
};
struct Money {
    int cent;
};
Money operator+(const Money& a, const Money& b) { return Money{a.cent + b.cent}; }

// 让探测器真正派上用场
template <typename T>
void report(const char* name) {
    std::printf("%-16s size()=%d  +=%d  value_type=%d\n", name, (int)has_size<T>::value,
                (int)has_plus<T>::value, (int)has_value_type<T>::value);
}

// 综合用法：只在"能做加法"时才求和，否则直接返回 0
template <typename T>
T sum_all(const T* p, std::size_t n) {
    if constexpr (has_plus<T>::value) {
        T acc = p[0];
        for (std::size_t i = 1; i < n; ++i) {
            acc = acc + p[i];
        }
        return acc;
    } else {
        (void)p;
        (void)n;
        return T{};
    }
}

int main() {
    report<int>("int");
    report<std::string>("std::string");
    report<std::vector<int>>("std::vector<int>");
    report<Point>("Point");
    report<Money>("Money");

    Money m[3] = {{100}, {250}, {30}};
    std::printf("sum_all<Money> = %d 分\n", sum_all(m, 3).cent);

    int ia[3] = {10, 20, 30};
    std::printf("sum_all<int>   = %d  (int 也能加，正常求和)\n", sum_all(ia, 3));
    return 0;
}
