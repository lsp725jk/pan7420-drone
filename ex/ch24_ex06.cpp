// ch24_ex06.cpp —— 例 24-6：RAII（资源获取即初始化）封装 C 资源
// C 的写法：拿资源 -> 用 -> 手动释放，中间任何一个 return/break 忘了释放就漏。
// C++ 的写法：把资源放进一个对象的生命周期里，构造时拿、析构时放。
#include <cstdio>
#include <cstring>
#include <utility>

// ==================================================================
// 场景：一段临界区（关中断 / 上锁），必须成对出现
// ==================================================================
// ---- C 写法 ----
int g_irq_nesting = 0;
void irq_disable() {
    ++g_irq_nesting;
    std::printf("    [irq] 关中断 (嵌套=%d)\n", g_irq_nesting);
}
void irq_enable() {
    --g_irq_nesting;
    std::printf("    [irq] 开中断 (嵌套=%d)\n", g_irq_nesting);
}

void c_style_update(int v) {
    irq_disable();
    if (v < 0) {
        irq_enable();  // 忘了这一行，中断就永远关着了 —— 系统直接卡死
        return;
    }
    std::printf("    [c]  更新值 %d\n", v);
    irq_enable();
}

// ---- C++ 写法：RAII ----
class IrqGuard {
public:
    IrqGuard() { irq_disable(); }
    ~IrqGuard() { irq_enable(); }  // 无论怎么退出（return / 异常 / break）都会执行

    IrqGuard(const IrqGuard&) = delete;
    IrqGuard& operator=(const IrqGuard&) = delete;

        // 允许移动：把"负责释放"的责任转交出去，自己不再释放
    IrqGuard(IrqGuard&& o) noexcept : active_(o.active_) { o.active_ = false; }
    IrqGuard& operator=(IrqGuard&&) = delete;

private:
    bool active_ = true;
};

void cpp_style_update(int v) {
    IrqGuard g;  // 构造函数关中断
    if (v < 0) {
        std::printf("    [cpp] 参数非法，直接 return\n");
        return;  // 析构函数自动开中断，不可能忘
    }
    std::printf("    [cpp] 更新值 %d\n", v);
}

// ==================================================================
// 第二种资源：堆内存
// ==================================================================
class Buffer {
public:
    explicit Buffer(std::size_t n) : p_(new char[n]), n_(n) {
        std::memset(p_, 0, n);
        std::printf("    [buf] 申请 %zu 字节\n", n);
    }
    ~Buffer() {
        delete[] p_;
        std::printf("    [buf] 释放\n");
    }
    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;
    char* data() { return p_; }
    std::size_t size() const { return n_; }

private:
    char* p_;
    std::size_t n_;
};

int main() {
    std::printf("C 风格（手工配对）：\n");
    c_style_update(42);
    std::printf("  嵌套计数回到 %d\n", g_irq_nesting);

    std::printf("\nC++ 风格（RAII）：\n");
    cpp_style_update(42);
    cpp_style_update(-1);  // 提前 return，照样安全
    std::printf("  嵌套计数回到 %d\n", g_irq_nesting);

    std::printf("\n内存资源：\n");
    {
        Buffer b(1024);
        std::printf("    用一下 b：size=%zu\n", b.size());
    }  // 出作用域自动释放
    std::printf("  出了作用域，内存已经还回去了\n");

    std::printf("\n标准库里的 RAII 例子（都是这个套路）：\n");
    std::printf("  std::vector / std::string  管理堆内存\n");
    std::printf("  std::lock_guard / unique_lock  管理互斥量\n");
    std::printf("  std::unique_ptr / shared_ptr   管理裸指针\n");
    std::printf("  std::fstream                   管理文件句柄\n");

    std::printf("\n坑 1：RAII 类被**拷贝**时会 double free。\n");
    std::printf("   上面两个类都显式 delete 了拷贝构造，这是必须的（见例 24-7）。\n");
    std::printf("坑 2：析构函数里不要抛异常（会 std::terminate）。\n");
    std::printf("坑 3：构造过程中抛异常时，已构造的成员会被析构，\n");
    std::printf("   但**你自己**的析构函数不会调用 —— 所以构造函数里要按顺序申请、\n");
    std::printf("   或者直接用 RAII 成员，别手工 new。\n");
    return 0;
}
