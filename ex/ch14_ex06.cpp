// ch14_ex06.cpp —— 【必做实验】用 weak_ptr 修掉循环引用，对比输出
#include <cstdio>
#include <memory>
#include <string>

struct Node {
    std::string name;
    std::shared_ptr<Node> strong;        // 强引用（会阻止析构）
    std::weak_ptr<Node>   weak;          // 弱引用（不增加引用计数）
    explicit Node(std::string n) : name(std::move(n))
    {
        std::printf("  Node(%s) 构造\n", name.c_str());
    }
    ~Node() { std::printf("  Node(%s) 析构\n", name.c_str()); }
};

int main()
{
    std::printf("=== 坏例子：用 shared_ptr 互指 ===\n");
    {
        auto a = std::make_shared<Node>("A");
        auto b = std::make_shared<Node>("B");
        a->strong = b;
        b->strong = a;
        std::printf("互指后：a.use_count()=%ld  b.use_count()=%ld\n",
                    a.use_count(), b.use_count());
        std::printf("离开作用域……\n");
    }
    std::printf(">> 观察：上面没有 Node 析构输出 —— 泄漏\n\n");

    std::printf("=== 好例子：一端改成 weak_ptr ===\n");
    {
        auto a = std::make_shared<Node>("A");
        auto b = std::make_shared<Node>("B");
        std::printf("刚创建：a.use_count()=%ld  b.use_count()=%ld\n",
                    a.use_count(), b.use_count());

        b->weak = a;      // B 弱引用 A —— 不计入 use_count
        a->strong = b;    // A 强引用 B
        std::printf("a->strong=b, b->weak=a 之后：a.use_count()=%ld  b.use_count()=%ld\n",
                    a.use_count(), b.use_count());

        std::printf("通过 weak 能不能拿到 A？");
        if (auto locked = b->weak.lock()) {
            std::printf(" 能，lock() 成功，拿到 name=%s，此时 a.use_count()=%ld\n",
                        locked->name.c_str(), a.use_count());
        } else {
            std::printf(" 不能（对象已死）\n");
        }
        std::printf("locked 离开作用域后 a.use_count()=%ld\n", a.use_count());
        std::printf("离开作用域……\n");
    }
    std::printf(">> 观察：A 和 B 都析构了，没有泄漏\n\n");

    std::printf("=== weak_ptr 过期后的行为 ===\n");
    {
        auto owner = std::make_shared<Node>("Owner");
        std::weak_ptr<Node> w = owner;
        std::printf("owner 还活着：w.expired()=%d  w.use_count()=%ld\n",
                    w.expired() ? 1 : 0, w.use_count());
        owner.reset();          // 最后一个强引用消失
        std::printf("owner.reset() 之后：w.expired()=%d  w.use_count()=%ld\n",
                    w.expired() ? 1 : 0, w.use_count());
        std::printf("再 lock() 一下：%s\n",
                    w.lock() ? "拿到了（不应该发生）" : "返回空 shared_ptr（正确）");
    }

    std::printf("main 结束\n");
    return 0;
}
