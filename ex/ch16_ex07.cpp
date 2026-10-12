// ch16_ex07.cpp —— 成员模板（member template）：类里的成员自己也是模板
#include <cstdio>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <utility>

class Printer {
public:
        // ---- 1) 普通成员函数模板：接受任何可打印的类型 ----
    template <class T>
    void print(const T& v) const
    {
        std::printf("    print<%s>：%s\n", typeid(T).name(), to_text(v).c_str());
    }

        // ---- 2) 带两个参数的成员模板 ----
    template <class A, class B>
    void print2(const A& a, const B& b) const
    {
        std::printf("    print2<%s,%s>：(%s, %s)\n",
                    typeid(A).name(), typeid(B).name(),
                    to_text(a).c_str(), to_text(b).c_str());
    }

        // ---- 3) 成员模板 + 完美转发：把参数"原样"交给被包装的对象 ----
    template <class T>
    void hold(T&& v)
    {
        std::printf("    hold：T 推导为 %s\n",
                    std::is_lvalue_reference_v<T> ? "左值引用" : "非左值引用");
        saved_ = std::forward<T>(v);            // 注意 saved_ 是 std::string
    }
    const std::string& saved() const { return saved_; }

private:
        // 一个只在本类里用的辅助模板
    template <class T>
    static std::string to_text(const T& v)
    {
        if constexpr (std::is_same_v<T, int>) {          // C++17 起：编译期 if
            return std::to_string(v);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::to_string(v);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return v;
        } else if constexpr (std::is_same_v<T, const char*>) {
            return std::string(v);
        } else {
            return "（不支持的类型）";
        }
    }

    std::string saved_;
};

// ---- 4) 成员模板做"转换构造"：从别的类型构造自己 ----
template <class T>
class Box {
public:
    Box() : v_() {}
    explicit Box(const T& v) : v_(v) {}

        // 成员模板构造：只要 U 能转成 T 就允许
    template <class U, class = std::enable_if_t<std::is_convertible_v<U, T>>>
    Box(const Box<U>& other) : v_(static_cast<T>(other.get()))
    {
        std::printf("    Box<%s> 从 Box<%s> 转换构造\n",
                    typeid(T).name(), typeid(U).name());
    }

    const T& get() const { return v_; }

private:
    T v_;
};

int main()
{
    std::printf("--- 1) 成员函数模板 ---\n");
    {
        Printer p;
        p.print(42);
        p.print(3.14);
        p.print(std::string("hello"));
        p.print("字面量");
    }

    std::printf("--- 2) 两个参数的成员模板 ---\n");
    {
        Printer p;
        p.print2(1, 2.5);
        p.print2(std::string("x"), 7);
    }

    std::printf("--- 3) 成员模板 + 完美转发 ---\n");
    {
        Printer p;
        std::string s = "可移动的字符串";
        p.hold(s);                        // 左值：拷贝
        std::printf("      hold 后 saved() = %s\n", p.saved().c_str());
        p.hold(std::string("临时字符串")); // 右值：移动
        std::printf("      hold 后 saved() = %s\n", p.saved().c_str());
    }

    std::printf("--- 4) 成员模板做转换构造 ---\n");
    {
        Box<double> bd(3.9);
        Box<int>    bi(bd);               // 用成员模板从 Box<double> 构造 Box<int>
        std::printf("      bi.get() = %d（3.9 被截断）\n", bi.get());

        Box<int>    bi2(5);
                // Box<std::string> bs(bi2);      // 编译错误：int 不能转成 string
        std::printf("      不能把 Box<int> 转成 Box<std::string>（enable_if 挡住了）\n");
    }

    std::printf("--- 5) 在类模板里，成员模板要写两个 template ---\n");
    {
        std::printf("      template <class T> class Outer {{\n");
        std::printf("          template <class U> void f(U);      // 外层 T + 内层 U\n");
        std::printf("      }};\n");
        std::printf("      类外定义时要写：\n");
        std::printf("      template <class T> template <class U>\n");
        std::printf("      void Outer<T>::f(U) {{ ... }}\n");
        std::printf("      两个 template 关键字都要写，这是初学最容易漏的地方\n");
    }

    return 0;
}
