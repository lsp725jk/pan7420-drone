// ch14_ex04.cpp —— 坑：两个 unique_ptr 管同一块内存 -> double free；以及正确交接写法
#include <cstdio>
#include <memory>

struct Res {
    int id;
    explicit Res(int i) : id(i) { std::printf("  Res(%d) 构造\n", id); }
    ~Res() { std::printf("  Res(%d) 析构\n", id); }
};

int main()
{
    std::printf("--- 正确的所有权交接（不会 double free）---\n");
    {
        auto a = std::make_unique<Res>(1);
        std::printf("  a.get() = %p\n", static_cast<void*>(a.get()));
        auto b = std::move(a);        // 用移动：a 变空，只剩 b 持有
        std::printf("  移动后 a.get() = %p, b.get() = %p\n",
                    static_cast<void*>(a.get()), static_cast<void*>(b.get()));
    }
    std::printf("只析构一次，正确\n\n");

    std::printf("--- 危险写法：两个 unique_ptr 分别 new 同一块内存（演示用，不要照抄）---\n");
    {
                // 下面这段是**错误示范**：让两个 unique_ptr 各自以裸指针构造，指向同一对象。
                // 运行到作用域结束会析构两次 -> 典型的 double free（未定义行为）。
                // 在 MSVC 的 Debug 堆里通常直接崩溃；这里我们用注释把它封起来，
                // 只把"正确做法"作为可运行代码，避免示例本身崩溃。
        
                // Res* raw = new Res(9);
                // std::unique_ptr<Res> p1(raw);
                // std::unique_ptr<Res> p2(raw);   // 错！两个所有者
        
                // 正确做法一：第二个用移动
        Res* raw = new Res(9);
        std::unique_ptr<Res> p1(raw);
        std::unique_ptr<Res> p2(std::move(p1));   // 对：所有权转移
        std::printf("  p1.get() = %p, p2.get() = %p\n",
                    static_cast<void*>(p1.get()), static_cast<void*>(p2.get()));
    }
    std::printf("仍然只析构一次\n\n");

    std::printf("--- 危险写法二：从一个 unique_ptr 的 get() 再包一个 ---\n");
    {
        auto p1 = std::make_unique<Res>(10);
                // std::unique_ptr<Res> p2(p1.get());  // 错！p1 和 p2 都认为自己是主人
                // 正确写法：
        std::unique_ptr<Res> p2 = std::move(p1);
        std::printf("  p1.get() = %p, p2.get() = %p\n",
                    static_cast<void*>(p1.get()), static_cast<void*>(p2.get()));
    }
    std::printf("main 结束\n");
    return 0;
}
