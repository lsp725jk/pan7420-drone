// ch06_ex12.cpp —— 大小端（endianness）：多字节数据在内存里的存放顺序
// 编译：check.bat ch06_ex12.cpp c++latest
#include <cstdio>
#include <cstring>
#include <cstdint>

// 打印一块内存的原始字节，用于观察字节序
void dump(const void* p, size_t n)
{
    const unsigned char* b = (const unsigned char*)p;
    for (size_t i = 0; i < n; ++i) printf("%02X ", b[i]);
}

int main()
{
    uint32_t v = 0x11223344;
    printf("[1] uint32_t v = 0x11223344 在内存里的字节顺序：\n");
    printf("    ");
    dump(&v, sizeof(v));
    printf("\n");

    unsigned char* bp = (unsigned char*)&v;
    printf("    按地址从低到高：bp[0]=0x%02X bp[1]=0x%02X bp[2]=0x%02X bp[3]=0x%02X\n",
           bp[0], bp[1], bp[2], bp[3]);
    if (bp[0] == 0x44) {
        printf("    bp[0] 是低位字节 0x44 → 本机是「小端」（little-endian）\n");
    } else {
        printf("    bp[0] 是高位字节 0x11 → 本机是「大端」（big-endian）\n");
    }

    printf("\n[2] 用 union 判断字节序（嵌入式里的经典写法）：\n");
    union { uint32_t u32; uint8_t u8[4]; } probe;
    probe.u32 = 1;
    printf("    probe.u32 = 1，probe.u8[0] = %u  → %s\n", probe.u8[0],
           probe.u8[0] == 1 ? "小端（低字节在低地址）" : "大端");

    printf("\n[3] 各种类型的内存布局：\n");
    uint16_t s = 0xABCD;
    printf("    uint16_t 0xABCD : "); dump(&s, 2); printf("\n");
    float f = 1.0f;
    printf("    float    1.0f   : "); dump(&f, 4); printf("  （IEEE-754: 0x3F800000）\n");
    double d = 1.0;
    printf("    double   1.0    : "); dump(&d, 8); printf("  （IEEE-754: 0x3FF0000000000000）\n");

    printf("\n[4] 为什么必须关心大小端：\n");
    printf("    - 网络字节序是大端（big-endian），本机多为小端 → 收发前要转换\n");
    printf("    - 用 memcpy 把结构体直接写进文件/发到串口，对方 CPU 字节序不同就读错\n");
    printf("    - 位域（bit-field）的分配顺序也和字节序、编译器实现有关（见下一章）\n");

        // 手工做一次字节序转换（不调库）
    uint32_t x = 0x11223344;
    uint32_t swapped = ((x & 0x000000FFu) << 24) |
                       ((x & 0x0000FF00u) << 8) |
                       ((x & 0x00FF0000u) >> 8) |
                       ((x & 0xFF000000u) >> 24);
    printf("\n[5] 手工字节序翻转: 0x%08X → 0x%08X\n", x, swapped);
    printf("    正规做法：C++23 的 std::byteswap（本机实测可用）；或编译器内建 _byteswap_ulong。\n");
    printf("    注意：C23 的 <stdbit.h> 并没有 byteswap —— 它只有字节序宏与位操作\n");
    return 0;
}
