// ch16_ex03.cpp —— 多个模板参数与默认模板参数
#include <cstdio>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>

// 把类型名变成字符串（typeid 的名字是实现定义的；MSVC 上 int 就是 "int"）
template <class T> const char* type_name() { return typeid(T).name(); }
// MSVC 上 std::string 的 typeid 名字很长（是 basic_string 的全名），特化一下方便看
template <> const char* type_name<std::string>() { return "std::string"; }
template <> const char* type_name<int>()         { return "int"; }
template <> const char* type_name<double>()      { return "double"; }
template <> const char* type_name<char>()        { return "char"; }

// ---- 两个模板参数：两个类型可以不同 ----
template <class T1, class T2>
void show_pair(const T1& a, const T2& b)
{
    std::printf("    T1 = %-8s T2 = %-8s 值 = (%s, %s)\n",
                type_name<T1>(), type_name<T2>(),
                std::to_string(a).c_str(), std::to_string(b).c_str());
}

// string 不能 to_string，单独给一个重载（这本身就是"重载 + 模板"的配合）
inline void show_pair(const std::string& a, const std::string& b)
{
    std::printf("    T1 = %-8s T2 = %-8s 值 = (%s, %s)\n",
                type_name<std::string>(), type_name<std::string>(),
                a.c_str(), b.c_str());
}
inline void show_pair(const std::string& a, int b)
{
    std::printf("    T1 = %-8s T2 = %-8s 值 = (%s, %d)\n",
                type_name<std::string>(), type_name<int>(), a.c_str(), b);
}

// ---- 两个模板参数 + 返回类型由调用者决定 ----
template <class R, class T1, class T2>
R add_as(const T1& a, const T2& b)
{
    return static_cast<R>(a + b);
}

// ---- 返回值类型推导 ----
template <class T1, class T2>
auto add_auto(const T1& a, const T2& b) -> decltype(a + b)   // C++11 风格：尾随返回类型
{
    return a + b;
}

template <class T1, class T2>
auto add_auto2(const T1& a, const T2& b)                     // C++14 起：直接 auto
{
    return a + b;
}

// ---- 默认模板参数：类模板也可以用 ----
template <class T, class Container = std::vector<T>>
class Stack {
public:
    void push(const T& v) { c_.push_back(v); }
    T    pop()
    {
        T v = c_.back();
        c_.pop_back();
        return v;
    }
    std::size_t size() const { return c_.size(); }
private:
    Container c_;
};

int main()
{
    std::printf("--- 1) 两个模板参数，类型可以不同 ---\n");
    show_pair(1, 2.5);                 // T1=int, T2=double
    show_pair(1, 'a');                 // T1=int, T2=char
    show_pair(std::string("x"), 3);    // T1=string, T2=int
    show_pair(std::string("x"), std::string("y"));

    std::printf("--- 2) 显式指定返回类型 ---\n");
    std::printf("    add_as<double>(1, 2)    = %f\n", add_as<double>(1, 2));
    std::printf("    add_as<int>(1.9, 2.9)   = %d  （小数被截断）\n", add_as<int>(1.9, 2.9));
    std::printf("    add_as<long long>(1, 2) = %lld\n", add_as<long long>(1, 2));
    std::printf("  注意：add_as 的第一个模板参数 R 无法从实参推导，必须显式写\n");

    std::printf("--- 3) 让编译器推导返回类型 ---\n");
    std::printf("    add_auto(1, 2)      = %d（int）\n", add_auto(1, 2));
    std::printf("    add_auto(1, 2.5)    = %.2f（double，因为 int+double 是 double）\n",
                add_auto(1, 2.5));
    std::printf("    add_auto2(1, 2)     = %d（C++14 的写法，更短）\n", add_auto2(1, 2));
    std::printf("    is_same<decltype(add_auto(1,2.5)), double> = %d\n",
                std::is_same_v<decltype(add_auto(1, 2.5)), double>);
    std::printf("    is_same<decltype(add_auto(1,2)), int>       = %d\n",
                std::is_same_v<decltype(add_auto(1, 2)), int>);

    std::printf("--- 4) 默认模板参数 ---\n");
    {
        Stack<int> s;                       // 等价于 Stack<int, std::vector<int>>
        s.push(10);
        s.push(20);
        std::printf("    Stack<int> 大小 = %zu，pop 出来 = %d\n", s.size(), s.pop());

        Stack<int, std::vector<int>> s2;    // 显式写出来，和上面完全同类型
        std::printf("    两种写法是同一个类型吗？%d\n",
                    std::is_same_v<Stack<int>, Stack<int, std::vector<int>>>);
    }

    std::printf("--- 5) 模板出错的报错信息怎么读 ---\n");
    std::printf("    如果给模板里的 to_string 传一个它不支持的类型，会看到一大串 note\n");
    std::printf("    读法：先看第一条 error 的\"文件(行号)\"，那是你代码里的位置；\n");
    std::printf("          再看最后一条 note 里的\"模板实例化上下文\"，确认是哪个 T 出的问题\n");

    return 0;
}
