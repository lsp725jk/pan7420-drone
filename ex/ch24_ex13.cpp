// ch24_ex13.cpp —— 例 24-13：接口设计——错误处理与异常规格
// 一个函数怎么报告失败？C 的做法是返回错误码，C++ 多了一个选择：异常。
// 两种都能用，但**不要混着用**，而且要在接口上写清楚。
#include <cstdint>
#include <cstdio>
#include <optional>
#include <stdexcept>
#include <string>

// ==================================================================
// 风格 1：错误码（嵌入式最常见）
// 优点：没有异常开销、可预测、适合中断/硬实时
// 缺点：调用者容易忘记检查
// ==================================================================
enum class Err {
    Ok = 0,
    Timeout,
    CrcMismatch,
    NotCalibrated,
};
[[nodiscard]] Err read_sensor(int channel, int& out) {
    if (channel == 3) {
        return Err::Timeout;
    }
    out = 100 + channel;
    return Err::Ok;
}
const char* to_string(Err e) {
    switch (e) {
        case Err::Ok: return "OK";
        case Err::Timeout: return "超时";
        case Err::CrcMismatch: return "CRC 错";
        case Err::NotCalibrated: return "未校准";
    }
    return "?";
}

// ==================================================================
// 风格 2：std::optional（"可能没有值"，失败原因不重要时最好用）
// ==================================================================
std::optional<int> find_channel(const char* name) {
    if (std::string(name) == "gps") {
        return 5;
    }
    return std::nullopt;  // 明确表示"没有"
}

// ==================================================================
// 风格 3：异常（PC 侧/上位机常用；嵌入式通常禁用）
// ==================================================================
class SensorError : public std::runtime_error {
public:
    SensorError(const std::string& what, Err code)
        : std::runtime_error(what), code_(code) {}
    Err code() const { return code_; }

private:
    Err code_;
};

int read_or_throw(int channel) {
    if (channel == 3) {
        throw SensorError("读取通道 3 超时", Err::Timeout);
    }
    return 100 + channel;
}

// ==================================================================
// noexcept：明确告诉调用者"这个函数不会抛"
// ==================================================================
int add_noexcept(int a, int b) noexcept {
    return a + b;  // 不可能抛
}
// 注意：noexcept 是承诺，不是"优化开关"。里面真抛了就直接 terminate。

// 析构函数默认就是 noexcept，这是有意的设计：
// 栈展开过程中如果再抛异常，程序会立刻终止，没法收拾。

int main() {
    std::printf("=== 风格 1：错误码 ===\n");
    for (int ch = 1; ch <= 3; ++ch) {
        int v = 0;
        const Err e = read_sensor(ch, v);
        if (e == Err::Ok) {
            std::printf("  通道 %d -> %d\n", ch, v);
        } else {
            std::printf("  通道 %d -> 失败: %s\n", ch, to_string(e));
        }
    }
        // 忘检查会怎样？v 保持原值 0，静默错误。
        // 所以现代 C++ 用 [[nodiscard]] 让编译器警告"返回值没被使用"。

    std::printf("\n=== 风格 2：std::optional ===\n");
    for (const char* n : {"gps", "baro"}) {
        if (auto v = find_channel(n)) {
            std::printf("  %s -> 通道 %d\n", n, *v);
        } else {
            std::printf("  %s -> 没找到\n", n);
        }
    }

    std::printf("\n=== 风格 3：异常 ===\n");
    try {
        std::printf("  通道 1 -> %d\n", read_or_throw(1));
        std::printf("  通道 3 -> %d\n", read_or_throw(3));
    } catch (const SensorError& e) {
        std::printf("  捕获到 SensorError: %s (code=%d)\n", e.what(), (int)e.code());
    }

    std::printf("\n=== noexcept ===\n");
    std::printf("  add_noexcept(1,2) = %d\n", add_noexcept(1, 2));
    std::printf("  对调用者的意义：标准容器看到 noexcept 才敢用移动（见例 23-13）\n");

    std::printf("\n接口设计建议：\n");
    std::printf("  1) 同一个模块里只选一种错误处理风格，别混。\n");
    std::printf("  2) 嵌入式（尤其中断和硬实时路径）优先错误码 / optional，\n");
    std::printf("     并且用 [[nodiscard]] 防止忘记检查。\n");
    std::printf("  3) 构造函数失败怎么办？—— 抛异常，或者用一个 create() 静态函数\n");
    std::printf("     返回 optional<unique_ptr<T>>。别搞【半构造】的对象。\n");
    std::printf("  4) 不会抛的函数就写 noexcept；移动构造/移动赋值**一定**写。\n");
    std::printf("  5) 异常规格是接口的一部分，变了就是破坏性改动。\n");
    std::printf("\n坑：const char* what() 返回的字符串必须是长期有效的，\n");
    std::printf("   不要返回一个局部 std::string 的 c_str()。\n");
    std::printf("sizeof(Err)=%zu  sizeof(std::optional<int>)=%zu\n", sizeof(Err),
                sizeof(std::optional<int>));
    return 0;
}
