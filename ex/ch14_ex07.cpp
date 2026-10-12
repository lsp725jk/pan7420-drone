// ch14_ex07.cpp —— shared_ptr 的三种创建方式、use_count 变化、为什么优先 make_shared
#include <cstdio>
#include <memory>

struct Big {
    int x, y, z;
    Big() : x(1), y(2), z(3) { std::printf("  Big 构造\n"); }
    ~Big() { std::printf("  Big 析构\n"); }
};

int main()
{
    std::printf("--- 写法 1：make_shared（推荐，一次分配）---\n");
    {
        auto p = std::make_shared<Big>();
        std::printf("  use_count=%ld\n", p.use_count());
    }

    std::printf("--- 写法 2：shared_ptr<T>(new T)：两次分配 ---\n");
    {
        std::shared_ptr<Big> p(new Big());
        std::printf("  use_count=%ld\n", p.use_count());
    }

    std::printf("--- 写法 3：reset 接管 ---\n");
    {
        std::shared_ptr<Big> p;
        std::printf("  默认构造的 p：(bool)p=%d use_count=%ld\n", p ? 1 : 0, p.use_count());
        p.reset(new Big());
        std::printf("  reset 之后：use_count=%ld\n", p.use_count());
    }

    std::printf("--- use_count 随拷贝/析构变化 ---\n");
    {
        auto a = std::make_shared<Big>();
        std::printf("  a 创建后       use_count=%ld\n", a.use_count());
        {
            auto b = a;                                   // 拷贝：计数 +1
            std::printf("  b = a 之后     use_count=%ld\n", a.use_count());
            std::shared_ptr<Big> c = b;                   // 再拷贝
            std::printf("  c = b 之后     use_count=%ld\n", a.use_count());
            auto d = std::move(c);                        // 移动：计数不变
            std::printf("  d = move(c) 后 use_count=%ld  (bool)c=%d\n",
                        a.use_count(), c ? 1 : 0);
        }                                                 // b、d 析构：计数 -2
        std::printf("  内层作用域结束 use_count=%ld\n", a.use_count());
        a.reset();
        std::printf("  a.reset() 之后  use_count=%ld\n", a.use_count());
    }

    std::printf("main 结束\n");
    return 0;
}
