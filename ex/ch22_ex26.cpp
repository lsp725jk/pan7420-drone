// ch22_ex26.cpp —— 例 22-26：类型特征（type traits）四件套
// 它们回答的问题都是"编译期的一个是/否 或 一个类型"，所以能直接喂给 if constexpr。
#include <cstddef>
#include <cstdio>
#include <string>
#include <type_traits>

// 打印一张对照表：同一个问题在不同类型上的答案
template <typename T>
void row(const char* name) {
    std::printf("%-16s %-8d %-10d %-9d %-10d %-9d\n", name,
                (int)std::is_same<T, int>::value,       // 一模一样吗
                (int)std::is_integral<T>::value,        // 是整数吗
                (int)std::is_pointer<T>::value,         // 是指针吗
                (int)std::is_reference<T>::value,       // 是引用吗
                (int)std::is_const<T>::value);          // 带 const 吗
}

int main() {
    std::printf("%-16s %-8s %-10s %-9s %-10s %-9s\n", "类型", "same<int>", "integral",
                "pointer", "reference", "const");
    std::printf("--------------------------------------------------------------------\n");
    row<int>("int");
    row<const int>("const int");
    row<int&>("int&");
    row<const int&>("const int&");
    row<int*>("int*");
    row<double>("double");
    row<std::string>("std::string");

    std::printf("\n-- 为什么需要 remove_* / decay --\n");
        // is_integral<int&> 是 false！引用/const 会让判断"跑偏"，
        // 所以模板里一般先把修饰去掉再判断。
    static_assert(std::is_integral<int>::value, "");
    static_assert(!std::is_integral<int&>::value, "引用类型不算整数");
    static_assert(std::is_integral<std::remove_reference_t<int&>>::value, "");
    static_assert(std::is_integral<std::remove_cv_t<const int>>::value, "");
    static_assert(std::is_integral<std::decay_t<const int&>>::value, "");
    std::printf("is_integral<int&>          = %d\n", (int)std::is_integral<int&>::value);
    std::printf("is_integral<remove_ref_t>  = %d\n",
                (int)std::is_integral<std::remove_reference_t<int&>>::value);

    std::printf("\n-- decay 到底做了什么（模拟按值传参时的类型变化）--\n");
    std::printf("decay<const int&>   == int         ? %d\n",
                (int)std::is_same<std::decay_t<const int&>, int>::value);
    std::printf("decay<int[3]>       == int*        ? %d\n",
                (int)std::is_same<std::decay_t<int[3]>, int*>::value);
    std::printf("decay<int(int)>     == int(*)(int) ? %d\n",
                (int)std::is_same<std::decay_t<int(int)>, int (*)(int)>::value);
    std::printf("remove_reference<const int&> == const int ? %d\n",
                (int)std::is_same<std::remove_reference_t<const int&>, const int>::value);
    std::printf("remove_cvref<const int&>     == int       ? %d  (C++20)\n",
                (int)std::is_same<std::remove_cvref_t<const int&>, int>::value);
    std::printf("remove_cv<const volatile int> == int      ? %d\n",
                (int)std::is_same<std::remove_cv_t<const volatile int>, int>::value);

        // 实用场景：模板里想"按值存一份"，就得先 decay，否则会存成引用，悬空
    std::printf("\nsizeof(std::string)=%zu\n", sizeof(std::string));
    return 0;
}
