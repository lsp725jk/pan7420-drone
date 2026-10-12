// ch24_ex16.cpp —— 例 24-16：用 static_assert 做"编译期单元测试"
// 有些东西不能在运行期测：类型关系、结构体布局、编译期计算的结果。
// 这些用 static_assert 测，测试结果在**编译期**就出来了——
// 编过了就是全绿，编不过就是有红。CI 上不需要跑任何东西。
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <string>
#include <type_traits>
#include <utility>

// ================= 被测的东西 =================

// 1) 一个定点数类型
template <int FracBits>
struct Fixed {
    std::int32_t raw;
    static constexpr int frac = FracBits;
    static constexpr std::int32_t one() { return 1 << FracBits; }
};
constexpr Fixed<8> fx_add(Fixed<8> a, Fixed<8> b) { return Fixed<8>{a.raw + b.raw}; }
constexpr Fixed<8> fx_from_int(int v) { return Fixed<8>{v * Fixed<8>::one()}; }

// 2) 一个协议结构体
#pragma pack(push, 1)
struct AttitudeFrame {
    std::uint8_t head;
    std::int16_t roll;
    std::int16_t pitch;
    std::int16_t yaw;
    std::uint8_t crc;
};
#pragma pack(pop)

// 3) 一个编译期生成的表
constexpr std::array<std::uint8_t, 8> make_mask_table() {
    std::array<std::uint8_t, 8> t{};
    for (int i = 0; i < 8; ++i) {
        t[static_cast<std::size_t>(i)] = static_cast<std::uint8_t>(1u << i);
    }
    return t;
}
constexpr auto kMask = make_mask_table();

// ================= 编译期测试 =================
// 类型关系
static_assert(sizeof(std::int32_t) == 4, "int32_t 必须是 4 字节");
static_assert(std::is_trivially_copyable<AttitudeFrame>::value, "协议帧必须可平凡复制");
static_assert(std::is_standard_layout<AttitudeFrame>::value, "");

// 内存布局（这类回归最容易悄悄发生）
static_assert(sizeof(AttitudeFrame) == 8, "协议帧大小变了：1+2+2+2+1");
static_assert(offsetof(AttitudeFrame, roll) == 1, "roll 的偏移必须紧跟 head");
static_assert(offsetof(AttitudeFrame, crc) == 7, "crc 必须在第 8 个字节");

// 编译期计算
static_assert(Fixed<8>::one() == 256, "Q8 的 1.0 = 256");
static_assert(fx_add(fx_from_int(2), fx_from_int(3)).raw == 5 * 256, "2.0 + 3.0 = 5.0");
static_assert(fx_from_int(-1).raw == -256, "负数也要对");

// 表
static_assert(kMask[0] == 1, "");
static_assert(kMask[7] == 128, "");
static_assert(kMask.size() == 8, "");

// 编译期"循环"测试：一条断言覆盖一批输入
template <std::size_t... I>
constexpr bool mask_ok(std::index_sequence<I...>) {
    return ((kMask[I] == (1u << I)) && ...);
}
static_assert(mask_ok(std::make_index_sequence<8>{}), "整张表逐项都对");

int main() {
    std::printf("sizeof(AttitudeFrame) = %zu\n", sizeof(AttitudeFrame));
    std::printf("offsetof(roll)=%zu pitch=%zu yaw=%zu crc=%zu\n", offsetof(AttitudeFrame, roll),
                offsetof(AttitudeFrame, pitch), offsetof(AttitudeFrame, yaw),
                offsetof(AttitudeFrame, crc));

    std::printf("\nQ8 定点：\n");
    const Fixed<8> a = fx_from_int(2);
    const Fixed<8> b = fx_from_int(3);
    const Fixed<8> c = fx_add(a, b);
    std::printf("  2.0 + 3.0 = %d/%d = %g\n", c.raw, Fixed<8>::one(),
                c.raw / static_cast<double>(Fixed<8>::one()));

    std::printf("\n掩码表： ");
    for (auto m : kMask) {
        std::printf("0x%02X ", m);
    }
    std::printf("\n");

    std::printf("\n编译期测试的好处：\n");
    std::printf("  1) 零运行时间、零测试代码体积（不生成任何机器码）\n");
    std::printf("  2) 改坏了当场编不过，比运行期测试更早发现\n");
    std::printf("  3) 特别适合嵌入式：目标板上跑不了测试框架，但能编译\n");
    std::printf("\n适合用 static_assert 测的：\n");
    std::printf("  - sizeof / alignof / offsetof（协议兼容性）\n");
    std::printf("  - 类型关系（is_same / is_trivially_copyable）\n");
    std::printf("  - constexpr 函数和编译期查表的结果\n");
    std::printf("  - 常量表的内容\n");
    std::printf("\n不适合的：任何依赖运行期输入的东西（那些还是要写运行期测试）。\n");
    std::printf("\n坑：static_assert 失败信息里不会自动显示实际数值，\n");
    std::printf("   所以条件要尽量写清楚，或者把期望值也写进消息里。\n");
    std::printf("sizeof(std::string)=%zu（对比：编译期测试不需要构造任何对象）\n",
                sizeof(std::string));
    return 0;
}
