// ch24_ex10.cpp —— 例 24-10：依赖注入（Dependency Injection）与可测试性
// 核心想法：**需要什么，就从外面传进来**，而不是在里面 new 出来或读全局单例。
// 这样测试时可以换成假的实现。
#include <cstdio>
#include <memory>
#include <string>
#include <vector>

// ---- 抽象出"时间"这个依赖 ----
class Clock {
public:
    virtual ~Clock() = default;
    virtual std::uint32_t millis() const = 0;
};

// ---- 真实实现：读硬件 ----
class HardwareClock : public Clock {
public:
    std::uint32_t millis() const override {
        ++calls_;
        return 1000u * calls_;  // 假装每次前进 1 秒
    }
    int calls() const { return calls_; }

private:
    mutable int calls_ = 0;
};

// ---- 测试替身（test double）：完全可控 ----
class FakeClock : public Clock {
public:
    explicit FakeClock(std::uint32_t t) : now_(t) {}
    std::uint32_t millis() const override { return now_; }
    void advance(std::uint32_t d) { now_ += d; }

private:
    std::uint32_t now_;
};

// ---- 被测试的业务逻辑：超时看门狗 ----
class Watchdog {
public:
        // 依赖从构造函数注入（"构造注入"），而不是在内部 new HardwareClock()
    Watchdog(Clock& clock, std::uint32_t timeout_ms)
        : clock_(clock), timeout_ms_(timeout_ms), last_(clock.millis()) {}

    void feed() { last_ = clock_.millis(); }

    bool expired() const { return clock_.millis() - last_ > timeout_ms_; }

private:
    Clock& clock_;  // 引用：不拥有这个依赖，也就不负责它的生命周期
    std::uint32_t timeout_ms_;
    std::uint32_t last_;
};

// ---- 反面示范：内部写死依赖 ----
class BadWatchdog {
public:
    explicit BadWatchdog(std::uint32_t timeout_ms) : timeout_ms_(timeout_ms) {
        last_ = hw_.millis();  // 写死了，测试没法控制时间
    }
    void feed() { last_ = hw_.millis(); }
    bool expired() const { return hw_.millis() - last_ > timeout_ms_; }

private:
    HardwareClock hw_;  // 硬依赖，测试时无法替换
    std::uint32_t timeout_ms_;
    std::uint32_t last_;
};

int main() {
    std::printf("=== 注入版：测试可以完全控制时间 ===\n");
    FakeClock fake(0);
    Watchdog wd(fake, 100);  // 超时 100ms

    std::printf("t=0    喂狗 -> expired = %d\n", (int)wd.expired());
    fake.advance(50);
    std::printf("t=50   未超时 -> expired = %d\n", (int)wd.expired());
    fake.advance(60);
    std::printf("t=110  超时了 -> expired = %d\n", (int)wd.expired());
    wd.feed();
    std::printf("t=110  喂狗后 -> expired = %d\n", (int)wd.expired());

    std::printf("\n注意：不需要等待真实时间，测试瞬间完成、而且 100%% 可重复。\n");

    std::printf("\n=== 写死依赖的版本只能这么测 ===\n");
    BadWatchdog bad(100);
    std::printf("expired = %d（只能靠真实硬件时钟，测试要么很慢要么不可靠）\n", (int)bad.expired());

    std::printf("\n依赖注入的三种常见形式：\n");
    std::printf("  1) 构造函数注入（本例）—— 依赖在整个生命周期内不变，首选\n");
    std::printf("  2) setter / init 注入 —— 依赖可以中途替换（比如换传感器）\n");
    std::printf("  3) 参数注入 —— 只在某一次调用里需要，直接当参数传\n");
    std::printf("  模板参数注入（静态多态）—— 追求零开销时用，代价是类型固定\n");

    std::printf("\n嵌入式上怎么落地（不想引入堆分配和虚函数时）：\n");
    std::printf("  - 用一个结构体把手头的模块指针收集起来：\n");
    std::printf("      struct Platform { Clock* clk; Uart* uart; Flash* flash; };\n");
    std::printf("    初始化时填好，之后一路传下去。这就是最朴素的 DI。\n");
    std::printf("  - 别用单例隐藏依赖：单例让测试无法替换，也让头文件互相纠缠。\n");
    std::printf("\n坑：注入的是**引用或指针**，一定要注意生命周期。\n");
    std::printf("   如果传进去的对象先析构了，后面就是野引用。\n");
    return 0;
}
