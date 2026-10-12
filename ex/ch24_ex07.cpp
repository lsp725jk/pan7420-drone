// ch24_ex07.cpp —— 例 24-7：RAII 的坑——拷贝与"三/五法则"
// 只写了析构函数、没管拷贝构造的类，拷贝一次就会 double free。
// 这个 bug 在 PC 上被 ASan 一眼抓住，在 MCU 上表现为"跑几秒就重启"。
#include <cstdio>
#include <cstring>
#include <memory>
#include <utility>

// ==================================================================
// 错误示范：只写了析构，没禁用/实现拷贝
// ==================================================================
class BadBuffer {
public:
    explicit BadBuffer(std::size_t n) : p_(new char[n]), n_(n) {}
    ~BadBuffer() {
        delete[] p_;
        std::printf("  [Bad] 析构，释放 %p\n", (void*)p_);
    }
    char* data() { return p_; }

private:
    char* p_;
    std::size_t n_;
};

// ==================================================================
// 正确示范：五法则（Rule of Five）
// 如果需要一个自定义的析构 / 拷贝构造 / 拷贝赋值 / 移动构造 / 移动赋值，
// 那这五个基本都要一起考虑。
// ==================================================================
class GoodBuffer {
public:
    explicit GoodBuffer(std::size_t n) : p_(new char[n]), n_(n) {}

    ~GoodBuffer() { delete[] p_; }

        // 1) 拷贝构造：深拷贝
    GoodBuffer(const GoodBuffer& o) : p_(new char[o.n_]), n_(o.n_) {
        std::memcpy(p_, o.p_, n_);
    }
        // 2) 拷贝赋值：先拷贝再交换（copy-and-swap，天然异常安全）
    GoodBuffer& operator=(const GoodBuffer& o) {
        if (this != &o) {
            GoodBuffer tmp(o);
            swap(tmp);
        }
        return *this;
    }
        // 3) 移动构造：偷指针
    GoodBuffer(GoodBuffer&& o) noexcept : p_(o.p_), n_(o.n_) {
        o.p_ = nullptr;
        o.n_ = 0;
    }
        // 4) 移动赋值
    GoodBuffer& operator=(GoodBuffer&& o) noexcept {
        if (this != &o) {
            delete[] p_;
            p_ = o.p_;
            n_ = o.n_;
            o.p_ = nullptr;
            o.n_ = 0;
        }
        return *this;
    }

    void swap(GoodBuffer& o) noexcept {
        std::swap(p_, o.p_);
        std::swap(n_, o.n_);
    }

    const char* data() const { return p_; }
    char* data() { return p_; }
    std::size_t size() const { return n_; }

private:
    char* p_;
    std::size_t n_;
};

// ==================================================================
// 更省事的办法：直接用标准库的智能指针，一个都不用写
// ==================================================================
class EasyBuffer {
public:
    explicit EasyBuffer(std::size_t n) : p_(std::make_unique<char[]>(n)), n_(n) {}
        // 析构、移动：编译器自动生成；拷贝：自动删除（unique_ptr 不可拷贝）
    char* data() { return p_.get(); }
    std::size_t size() const { return n_; }

private:
    std::unique_ptr<char[]> p_;
    std::size_t n_;
};

int main() {
    std::printf("错误示范（不要运行，这里只是演示会释放同一个指针两次）：\n");
    std::printf("  BadBuffer a(16);\n");
    std::printf("  BadBuffer b = a;   // 编译器生成的拷贝：b.p_ == a.p_\n");
    std::printf("  // 离开作用域：a 和 b 各 delete[] 一次同一个指针 -> double free\n");
    std::printf("  本机用 ASan 会直接报 heap-use-after-free / double-free（见例 24-18）\n\n");

    std::printf("正确示范：\n");
    {
        GoodBuffer a(8);
        std::memset(a.data(), 'A', a.size());
        GoodBuffer b = a;  // 深拷贝，两块独立内存
        std::printf("  a.data()=%p  b.data()=%p  不同: %d\n", (const void*)a.data(),
                    (const void*)b.data(), (int)(a.data() != b.data()));

        GoodBuffer c = std::move(a);  // 移动：c 接管 a 的指针
        std::printf("  c.data()=%p  a.data()=%p（a 被搬空）\n", (const void*)c.data(),
                    (const void*)a.data());
    }

    std::printf("\n用标准库智能指针的版本（什么都写，但自动就对）：\n");
    {
        EasyBuffer e(8);
        std::printf("  e.size()=%zu\n", e.size());
                // EasyBuffer f = e;   // 编译期就报错：拷贝构造被删除
        EasyBuffer g = std::move(e);  // 移动没问题
        std::printf("  移动后 g.size()=%zu\n", g.size());
    }

    std::printf("\n三条实用规则：\n");
    std::printf("  1) 优先用标准库的 RAII 类型（vector/string/unique_ptr/lock_guard），\n");
    std::printf("     自己写的 RAII 类越少越好。\n");
    std::printf("  2) 真要靠自己管理资源时，记住【五法则】：析构、拷贝构造、拷贝赋值、\n");
    std::printf("     移动构造、移动赋值，缺一个都可能出问题。\n");
    std::printf("  3) 只想要【不可拷贝】，就显式 = delete，让编译器帮你拦住误用。\n");
    return 0;
}
