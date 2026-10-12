// ch15_ex09.cpp —— std::forward 与完美转发：为什么转发时必须写 forward
#include <cstdio>
#include <string>
#include <utility>

struct Widget {
    std::string name;
    explicit Widget(std::string n) : name(std::move(n))
    {
        std::printf("    Widget(std::string) 构造，name=%s\n", name.c_str());
    }
    Widget(const Widget& o) : name(o.name)
    {
        std::printf("    Widget 拷贝构造，name=%s\n", name.c_str());
    }
    Widget(Widget&& o) noexcept : name(std::move(o.name))
    {
        std::printf("    Widget 移动构造，name=%s\n", name.c_str());
    }
};

void sink(const Widget& w) { std::printf("    sink(const Widget&)  收到 name=%s\n", w.name.c_str()); }
void sink(Widget&& w)      { std::printf("    sink(Widget&&)       收到 name=%s\n", w.name.c_str()); }

// ---- 写法 1：不用 forward，直接把参数传下去 ----
template <class T>
void wrapper_no_forward(T&& arg)
{
    std::printf("  [wrapper_no_forward] 传参给 sink（arg 本身是左值）：\n");
    sink(arg);                        // arg 有名字 -> 左值 -> 走 const Widget&
}

// ---- 写法 2：用 std::forward 完美转发 ----
template <class T>
void wrapper_forward(T&& arg)
{
    std::printf("  [wrapper_forward] 传参给 sink（用 forward 保持原类别）：\n");
    sink(std::forward<T>(arg));
}

// ---- 写法 3：错用 std::move 转发 ----
template <class T>
void wrapper_move(T&& arg)
{
    std::printf("  [wrapper_move] 传参给 sink（用 move，强行当右值）：\n");
    sink(std::move(arg));             // 即使调用者传的是左值，也被当成右值
}

// ---- 一个真正需要完美转发的场景：可变参数工厂 ----
template <class T, class... Args>
T* raw_new(Args&&... args)
{
    std::printf("  [raw_new] 转发 %zu 个参数\n", sizeof...(args));
    return new T(std::forward<Args>(args)...);
}

int main()
{
    std::printf("--- 1) 不用 forward：右值属性丢了 ---\n");
    {
        Widget w("A");
        std::printf("  调用 wrapper_no_forward(std::move(w))：\n");
        wrapper_no_forward(std::move(w));
        std::printf("  结果：本来是个右值，进到 wrapper 里变成了左值，sink 走了 const& 重载\n");
        std::printf("  —— 一次拷贝/绑定被浪费掉了\n");
    }

    std::printf("\n--- 2) 用 forward：右值属性保住了 ---\n");
    {
        Widget w("B");
        std::printf("  调用 wrapper_forward(std::move(w))：\n");
        wrapper_forward(std::move(w));

        Widget w2("C");
        std::printf("  调用 wrapper_forward(w2)（传左值）：\n");
        wrapper_forward(w2);
        std::printf("  同一个模板，左值仍走 const&，右值走 && —— 这就是\"完美转发\"\n");
    }

    std::printf("\n--- 3) 错用 move 转发：左值也被当成右值 ---\n");
    {
        Widget w("D");
        std::printf("  调用 wrapper_move(w)（明明传的是左值）：\n");
        wrapper_move(w);
        std::printf("  危险：w 之后可能被搬空，而调用者完全没想到\n");
        std::printf("  正确做法是 std::forward<T>(arg)，它只在 T 是右值时才产生右值\n");
    }

    std::printf("\n--- 4) forward<decltype(x)>(x)：括号很重要 ---\n");
    {
        Widget w("E");
        std::printf("  用 std::forward<decltype((w))>(w)（decltype((w)) = Widget&，转发成左值）：\n");
        sink(std::forward<decltype((w))>(w));
        std::printf("  用 std::forward<decltype(w)>(w)（decltype(w) = Widget，转发成右值！）：\n");
        sink(std::forward<decltype(w)>(w));
        std::printf("  原文：模板里要写 std::forward<T>(arg) 而不是 forward<decltype(arg)>(arg)，\n");
        std::printf("        因为 decltype(arg) 在 T 推成左值引用时才带 &，容易写错\n");
    }

    std::printf("\n--- 5) 可变参数模板 + forward：工厂函数 ---\n");
    {
        Widget* p = raw_new<Widget>(std::string("F"));
        std::printf("  构造出来的 p->name = %s\n", p->name.c_str());
        delete p;

        Widget       src("G");
        Widget*      q = raw_new<Widget>(src);              // 传左值 -> 拷贝
        std::printf("  q->name = %s，src.name = %s（没被搬空）\n", q->name.c_str(), src.name.c_str());
        delete q;
    }

    std::printf("\n--- 6) 转发时该用 forward 还是 move 的判据 ---\n");
    std::printf("  forward<T>(x)：T 是模板参数，转发\"调用者原本给的类别\"  <- 转发用这个\n");
    std::printf("  move(x)      ：无条件当成右值，表示\"我不要了\"          <- 主动放弃用这个\n");

    return 0;
}
