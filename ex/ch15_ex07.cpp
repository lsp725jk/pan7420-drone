// ch15_ex07.cpp —— 【必做实验】RVO/NRVO：直接 return 局部变量 vs return std::move(局部变量)
#include <cstdio>
#include <string>
#include <utility>

class Tracked {
public:
    std::string tag;

    explicit Tracked(std::string t) : tag(std::move(t))
    {
        std::printf("    构造        tag=%s  this=%p\n", tag.c_str(), static_cast<void*>(this));
    }
    Tracked(const Tracked& o) : tag(o.tag)
    {
        std::printf("    拷贝构造    tag=%s  this=%p\n", tag.c_str(), static_cast<void*>(this));
    }
    Tracked(Tracked&& o) noexcept : tag(std::move(o.tag))
    {
        std::printf("    移动构造    tag=%s  this=%p\n", tag.c_str(), static_cast<void*>(this));
        o.tag = "(已搬空)";
    }
    Tracked& operator=(const Tracked& o)
    {
        tag = o.tag;
        std::printf("    拷贝赋值    tag=%s\n", tag.c_str());
        return *this;
    }
    Tracked& operator=(Tracked&& o) noexcept
    {
        tag = std::move(o.tag);
        std::printf("    移动赋值    tag=%s\n", tag.c_str());
        return *this;
    }
    ~Tracked() { std::printf("    析构        tag=%s\n", tag.c_str()); }
};

// 写法 A：直接返回局部变量 —— 编译器可以做 NRVO，或者至少用移动
Tracked make_direct()
{
    Tracked local("direct");
    return local;
}

// 写法 B：return std::move(局部变量) —— 阻止 NRVO，强制一次移动构造
Tracked make_with_move()
{
    Tracked local("with_move");
    return std::move(local);
}

// 写法 C：返回临时对象 —— 一定是 prvalue，编译器直接就地构造
Tracked make_temporary()
{
    return Tracked("temporary");
}

int main()
{
    std::printf("=== A) Tracked a = make_direct();  —— 直接 return 局部变量 ===\n");
    {
        std::printf("  进入函数：\n");
        Tracked a = make_direct();
        std::printf("  回到 main，a.tag = %s（a 的地址 = %p）\n",
                    a.tag.c_str(), static_cast<void*>(&a));
        std::printf("  离开作用域：\n");
    }

    std::printf("\n=== B) Tracked b = make_with_move();  —— return std::move(局部变量) ===\n");
    {
        std::printf("  进入函数：\n");
        Tracked b = make_with_move();
        std::printf("  回到 main，b.tag = %s（b 的地址 = %p）\n",
                    b.tag.c_str(), static_cast<void*>(&b));
        std::printf("  离开作用域：\n");
    }

    std::printf("\n=== C) Tracked c = make_temporary();  —— 返回临时对象 ===\n");
    {
        std::printf("  进入函数：\n");
        Tracked c = make_temporary();
        std::printf("  回到 main，c.tag = %s（c 的地址 = %p）\n",
                    c.tag.c_str(), static_cast<void*>(&c));
        std::printf("  离开作用域：\n");
    }

    std::printf("\n=== 统计一下：上面三种写法各发生了几次拷贝 / 几次移动 ===\n");
    std::printf("  这是肉眼数出来的（见上面的输出）：\n");
    std::printf("    A 直接 return 局部变量 ：拷贝 0 次，移动 0 次（NRVO，就地构造）\n");
    std::printf("    B return std::move(x)  ：拷贝 0 次，移动 1 次（NRVO 被阻止）\n");
    std::printf("    C 返回临时对象         ：拷贝 0 次，移动 0 次（纯右值，就地构造）\n");
    std::printf("  结论：return std::move(x) 比 return x 多做了一次移动构造 —— 更慢，且没有好处\n");

    return 0;
}
