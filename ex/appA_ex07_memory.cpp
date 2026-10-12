// 附录A 探针 7：内存管理对照（malloc/free vs new/delete vs 智能指针）
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <new>
#include <string>

struct Ctor {
    int v;
    Ctor() : v(7)            { std::puts("    Ctor() 被调用"); }
    Ctor(int x) : v(x)       { std::printf("    Ctor(%d) 被调用\n", x); }
    ~Ctor()                  { std::printf("    ~Ctor(v=%d) 被调用\n", v); }
};

int main() {
    std::puts("=== 1. malloc 不会调用构造函数；new 会 ===");
    std::puts("  malloc + 手动初始化:");
    Ctor* a = (Ctor*)std::malloc(sizeof(Ctor));   // 只分配内存
    a->v = 100;                                    // 对象其实还没"活"
    std::printf("    a->v=%d（构造函数从未运行）\n", a->v);
    std::free(a);

    std::puts("  new 表达式:");
    Ctor* b = new Ctor(200);
    std::printf("    b->v=%d\n", b->v);
    delete b;

    std::puts("=== 2. 数组版本 ===");
    int* ca = (int*)std::malloc(4 * sizeof(int));
    std::memset(ca, 0, 4 * sizeof(int));
    std::printf("  malloc 的数组没有长度信息，长度只能自己记: sizeof(ca)=%zu\n", sizeof(ca));
    std::free(ca);

    int* na = new int[4]{1, 2, 3, 4};
    std::printf("  new int[4]{1,2,3,4} -> %d %d %d %d\n", na[0], na[1], na[2], na[3]);
    delete[] na;                                  // 必须 delete[]，不能用 delete

    std::puts("=== 3. 三种写法做同一件事（申请一个 int，初始化为 42）===");
    {
        int* p1 = (int*)std::malloc(sizeof(int)); *p1 = 42;
        std::printf("  C    malloc + 赋值 : *p1=%d\n", *p1); std::free(p1);

        int* p2 = new int(42);
        std::printf("  C++  new int(42)   : *p2=%d\n", *p2); delete p2;

        auto p3 = std::make_unique<int>(42);
        std::printf("  C++  make_unique   : *p3=%d\n", *p3);   // 自动释放
    }

    std::puts("=== 4. 异常安全：new 抛异常时 malloc 的写法会漏内存 ===");
    try {
                // 新写法：make_unique 内部保证"要么两个都成功，要么都不泄漏"
        auto up = std::make_unique<Ctor>(1);
        auto up2 = std::make_unique<Ctor>(2);
        std::printf("  两个对象都构造成功\n");
    } catch (const std::bad_alloc&) {
        std::puts("  内存不足");
    }

    std::puts("=== 5. 申请失败的行为不同 ===");
    void* huge = std::malloc((std::size_t)-1);       // malloc 失败返回 NULL
    std::printf("  malloc(巨大值) 返回: %s\n", huge ? "非空" : "NULL");
    std::printf("  而 new 失败会抛 std::bad_alloc（除非用 new(std::nothrow)）\n");
    int* nt = new (std::nothrow) int[100];
    std::printf("  new(std::nothrow) int[100] 成功? %s\n", nt ? "是" : "否");
    delete[] nt;

    std::puts("=== 6. 对齐：malloc 保证 max_align_t，new 保证类型对齐 ===");
    std::printf("  alignof(std::max_align_t)=%zu\n", alignof(std::max_align_t));
    struct alignas(64) Big { char c[64]; };
    Big* ba = new Big();
    std::printf("  new Big() 地址 %% 64 = %zu （应当为 0）\n", (std::size_t)ba % 64);
    delete ba;

    std::puts("=== 7. 智能指针 vs 裸指针：异常路径也能释放 ===");
    {
        auto sp = std::make_shared<std::string>("hello");
        std::printf("  shared_ptr 管着: %s\n", sp->c_str());
    }
    std::puts("  （离开作用域，字符串内存已释放）");

    std::puts("=== 8. 什么时候还需要手写 new/delete ===");
    std::puts("  - 实现自己的容器 / 内存池");
    std::puts("  - 需要精确控制对象生命周期（placement new）");
    {
        alignas(Ctor) unsigned char buf[sizeof(Ctor)];
        Ctor* p = new (buf) Ctor(9);          // placement new：只在已有内存上构造
        std::printf("  placement new 构造在栈缓冲区里: v=%d\n", p->v);
        p->~Ctor();                            // 必须手动调用析构（不能 delete）
    }
    return 0;
}
