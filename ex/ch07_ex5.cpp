// ch07_ex5.cpp —— typedef：给类型起别名，结构体的四种常见写法
// 编译：check.bat ch07_ex5.cpp c++latest
#include <cstdio>
#include <cstring>
#include <cstdint>

// 写法 1：最原始的 C 写法，用的时候必须写 struct
struct Point1 { int x, y; };

// 写法 2：typedef struct X { ... } X;  —— 名字写两遍（C 和 C++ 通用，最常见）
typedef struct Point2 { int x, y; } Point2;

// 写法 3：typedef struct { ... } X;  —— 匿名结构体 + 别名（推荐）
typedef struct { int x, y; } Point3;

// 写法 4：C++ 里直接给结构体起名，不用 typedef（C++ 中 struct Point4 就是类型名）
struct Point4 { int x, y; };
// C++11 起的类型别名语法，比 typedef 更好读
using Point4Alias = Point4;

// typedef 也常用于基本类型：嵌入式里到处是这种
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;
typedef int32_t        i32;
typedef volatile u32   reg32;       // "32 位寄存器"这个抽象

// 函数指针的 typedef（第 5 章见过）
typedef int (*BinOp)(int, int);
int add(int a, int b) { return a + b; }

int main()
{
    struct Point1 a = { 1, 2 };     // 必须写 struct
    Point2 b = { 3, 4 };            // 别名，不用写 struct
    Point3 c = { 5, 6 };
    Point4 d = { 7, 8 };
    Point4Alias e = { 9, 10 };

    printf("四种结构体写法都能用:\n");
    printf("  struct Point1 a = {%d,%d}   （必须带 struct）\n", a.x, a.y);
    printf("  Point2 b        = {%d,%d}   （typedef struct X {...} X;）\n", b.x, b.y);
    printf("  Point3 c        = {%d,%d}   （匿名 + 别名）\n", c.x, c.y);
    printf("  Point4 d        = {%d,%d}   （C++ 里 struct 名就是类型名）\n", d.x, d.y);
    printf("  Point4Alias e   = {%d,%d}   （using 起的别名）\n", e.x, e.y);

    printf("\n[1] 类型大小完全一样: %zu %zu %zu %zu\n",
           sizeof(Point1), sizeof(Point2), sizeof(Point3), sizeof(Point4));
    printf("    typedef 只是「换个名字」，不产生新类型，也不占内存\n");

    printf("\n[2] 基本类型别名（嵌入式的日常）:\n");
    u8  x = 0xFF;
    u16 y = 0x1234;
    u32 z = 0xDEADBEEF;
    reg32 reg = 0;
    printf("    u8 x=0x%02X sizeof=%zu\n", x, sizeof(u8));
    printf("    u16 y=0x%04X sizeof=%zu\n", y, sizeof(u16));
    printf("    u32 z=0x%08X sizeof=%zu\n", z, sizeof(u32));
    printf("    reg32 reg 可写: ");
    reg = 0x5A5A5A5A;               // volatile 保证这行不会被优化掉
    printf("0x%08X\n", (unsigned)reg);
    printf("    为什么用 u8 而不是 unsigned char？——少打字，且一眼看出宽度\n");

    printf("\n[3] 函数指针的 typedef:\n");
    BinOp op = add;
    printf("    BinOp op = add; op(3,4) = %d\n", op(3, 4));
    printf("    不 typedef 的话要写 int (*op)(int,int)，typedef 之后清爽很多\n");

    printf("\n[4] 别名的两个坑:\n");
    printf("    (a) typedef 不创建新类型，所以 u8 和 unsigned char 可以互相赋值（不会类型报错）\n");
    printf("    (b) 用 typedef 藏指针很容易看错：\n");
    typedef char* CharPtr;
    printf("        typedef char* CharPtr; 之后 CharPtr a, b; 两个都是指针（不是 a 指针 b 字符）\n");
    CharPtr pa, pb;
    pa = nullptr; pb = nullptr;
    printf("        pa=%p pb=%p （sizeof(CharPtr)=%zu）\n", (void*)pa, (void*)pb, sizeof(CharPtr));
    printf("        但 #define CHARP char*  就会踩坑：CHARP a, b; 只有 a 是指针\n");
    return 0;
}
