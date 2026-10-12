// ch07_ex10.cpp —— 位域的布局实测：MSVC 在小端上怎么排这几位
// 编译：check.bat ch07_ex10.cpp c++latest
#include <cstdio>
#include <cstring>
#include <cstdint>

// 5+5+6 = 16 位，观察两个字节怎么分
struct S1 { unsigned int a : 5; unsigned int b : 5; unsigned int c : 6; };
// 1+1+1+1+1+1+1+1 = 8 位
struct S2 { unsigned char a : 1; unsigned char b : 1; unsigned char c : 1; unsigned char d : 1;
            unsigned char e : 1; unsigned char f : 1; unsigned char g : 1; unsigned char h : 1; };
// 跨过字节边界：12 + 8 = 20 位
struct S3 { unsigned int a : 12; unsigned int b : 8; };
// 用 :0 强制从下一个字节/单元开始
struct S4 { unsigned char a : 3; unsigned char : 0; unsigned char b : 3; };

void dump(const void* p, size_t n)
{
    const unsigned char* b = (const unsigned char*)p;
    for (size_t i = 0; i < n; ++i) printf("%02X ", b[i]);
}

int main()
{
    printf("[1] struct { unsigned a:5; unsigned b:5; unsigned c:6; }  sizeof=%zu\n", sizeof(S1));
    printf("    （16 位内容，但因为成员类型是 unsigned，整体对齐到 4 字节）\n");
    struct S1 s1;
    memset(&s1, 0, sizeof(s1));
    s1.a = 0x1F; s1.b = 0x00; s1.c = 0x00;
    printf("    只设 a=31:      "); dump(&s1, sizeof(s1));
    printf("  → a 占最低 5 位\n");
    memset(&s1, 0, sizeof(s1));
    s1.a = 0; s1.b = 0x1F; s1.c = 0;
    printf("    只设 b=31:      "); dump(&s1, sizeof(s1));
    printf("  → b 紧跟在 a 后面（第 6~10 位）\n");
    memset(&s1, 0, sizeof(s1));
    s1.a = 0; s1.b = 0; s1.c = 0x3F;
    printf("    只设 c=63:      "); dump(&s1, sizeof(s1));
    printf("  → c 在第 11~16 位，跨到了第二个字节\n");

    printf("\n[2] 8 个 1 位标志（看谁对应最高位）:\n");
    struct S2 s2;
    memset(&s2, 0, sizeof(s2));
    s2.a = 1;
    printf("    设 a=1:   "); dump(&s2, sizeof(s2)); printf("\n");
    memset(&s2, 0, sizeof(s2));
    s2.h = 1;
    printf("    设 h=1:   "); dump(&s2, sizeof(s2)); printf("  → h 是最高位\n");
    printf("    结论：声明在前 → 位更低（小端 MSVC 的实现）\n");

    printf("\n[3] 跨字节边界（12+8=20 位，sizeof=%zu，同样是 4 字节对齐）:\n", sizeof(S3));
    struct S3 s3;
    memset(&s3, 0, sizeof(s3));
    s3.a = 0xFFF;
    printf("    a=0xFFF: "); dump(&s3, sizeof(s3)); printf("\n");
    memset(&s3, 0, sizeof(s3));
    s3.b = 0xFF;
    printf("    b=0xFF:  "); dump(&s3, sizeof(s3)); printf("\n");

    printf("\n[4] 用 :0 强制换新单元（sizeof=%zu）:\n", sizeof(S4));
    struct S4 s4;
    memset(&s4, 0, sizeof(s4));
    s4.a = 0x7;
    printf("    a=7: "); dump(&s4, sizeof(s4)); printf("\n");
    memset(&s4, 0, sizeof(s4));
    s4.b = 0x7;
    printf("    b=7: "); dump(&s4, sizeof(s4)); printf("  → b 从下一个字节开始，中间那个字节是填充\n");

    printf("\n[5] 结论与忠告:\n");
    printf("    位域的排布顺序、跨字节行为、:0 的效果都取决于「实现定义」的行为。\n");
    printf("    上面的输出只代表「MSVC 14.x + x86-64 小端」，换个编译器就可能不同。\n");
    printf("    要和硬件寄存器/协议字段严格对应时，用显式的移位和掩码更可靠：\n");
    printf("      raw = (raw & ~(0x1Fu << 0)) | ((val & 0x1Fu) << 0);   // 写 a\n");
    printf("      val = (raw >> 0) & 0x1Fu;                              // 读 a\n");
    return 0;
}
