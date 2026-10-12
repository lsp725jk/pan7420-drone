// ch20_ex03_concepts.cpp —— concepts 与 requires：给模板参数加"说明书"
// 编译: check.bat ch20_ex03_concepts.cpp c++latest
#include <iostream>
#include <concepts>
#include <string>
#include <vector>
#include <type_traits>

// ============================================================
// A. 定义概念（concept）：一句话说明"什么样的类型才行"
// ============================================================
template <class T>
concept Numeric = std::integral<T> || std::floating_point<T>;

template <class T>
concept Printable = requires(std::ostream& os, const T& v) {
    { os << v } -> std::same_as<std::ostream&>;      // 必须能 os << v
};

template <class T>
concept HasSize = requires(const T& v) {
    { v.size() } -> std::convertible_to<std::size_t>;
};

// ============================================================
// B. 用概念约束函数模板：四种等价写法
// ============================================================
// 写法 1：requires 子句（最直观）
template <class T> requires Numeric<T>
T add1(T a, T b) { return a + b; }

// 写法 2：简写形式的模板参数（最简洁，C++20）
Numeric auto add2(Numeric auto a, Numeric auto b) { return a + b; }

// 写法 3：尾置 requires（条件可依赖多个参数）
template <class T, class U>
    requires Numeric<T> && Numeric<U>
auto add3(T a, U b) { return a + b; }

// 写法 4：concept 直接当类型约束用（等价于写法 1）
template <Numeric T>
T add4(T a, T b) { return a + b; }

// ============================================================
// C. 概念做重载分派：比 if constexpr 更"声明式"
// ============================================================
void show(const HasSize auto& v)            // 有 size() 的走这里
{
    std::cout << "  有 size()，长度 = " << v.size() << "\n";
}

template <class T>
    requires (!HasSize<T>)                   // 没有 size() 的走这里
void show(const T& v)
{
    std::cout << "  没有 size()，直接打印: " << v << "\n";
}

// ============================================================
// D. 概念之间可以组合、可以互相引用
// ============================================================
template <class T>
concept PrintableContainer = HasSize<T> && Printable<T>;

template <PrintableContainer C>
void dump(const C& c)
{
    std::cout << "  容器长度 " << c.size() << "，且可以整体打印: " << c << "\n";
}

// 一个同时满足"有 size()"和"能打印"的类型，用来演示概念组合
struct LaserFrame {
    std::vector<int> points{100, 200, 300};
    std::size_t size() const { return points.size(); }
    friend std::ostream& operator<<(std::ostream& os, const LaserFrame& f)
    {
        os << "LaserFrame{";
        for (std::size_t i = 0; i < f.points.size(); ++i) {
            if (i) os << ",";
            os << f.points[i];
        }
        return os << "}";
    }
};

int main()
{
    std::cout << "=== A. 四个等价写法都能用 ===\n";
    std::cout << "  add1(1, 2)       = " << add1(1, 2) << "\n";
    std::cout << "  add2(1.5, 2.5)   = " << add2(1.5, 2.5) << "\n";
    std::cout << "  add3(1, 2.5)     = " << add3(1, 2.5) << "  （两个不同类型）\n";
    std::cout << "  add4(3, 4)       = " << add4(3, 4) << "\n";
    std::cout << "  首选写法 1 或 4：报错信息最清楚，可读性最好。\n";

    std::cout << "\n=== B. 编译期检查概念是否满足 ===\n";
    std::cout << "  Numeric<int>          = " << Numeric<int> << "\n";
    std::cout << "  Numeric<double>       = " << Numeric<double> << "\n";
    std::cout << "  Numeric<std::string>  = " << Numeric<std::string> << "   <- 不满足\n";
    std::cout << "  HasSize<std::vector<int>> = " << HasSize<std::vector<int>> << "\n";
    std::cout << "  HasSize<int>          = " << HasSize<int> << "\n";
    std::cout << "  所以 add1(std::string(\"a\"), std::string(\"b\")) 会在**第一行**报错：\n";
    std::cout << "    C2672: \"add1\": 未找到匹配的重载函数\n";
    std::cout << "    note: 约束\"Numeric<T>\"未得到满足\n";

    std::cout << "\n=== C. 概念重载分派 ===\n";
    std::vector<int> v{1, 2, 3};
    show(v);
    show(42);
    show(std::string("hello"));

    std::cout << "\n=== D. 概念组合 ===\n";
    LaserFrame frame;
    std::cout << "  PrintableContainer<LaserFrame>   = " << PrintableContainer<LaserFrame> << "\n";
    std::cout << "  PrintableContainer<vector<int>>  = " << PrintableContainer<std::vector<int>>
              << "   <- 有 size() 但不能整体 << （C++23 也没有 vector 的 << 重载）\n";
    dump(frame);
    std::cout << "  所以 dump(std::vector<int>{1,2}) 会编译失败，错误信息直接指向概念名。\n";

    std::cout << "\n=== E. 概念 vs 老写法（SFINAE / enable_if）===\n";
    std::cout << "  C++11 写法：\n";
    std::cout << "    template <class T>\n";
    std::cout << "    typename std::enable_if<std::is_integral<T>::value, T>::type f(T a);\n";
    std::cout << "  报错时给你 200 行模板实例化回溯，找不到重点。\n";
    std::cout << "  C++20 写法：\n";
    std::cout << "    template <std::integral T> T f(T a);\n";
    std::cout << "  报错只说\"约束未满足\"，一行定位。\n";
    std::cout << "  这是 C++20 最值得学的一个特性 —— 它主要改善的是**报错信息**和**接口自文档化**。\n";

    std::cout << "\n=== F. 嵌入式视角 ===\n";
    std::cout << "  概念是纯粹的编译期机制，**不产生任何运行时代码**，\n";
    std::cout << "  也不增加 RAM/Flash 占用（零开销抽象）。\n";
    std::cout << "  代价：编译时间变长，报错信息虽好但错误本身变多时依然难读。\n";

    std::cout << "\nmain 正常结束\n";
    return 0;
}
