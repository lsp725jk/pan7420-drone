// ch06_ex10.cpp —— 结构体对齐与填充（padding）：sizeof 比字段加起来大
// 编译：check.bat ch06_ex10.cpp c++latest
#include <cstdio>
#include <cstddef>   // offsetof

// 按"从大到小"排列字段：填充最少
struct Good { double d; int i; short s; char c; };
// 按"从大到小"排列但顺序打乱：填充变多
struct Bad { char c; double d; short s; int i; };

struct A { char c; int i; };
struct B { char c1; char c2; int i; };
struct C { char c; };

int main()
{
    printf("各类型的大小与对齐要求：\n");
    printf("  char   size=%zu align=%zu\n", sizeof(char), alignof(char));
    printf("  short  size=%zu align=%zu\n", sizeof(short), alignof(short));
    printf("  int    size=%zu align=%zu\n", sizeof(int), alignof(int));
    printf("  double size=%zu align=%zu\n", sizeof(double), alignof(double));

    printf("\n[1] 结构体整体大小不是字段之和：\n");
    printf("  struct A { char c; int i; }        sizeof=%zu （字段和=5，多了 %zu 字节填充）\n",
           sizeof(A), sizeof(A) - 5);
    printf("  struct B { char c1;char c2; int i;} sizeof=%zu （字段和=6，多了 %zu 字节填充）\n",
           sizeof(B), sizeof(B) - 6);
    printf("  struct C { char c; }               sizeof=%zu\n", sizeof(C));

    printf("\n[2] 每个字段的真实偏移（用 offsetof 打印）:\n");
    printf("  A: c 在偏移 %zu, i 在偏移 %zu  ← i 前面插了 3 字节填充\n",
           offsetof(A, c), offsetof(A, i));
    printf("  B: c1 在 %zu, c2 在 %zu, i 在 %zu  ← c2 正好补上，填充少了\n",
           offsetof(B, c1), offsetof(B, c2), offsetof(B, i));
    printf("  （offsetof 是标准宏，定义在 <cstddef>；对非标准布局类型用它才有意义）\n");

    printf("\n[3] 字段顺序影响总大小：\n");
    printf("  struct Good { double d; int i; short s; char c; } sizeof=%zu （字段和=15）\n",
           sizeof(Good));
    printf("  struct Bad  { char c; double d; short s; int i; } sizeof=%zu （字段和=15）\n",
           sizeof(Bad));
    printf("  ← 同样的字段，一个 %zu 字节，一个 %zu 字节，差的 %zu 字节全是填充\n",
           sizeof(Good), sizeof(Bad), sizeof(Bad) - sizeof(Good));
    printf("  结论：按「从大到小」声明字段最省内存，这在单片机上很有意义\n");

    printf("\n[4] 为什么要对齐：CPU 一次读 4/8 字节，跨边界要读两次\n");
    printf("  struct A 的布局（64 位，int 对齐 4）:\n");
    printf("    偏移 0 : [c][pad][pad][pad]   ← char 只占 1 字节，后面 3 字节空着\n");
    printf("    偏移 4 : [i ][i  ][i  ][i  ]   ← int 从 4 的倍数开始，一次就能读完\n");
    printf("    总大小 8 而不是 5，是为了让「下一个 A 的 i」也落在 4 的倍数上\n");
    return 0;
}
