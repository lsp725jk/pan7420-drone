// ch15_ex05.cpp —— 自己写移动构造与移动赋值：一个"拥有堆内存"的类
#include <cstdio>
#include <cstring>
#include <type_traits>
#include <utility>

class Buffer {
public:
        // ---- 普通构造 ----
    explicit Buffer(int n) : p_(new char[static_cast<std::size_t>(n)]), n_(n)
    {
        std::memset(p_, 0, static_cast<std::size_t>(n));
        std::printf("  [普通构造] 分配 %d 字节 @ %p\n", n_, static_cast<void*>(p_));
    }

        // ---- 析构 ----
    ~Buffer()
    {
        std::printf("  [析构] 释放 %p\n", static_cast<void*>(p_));
        delete[] p_;
    }

        // ---- 拷贝构造：深拷贝 ----
    Buffer(const Buffer& o) : p_(new char[static_cast<std::size_t>(o.n_)]), n_(o.n_)
    {
        std::memcpy(p_, o.p_, static_cast<std::size_t>(n_));
        std::printf("  [拷贝构造] 复制 %d 字节 -> %p\n", n_, static_cast<void*>(p_));
    }

        // ---- 拷贝赋值：copy-and-swap，天然异常安全 ----
        // 先把右操作数完整拷一份（这一步失败也不会动到自己），再交换，最后旧数据随临时对象析构
    Buffer& operator=(const Buffer& o)
    {
        std::printf("  [拷贝赋值] 入口：自己持有 %p，目标是复制 %p\n",
                    static_cast<void*>(p_), static_cast<const void*>(o.p_));
        Buffer tmp(o);        // 可能失败（内存不足抛 bad_alloc）——失败时自己丝毫未变
        swap(tmp);            // 不抛
        std::printf("  [拷贝赋值] 交换完成，现在持有 %p\n", static_cast<void*>(p_));
        return *this;
    }

        // ---- 移动构造：偷指针，把源对象置空 ----
    Buffer(Buffer&& o) noexcept : p_(o.p_), n_(o.n_)
    {
        o.p_ = nullptr;
        o.n_ = 0;
        std::printf("  [移动构造] 偷走 %p（O(1)，没有分配、没有复制）\n",
                    static_cast<void*>(p_));
    }

        // ---- 移动赋值：先放掉自己的，再偷对方的 ----
    Buffer& operator=(Buffer&& o) noexcept
    {
        if (this != &o) {
            std::printf("  [移动赋值] 先释放自己的 %p\n", static_cast<void*>(p_));
            delete[] p_;
            p_ = o.p_;
            n_ = o.n_;
            o.p_ = nullptr;
            o.n_ = 0;
            std::printf("  [移动赋值] 接管 %p\n", static_cast<void*>(p_));
        }
        return *this;
    }

    void swap(Buffer& o) noexcept
    {
        std::swap(p_, o.p_);
        std::swap(n_, o.n_);
    }

    const char* data() const { return p_; }
    int         size() const { return n_; }
    bool        empty() const { return p_ == nullptr; }
    void        fill(char c) { std::memset(p_, c, static_cast<std::size_t>(n_)); }

private:
    char* p_;
    int   n_;
};

int main()
{
    std::printf("--- 1) 拷贝构造 vs 移动构造 ---\n");
    {
        Buffer a(8);
        a.fill('A');
        std::printf("  接下来：Buffer b = a;（拷贝）\n");
        Buffer b = a;
        std::printf("  接下来：Buffer c = std::move(a);（移动）\n");
        Buffer c = std::move(a);
        std::printf("  移动后 a.empty() = %d（源对象被置空）\n", a.empty() ? 1 : 0);
        std::printf("  b.data() = %p  c.data() = %p（不同内存，各管各的）\n",
                    static_cast<const void*>(b.data()), static_cast<const void*>(c.data()));
    }

    std::printf("--- 2) 移动赋值 ---\n");
    {
        Buffer x(4);
        Buffer y(16);
        std::printf("  接下来：y = std::move(x);\n");
        y = std::move(x);
        std::printf("  y.size() = %d  x.empty() = %d\n", y.size(), x.empty() ? 1 : 0);
    }

    std::printf("--- 3) 没有移动构造会怎样：退化成拷贝 ---\n");
    {
        struct NoMove {
            int* p;
            explicit NoMove(int v) : p(new int(v)) {}
            ~NoMove() { delete p; }
            NoMove(const NoMove& o) : p(new int(*o.p)) { std::printf("    NoMove 拷贝构造\n"); }
                        // 没有移动构造：编译器不会自动生成（因为有用户声明的析构和拷贝构造）
            NoMove& operator=(const NoMove& o) { *p = *o.p; return *this; }
        };
        NoMove m(5);
        std::printf("  用 std::move(m) 造对象：\n");
        NoMove n(std::move(m));     // 静默地调用了拷贝构造！
        std::printf("  n 的值 = %d，m 的值 = %d（m 没被搬空）\n", *n.p, *m.p);
        std::printf("  这就是坑：move 只是类型转换，能不能真的移动取决于类型有没有移动构造\n");
    }

    std::printf("--- 4) 移动构造 noexcept 与否，用类型特性看 ---\n");
    {
        std::printf("  std::is_nothrow_move_constructible_v<Buffer> = %d\n",
                    std::is_nothrow_move_constructible_v<Buffer>);
        std::printf("  std::is_move_constructible_v<Buffer>         = %d\n",
                    std::is_move_constructible_v<Buffer>);
        std::printf("  std::is_copy_constructible_v<Buffer>         = %d\n",
                    std::is_copy_constructible_v<Buffer>);
    }

    std::printf("main 结束\n");
    return 0;
}
