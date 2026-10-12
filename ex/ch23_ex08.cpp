// ch23_ex08.cpp —— 例 23-8：位运算打包协议帧
// 场景：4 字节的状态帧要塞进 5 个字段，且必须和地面站约定一致。
// 两种写法：位域（bit-field）和手工掩码。都要会，因为各有各的坑。
#include <cstdint>
#include <cstdio>
#include <cstring>

// ---- 写法 A：位域 ----
struct StatusBits {
    std::uint32_t mode : 3;      // 0..7      飞行模式
    std::uint32_t armed : 1;     // 0/1       解锁标志
    std::uint32_t sats : 5;      // 0..31     卫星数
    std::uint32_t voltage : 12;  // 0.001V 单位，0..4095 -> 0..4.095V
    std::uint32_t errors : 11;   // 错误位图
};                               // 合计 3+1+5+12+11 = 32 位 = 4 字节

// ---- 写法 B：手工掩码（同一份布局）----
constexpr std::uint32_t pack_status(std::uint32_t mode, std::uint32_t armed, std::uint32_t sats,
                                   std::uint32_t voltage, std::uint32_t errors) {
    return ((mode & 0x7u) << 0) | ((armed & 0x1u) << 3) | ((sats & 0x1Fu) << 4) |
           ((voltage & 0xFFFu) << 9) | ((errors & 0x7FFu) << 21);
}
constexpr std::uint32_t unpack_field(std::uint32_t frame, unsigned shift, unsigned width) {
    return (frame >> shift) & ((1u << width) - 1u);
}

static_assert(sizeof(StatusBits) == 4, "位域结构体必须正好 4 字节");
static_assert(pack_status(5, 1, 12, 3700, 0x22) != 0, "");

int main() {
        // ---- A：填位域 ----
    StatusBits s{};
    s.mode = 5;
    s.armed = 1;
    s.sats = 12;
    s.voltage = 3700;
    s.errors = 0x22;

    std::uint32_t raw = 0;
    std::memcpy(&raw, &s, 4);
    std::printf("位域写法  : raw = 0x%08X\n", raw);

        // ---- B：手工打包 ----
    const std::uint32_t raw2 = pack_status(5, 1, 12, 3700, 0x22);
    std::printf("手工打包  : raw = 0x%08X\n", raw2);
    std::printf("两者一致  : %d\n", (int)(raw == raw2));

    std::printf("\n按手工约定的顺序解出来：\n");
    std::printf("  mode    = %u\n", unpack_field(raw2, 0, 3));
    std::printf("  armed   = %u\n", unpack_field(raw2, 3, 1));
    std::printf("  sats    = %u\n", unpack_field(raw2, 4, 5));
    std::printf("  voltage = %u  (=%.3f V)\n", unpack_field(raw2, 9, 12),
                unpack_field(raw2, 9, 12) / 1000.0);
    std::printf("  errors  = 0x%X\n", unpack_field(raw2, 21, 11));

    std::printf("\nsizeof(StatusBits) = %zu\n", sizeof(StatusBits));

        // ---- 位运算小技巧 ----
    std::uint32_t flags = 0;
    flags |= (1u << 3);         // 置位
    flags |= (1u << 7);
    std::printf("置位后 flags = 0x%02X\n", flags);
    flags &= ~(1u << 3);        // 清位
    std::printf("清掉 bit3    = 0x%02X\n", flags);
    std::printf("bit7 是否为 1: %d\n", (int)((flags >> 7) & 1u));
    flags ^= (1u << 7);         // 翻转
    std::printf("翻转 bit7    = 0x%02X\n", flags);

        // 坑 1：位域的布局是**实现定义**的。MSVC 从低位往高位排（小端），
        // 换成别的编译器/大端 MCU，同一个结构体在内存里的字节序可能不同。
        // 跨平台协议**一律用手工掩码** + 固定字节序。
        // 坑 2：位域不能取地址（&s.mode 是错的），也不能直接 memcpy 单个字段。
        // 坑 3：(1 << 31) 在 int 上是溢出 UB，一定要写 1u << 31。
    const std::uint32_t high = 1u << 31;
    std::printf("1u << 31 = 0x%08X（写 1 << 31 是未定义行为）\n", high);
    return 0;
}
