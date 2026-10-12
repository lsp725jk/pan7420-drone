// ch16_ex08.cpp —— 模板与继承：CRTP、模板基类、依赖名（typename）
#include <cstdio>
#include <string>
#include <typeinfo>
#include <vector>

// ---------------- 1) 从模板基类继承 ----------------
template <class T>
class Base {
public:
    void base_hello()
    {
        std::printf("    Base<%s>::base_hello()\n", typeid(T).name());
    }
    T value_{};
};

class Derived : public Base<int> {        // 把 T 钉成 int
public:
    void derived_hello() { std::printf("    Derived::derived_hello()，value_ = %d\n", value_); }
};

// ---------------- 2) CRTP：派生类把自己当模板参数传给基类 ----------------
// 用途：基类里可以用 static_cast<Derived*>(this) 调用派生类的函数 —— "编译期多态"
template <class Derived>
class Countable {
public:
    int count() const
    {
                // 注意下面这个 this 转换：基类知道自己的真实类型
        return static_cast<const Derived*>(this)->count_impl();
    }
};

class Widget : public Countable<Widget> {
public:
    int count_impl() const
    {
        std::printf("    Widget::count_impl() 被 CRTP 基类调用了\n");
        return 42;
    }
};

class Gadget : public Countable<Gadget> {
public:
    int count_impl() const
    {
        std::printf("    Gadget::count_impl() 被 CRTP 基类调用了\n");
        return 7;
    }
};

// ---------------- 3) 依赖名：模板基类的成员要用 this-> 或 Base<T>:: 限定 ----------------
template <class T>
class Holder : public Base<T> {
public:
    void use_base_members(T v)
    {
                // this->value_ = v;          // 必须写 this->，否则 MSVC 报 C3861「找不到标识符」
        this->value_ = v;
                // this->base_hello();        // 同上
        this->base_hello();
    }
};

// ---------------- 4) 派生类也是模板 ----------------
template <class T>
class DerivedTemplate : public Base<T> {
public:
    T twice() const { return this->value_ + this->value_; }
};

// ---------------- 5) 用继承 + 模板做"接口 + 实现" ----------------
template <class T>
class Serializable {
public:
    std::string serialize() const
    {
        return static_cast<const T*>(this)->to_string_impl();
    }
};

class Point : public Serializable<Point> {
public:
    Point(int x, int y) : x_(x), y_(y) {}
    std::string to_string_impl() const
    {
        return "Point(" + std::to_string(x_) + "," + std::to_string(y_) + ")";
    }
private:
    int x_, y_;
};

int main()
{
    std::printf("--- 1) 从模板基类继承 ---\n");
    {
        Derived d;
        d.value_ = 5;
        d.base_hello();
        d.derived_hello();
        std::printf("    sizeof(Derived) = %zu\n", sizeof(Derived));
    }

    std::printf("--- 2) CRTP：编译期多态，没有虚函数表 ---\n");
    {
        Widget w;
        Gadget g;
        std::printf("    w.count() = %d\n", w.count());
        std::printf("    g.count() = %d\n", g.count());
        std::printf("    sizeof(Widget) = %zu（空类最小就是 1 字节）\n", sizeof(Widget));
        std::printf("    关键：这里没有任何虚函数，所以没有虚表指针（vptr）\n");
        std::printf("    如果改成虚函数方案，每个对象都会多出一个 vptr（64 位下 8 字节）\n");
        std::printf("    Countable<Widget> 和 Countable<Gadget> 是两个不同的基类\n");
        std::printf("    is_same<Countable<Widget>,Countable<Gadget>> = %d\n",
                    std::is_same_v<Countable<Widget>, Countable<Gadget>>);
    }

    std::printf("--- 3) 依赖名：模板基类的成员要用 this-> 限定 ---\n");
    {
        Holder<int> h;
        h.use_base_members(99);
        std::printf("    Holder<int> 用完了\n");
        std::printf("    如果写成 value_ = v;（不加 this->），MSVC 会报 C3861 找不到标识符\n");
        std::printf("    原因：编译器在第一遍扫描时不知道 Base<T> 里有没有 value_\n");
        std::printf("         （因为 T 还没确定，Base<T> 可能被特化成别的样子）\n");
    }

    std::printf("--- 4) 派生类也是模板 ---\n");
    {
        DerivedTemplate<int> di;
        di.value_ = 21;
        DerivedTemplate<double> dd;
        dd.value_ = 1.5;
        std::printf("    DerivedTemplate<int>.twice()    = %d\n", di.twice());
        std::printf("    DerivedTemplate<double>.twice() = %.2f\n", dd.twice());
    }

    std::printf("--- 5) CRTP 实战：给类免费加上 serialize() ---\n");
    {
        Point p(3, 4);
        std::printf("    p.serialize() = %s\n", p.serialize().c_str());
        std::printf("    好处：不用写虚函数、不用 vtable、能内联（嵌入式友好）\n");
        std::printf("    坏处：不能在运行时通过基类指针统一处理不同类型（那是虚函数的活）\n");
    }

    std::printf("--- 6) typename 关键字：告诉编译器\"这是个类型\" ---\n");
    {
        std::printf("    在模板里访问依赖类型必须加 typename：\n");
        std::printf("      template<class T> void f() {{\n");
        std::printf("          typename T::value_type v;   // 不加 typename 会报语法错误\n");
        std::printf("      }}\n");
        std::printf("    因为编译器不知道 T::value_type 是类型还是静态成员\n");
        std::printf("    记忆：只有\"从属名字\"需要 typename，普通类型名不需要\n");
    }

    return 0;
}
