// ch23_ex19.cpp —— 例 23-19：返回值优化（RVO / NRVO）
// 编译器的这条优化比移动语义还狠：直接在调用者的空间里构造对象，
// 一次拷贝、一次移动都没有。
#include <cstdio>
#include <string>
#include <utility>

struct Tracked {
    std::string tag;
    int payload[64]{};  // 撑大一点，让拷贝的代价看得见

    explicit Tracked(const char* t) : tag(t) { std::printf("  构造   %s\n", t); }
    Tracked(const Tracked& o) : tag(o.tag) { std::printf("  拷贝构造 %s\n", tag.c_str()); }
    Tracked(Tracked&& o) noexcept : tag(o.tag) { std::printf("  移动构造 %s\n", tag.c_str()); }
    ~Tracked() { std::printf("  析构   %s\n", tag.c_str()); }
};

// 1) 返回局部变量：NRVO，编译器直接在调用者处构造
Tracked make_nrvo() {
    Tracked t("nrvo局部");
    return t;
}

// 2) 返回临时对象：RVO
Tracked make_rvo() {
    return Tracked("rvo临时");
}

// 3) 画蛇添足：加 std::move，反而挡住 NRVO，只能走移动构造
Tracked make_moved() {
    Tracked t("moved局部");
    return std::move(t);
}

int main() {
    std::printf("---- 1) 返回局部变量（NRVO）----\n");
    {
        Tracked a = make_nrvo();
        std::printf("  -> 得到 %s（只构造了一次，没有拷贝/移动）\n", a.tag.c_str());
    }

    std::printf("\n---- 2) 返回临时对象（RVO，C++17 起是强制的）----\n");
    {
        Tracked b = make_rvo();
        std::printf("  -> 得到 %s\n", b.tag.c_str());
    }

    std::printf("\n---- 3) 多写了一个 std::move（反面教材）----\n");
    {
        Tracked c = make_moved();
        std::printf("  -> 得到 %s（多了一次移动构造）\n", c.tag.c_str());
    }

    std::printf("\nC++17 的一个硬性变化：\n");
    std::printf("  Tracked x = Tracked(\"临时\"); 这种写法里，临时对象**不会**被创建，\n");
    std::printf("  直接在 x 的位置构造。（这叫保证的拷贝消除，guaranteed copy elision）\n");

    std::printf("\n结论：\n");
    std::printf("  - 放心按值返回局部变量/临时对象，不用 std::move，编译器会处理\n");
    std::printf("  - return std::move(x) 是**反优化**：它把返回值变成了右值引用，\n");
    std::printf("    编译器不能再用 NRVO，只能退而求其次做一次移动\n");
    std::printf("  - 但返回**成员变量**或**函数参数**时不会有 NRVO（它们不是局部对象），\n");
    std::printf("    这种情况才需要手写 std::move\n");
    std::printf("  - 坑：多个 return 分支返回不同的局部变量时，NRVO 仍然可以做；\n");
    std::printf("    但如果是 if/else 返回**同一个**变量的不同表达式，就做不了了\n");
    return 0;
}
