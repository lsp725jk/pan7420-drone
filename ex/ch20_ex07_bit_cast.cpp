// ch20_ex07_bit_cast.cpp —— std::bit_cast：安全地重新解释二进制
// 编译: check.bat ch20_ex07_bit_cast.cpp c++latest
#include <iostream>
#include <bit>
#include <cstdint>
#include <cstring>
#include <array>
#include <iomanip>

// ============================================================
// A. 最典型用途：浮点 <-> 整数位模式（嵌入式调试必备）
// ============================================================
void showFloatBits(float f)
{
        // bit_cast：把 float 的 4 个字节按位解释成 uint32_t
    auto bits = std::bit_cast<std::uint32_t>(f);
    std::cout << "  float " << std::setw(8) << f
              << "  ->  0x" << std::hex << std::setw(8) << std::setfill('0') << bits
              << std::dec << std::setfill(' ')
              << "   符号位=" << ((bits >> 31) & 1)
              << " 指数=" << ((bits >> 23) & 0xFF)
              << " 尾数=" << (bits & 0x7FFFFF) << "\n";
}

// ============================================================
// B. 反向：从位模式还原浮点（解析传感器原始寄存器值）
// ============================================================
float rawToFloat(std::uint32_t raw)
{
    return std::bit_cast<float>(raw);
}

// ============================================================
// C. 小结构体 <-> 字节数组（打包发送/接收）
// ============================================================
struct PackedFrame {
    std::uint8_t  head;      // 帧头
    std::uint16_t value;     // 数据
    std::uint8_t  crc;       // 校验
};
// 注意：sizeof 是 6 不是 4 —— 因为 uint16_t 要 2 字节对齐，
// head 后面会插入 1 字节填充（padding），value 后面再插 1 字节。
static_assert(sizeof(PackedFrame) == 6, "PackedFrame 的大小含对齐填充，必须是 6 字节");

// 真正想"紧凑打包"（不含填充）要用 #pragma pack，或者干脆手工按字节拼
#pragma pack(push, 1)
struct TightFrame {
    std::uint8_t  head;
    std::uint16_t value;
    std::uint8_t  crc;
};
#pragma pack(pop)
static_assert(sizeof(TightFrame) == 4, "TightFrame 用 pack(1) 取消了填充，必须是 4 字节");

// ============================================================
// D. 对比：memcpy 和 reinterpret_cast 的写法
// ============================================================
std::uint32_t viaMemcpy(float f)
{
    std::uint32_t out;
    std::memcpy(&out, &f, sizeof(out));          // 合法、但没有编译期检查
    return out;
}
std::uint32_t viaReinterpret(float f)
{
        // ✘ 违反严格别名规则（strict aliasing），UB。
        // 很多编译器上"看起来能用"，但开 -O2 可能被优化掉。
        // return *reinterpret_cast<std::uint32_t*>(&f);
    (void)f;    // 故意不使用参数：本函数只是用来展示"这种写法是 UB，所以不写"
    return 0;   // 故意不真的这么写
}

int main()
{
    std::cout << "=== A. 浮点数的位模式 ===\n";
    showFloatBits(1.0f);
    showFloatBits(-2.5f);
    showFloatBits(0.0f);

    std::cout << "\n=== B. 从原始寄存器值还原浮点 ===\n";
        // 0x3F800000 就是 1.0f
    std::cout << "  0x3F800000 -> " << rawToFloat(0x3F800000u) << "\n";
        // 0x40200000 就是 2.5f
    std::cout << "  0x40200000 -> " << rawToFloat(0x40200000u) << "\n";
    std::cout << "  用途：IMU/气压计寄存器给出的就是 32 位原始值。\n";

    std::cout << "\n=== C. 结构体 <-> 字节数组（顺带讲对齐填充）===\n";
    std::cout << "  sizeof(PackedFrame) = " << sizeof(PackedFrame)
              << "   <- 成员只占 1+2+1=4 字节，多出的 2 字节是对齐填充\n";
    PackedFrame fr{0xAA, 0x1234, 0x55};
    auto bytes = std::bit_cast<std::array<std::uint8_t, sizeof(PackedFrame)>>(fr);
    std::cout << "  打包后的字节: ";
    for (auto b : bytes) std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0')
                                   << static_cast<int>(b) << " " << std::dec << std::setfill(' ');
    std::cout << "\n";
    std::cout << "  注意：sizeof 是 6，但其中 2 个字节是**未初始化的对齐填充**，\n";
    std::cout << "        所以上面那两个位置的数值每次编译/运行都可能不同 ——\n";
    std::cout << "        这正是\"把结构体直接当成字节流传出去\"不可靠的原因之一。\n";
    std::cout << "  用 #pragma pack(1) 取消填充后：sizeof(TightFrame) = "
              << sizeof(TightFrame) << "\n";
    TightFrame tf{0xAA, 0x1234, 0x55};
    auto tbytes = std::bit_cast<std::array<std::uint8_t, sizeof(TightFrame)>>(tf);
    std::cout << "  紧凑字节: ";
    for (auto b : tbytes) std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0')
                                    << static_cast<int>(b) << " " << std::dec << std::setfill(' ');
    std::cout << "\n";
    std::cout << "  注意：字节顺序取决于本机大小端。x86 和 Cortex-M 都是小端，\n";
    std::cout << "        所以 head 在最低地址，value 是低字节在前（34 12）。\n";
    std::cout << "  坑：pack(1) 会让 uint16_t 变成非对齐访问。x86 无所谓，\n";
    std::cout << "      Cortex-M0+ 对非对齐访问会触发 HardFault —— 详见第 21 章。\n";

    std::cout << "\n=== D. 三种写法的对比 ===\n";
    float f = 1.0f;
    std::cout << "  bit_cast        = 0x" << std::hex << std::bit_cast<std::uint32_t>(f) << "\n";
    std::cout << "  memcpy 等价结果 = 0x" << viaMemcpy(f) << std::dec << "\n";
    std::cout << "  reinterpret_cast = 0x" << std::hex << viaReinterpret(f) << std::dec
              << "（故意返回 0，因为那是 UB，不应照抄）\n";
    std::cout << "  bit_cast 的优势：\n";
    std::cout << "    1) 编译期检查长度必须一致（写错直接编译失败）；\n";
    std::cout << "    2) constexpr 可用（memcpy 在 C++20 前不行）；\n";
    std::cout << "    3) 不违反严格别名规则，优化器不会坑你。\n";

    std::cout << "\n=== E. 编译期就能算 ===\n";
    constexpr std::uint32_t kOneBits = std::bit_cast<std::uint32_t>(1.0f);
    static_assert(kOneBits == 0x3F800000u, "1.0f 的 IEEE754 位模式必须是 0x3F800000");
    std::cout << "  constexpr auto kOneBits = bit_cast<uint32_t>(1.0f);\n";
    std::cout << "  static_assert(kOneBits == 0x3F800000u);  -> 编译通过\n";
    std::cout << "  这在嵌入式里很有用：把它写进 static_assert，\n";
    std::cout << "  以后换编译器/换优化级别时如果浮点格式变了会立刻发现。\n";

    std::cout << "\n=== F. 相关工具 ===\n";
    std::cout << "  std::has_single_bit(x)  : x 是不是 2 的幂\n";
    std::cout << "  std::popcount(x)        : 有几个 1 位（C++20）\n";
    std::cout << "  std::countl_zero(x)     : 高位有几个 0（做归一化很常用）\n";
    std::cout << "  std::byteswap(x)        : 大小端转换（C++23）\n";
    std::cout << "  has_single_bit(64) = " << std::has_single_bit(64u)
              << ", popcount(0xFF) = " << std::popcount(0xFFu)
              << ", countl_zero(1u) = " << std::countl_zero(1u) << "\n";
    std::cout << "  这些在 Cortex-M0+ 上没有硬件指令，但编译器会用位运算展开，\n";
    std::cout << "  比手写循环快得多。\n";

    std::cout << "\nmain 正常结束\n";
    return 0;
}
