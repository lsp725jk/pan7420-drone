// ch07_ex11.cpp —— 匿名结构体与匿名成员：省一层名字
// 编译：check.bat ch07_ex11.cpp c++latest
#include <cstdio>
#include <cstring>

// 1) 匿名结构体类型 + typedef（就是前面见过的 Point3 写法）
typedef struct { int x, y; } Point;

// 2) 匿名成员（anonymous member）：直接把字段"摊平"到外层
struct Packet {
    unsigned char head;
    union {                        // 匿名 union：不用起名字
        unsigned int  raw;
        unsigned char bytes[4];
    };
    struct {                       // 匿名 struct：字段直接可访问
        unsigned int  id   : 12;
        unsigned int  type : 4;
    };
};

// 3) 具名成员作为对照
struct PacketNamed {
    unsigned char head;
    union { unsigned int raw; unsigned char bytes[4]; } payload;
};

int main()
{
    printf("[1] 匿名成员让字段少写一层名字:\n");
    struct Packet p;
    memset(&p, 0, sizeof(p));
    p.head = 0xAA;
    p.raw = 0x11223344;            // ← 不用写 p.payload.raw
    printf("    p.raw = 0x%08X\n", p.raw);
    printf("    p.bytes[0..3] = %02X %02X %02X %02X\n",
           p.bytes[0], p.bytes[1], p.bytes[2], p.bytes[3]);
    p.id = 0x123;
    p.type = 0x5;
    printf("    p.id=%u p.type=%u  （位域也直接可访问）\n", p.id, p.type);
    printf("    sizeof(struct Packet) = %zu\n", sizeof(struct Packet));

    printf("\n[2] 具名成员的对照写法:\n");
    struct PacketNamed q;
    memset(&q, 0, sizeof(q));
    q.head = 0xAA;
    q.payload.raw = 0x11223344;    // ← 多写一层 payload
    printf("    q.payload.raw = 0x%08X\n", q.payload.raw);
    printf("    sizeof(struct Packet)      = %zu  （匿名 union 4 + 匿名位域结构 4 + head 1 + 填充 3）\n",
           sizeof(struct Packet));
    printf("    sizeof(struct PacketNamed) = %zu  （head 1 + 填充 3 + 具名 union 4，少了那 4 字节位域）\n",
           sizeof(struct PacketNamed));
    printf("    ← 两个版本大小不同是因为字段本来就不同，不是匿名/具名造成的\n");

    printf("\n[3] 匿名结构体的几种形式对照:\n");
    printf("    (a) typedef struct { int x; } Point;        → 类型叫 Point，结构体本身无名\n");
    printf("    (b) struct { int x; } obj;                  → 直接定义了一个匿名类型的变量 obj\n");
    printf("    (c) struct Named { ... };                   → 具名结构体\n");
    printf("    (d) struct { ... };                         → 完全没用，编译器会警告\n");
        // (b) 的实例：只能这么用一次
    struct { int v; } one_shot;
    one_shot.v = 7;
    printf("    (b) 的实例 one_shot.v = %d\n", one_shot.v);

    printf("\n[4] 什么时候值得用匿名成员:\n");
    printf("    - 寄存器映射：想同时用 raw 和位域两种视角访问同一个寄存器\n");
    printf("    - 协议包：字段都很短，不想写 p.hdr.len 这种层层前缀\n");
    printf("    注意：匿名成员是 C11 起进入 C 标准的；C++ 也支持，但不要和\n");
    printf("          「匿名结构体类型」搞混（后者只是没有名字的类型）\n");

    printf("\n[5] 一个真实的寄存器访问模板（嵌入式常用形态）:\n");
    typedef struct {
        volatile unsigned int CR;      // 控制寄存器
        volatile unsigned int SR;      // 状态寄存器
    } Periph;
    Periph periph;                      // 真实工程里是 (Periph*)0x40021000 这种地址
    memset((void*)&periph, 0, sizeof(periph));
    periph.CR = 0x1;
    printf("    periph.CR = 0x%X  periph.SR = 0x%X\n", periph.CR, periph.SR);
    printf("    真实用法：Periph* p = (Periph*)0x40021000; p->CR |= 1u;\n");
    printf("    （指针转整数地址的细节见第 5 章 void* 一节）\n");
    return 0;
}
