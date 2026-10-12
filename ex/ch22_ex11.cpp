// ch22_ex11.cpp —— 例 22-11：if constexpr——让"分支"在编译期就被砍掉
// 问题：模板函数里写普通 if，两个分支**都**要能编译通过，
// 可 T=int 时 "把 v 当指针用"这件事根本没法编译。
#include <cstdio>
#include <type_traits>

// 下面这个朴素写法编不过（T=int 时 (void*)v 非法），所以只能注释掉：

// template <typename T>
// void show_bad(T v) {
// if (std::is_pointer<T>::value) {
// std::printf("指针 %p\n", (void*)v);   // error C2440: 无法从 int 转换为 void*
// } else {
// std::printf("值 %d\n", (int)v);
// }
// }

// 原因：普通 if 是**运行期**分支，编译器必须把两条路都生成出来。

// 正确写法：if constexpr —— 条件为假的那一支在编译期被"丢弃"
template <typename T>
void show(T v) {
    if constexpr (std::is_pointer<T>::value) {
        std::printf("指针  : %p -> %d\n", static_cast<const void*>(v), *v);
    } else if constexpr (std::is_integral<T>::value) {
        std::printf("整数  : %lld\n", static_cast<long long>(v));
    } else if constexpr (std::is_floating_point<T>::value) {
        std::printf("浮点  : %g\n", static_cast<double>(v));
    } else {
        std::printf("其它  : 大小 %zu 字节\n", sizeof(T));
    }
}

struct Point {
    int x, y;
};

int main() {
    int n = 7;
    show(42);        // T = int
    show(&n);        // T = int*
    show(3.5);       // T = double
    show('A');       // T = char（整数一族）
    show(Point{1, 2});  // T = Point，走到"其它"

        // 重要的取舍：
        // if constexpr 必须写在**模板**里，条件必须是编译期常量；
        // 普通 if 里的分支永远都会被编译，只是运行时不走。
    std::printf("sizeof(show) 这个函数模板按用到的类型各生成一份\n");
    return 0;
}
