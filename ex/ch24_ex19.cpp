// ch24_ex19.cpp —— 例 24-19：从 C 迁移到 C++ 的渐进策略（一个可运行的例子）
// 结论先放在前面：**不要重写**。让 C 和 C++ 共存，一个文件一个文件地换。
// 这个例子演示"同一个模块，C 接口保持不变、内部换成 C++ 实现"。
#include <cstdint>
#include <cstdio>
#include <cstring>

// ==================================================================
// 第一步：接口保持 C 风格（extern "C"），内部用 C++ 实现
// 这样所有还在用 C 写的调用方**一行都不用改**。
// ==================================================================
extern "C" {

// 原 C 版本里可能是：
// typedef struct { uint16_t raw[8]; uint8_t n; } SampleBuf;
// void sample_push(SampleBuf* b, uint16_t v);
struct SampleBuf {          // 名字保持不变
    std::uint16_t raw[8];   // 内部先不动，保证 ABI/内存布局不变
    std::uint8_t n;
};

// 用 C++ 重写实现：内部用 RAII、边界检查、更好的算法，
// 但签名和 C 版一模一样。
void sample_push(SampleBuf* b, std::uint16_t v) {
    if (b == nullptr) {
        return;  // 防御性检查（C 版常常忘了）
    }
    if (b->n < 8) {
        b->raw[b->n++] = v;
    } else {
                // 满了：丢弃最旧的，腾出位置（C 版可能是直接越界写）
        std::memmove(&b->raw[0], &b->raw[1], 7 * sizeof(std::uint16_t));
        b->raw[7] = v;
    }
}

std::uint32_t sample_sum(const SampleBuf* b) {
    if (b == nullptr) {
        return 0;
    }
    std::uint32_t s = 0;
    for (std::uint8_t i = 0; i < b->n; ++i) {
        s += b->raw[i];
    }
    return s;
}

void sample_clear(SampleBuf* b) {
    if (b != nullptr) {
        b->n = 0;
    }
}

}  // extern "C"

// ==================================================================
// 第二步（可选）：在不改调用方的前提下，内部换成 C++ 容器
// 这时需要 pimpl：把 SampleBuf 变成一个不透明句柄。
// 这里只做结构上的演示，不真的替换（因为上面要保持布局）。
// ==================================================================

int main() {
    SampleBuf buf{};
    sample_clear(&buf);

    for (std::uint16_t v = 1; v <= 12; ++v) {
        sample_push(&buf, static_cast<std::uint16_t>(v * 10));
    }
    std::printf("塞入 12 个（容量 8），当前 n = %u\n", buf.n);
    std::printf("内容: ");
    for (std::uint8_t i = 0; i < buf.n; ++i) {
        std::printf("%u ", buf.raw[i]);
    }
    std::printf("\n和 = %u\n", sample_sum(&buf));
    std::printf("（超出容量的旧数据被丢掉，而不是越界写 —— 这就是重写的收益）\n");

    std::printf("\n=== 从 C 迁移到 C++ 的推荐顺序 ===\n");
    std::printf("\n【第一批：立刻就能做，收益高、风险低】\n");
    std::printf("  1) 打开 C++ 编译器（.c 文件仍按 C 编译，只是工程加上 C++ 支持）\n");
    std::printf("  2) 新写的模块直接用 .cpp，用 std::array / std::span 替代裸数组参数\n");
    std::printf("  3) 用 constexpr 替代 #define 常量（有类型、有作用域、能调试）\n");
    std::printf("  4) 用 enum class 替代 #define 出来的状态码\n");
    std::printf("  5) 用 nullptr 替代 NULL，用 static_cast 替代 C 风格强制转换\n");
    std::printf("  6) 单测/上位机工具先全面上 C++（风险最低、收益最直接）\n");

    std::printf("\n【第二批：动到已有模块，要一个一个来】\n");
    std::printf("  7) 把【一对 init/deinit】改成 RAII 对象（I2C 总线、Flash 锁、电机使能）\n");
    std::printf("  8) 把【手工数组 + 长度变量】改成 std::array 或自写的定长容器\n");
    std::printf("  9) 把查表/换算常量的生成搬到编译期（第 23 章）\n");
    std::printf("  10) 把回调注册从函数指针表改成 std::function 或模板（看是否热路径）\n");
    std::printf("  11) 用模板/概念重构【同一份逻辑写了三遍】的地方\n");

    std::printf("\n【不要动的地方】\n");
    std::printf("  - 启动文件、链接脚本、中断向量表：保持 C / 汇编原样\n");
    std::printf("  - 中断服务函数：不要在里面用会抛异常/会分配堆的 C++ 特性\n");
    std::printf("  - 已经稳定运行、没有测试覆盖的旧模块：收益不抵风险\n");
    std::printf("  - 与硬件寄存器直接交互的部分：保持 volatile 和原始布局\n");
    std::printf("  - 有严格时序要求、经过逐指令验证的汇编/内联汇编\n");
    std::printf("  - 第三方 C 库的头文件：原样包含，别去改\n");

    std::printf("\n【迁移时必须守住的几条】\n");
    std::printf("  - 关掉异常和 RTTI（-fno-exceptions -fno-rtti），体积和可预测性都更好\n");
    std::printf("  - 不要在中断里用堆（new / std::string / std::function 都可能分配）\n");
    std::printf("  - 每改一个模块就跑一次完整回归；没有测试就先补测试\n");
    std::printf("  - 接口边界用 extern \"C\" 包住，C 和 C++ 混编才不会链接报错\n");
    std::printf("  - 关注 Flash/RAM 增量（arm-none-eabi-size），别一次涨太多\n");

    std::printf("\n本例子本身演示了最关键的一步：\n");
    std::printf("  接口是 C（extern \"C\" + POD 结构体），实现是 C++。\n");
    std::printf("  调用方完全不知道里面换了语言，可以随时回退。\n");
    return 0;
}
