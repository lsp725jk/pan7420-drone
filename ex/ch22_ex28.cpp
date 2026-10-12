// ch22_ex28.cpp —— 例 22-28：自己动手写一个 trait（主模板 + 偏特化 + 变量模板）
// 标准库的 is_pointer 就是这么实现的。看懂这一段，标准库里几百个 trait 都能看懂。
#include <cstdio>
#include <type_traits>

// -------- 主模板：默认"不是指针" --------
template <typename T>
struct my_is_pointer : std::false_type {};

// -------- 偏特化：只有当 T 真的是 T* 形状时才匹配 --------
template <typename T>
struct my_is_pointer<T*> : std::true_type {};
// const/volatile 也会被 T 吃掉，所以下面几个写法其实匹配的是 T* const 等
template <typename T>
struct my_is_pointer<T* const> : std::true_type {};
template <typename T>
struct my_is_pointer<T* volatile> : std::true_type {};
template <typename T>
struct my_is_pointer<T* const volatile> : std::true_type {};

// -------- C++17 变量模板：把 ::value 省掉 --------
template <typename T>
inline constexpr bool my_is_pointer_v = my_is_pointer<T>::value;

// -------- 再实现一个 remove_pointer --------
template <typename T>
struct my_remove_pointer {
    using type = T;
};
template <typename T>
struct my_remove_pointer<T*> {
    using type = T;
};
template <typename T>
using my_remove_pointer_t = typename my_remove_pointer<T>::type;

// -------- 和标准库逐项对照 --------
#define SAME_AS_STD(TYPE)                                                          \
    static_assert(my_is_pointer<TYPE>::value == std::is_pointer<TYPE>::value,      \
                  "和标准库不一致: " #TYPE)

SAME_AS_STD(int);
SAME_AS_STD(int*);
SAME_AS_STD(const int*);
SAME_AS_STD(int* const);
SAME_AS_STD(int**);
SAME_AS_STD(double);
SAME_AS_STD(void*);
SAME_AS_STD(int&);

static_assert(std::is_same<my_remove_pointer_t<int*>, int>::value, "");
static_assert(std::is_same<my_remove_pointer_t<const char*>, const char>::value, "");
static_assert(std::is_same<my_remove_pointer_t<int>, int>::value, "不是指针就原样返回");

int main() {
    std::printf("%-16s %-12s %-12s\n", "类型", "my_is_pointer", "std::is_pointer");
    std::printf("------------------------------------------\n");
    std::printf("%-16s %-12d %-12d\n", "int", (int)my_is_pointer_v<int>, (int)std::is_pointer_v<int>);
    std::printf("%-16s %-12d %-12d\n", "int*", (int)my_is_pointer_v<int*>,
                (int)std::is_pointer_v<int*>);
    std::printf("%-16s %-12d %-12d\n", "const int*", (int)my_is_pointer_v<const int*>,
                (int)std::is_pointer_v<const int*>);
    std::printf("%-16s %-12d %-12d\n", "int* const", (int)my_is_pointer_v<int* const>,
                (int)std::is_pointer_v<int* const>);
    std::printf("%-16s %-12d %-12d\n", "int&", (int)my_is_pointer_v<int&>,
                (int)std::is_pointer_v<int&>);
    std::printf("%-16s %-12d %-12d\n", "int[3]", (int)my_is_pointer_v<int[3]>,
                (int)std::is_pointer_v<int[3]>);

    std::printf("\nremove_pointer:\n");
    std::printf("  int*        -> 去掉指针后是 int，%s\n",
                my_is_pointer_v<my_remove_pointer_t<int*>> ? "还是指针" : "不再是指针");
    std::printf("  const char* -> 去掉指针后大小 %zu 字节\n",
                sizeof(my_remove_pointer_t<const char*>));
    std::printf("  double      -> 去掉指针后还是 double（%zu 字节）\n",
                sizeof(my_remove_pointer_t<double>));

        // 坑：变量模板要写 inline（C++17 起），否则在多个 .cpp 里包含这个头文件时
        // 会出现重复定义（链接错误 LNK2005）。C++17 之前的写法是
        // 把它放进一个 struct 里当 static constexpr 成员。
    std::printf("sizeof(my_is_pointer<int>) = %zu（空基类，1 字节）\n", sizeof(my_is_pointer<int>));
    return 0;
}
