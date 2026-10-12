// ch14_ex10.cpp —— RAII 封装自己的资源类：把 C 的 open/close 变成"活的"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <utility>

// 假设这是一段单片机/驱动的 C 接口：申请一块 DMA 缓冲区，必须成对申请/释放
namespace driver {
    int  dma_alloc(int bytes, void** out)   // 返回 0 成功，负数为错误码
    {
        if (bytes <= 0) return -1;
        void* p = std::malloc(static_cast<size_t>(bytes));
        if (!p) return -2;
        *out = p;
        std::printf("  [驱动] 分配 %d 字节 -> %p\n", bytes, p);
        return 0;
    }
    void dma_free(void* p)
    {
        std::printf("  [驱动] 释放 %p\n", p);
        std::free(p);
    }
}

// ---------- RAII 包装类：构造时获取，析构时释放 ----------
class DmaBuffer {
public:
    explicit DmaBuffer(int bytes) : p_(nullptr), size_(bytes)
    {
        if (driver::dma_alloc(bytes, &p_) != 0) {
            throw std::runtime_error("DMA 分配失败");   // 构造失败就抛异常，不产生半成品对象
        }
    }
    ~DmaBuffer() { driver::dma_free(p_); }              // 唯一需要写释放的地方

        // 资源类必须"不可拷贝"：否则会 double free
    DmaBuffer(const DmaBuffer&)            = delete;
    DmaBuffer& operator=(const DmaBuffer&) = delete;

        // 但可以"移动"：所有权搬家，源对象置空
    DmaBuffer(DmaBuffer&& o) noexcept : p_(o.p_), size_(o.size_)
    {
        o.p_ = nullptr;
        std::printf("  [DmaBuffer] 移动构造\n");
    }
    DmaBuffer& operator=(DmaBuffer&& o) noexcept
    {
        if (this != &o) {
            driver::dma_free(p_);       // 先放掉自己手里的
            p_ = o.p_;
            size_ = o.size_;
            o.p_ = nullptr;
            std::printf("  [DmaBuffer] 移动赋值\n");
        }
        return *this;
    }

    void*       data()       { return p_; }
    const void* data() const { return p_; }
    int         size() const { return size_; }
    bool        valid() const { return p_ != nullptr; }

private:
    void* p_;
    int   size_;
};

int main()
{
    std::printf("--- 1) 基本用法：自动释放 ---\n");
    {
        DmaBuffer b(128);
        std::printf("  b.valid()=%d b.size()=%d\n", b.valid() ? 1 : 0, b.size());
        std::memset(b.data(), 0, static_cast<size_t>(b.size()));
        std::printf("  离开作用域……\n");
    }

    std::printf("--- 2) 中途 return / 抛异常都不会漏 ---\n");
    try {
        DmaBuffer b(256);
        std::printf("  故意抛异常\n");
        throw std::runtime_error("模拟错误");
    }
    catch (const std::exception& e) {
        std::printf("  捕获: %s（注意上面已经打印了释放）\n", e.what());
    }

    std::printf("--- 3) 构造失败时的行为 ---\n");
    try {
        DmaBuffer bad(-1);        // 驱动返回 -1，构造函数抛异常
        std::printf("  不该走到这里\n");
    }
    catch (const std::exception& e) {
        std::printf("  捕获: %s（没有产生半成品对象，也不会调用析构）\n", e.what());
    }

    std::printf("--- 4) 移动：放进容器 ---\n");
    {
        DmaBuffer a(64);
        DmaBuffer c(std::move(a));          // 移动构造
        std::printf("  移动后 a.valid()=%d  c.valid()=%d\n", a.valid() ? 1 : 0, c.valid() ? 1 : 0);
        std::printf("  离开作用域（只有 c 需要释放）……\n");
    }

    std::printf("main 结束\n");
    return 0;
}
