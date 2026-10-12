// ch16_ex11.cpp —— decltype、decltype(auto)、auto 返回类型推导
#include <cstdio>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>

// ---------------- 1) decltype 的基本规则 ----------------
// decltype(名字)      -> 该名字声明时的类型（不带额外引用）
// decltype((名字))    -> 该表达式是左值 -> 得到 T&
// decltype(表达式)    -> 表达式的类型（值类别决定带不带引用）

template <class T>
void show_decltype(const char* who, const char* what)
{
    using NoRef = std::remove_reference_t<T>;
    const char* sfx = std::is_lvalue_reference_v<T> ? "&"
                    : std::is_rvalue_reference_v<T> ? "&&"
                                                    : "";
    std::printf("    %-26s decltype = %s%s   (%s)\n",
                who, typeid(NoRef).name(), sfx, what);
}

// ---------------- 2) auto 的推导规则与 decltype 不同 ----------------
template <class T>
void by_value(T) {}          // 按值：auto 会剥掉 const 和引用，decltype 不会

// ---------------- 3) 返回类型：auto（剥引用）vs decltype(auto)（保留引用） ----------------
std::vector<int> g_vec{ 1, 2, 3 };

auto           first_auto()          { return g_vec[0]; }        // 返回 int（拷贝）
decltype(auto) first_decltype_auto() { return g_vec[0]; }        // 返回 int&（引用！）
auto&          first_auto_ref()      { return g_vec[0]; }        // 显式写引用

// ---------------- 4) 实战：一个"取值"的包装函数 ----------------
template <class Container>
decltype(auto) at_front(Container& c)
{
    return c[0];          // 想返回引用就必须用 decltype(auto)
}

template <class Container>
auto at_front_copy(Container& c)
{
    return c[0];          // auto：返回拷贝（c[0] 是左值，auto 剥掉引用）
}

// ---------------- 5) decltype 用于"求表达式的类型"（不实际求值） ----------------
struct Meter { double v; };
Meter operator+(Meter a, Meter b) { return Meter{ a.v + b.v }; }

// ---------------- 6) auto 在 C++14/17 的其他用法 ----------------
auto make_pair_auto() { return std::pair<int, std::string>{ 1, "one" }; }

int main()
{
    std::printf("--- 1) decltype 的四条规则 ---\n");
    {
        int        n  = 1;
        const int  cn = 2;
        int&       rn = n;
        int&&      rr = std::move(n);

        show_decltype<decltype(n)>("decltype(n)", "变量声明的类型 = int");
        show_decltype<decltype(cn)>("decltype(cn)", "保留 const");
        show_decltype<decltype(rn)>("decltype(rn)", "引用类型原样保留");
        show_decltype<decltype(rr)>("decltype(rr)", "右值引用变量，类型是 int&&");
        show_decltype<decltype((n))>("decltype((n))", "加括号 -> 表达式是左值 -> int&");
        show_decltype<decltype((cn))>("decltype((cn))", "const 左值 -> const int&");
        show_decltype<decltype(n + 1)>("decltype(n + 1)", "纯右值 -> int，不带引用");
        show_decltype<decltype(n = 5)>("decltype(n = 5)", "赋值表达式是左值 -> int&");
    }

    std::printf("--- 2) 编译期直接比类型 ---\n");
    {
        int n = 1;
        std::printf("    is_same<decltype(n), int>          = %d\n",
                    std::is_same_v<decltype(n), int>);
        std::printf("    is_same<decltype((n)), int&>       = %d\n",
                    std::is_same_v<decltype((n)), int&>);
        std::printf("    is_same<decltype(n + 1), int>      = %d\n",
                    std::is_same_v<decltype(n + 1), int>);
        std::printf("    is_same<decltype(1 + 2.0), double> = %d\n",
                    std::is_same_v<decltype(1 + 2.0), double>);
        std::printf("    注意 decltype 不求值：decltype(n++) 里 n 不会真的自增\n");
        std::printf("    decltype(n++) = %s（int，且 n 仍然是 %d）\n",
                    typeid(decltype(n++)).name(), n);
    }

    std::printf("--- 3) auto 和 decltype 的差别：auto 会\"脱一层\" ---\n");
    {
        const int  cn = 7;
        const int& cr = cn;

        auto   a1 = cn;      // auto  = int     （const 被丢掉）
        (void)a1;
        auto   a2 = cr;      // auto  = int     （引用被丢掉）
        (void)a2;
        decltype(cn) d1 = cn;   // decltype = const int
        decltype(cr) d2 = cn;   // decltype = const int&

        std::printf("    is_same<decltype(a1), int>            = %d\n",
                    std::is_same_v<decltype(a1), int>);
        std::printf("    is_same<decltype(a2), int>            = %d（引用也丢了）\n",
                    std::is_same_v<decltype(a2), int>);
        std::printf("    is_same<decltype(d1), const int>      = %d\n",
                    std::is_same_v<decltype(d1), const int>);
        std::printf("    is_same<decltype(d2), const int&>     = %d\n",
                    std::is_same_v<decltype(d2), const int&>);
                // a1 = 8;   // 合法（a1 是 int）
                // d1 = 8;   // 编译错误（d1 是 const int）
        std::printf("    auto 剥掉 const 和引用 -> 拷贝了一份；decltype 原样保留\n");
    }

    std::printf("--- 4) auto 返回类型 vs decltype(auto) 返回类型 ---\n");
    {
        std::printf("    g_vec = {1,2,3}\n");
        std::printf("    first_auto()  返回类型是 int ?  %d\n",
                    std::is_same_v<decltype(first_auto()), int>);
        std::printf("    first_decltype_auto() 返回 int& ? %d\n",
                    std::is_same_v<decltype(first_decltype_auto()), int&>);
        std::printf("    first_auto_ref()      返回 int& ? %d\n",
                    std::is_same_v<decltype(first_auto_ref()), int&>);

        first_decltype_auto() = 100;      // 通过引用改到了容器里的元素
        std::printf("    通过 first_decltype_auto() = 100 改了容器：g_vec[0] = %d\n", g_vec[0]);

        std::printf("    at_front(g_vec)      返回 int& ? %d\n",
                    std::is_same_v<decltype(at_front(g_vec)), int&>);
        std::printf("    at_front_copy(g_vec) 返回 int  ? %d\n",
                    std::is_same_v<decltype(at_front_copy(g_vec)), int>);
        std::printf("    模板里想返回引用，必须写 decltype(auto) 或 auto&\n");
    }

    std::printf("--- 5) decltype 用于推导表达式类型（常用于泛型代码）---\n");
    {
        Meter a{ 1.5 }, b{ 2.5 };
        auto c = a + b;      // auto: Meter
        std::printf("    decltype(a + b) 就是 Meter ? %d\n",
                    std::is_same_v<decltype(a + b), Meter>);
        std::printf("    c.v = %.2f\n", c.v);
        std::printf("    C++11 里模板函数想推导返回类型，靠的就是 -> decltype(表达式)\n");
    }

    std::printf("--- 6) auto 的其它常见用法 ---\n");
    {
        auto p = make_pair_auto();
        std::printf("    auto p = make_pair_auto(); p.first = %d, p.second = %s\n",
                    p.first, p.second.c_str());

        std::vector<int> v{ 1, 2, 3 };
        for (auto it = v.begin(); it != v.end(); ++it) {   // 迭代器类型太长，用 auto
            *it += 1;
        }
        std::printf("    auto 接迭代器：v = {");
        for (std::size_t i = 0; i < v.size(); ++i) {
            std::printf("%d%s", v[i], i + 1 < v.size() ? ", " : "");
        }
        std::printf("}\n");

        std::printf("    C++17 起还有：auto [x, y] = pair;（结构化绑定）—— 本章不展开\n");
    }

    return 0;
}
