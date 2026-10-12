// 附录A 探针 4：指针 / 引用 / 智能指针
#include <cstdio>
#include <memory>
#include <vector>
#include <string>
#include <cstring>

struct Widget {
    int id;
    explicit Widget(int i) : id(i) { std::printf("  [构造] Widget(%d)\n", id); }
    ~Widget() { std::printf("  [析构] Widget(%d)\n", id); }
};

int main() {
    std::puts("=== 1. 指针 vs 引用：写法差异 ===");
    int x = 10, y = 20;
    int* p = &x;
    int& r = x;                    // 引用必须在定义时绑定，且不能改绑
    *p = 11;
    r = 12;                        // 改的是 x 本身
    p = &y;                        // 指针可以改指向
    std::printf("x=%d y=%d r=%d *p=%d  sizeof(int*)=%zu sizeof(int&)=%zu\n",
                x, y, r, *p, sizeof(int*), sizeof(int&));

    std::puts("=== 2. 空指针的几种写法 ===");
    int* p1 = nullptr;             // C++11 起
    int* p2 = NULL;                // 来自 <cstddef>，本质是 0
    int* p3 = 0;
    std::printf("p1==p2: %d  p2==p3: %d  sizeof(NULL)=%zu sizeof(nullptr_t)=%zu\n",
                p1 == p2, p2 == p3, sizeof(NULL), sizeof(std::nullptr_t));

    std::puts("=== 3. 指针与数组：sizeof 的陷阱 ===");
    int buf[8] = {0};
    int* pb = buf;
    std::printf("sizeof(buf)=%zu  sizeof(pb)=%zu  （函数形参里的 int a[] 其实是指针）\n",
                sizeof(buf), sizeof(pb));

    std::puts("=== 4. unique_ptr：独占所有权 ===");
    {
        std::unique_ptr<Widget> u1 = std::make_unique<Widget>(1);
        std::printf("  u1->id=%d  sizeof(unique_ptr<Widget>)=%zu\n", u1->id, sizeof(u1));
        std::unique_ptr<Widget> u2 = std::move(u1);      // 所有权转移
        std::printf("  移动后 u1 是否为空: %d, u2->id=%d\n", (u1 == nullptr), u2->id);
    }   // 离开作用域自动 delete

    std::puts("=== 5. shared_ptr / weak_ptr：共享所有权 + 打破循环引用 ===");
    {
        std::shared_ptr<Widget> s1 = std::make_shared<Widget>(2);
        std::printf("  构造后 use_count=%ld  sizeof(shared_ptr)=%zu\n",
                    (long)s1.use_count(), sizeof(s1));
        std::weak_ptr<Widget> w = s1;
        std::printf("  存进 weak_ptr 后 use_count=%ld （weak 不增加计数）\n", (long)s1.use_count());
        std::shared_ptr<Widget> s2 = s1;
        std::printf("  再拷贝一份 use_count=%ld\n", (long)s1.use_count());
        if (auto locked = w.lock()) {
            std::printf("  weak_ptr::lock() 成功, id=%d\n", locked->id);
        }
        s1.reset(); s2.reset();
        std::printf("  全部 reset 后 use_count=%ld, weak 是否过期=%d\n",
                    (long)w.use_count(), w.expired());
    }

    std::puts("=== 6. 自定义删除器（把 C 的 malloc/free、FILE* 也管起来）===");
    {
        auto del = [](int* q) { std::printf("  [自定义删除器] free(%p)\n", (void*)q); std::free(q); };
        std::unique_ptr<int, decltype(del)> up((int*)std::malloc(sizeof(int)), del);
        *up = 42;
        std::printf("  托管 malloc 内存: *up=%d sizeof(unique_ptr<int,deleter>)=%zu\n",
                    *up, sizeof(up));
    }

    std::puts("=== 7. 智能指针大小对照 ===");
    std::printf("  sizeof(int*)=%zu  unique_ptr<int>=%zu  shared_ptr<int>=%zu  weak_ptr<int>=%zu\n",
                sizeof(int*), sizeof(std::unique_ptr<int>), sizeof(std::shared_ptr<int>),
                sizeof(std::weak_ptr<int>));

    std::puts("=== 8. 观察者/非拥有指针：C++ 里就用裸指针或引用 ===");
    Widget stackWidget(99);
    Widget* observer = &stackWidget;      // 不拥有，不负责 delete
    std::printf("  observer->id=%d\n", observer->id);
    return 0;
}
