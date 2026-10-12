// ch07_ex15.cpp —— 结构体的内存布局：按字段顺序 + 对齐要求逐格摆放
// 编译：check.bat ch07_ex15.cpp c++latest
#include <cstdio>
#include <cstddef>

// 故意做几种顺序，看 sizeof 和偏移怎么变
struct S_A { char a; char b; char c; char d; };                  // 4 个 char
struct S_B { char a; int  b; char c; int  d; };                   // char/int/char/int
struct S_C { int  b; int  d; char a; char c; };                   // 从大到小
struct S_D { char a; short b; int  c; double d; };                // 从小到大
struct S_E { double d; int  c; short b; char a; };                // 从大到小

// 用一张 ASCII 图说明"逐格摆放"的过程
void explain_layout()
{
    printf("    以 struct { char a; int b; short c; } 为例（这是 S_B 的简化版）:\n");
    printf("    规则 1：每个字段必须从「自己的对齐倍数」开始\n");
    printf("    规则 2：整个结构体大小必须是「最大对齐」的倍数\n\n");
    printf("      偏移: 0    1    2    3    4    5    6    7    8    9   10   11\n");
    printf("            ┌────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┐\n");
    printf("            │ a  │pad │pad │pad │ b  │ b  │ b  │ b  │ c  │ c  │pad │pad │\n");
    printf("            └────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┘\n");
    printf("            a@0 (1字节)，b 要对齐 4 所以从 4 开始，c 从 8 开始，\n");
    printf("            整体要凑成 4 的倍数 → 9 补到 12\n");
}

int main()
{
    printf("[1] 四种字段顺序的 sizeof:\n");
    printf("    S_A { char,char,char,char }            sizeof=%2zu  字段和=4\n", sizeof(S_A));
    printf("    S_B { char,int,char,int }              sizeof=%2zu  字段和=%zu\n",
           sizeof(S_B), sizeof(char) * 2 + sizeof(int) * 2);
    printf("    S_C { int,int,char,char }（大→小）     sizeof=%2zu  字段和=%zu\n",
           sizeof(S_C), sizeof(char) * 2 + sizeof(int) * 2);
    printf("    S_D { char,short,int,double }（小→大） sizeof=%2zu  字段和=%zu\n",
           sizeof(S_D), sizeof(char) + sizeof(short) + sizeof(int) + sizeof(double));
    printf("    S_E { double,int,short,char }（大→小） sizeof=%2zu  字段和=%zu\n",
           sizeof(S_E), sizeof(char) + sizeof(short) + sizeof(int) + sizeof(double));

    printf("\n[2] 每个字段的真实偏移:\n");
    printf("    S_B: a@%zu b@%zu c@%zu d@%zu\n",
           offsetof(S_B, a), offsetof(S_B, b), offsetof(S_B, c), offsetof(S_B, d));
    printf("    S_C: b@%zu d@%zu a@%zu c@%zu\n",
           offsetof(S_C, b), offsetof(S_C, d), offsetof(S_C, a), offsetof(S_C, c));
    printf("    S_D: a@%zu b@%zu c@%zu d@%zu\n",
           offsetof(S_D, a), offsetof(S_D, b), offsetof(S_D, c), offsetof(S_D, d));
    printf("    S_E: d@%zu c@%zu b@%zu a@%zu\n",
           offsetof(S_E, d), offsetof(S_E, c), offsetof(S_E, b), offsetof(S_E, a));

    printf("\n[3] 逐字节看 S_C（大→小，最省）和 S_B（交错，最费）:\n");
    S_C c1; for (size_t i = 0; i < sizeof(c1); ++i) ((unsigned char*)&c1)[i] = (unsigned char)(0x10 + i);
    printf("    S_C 内存: ");
    for (size_t i = 0; i < sizeof(c1); ++i) printf("%02X ", ((unsigned char*)&c1)[i]);
    printf("\n");
    S_B b1; for (size_t i = 0; i < sizeof(b1); ++i) ((unsigned char*)&b1)[i] = (unsigned char)(0x10 + i);
    printf("    S_B 内存: ");
    for (size_t i = 0; i < sizeof(b1); ++i) printf("%02X ", ((unsigned char*)&b1)[i]);
    printf("\n    （注意：填充字节里可能是任何值，不要依赖它，也不要用 memcmp 比结构体！）\n");

    printf("\n[4] 结论：\n");
    printf("    - 想省内存：字段按「对齐从大到小」排列（S_C/S_E 那种）\n");
    printf("    - 想省内存还有一招：把小的字段打包成位域（见下一例）\n");
    printf("    - 千万不要用 memcmp 比较两个结构体：填充字节的值不确定\n");
    printf("      要比就逐字段比，或者先 memset 清零再赋值\n");

    explain_layout();
    return 0;
}
