// ch14_ex11.cpp —— 三种异常安全保证：基本保证 / 强保证 / 不抛保证
#include <cstdio>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

// ---------- 1) 基本保证（basic guarantee）：不泄漏，但对象状态可能已改 ----------
class BasicBuf {
public:
    BasicBuf() : p_(new int[8]), n_(0) {}
    ~BasicBuf() { delete[] p_; }
    BasicBuf(const BasicBuf&) = delete;
    BasicBuf& operator=(const BasicBuf&) = delete;

        // 一个一个 push：第 fail_at 个抛异常，前面几个已经进去了 —— "有效但不确定"
    void push_many(int count, int fail_at)
    {
        for (int i = 0; i < count; ++i) {
            if (i == fail_at) {
                throw std::runtime_error("push_many 中途失败");
            }
            if (n_ < 8) {
                p_[n_++] = i + 1;
            }
        }
    }
    int n() const { return n_; }
private:
    int* p_;
    int  n_;
};

// ---------- 2) 强保证（strong guarantee）：要么成功，要么一点都不变 ----------
class StrongBuf {
public:
    void push_many(int count, int fail_at)
    {
        std::vector<int> tmp = data_;         // 先在副本上操作
        for (int i = 0; i < count; ++i) {
            if (i == fail_at) {
                throw std::runtime_error("push_many 中途失败");
            }
            tmp.push_back(i + 1);
        }
        data_.swap(tmp);                      // 只有"交换"这一步对外可见，它不抛异常
    }
    std::size_t size() const { return data_.size(); }
    const std::vector<int>& data() const { return data_; }
private:
    std::vector<int> data_;
};

// ---------- 3) 不抛保证（nothrow guarantee）：写进接口契约里 ----------
class NoThrowSwap {
public:
    explicit NoThrowSwap(std::string s) : s_(std::move(s)) {}
        // 标了 noexcept：编译器知道它绝不会抛，容器扩容时才敢用"移动"而不是"拷贝"
    void swap(NoThrowSwap& o) noexcept { s_.swap(o.s_); }
    const std::string& str() const { return s_; }
private:
    std::string s_;
};

int main()
{
    std::printf("--- 1) 基本保证：不泄漏，但状态变成\"部分完成\" ---\n");
    {
        BasicBuf b;
        try {
            b.push_many(4, 2);
        }
        catch (const std::exception& e) {
            std::printf("  捕获: %s\n", e.what());
        }
        std::printf("  异常后 b.n() = %d（前 2 个已经进去了）\n", b.n());
        std::printf("  内存没泄漏（析构会 delete[]），但对象处在中间状态\n");
    }

    std::printf("--- 2) 强保证：失败后完全回到原样（copy-and-swap）---\n");
    {
        StrongBuf s;
        std::printf("  调用前 size = %zu\n", s.size());
        try {
            s.push_many(5, 2);
        }
        catch (const std::exception& e) {
            std::printf("  捕获: %s\n", e.what());
        }
        std::printf("  异常后 size = %zu（一点都没变，这就是强保证）\n", s.size());

        s.push_many(3, -1);
        std::printf("  正常调用后 size = %zu, 内容 = ", s.size());
        for (int v : s.data()) std::printf("%d ", v);
        std::printf("\n");
    }

    std::printf("--- 3) 不抛保证：noexcept 是接口契约，也是性能来源 ---\n");
    {
        NoThrowSwap a("aaa"), b("bbb");
        std::printf("  swap 前: a=%s b=%s\n", a.str().c_str(), b.str().c_str());
        a.swap(b);
        std::printf("  swap 后: a=%s b=%s\n", a.str().c_str(), b.str().c_str());
        std::printf("  noexcept(a.swap(b)) = %d\n", noexcept(a.swap(b)) ? 1 : 0);
        std::printf("  noexcept 是编译期常量表达式，直接问编译器\n");
    }

    std::printf("main 结束\n");
    return 0;
}
