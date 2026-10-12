// ch23_ex18.cpp —— 例 23-18：移动语义到底省了什么
// 拷贝 = 申请新内存 + 把内容搬过去；
// 移动 = 把对方的指针偷过来，然后把对方置空。O(1) vs O(n)。
#include <chrono>
#include <cstdio>
#include <cstring>
#include <string>
#include <utility>
#include <vector>

// 一个"能看见拷贝和移动"的类
class Buffer {
public:
    explicit Buffer(std::size_t n) : n_(n), p_(new char[n]) { std::memset(p_, 0, n); }
    ~Buffer() { delete[] p_; }

    Buffer(const Buffer& o) : n_(o.n_), p_(new char[o.n_]) {  // 拷贝：分配 + 搬运
        std::memcpy(p_, o.p_, n_);
        ++copies;
    }
    Buffer(Buffer&& o) noexcept : n_(o.n_), p_(o.p_) {  // 移动：偷指针
        o.p_ = nullptr;
        o.n_ = 0;
        ++moves;
    }
    Buffer& operator=(const Buffer&) = delete;
    Buffer& operator=(Buffer&&) noexcept = default;

    static int copies;
    static int moves;

private:
    std::size_t n_;
    char* p_;
};
int Buffer::copies = 0;
int Buffer::moves = 0;

int main() {
    constexpr std::size_t SZ = 1 << 20;  // 1 MB

        // ---- 拷贝 ----
    {
        Buffer a(SZ);
        Buffer::copies = Buffer::moves = 0;
        const auto t0 = std::chrono::steady_clock::now();
        Buffer b(a);  // 拷贝构造
        const auto t1 = std::chrono::steady_clock::now();
        std::printf("拷贝 1 MB : %.3f ms   拷贝次数=%d 移动次数=%d\n",
                    std::chrono::duration<double, std::milli>(t1 - t0).count(), Buffer::copies,
                    Buffer::moves);
    }

        // ---- 移动 ----
    {
        Buffer a(SZ);
        Buffer::copies = Buffer::moves = 0;
        const auto t0 = std::chrono::steady_clock::now();
        Buffer b(std::move(a));  // 移动构造
        const auto t1 = std::chrono::steady_clock::now();
        std::printf("移动 1 MB : %.6f ms  拷贝次数=%d 移动次数=%d\n",
                    std::chrono::duration<double, std::milli>(t1 - t0).count(), Buffer::copies,
                    Buffer::moves);
    }

        // ---- std::string：最常见的受益者 ----
    {
        std::string big(1 << 20, 'x');
        const auto t0 = std::chrono::steady_clock::now();
        std::string c = big;  // 拷贝
        const auto t1 = std::chrono::steady_clock::now();
        std::string m = std::move(big);  // 移动
        const auto t2 = std::chrono::steady_clock::now();
        std::printf("\nstd::string 1 MB 拷贝 : %.3f ms\n",
                    std::chrono::duration<double, std::milli>(t1 - t0).count());
        std::printf("std::string 1 MB 移动 : %.6f ms\n",
                    std::chrono::duration<double, std::milli>(t2 - t1).count());
        std::printf("移动后原串长度 = %zu（被搬空了）\n", big.size());
        std::printf("移动后新串长度 = %zu\n", m.size());
    }

        // ---- vector 扩容：移动让"搬元素"变便宜 ----
    {
        std::vector<std::string> v;
        v.reserve(4);
        for (int i = 0; i < 4; ++i) {
            v.emplace_back(64, 'a');
        }
        std::printf("\nvector 扩容时会搬元素：\n");
        std::printf("  std::string 的移动构造是 noexcept -> 用移动，几乎免费\n");
        std::printf("  sizeof(std::string)=%zu（只搬这个头部，不搬字符数据）\n", sizeof(std::string));
    }

    std::printf("\n什么时候该用 std::move：\n");
    std::printf("  1) 你确实不再需要这个对象了（比如把局部变量交给容器）\n");
    std::printf("  2) 容器里放的是大对象（string/vector/自定义缓冲）\n");
    std::printf("  3) 写移动构造函数/移动赋值时\n");
    std::printf("不该用的时候：\n");
    std::printf("  1) return std::move(local);  -- 反而挡住 NRVO，更慢（见例 23-19）\n");
    std::printf("  2) 对 const 对象用 move    -- 退化成拷贝（拿不到可移动的右值）\n");
    std::printf("  3) 对 int/double 这种小类型 -- 没有意义\n");

        // 坑：移动之后的对象处于"有效但未指定"的状态，只能析构或重新赋值。
        // 千万不要 std::move(a) 之后还去读 a 的内容。
        // 坑：不是所有类型都有移动构造。只写了析构/拷贝的类型，
        // 编译器不会自动生成移动构造，std::move 会退化成拷贝而且不报错。
    return 0;
}
