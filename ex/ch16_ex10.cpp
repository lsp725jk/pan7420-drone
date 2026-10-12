// ch16_ex10.cpp —— 可变参数模板（variadic template）与 sizeof...
#include <cstdio>
#include <string>
#include <type_traits>
#include <vector>

// ---------------- 1) 基本形态：sizeof... 数出有几个参数 ----------------
template <class... Ts>
std::size_t how_many(const Ts&... args)
{
    ((void)args, ...);             // 参数本身用不到，但要用一下免得 /W4 报 C4100
    return sizeof...(args);        // 注意：sizeof... 后面有省略号，且**不要求参数求值**
}

// ---------------- 2) C++11 风格的递归展开：一个参数 + 剩下的打包 ----------------
void print_c11()                        // 递归终点：没有参数了
{
    std::printf("\n");
}
template <class T, class... Rest>
void print_c11(const T& first, const Rest&... rest)
{
    std::printf("%s ", std::to_string(first).c_str());
    print_c11(rest...);                 // 递归：把剩下的继续展开
}

// ---------------- 3) C++17 起的折叠表达式（fold expression）：一行搞定 ----------------
template <class... Ts>
auto sum_fold(const Ts&... args)
{
    return (args + ...);                // 一元右折叠：a1 + (a2 + (a3 + ...))
}
template <class... Ts>
auto sum_fold_left(const Ts&... args)
{
    return (... + args);                // 一元左折叠：((a1 + a2) + a3) + ...
}
template <class... Ts>
void print_fold(const Ts&... args)
{
    ((std::printf("%s ", std::to_string(args).c_str())), ...);   // 逗号折叠
    std::printf("\n");
}
template <class... Bools>
bool all_true(Bools... bs)
{
    return (bs && ...);                 // 短路与折叠
}

// ---------------- 4) 折叠 + 初始值（二元折叠） ----------------
template <class... Ts>
auto sum_with_init(int init, const Ts&... args)
{
    return (init + ... + args);         // 二元折叠：(init + a1) + a2 + ...
}

// ---------------- 5) 实用：把一组参数装进 vector / 打印类型表 ----------------
template <class... Ts>
std::vector<int> to_ints(const Ts&... args)
{
    std::vector<int> v;
    v.reserve(sizeof...(args));         // sizeof... 直接当常量用
    (v.push_back(static_cast<int>(args)), ...);
    return v;
}

template <class... Ts>
struct TypeList {
    static constexpr std::size_t size = sizeof...(Ts);
};

// ---------------- 6) 可变参数 + 完美转发（第15章的 forward） ----------------
template <class T, class... Args>
T* make_one(Args&&... args)
{
    std::printf("    收到 %zu 个构造参数\n", sizeof...(args));
    return new T(std::forward<Args>(args)...);
}

struct Point {
    int x, y;
    Point(int a, int b) : x(a), y(b) {}
    explicit Point(int a) : x(a), y(0) {}
};

int main()
{
    std::printf("--- 1) sizeof... 数参数个数 ---\n");
    std::printf("    how_many()              = %zu\n", how_many());
    std::printf("    how_many(1)             = %zu\n", how_many(1));
    std::printf("    how_many(1, 2.5, 'c')   = %zu\n", how_many(1, 2.5, 'c'));
    std::printf("    sizeof... 是编译期常量，常见写法：v.reserve(sizeof...(args))\n");

    std::printf("--- 2) C++11 递归展开 ---\n");
    std::printf("    print_c11(1, 2, 3)      -> ");
    print_c11(1, 2, 3);
    std::printf("    print_c11(1, 2.5, 3.5f) -> ");
    print_c11(1, 2.5, 3.5f);

    std::printf("--- 3) C++17 折叠表达式（比递归清爽得多）---\n");
    std::printf("    sum_fold(1,2,3,4)        = %d\n", sum_fold(1, 2, 3, 4));
    std::printf("    sum_fold_left(1,2,3,4)   = %d\n", sum_fold_left(1, 2, 3, 4));
    std::printf("    sum_fold(1,2.5,3)        = %.2f（类型按 + 的规则提升）\n", sum_fold(1, 2.5, 3));
    std::printf("    print_fold(10,20,30)     -> ");
    print_fold(10, 20, 30);
    std::printf("    all_true(true,true,false,true) = %d\n", all_true(true, true, false, true));
    std::printf("    all_true(true,true,true)       = %d\n", all_true(true, true, true));

    std::printf("--- 4) 二元折叠：带初始值 ---\n");
    std::printf("    sum_with_init(100, 1,2,3) = %d\n", sum_with_init(100, 1, 2, 3));
    std::printf("    sum_with_init(0)          = %d（零个参数也合法，这是二元折叠的好处）\n",
                sum_with_init(0));
    std::printf("    对比：一元折叠 sum_fold() 对空的参数包是**编译错误**（无法确定类型）\n");

    std::printf("--- 5) 把参数装进容器 ---\n");
    {
        auto v = to_ints(1, 2.9, 'A', true);
        std::printf("    to_ints(1, 2.9, 'A', true) = {");
        for (std::size_t i = 0; i < v.size(); ++i) {
            std::printf("%d%s", v[i], i + 1 < v.size() ? ", " : "");
        }
        std::printf("}\n");
        std::printf("    注意 2.9 被截成 2，'A' 变成 65，true 变成 1\n");
    }

    std::printf("--- 6) 可变参数模板类型的成员用 sizeof... ---\n");
    std::printf("    TypeList<>::size              = %zu\n", TypeList<>::size);
    std::printf("    TypeList<int,double>::size    = %zu\n", TypeList<int, double>::size);
    std::printf("    TypeList<int,double,char>::size = %zu\n",
                TypeList<int, double, char>::size);
    std::printf("    这就是 std::tuple / std::variant 的基础\n");

    std::printf("--- 7) 可变参数 + 完美转发 ---\n");
    {
        Point* p1 = make_one<Point>(1, 2);
        Point* p2 = make_one<Point>(5);
        std::printf("    Point(1,2) -> (%d,%d)\n", p1->x, p1->y);
        std::printf("    Point(5)   -> (%d,%d)\n", p2->x, p2->y);
        delete p1;
        delete p2;
        std::printf("    std::make_unique / emplace_back 内部就是这样实现的\n");
    }

    return 0;
}
