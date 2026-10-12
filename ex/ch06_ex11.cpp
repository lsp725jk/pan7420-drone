// ch06_ex11.cpp —— 控制填充：#pragma pack 与 __attribute__((packed))
// 编译：check.bat ch06_ex11.cpp c++latest
#include <cstdio>
#include <cstddef>

// 默认对齐
struct Normal { char c; int i; short s; };

// #pragma pack 把对齐压到 1 字节：完全没有填充
#pragma pack(push, 1)
struct Packed1 { char c; int i; short s; };
#pragma pack(pop)

// 压到 2 字节
#pragma pack(push, 2)
struct Packed2 { char c; int i; short s; };
#pragma pack(pop)

// GCC/Clang 风格的写法（MSVC 也支持 __attribute__ 语法吗？见下文实测）
struct __declspec(align(8)) Aligned8 { char c; };   // MSVC 提高对齐用这个

int main()
{
    printf("同一个结构体 { char c; int i; short s; }（字段和 = 1+4+2 = 7）:\n");
    printf("  默认对齐            sizeof=%zu   c@%zu i@%zu s@%zu\n",
           sizeof(Normal), offsetof(Normal, c), offsetof(Normal, i), offsetof(Normal, s));
    printf("  #pragma pack(1)     sizeof=%zu   c@%zu i@%zu s@%zu\n",
           sizeof(Packed1), offsetof(Packed1, c), offsetof(Packed1, i), offsetof(Packed1, s));
    printf("  #pragma pack(2)     sizeof=%zu   c@%zu i@%zu s@%zu\n",
           sizeof(Packed2), offsetof(Packed2, c), offsetof(Packed2, i), offsetof(Packed2, s));

    printf("\n[1] pack(1) 的代价：成员可能「不对齐」，某些 CPU 上访问会变慢甚至硬件异常\n");
    printf("    x86-64 对未对齐访问比较宽容（只是慢），ARM Cortex-M0 会直接 HardFault\n");

    printf("\n[2] 什么时候必须用 pack(1)：和外部约定的二进制格式对齐时\n");
    printf("    例如通信协议的包头，对方按 7 字节紧凑排列，你这边也必须 7 字节\n");

    struct ProtoDefault { unsigned char head; unsigned int id; unsigned short crc; };
#pragma pack(push, 1)
    struct ProtoPacked { unsigned char head; unsigned int id; unsigned short crc; };
#pragma pack(pop)
    printf("    协议头默认布局 sizeof=%zu（多出 %zu 字节填充，收发就对不上了）\n",
           sizeof(ProtoDefault), sizeof(ProtoDefault) - 7);
    printf("    协议头 pack(1) 布局 sizeof=%zu  ← 和协议定义一致\n", sizeof(ProtoPacked));

    printf("\n[3] 另一种控制对齐的写法 __declspec(align(N))：\n");
    printf("    struct __declspec(align(8)) Aligned8 { char c; }  sizeof=%zu\n", sizeof(Aligned8));
    printf("    ← 不只不填充，还强制整体按 8 字节对齐（常用于 DMA 缓冲区、缓存行）\n");

        // __attribute__((packed)) 在 MSVC 下不支持，这里只做说明（未实测）
    printf("\n[4] GCC/Clang 的写法是 struct __attribute__((packed)) S {...};\n");
    printf("    MSVC 不支持 __attribute__((packed))（要用 #pragma pack），本示例未实测该写法\n");
    return 0;
}
