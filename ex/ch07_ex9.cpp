// ch07_ex9.cpp —— 位域（bit-field）：省内存，但不能取地址
// 编译：check.bat ch07_ex9.cpp c++latest
#include <cstdio>
#include <cstring>
#include <cstdint>

// 不用位域：4 个 bool 各占 1 字节（甚至 4 字节）
struct PlainFlags {
    unsigned char a;
    unsigned char b;
    unsigned char c;
    unsigned char d;
};

// 用位域：4 个 1 位标志挤进 1 字节
struct BitFlags {
    unsigned char a : 1;
    unsigned char b : 1;
    unsigned char c : 1;
    unsigned char d : 1;
};

// 位域跨字节：5+5+6 = 16 位
struct Wide {
    unsigned int x : 5;
    unsigned int y : 5;
    unsigned int z : 6;
};

int main()
{
    printf("[1] 省内存的效果:\n");
    printf("    sizeof(struct PlainFlags) = %zu 字节（4 个标志）\n", sizeof(PlainFlags));
    printf("    sizeof(struct BitFlags)   = %zu 字节（4 个 1 位标志挤在一起）\n", sizeof(BitFlags));
    printf("    sizeof(struct Wide)       = %zu 字节（5+5+6=16 位内容，但按 unsigned 对齐到 4 字节）\n", sizeof(Wide));
    printf("    ↑ 注意：位域只省「字段之间」的空间，结构体整体仍按成员类型的对齐来补齐\n");

    printf("\n[2] 取值/赋值就像普通成员（会自动按位宽截断）:\n");
    struct BitFlags f;
    memset(&f, 0, sizeof(f));
    f.a = 1; f.b = 0; f.c = 1; f.d = 1;
    printf("    a=%u b=%u c=%u d=%u\n", f.a, f.b, f.c, f.d);
    f.a = 3;                       // 1 位放不下 3
    printf("    f.a = 3 之后再读: %u  ← 只保留最低 1 位（3 & 1 = 1）\n", f.a);
    f.a = 255;
    printf("    f.a = 255 之后再读: %u\n", f.a);

    printf("\n[3] 位域不能取地址（这是硬限制）:\n");
    printf("    &f.a 会编译报错 C2102: 「&」要求左值\n");
    printf("    原因：位域可能不是从字节边界开始，没有「自己的地址」这回事\n");
        // printf("%p\n", (void*)&f.a);      // ← 编译错误
    printf("    需要指针时：先把值取出来放进普通变量\n");
    unsigned char tmp = f.a;
    printf("    unsigned char tmp = f.a; 然后 &tmp = %p 就行\n", (void*)&tmp);

    printf("\n[4] 布局依赖编译器（重要坑）:\n");
    struct Wide w;
    memset(&w, 0, sizeof(w));
    w.x = 0x1F;    // 5 位全 1
    w.y = 0x1F;
    w.z = 0x3F;    // 6 位全 1
    unsigned char raw[4];
    memcpy(raw, &w, sizeof(w));
    printf("    w.x=31 w.y=31 w.z=63，内存字节: ");
    for (size_t i = 0; i < sizeof(w); ++i) printf("%02X ", raw[i]);
    printf("\n");
    printf("    x 从最低位开始排（MSVC 在小端上的实现），所以第一字节的低 5 位是 x 的 11111\n");
    printf("    换成别的编译器/大端 CPU，「x 和 y 谁在前」可能相反 —— 标准没规定\n");

    printf("\n[5] 用不用位域，怎么选:\n");
    printf("    - 和硬件寄存器一一对应：用（配合 union + volatile）\n");
    printf("    - 只是想在本地省内存：可以，但注意访问速度可能比普通成员慢\n");
    printf("    - 要跨平台/跨编译器收发数据：不要用位域结构体，改用显式的移位+掩码\n");
    printf("      value = (raw >> 5) & 0x1F;   这样到哪里结果都一样\n");

    printf("\n[6] 无名称位域（:0 和 :n）:\n");
    struct Named {
        unsigned int a : 3;
        unsigned int   : 5;      // 5 位填充，不命名 → 只是占位
        unsigned int b : 4;
    };
    printf("    struct { unsigned a:3; unsigned :5; unsigned b:4; } sizeof=%zu\n", sizeof(Named));
    printf("    无名称位域用来「占位对齐」，读写不到它\n");
    return 0;
}
