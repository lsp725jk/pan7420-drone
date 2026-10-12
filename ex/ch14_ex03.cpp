// ch14_ex03.cpp —— unique_ptr 的四种写法：make_unique / 直接 new / release / reset / get
#include <cstdio>
#include <memory>
#include <utility>

struct Widget {
    int id;
    explicit Widget(int i) : id(i) { std::printf("  Widget(%d) 构造\n", id); }
    ~Widget() { std::printf("  Widget(%d) 析构\n", id); }
    void hello() const { std::printf("  Widget(%d)::hello()\n", id); }
};

int main()
{
    std::printf("--- 1) make_unique：推荐写法 ---\n");
    {
        auto p = std::make_unique<Widget>(1);   // 推荐：一次表达式完成"分配+构造"
        p->hello();
        std::printf("  p.get() = %p, (bool)p = %d\n", static_cast<void*>(p.get()), p ? 1 : 0);
    }

    std::printf("--- 2) 直接 new：能work但不推荐 ---\n");
    {
        std::unique_ptr<Widget> p(new Widget(2));   // 合法，但分配和交给智能指针是两步
        p->hello();
    }

    std::printf("--- 3) release：放弃所有权，交回裸指针 ---\n");
    {
        auto p = std::make_unique<Widget>(3);
        Widget* raw = p.release();                  // p 变空，谁拿到 raw 谁负责 delete
        std::printf("  release 后 p.get() = %p, (bool)p = %d\n",
                    static_cast<void*>(p.get()), p ? 1 : 0);
        std::printf("  raw = %p，需要手工 delete\n", static_cast<void*>(raw));
        delete raw;                                 // 不 delete 就是泄漏
    }

    std::printf("--- 4) reset：换一个新对象，旧的自动析构 ---\n");
    {
        auto p = std::make_unique<Widget>(4);
        std::printf("  reset 之前 p->id = %d\n", p->id);
        p.reset(new Widget(5));                     // Widget(4) 在这里析构
        std::printf("  reset 之后 p->id = %d\n", p->id);

        p.reset();                                  // 传 nullptr：只析构，不接管新对象
        std::printf("  reset() 之后 (bool)p = %d\n", p ? 1 : 0);
    }

    std::printf("--- 5) 移动：所有权搬家（不是拷贝）---\n");
    {
        auto a = std::make_unique<Widget>(6);
        std::printf("  a.get() = %p\n", static_cast<void*>(a.get()));
        auto b = std::move(a);                      // a 被掏空，b 接管
        std::printf("  移动后 a.get() = %p, b.get() = %p\n",
                    static_cast<void*>(a.get()), static_cast<void*>(b.get()));
        std::printf("  b->id = %d\n", b->id);
    }   // b 析构，Widget(6) 只析构一次

    std::printf("main 结束\n");
    return 0;
}
