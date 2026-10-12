// ch22_ex19.cpp —— 例 22-19：void_t 实战——"能打印就打印，不能打印就说明原因"
// 这在调试库、日志库里非常常见：不想为每种类型手写重载，也不想编不过。
#include <cstdio>
#include <iostream>
#include <ostream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

// 探测：std::cout << v 合法吗？
template <typename T, typename = void>
struct is_printable : std::false_type {};
template <typename T>
struct is_printable<T, std::void_t<decltype(std::declval<std::ostream&>()
                                            << std::declval<const T&>())>> : std::true_type {};

// 探测：两个 T 能不能相加
template <typename T, typename = void>
struct is_addable : std::false_type {};
template <typename T>
struct is_addable<T, std::void_t<decltype(std::declval<const T&>() + std::declval<const T&>())>>
    : std::true_type {};

struct Point {
    int x, y;
};
std::ostream& operator<<(std::ostream& os, const Point& p) {
    return os << "Point(" << p.x << "," << p.y << ")";
}

struct Opaque {
    int secret;
};

template <typename T>
void show(const char* name, const T& v) {
    if constexpr (is_printable<T>::value) {
        std::cout << name << " 可打印 -> " << v << '\n';
    } else {
        std::cout << name << " 不可打印（没有匹配的 operator<<），跳过\n";
    }
}

int main() {
    show("int           ", 42);
    show("double        ", 3.5);
    show("std::string   ", std::string("hello"));
    show("Point         ", Point{1, 2});
    show("vector<int>   ", std::vector<int>{1, 2, 3});
    show("Opaque        ", Opaque{7});

    std::printf("is_addable<int>    = %d\n", (int)is_addable<int>::value);
    std::printf("is_addable<Point>  = %d\n", (int)is_addable<Point>::value);
    std::printf("is_addable<Opaque> = %d\n", (int)is_addable<Opaque>::value);

        // 坑：vector<int> 在 is_printable 上是 false，但它**可以**被隐式转成别的？
        // 不会。探测的是"表达式是否合法"，不是"能不能转"。
    std::printf("is_printable<void*> = %d（任何指针都能走 operator<<(const void*)）\n",
                (int)is_printable<void*>::value);
    return 0;
}
