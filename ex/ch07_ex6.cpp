// ch07_ex6.cpp —— union：所有成员共用同一块内存
// 编译：check.bat ch07_ex6.cpp c++latest
#include <cstdio>
#include <cstring>
#include <cstdint>

// 最基本的 union：大小 = 最大成员的大小
union Mix {
    uint32_t u32;
    uint16_t u16[2];
    uint8_t  u8[4];
    char     c[4];
};

// 用 union 拆解一个 32 位寄存器（嵌入式最常用法）
union Reg {
    uint32_t raw;
    struct {
        uint32_t enable   : 1;
        uint32_t mode     : 3;
        uint32_t prescale : 4;
        uint32_t reserved : 24;
    } bits;
};

int main()
{
    printf("[1] union 的大小和对齐:\n");
    printf("    sizeof(union Mix)  = %zu   （等于最大成员 uint32_t 的 4 字节）\n", sizeof(Mix));
    printf("    alignof            = %zu\n", alignof(Mix));
    printf("    四个成员地址相同吗？\n");

    union Mix m;
    m.u32 = 0x11223344;
    printf("    &m.u32 = %p\n", (void*)&m.u32);
    printf("    &m.u16 = %p\n", (void*)&m.u16);
    printf("    &m.u8  = %p\n", (void*)&m.u8);
    printf("    &m.c   = %p  ← 全是同一个地址\n", (void*)&m.c);

    printf("\n[2] 改了 u32，再看 u16/u8（能看到字节序）:\n");
    printf("    m.u32    = 0x%08X\n", m.u32);
    printf("    m.u16[0] = 0x%04X  m.u16[1] = 0x%04X\n", m.u16[0], m.u16[1]);
    printf("    m.u8[0]  = 0x%02X    m.u8[1]  = 0x%02X\n", m.u8[0], m.u8[1]);
    printf("    m.u8[2]  = 0x%02X    m.u8[3]  = 0x%02X\n", m.u8[2], m.u8[3]);
    printf("    ← 低字节在前 = 小端\n");

    printf("\n[3] 用小端解释再看一遍（改成用 u8 逐个赋值，再读 u32）:\n");
    m.u8[0] = 0xAA; m.u8[1] = 0xBB; m.u8[2] = 0xCC; m.u8[3] = 0xDD;
    printf("    逐个写 u8 = AA BB CC DD，读 m.u32 = 0x%08X\n", m.u32);
    printf("    ← 同样证明小端：先写的字节在低地址，就是最低位\n");

    printf("\n[4] 位域 + union 访问寄存器（嵌入式经典）:\n");
    union Reg r;
    r.raw = 0;
    r.bits.enable = 1;
    r.bits.mode = 5;
    r.bits.prescale = 9;
    printf("    r.bits.enable=%u mode=%u prescale=%u\n",
           r.bits.enable, r.bits.mode, r.bits.prescale);
    printf("    r.raw = 0x%08X   ← 一次写整个寄存器，不用手算掩码\n", r.raw);
    printf("    拆开看: raw = enable(1bit) | mode(3bit)<<1 | prescale(4bit)<<4\n");
    printf("            = 1 | (5<<1) | (9<<4) = %u + %u + %u = %u\n",
           r.bits.enable, r.bits.mode << 1, r.bits.prescale << 4,
           r.bits.enable + (r.bits.mode << 1) + (r.bits.prescale << 4));

    printf("\n[5] union 的坑:\n");
    printf("    - 同一时刻只有一个成员是「有效」的，写完 A 再读 B 是重新解释字节，不是转换\n");
    printf("    - union 里不能放带构造函数/析构函数的 C++ 类（要自己管生命周期）\n");
    printf("    - 位域成员的具体排布依赖编译器和字节序，「跨平台通信」不能用位域结构体直接收发\n");
    return 0;
}
