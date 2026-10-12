// ch23_ex04.cpp —— 例 23-4：编译期生成 CRC16 查表
// 逐位算 CRC 每字节要循环 8 次；256 项查表法每字节只循环 1 次。
// 表在编译期算好，Flash 里只有 512 字节的 .rdata，运行期零初始化开销。
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>

// CRC-16/CCITT-FALSE：多项式 0x1021，初值 0xFFFF，不反转，不异或输出
constexpr std::uint16_t kPoly = 0x1021;
constexpr std::uint16_t kInit = 0xFFFF;

constexpr std::array<std::uint16_t, 256> make_crc_table() {
    std::array<std::uint16_t, 256> t{};
    for (int i = 0; i < 256; ++i) {
        std::uint16_t c = static_cast<std::uint16_t>(i << 8);
        for (int j = 0; j < 8; ++j) {
            c = (c & 0x8000u) ? static_cast<std::uint16_t>((c << 1) ^ kPoly)
                              : static_cast<std::uint16_t>(c << 1);
        }
        t[static_cast<std::size_t>(i)] = c;
    }
    return t;
}

constexpr auto kCrcTable = make_crc_table();

// 编译期就抽查两项，写错了当场编不过
static_assert(kCrcTable[0x00] == 0x0000, "");
static_assert(kCrcTable[0x01] == 0x1021, "");
static_assert(kCrcTable[0x80] == 0x9188, "");

// ---- 版本 A：逐位算（不占 Flash，慢）----
std::uint16_t crc16_bitwise(const std::uint8_t* p, std::size_t n) {
    std::uint16_t crc = kInit;
    for (std::size_t i = 0; i < n; ++i) {
        crc ^= static_cast<std::uint16_t>(p[i]) << 8;
        for (int b = 0; b < 8; ++b) {
            crc = (crc & 0x8000u) ? static_cast<std::uint16_t>((crc << 1) ^ kPoly)
                                  : static_cast<std::uint16_t>(crc << 1);
        }
    }
    return crc;
}

// ---- 版本 B：查表（占 512 字节 Flash，快）----
std::uint16_t crc16_table(const std::uint8_t* p, std::size_t n) {
    std::uint16_t crc = kInit;
    for (std::size_t i = 0; i < n; ++i) {
                // 每字节只做一次查表 + 一次异或
        crc = static_cast<std::uint16_t>((crc << 8) ^ kCrcTable[((crc >> 8) ^ p[i]) & 0xFF]);
    }
    return crc;
}

int main() {
    const char* msg = "123456789";  // CRC 标准测试向量
    const std::uint16_t a = crc16_bitwise(reinterpret_cast<const std::uint8_t*>(msg), 9);
    const std::uint16_t b = crc16_table(reinterpret_cast<const std::uint8_t*>(msg), 9);
    std::printf("CRC16/CCITT-FALSE(\"123456789\")\n");
    std::printf("  逐位算 : 0x%04X\n", a);
    std::printf("  查表法 : 0x%04X  <- 标准答案 0x29B1\n", b);
    std::printf("  两者一致: %d\n", (int)(a == b));
    std::printf("表大小 = 256 x 2 = %zu 字节\n", sizeof(kCrcTable));

        // ---- 大数据量对比 ----
    static std::uint8_t buf[4096];
    for (int i = 0; i < 4096; ++i) {
        buf[i] = static_cast<std::uint8_t>(i * 31 + 7);
    }
    const int ROUNDS = 400;  // 约 1.6 MB

    const auto t0 = std::chrono::steady_clock::now();
    std::uint16_t s1 = 0;
    for (int r = 0; r < ROUNDS; ++r) {
        s1 ^= crc16_bitwise(buf, sizeof buf);
    }
    const auto t1 = std::chrono::steady_clock::now();

    std::uint16_t s2 = 0;
    for (int r = 0; r < ROUNDS; ++r) {
        s2 ^= crc16_table(buf, sizeof buf);
    }
    const auto t2 = std::chrono::steady_clock::now();

    const double ms_bit = std::chrono::duration<double, std::milli>(t1 - t0).count();
    const double ms_tab = std::chrono::duration<double, std::milli>(t2 - t1).count();
    std::printf("\n处理 %d KB：\n", ROUNDS * 4);
    std::printf("  逐位算 : %7.2f ms\n", ms_bit);
    std::printf("  查表法 : %7.2f ms  （快 %.1f 倍）\n", ms_tab, ms_bit / ms_tab);
    std::printf("  校验和 %04X / %04X\n", s1, s2);

        // 权衡：花 512 字节 Flash 换 5~8 倍速度。
        // 在 64KB Flash 的 PY32F030 上，如果 Flash 已经用到 96%，
        // 可以考虑 16 项的半字节查表法（只占 32 字节，速度仍是逐位法的 2 倍左右）。
    return 0;
}
