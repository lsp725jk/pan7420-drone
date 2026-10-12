// ch18_ex06_noexcept_move.cpp —— noexcept：含义、什么时候必须写、对移动的影响
// 编译: check.bat ch18_ex06_noexcept_move.cpp c++latest
#include <iostream>
#include <vector>
#include <string>
#include <type_traits>
#include <utility>

// 每做一次拷贝/移动就印一行，这样能"看见"vector 扩容时选的是哪一个
struct Tracer {
    std::string tag;
    explicit Tracer(std::string t) : tag(std::move(t)) {}

    Tracer(const Tracer& o) : tag(o.tag) {
        std::cout << "      [拷贝构造] " << tag << "\n";
    }
    Tracer(Tracer&& o) noexcept : tag(std::move(o.tag)) {
        std::cout << "      [移动构造 noexcept] " << tag << "\n";
    }
    Tracer& operator=(const Tracer& o) { tag = o.tag; std::cout << "      [拷贝赋值]\n"; return *this; }
    Tracer& operator=(Tracer&& o) noexcept { tag = std::move(o.tag); std::cout << "      [移动赋值]\n"; return *this; }
    ~Tracer() = default;
};

// 同一个人，但移动构造没有 noexcept —— 这是最经典的性能坑
struct BadTracer {
    std::string tag;
    explicit BadTracer(std::string t) : tag(std::move(t)) {}

    BadTracer(const BadTracer& o) : tag(o.tag) {
        std::cout << "      [拷贝构造] " << tag << "\n";
    }
    BadTracer(BadTracer&& o) : tag(std::move(o.tag)) {          // ← 少了 noexcept
        std::cout << "      [移动构造 没有noexcept] " << tag << "\n";
    }
    ~BadTracer() = default;
};

// ---------- 编译期就能问出答案的两个 trait ----------
template <class T>
void inspect(const char* name)
{
    std::cout << "  " << name << ":\n"
              << "    is_nothrow_move_constructible = "
              << (std::is_nothrow_move_constructible_v<T> ? "true" : "false") << "\n"
              << "    is_copy_constructible         = "
              << (std::is_copy_constructible_v<T> ? "true" : "false") << "\n";
}

// ---------- noexcept 的三种写法 ----------
void f1() noexcept {}                       // 无条件承诺：绝不抛
void f2() noexcept(true) {}                 // 同上，显式写法
void f3() noexcept(false) {}                // 明确声明"我可能抛"
template <class T> void f4() noexcept(noexcept(T{})) {}   // 条件式：取决于表达式

int main()
{
    std::cout << "=== A. 类型层面的事实 ===\n";
    inspect<Tracer>("Tracer（移动构造标了 noexcept）");
    inspect<BadTracer>("BadTracer（移动构造没标）");
    inspect<std::string>("std::string");
    inspect<std::vector<int>>("std::vector<int>");

    std::cout << "\n=== B. 亲手看看 vector 扩容选谁 ===\n";
    std::cout << "  先 reserve(2)，让第 3 个元素触发真正的扩容：\n";
    {
        std::vector<Tracer> v;
        v.reserve(2);
        for (int i = 0; i < 3; ++i) {
            std::cout << "    push_back 第 " << i + 1 << " 个\n";
            v.emplace_back("T" + std::to_string(i));
        }
    }

    std::cout << "\n  换成没标 noexcept 的 BadTracer：\n";
    {
        std::vector<BadTracer> v;
        v.reserve(2);
        for (int i = 0; i < 3; ++i) {
            std::cout << "    push_back 第 " << i + 1 << " 个\n";
            v.emplace_back("B" + std::to_string(i));
        }
    }

    std::cout << "\n=== C. 条件式 noexcept：noexcept(表达式) ===\n";
    std::cout << "  noexcept(std::string{})              = "
              << noexcept(std::string{}) << "\n";
    std::cout << "  noexcept(1 + 1 这个表达式)           = "
              << noexcept(1 + 1) << "   <- 没有可能抛异常的运算\n";
    std::cout << "  noexcept(读一个 int 变量)            = "
              << noexcept(std::declval<int&>() = 1) << "\n";
    f1(); f2(); f3(); f4<int>();
    std::cout << "  f1/f2/f3/f4<int> 都调用成功\n";

    std::cout << "\n=== D. 手动用 std::move_if_noexcept 演绎同一件事 ===\n";
    Tracer t("样例");
    std::cout << "  对 Tracer（move 是 noexcept）选：";
    auto&& a = std::move_if_noexcept(t);      // move 安全 -> 返回 Tracer&&
    std::cout << (std::is_rvalue_reference_v<decltype(a)> ? "移动 OK" : "拷贝") << "\n";
    BadTracer b("样例");
    std::cout << "  对 BadTracer（move 可能抛）选：";
    auto&& c = std::move_if_noexcept(b);      // move 不安全 -> 返回 const BadTracer&
    std::cout << (std::is_rvalue_reference_v<decltype(c)> ? "移动" : "拷贝 OK") << "\n";
    std::cout << "  这就是 vector 扩容时内部做的判断：move 是 noexcept 才敢用 move。\n";

    std::cout << "\nmain 正常结束\n";
    return 0;
}
