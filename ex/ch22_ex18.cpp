// ch22_ex18.cpp —— 例 22-18：std::void_t——写类型探测器的标准零件
// void_t<Ts...> 永远等于 void，它的价值在"求值过程中"：
// 如果 Ts... 里任何一个表达式替换失败，这个偏特化就不成立 -> 回落到主模板。
#include <cstddef>
#include <cstdio>
#include <string>
#include <type_traits>
#include <vector>

// C++17 之前要自己写（这是修正了 CWG1558 之后的写法）
template <typename...>
struct my_void_t {
    using type = void;
};
template <typename... Ts>
using my_void = typename my_void_t<Ts...>::type;

// 主模板：默认"没有"
template <typename T, typename = void>
struct has_value_type : std::false_type {};

// 偏特化：只有当 T::value_type 存在时，第二个实参才能替换成 void，这个版本才成立
template <typename T>
struct has_value_type<T, std::void_t<typename T::value_type>> : std::true_type {};

// 同样套路：有没有 difference_type / 有没有 operator[]
template <typename T, typename = void>
struct has_subscript : std::false_type {};
template <typename T>
struct has_subscript<T, std::void_t<decltype(std::declval<const T&>()[std::size_t{0}])>>
    : std::true_type {};

// 手写版和标准库版必须一致
static_assert(has_value_type<std::vector<int>>::value, "vector 有 value_type");
static_assert(std::is_same<my_void<int, double>, void>::value, "my_void 化简成 void");

struct NoValueType {
    int a;
};

int main() {
    std::printf("has_value_type<vector<int>>  = %d\n", (int)has_value_type<std::vector<int>>::value);
    std::printf("has_value_type<std::string>  = %d\n", (int)has_value_type<std::string>::value);
    std::printf("has_value_type<int>          = %d\n", (int)has_value_type<int>::value);
    std::printf("has_value_type<NoValueType>  = %d\n", (int)has_value_type<NoValueType>::value);

    std::printf("has_subscript<vector<int>>   = %d\n", (int)has_subscript<std::vector<int>>::value);
    std::printf("has_subscript<std::string>   = %d\n", (int)has_subscript<std::string>::value);
    std::printf("has_subscript<int>           = %d\n", (int)has_subscript<int>::value);

        // 坑：void_t 里的表达式必须是"能被替换"的。
        // 写成 std::void_t<typename T::value_type> 才对；
        // 写成 std::void_t<T::value_type>（少 typename）在依赖上下文里直接报错，
        // 因为编译器没法判断 value_type 是类型还是变量。
        // 坑：void_t 化简出来就是 void，所以 sizeof(std::void_t<int>) 是错的
        // （error C2070: "void": 格式错误的 sizeof 操作数）。
        // 它的用途只有一个：把"替换成功"这件事变成一个类型。
    static_assert(std::is_same<std::void_t<int, double>, void>::value, "void_t 化简成 void");
    std::printf("std::void_t<int,double> 就是 void，不能 sizeof 它\n");
    return 0;
}
