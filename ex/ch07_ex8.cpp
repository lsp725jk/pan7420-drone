// ch07_ex8.cpp —— 用 union 判断大小端（多种写法对照）
// 编译：check.bat ch07_ex8.cpp c++latest
#include <cstdio>
#include <cstring>
#include <cstdint>

// 写法 1：union（最经典）
int is_little_endian_union()
{
    union { uint16_t v; uint8_t b[2]; } u;
    u.v = 0x0100;                 // 高位字节是 0x01，低位是 0x00
    return u.b[0] == 0x00;        // 低地址存的是低位 → 小端
}

// 写法 2：指针取首字节
int is_little_endian_ptr()
{
    uint32_t v = 0x00000001;
    return *(uint8_t*)&v == 1;
}

// 写法 3：memcpy（最"干净"，不违反严格别名规则）
int is_little_endian_memcpy()
{
    uint16_t v = 0x0001;
    uint8_t b[2];
    memcpy(b, &v, sizeof(v));
    return b[0] == 1;
}

const char* name(int little) { return little ? "小端 (little-endian)" : "大端 (big-endian)"; }

int main()
{
    printf("[1] 三种判断写法，结果应该一致：\n");
    printf("    union 写法   → %s\n", name(is_little_endian_union()));
    printf("    pointer 写法 → %s\n", name(is_little_endian_ptr()));
    printf("    memcpy 写法  → %s\n", name(is_little_endian_memcpy()));

    printf("\n[2] 指针写法为什么有人反对：\n");
    printf("    *(uint8_t*)&v 是「用 char 类型访问别的类型」，标准允许（char/unsigned char 是例外）\n");
    printf("    但如果写成 *(uint32_t*)&float_var 就是违反严格别名规则（strict aliasing），可能被优化出错\n");

    printf("\n[3] 实际布线数据的样子（本机是小端）:\n");
    uint32_t frame_id = 0x0000002A;
    printf("    frame_id = 0x%08X，按字节存进数组: ", frame_id);
    uint8_t bytes[4];
    memcpy(bytes, &frame_id, 4);
    for (int i = 0; i < 4; ++i) printf("%02X ", bytes[i]);
    printf("\n");

        // 手工做大端序（网络字节序）编码：高字节在前
    uint8_t be[4];
    be[0] = (uint8_t)((frame_id >> 24) & 0xFF);
    be[1] = (uint8_t)((frame_id >> 16) & 0xFF);
    be[2] = (uint8_t)((frame_id >> 8) & 0xFF);
    be[3] = (uint8_t)(frame_id & 0xFF);
    printf("    手工编码成网络字节序(大端): ");
    for (int i = 0; i < 4; ++i) printf("%02X ", be[i]);
    printf("\n");
    printf("    ← 这就是为什么协议里规定「多字节字段用大端」，双方约定好就不用猜 CPU\n");

    printf("\n[4] 一个容易忽视的事实：\n");
    printf("    x86-64 / ARM(默认) / RISC-V 都是小端；\n");
    printf("    部分网络设备、老式 PowerPC/SPARC 是大端，做协议解析时必须显式转换\n");
    return 0;
}
