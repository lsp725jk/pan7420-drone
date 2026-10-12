// ch14_ex05.cpp —— 【必做实验】shared_ptr 循环引用：use_count 不为 0，对象不析构
#include <cstdio>
#include <memory>
#include <string>

struct Node {
    std::string name;
    std::shared_ptr<Node> next;          // 强引用：会 +1 引用计数
    explicit Node(std::string n) : name(std::move(n))
    {
        std::printf("  Node(%s) 构造\n", name.c_str());
    }
    ~Node() { std::printf("  Node(%s) 析构\n", name.c_str()); }
};

int main()
{
    std::printf("=== 实验一：两个 shared_ptr 互相指着对方（循环引用）===\n");
    {
        auto a = std::make_shared<Node>("A");
        auto b = std::make_shared<Node>("B");
        std::printf("刚创建：a.use_count()=%ld  b.use_count()=%ld\n",
                    a.use_count(), b.use_count());

        a->next = b;   // B 的引用计数 +1
        b->next = a;   // A 的引用计数 +1
        std::printf("互指之后：a.use_count()=%ld  b.use_count()=%ld\n",
                    a.use_count(), b.use_count());

        std::printf("离开作用域……\n");
    }
    std::printf("!! 作用域已结束，但上面没有出现 Node(A)/Node(B) 的析构 —— 内存泄漏了\n\n");

    std::printf("=== 实验二：单个 shared_ptr 自引用（自己指自己）===\n");
    {
        auto c = std::make_shared<Node>("C");
        c->next = c;   // 自己指自己：引用计数至少为 2
        std::printf("自指之后：c.use_count()=%ld\n", c.use_count());
        std::printf("离开作用域……\n");
    }
    std::printf("!! 同样没有析构\n\n");

    std::printf("=== 实验三：一对多的共享（正常情况，会正确析构）===\n");
    {
        auto owner = std::make_shared<Node>("Owner");
        auto p1 = owner;
        auto p2 = owner;
        std::printf("owner.use_count()=%ld\n", owner.use_count());
        std::printf("离开作用域……\n");
    }
    std::printf("这个析构了（引用计数归零的正确情形）\n");

    std::printf("main 结束\n");
    return 0;
}
