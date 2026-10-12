// ch23_ex09.cpp —— 例 23-9：环形缓冲索引 + 无分支写法
// 串口接收、ADC 采样、日志缓冲都要环形队列。
// 只要缓冲大小是 2 的幂，取模就能变成按位与——这在 M0+ 上是实打实的省时间。
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>

// ---- 缓冲区大小取 2 的幂 ----
constexpr std::size_t kBufSize = 8;  // 用 2 的幂！
static_assert((kBufSize & (kBufSize - 1)) == 0, "必须是 2 的幂");

class RingBuffer {
public:
    void push(std::uint8_t v) {
        buf_[head_ & (kBufSize - 1)] = v;  // 等价于 head_ % kBufSize
        ++head_;
        if (head_ - tail_ > kBufSize) {
            tail_ = head_ - kBufSize;  // 满了就丢最旧的
            ++dropped_;
        }
    }

    bool pop(std::uint8_t& out) {
        if (head_ == tail_) {
            return false;
        }
        out = buf_[tail_ & (kBufSize - 1)];
        ++tail_;
        return true;
    }

    std::size_t size() const { return head_ - tail_; }
    std::uint32_t dropped() const { return dropped_; }

private:
    std::array<std::uint8_t, kBufSize> buf_{};
    std::uint32_t head_ = 0;
    std::uint32_t tail_ = 0;
    std::uint32_t dropped_ = 0;
};

int main() {
    RingBuffer rb;
    std::printf("缓冲大小 = %zu（2 的幂），索引用 (i & %zu) 代替 i %% %zu\n", kBufSize, kBufSize - 1,
                kBufSize);

        // 塞 10 个字节进 8 字节的缓冲：会有 2 个被丢掉
    for (int i = 0; i < 10; ++i) {
        rb.push(static_cast<std::uint8_t>(0xA0 + i));
    }
    std::printf("塞入 10 个，size=%zu，丢弃=%u\n", rb.size(), rb.dropped());

    std::uint8_t v = 0;
    std::printf("取出: ");
    while (rb.pop(v)) {
        std::printf("0x%02X ", v);
    }
    std::printf("\n");
    std::printf("空队列再取一次: %d（返回 false，不越界）\n", (int)rb.pop(v));

        // ---- 无分支的绝对值 / 饱和 ----
        // 用位运算代替 if，在 M0+ 上省下分支预测失败的代价（M0+ 连分支预测都没有）
    auto abs_branchless = [](std::int32_t x) {
        const std::int32_t m = x >> 31;  // 负数时 m = -1（全 1），非负时 0
        return (x ^ m) - m;
    };
    for (std::int32_t x : {5, -5, 0, -2147483647}) {
        std::printf("abs_branchless(%d) = %d\n", x, abs_branchless(x));
    }

        // ---- 限幅（clamp）的无分支写法 ----
    auto clamp01 = [](std::int32_t x) {
                // 先和 0 取大，再和 255 取小
        x = x < 0 ? 0 : x;
        return x > 255 ? 255 : x;
    };
    std::printf("clamp01(-10)=%d clamp01(100)=%d clamp01(999)=%d\n", clamp01(-10), clamp01(100),
                clamp01(999));

        // 坑 1：缓冲大小不是 2 的幂时，(i & (N-1)) 就是错的，而且**不会报错**，
        // 只会偶尔读到脏数据——这类 bug 最难查。
        // 坑 2：head_/tail_ 用无符号整数递增，溢出回绕是**故意**的设计
        // （这样就不用每次归零）。但如果类型是有符号 int，溢出就是 UB。
    std::printf("sizeof(RingBuffer) = %zu 字节\n", sizeof(RingBuffer));
    return 0;
}
