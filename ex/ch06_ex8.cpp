// ch06_ex8.cpp —— 越界写（buffer overflow）：为什么可能"不报错但结果不对"
// 编译（调试堆 + /GS 栈保护）：cl /nologo /utf-8 /std:c++latest /EHsc /W4 /MDd ch06_ex8.cpp
// 说明：本程序后半段故意越界写，可能崩也可能"看起来正常"，正是要说明这一点。
#include <cstdio>
#include <cstdlib>
#include <cstring>

// 越界 1：差一错误（off-by-one）—— 循环写到 n 而不是 n-1
void off_by_one()
{
    int a[5];
    for (int i = 0; i <= 5; ++i) {   // ← 应该是 i < 5
        a[i] = i;
    }
    printf("  off_by_one 跑完了，没有报错！a[4]=%d\n", a[4]);
    printf("  但 a[5] 已经写到数组外面（那是相邻变量/栈保护字节的位置）\n");
}

// 越界 2：改了"紧邻的另一个变量"，症状是另一个变量莫名其妙变了
void clobber_neighbour()
{
    int guard = 0xAAAAAAAA;
    int a[4];
    printf("  写越界前: guard = 0x%08X\n", (unsigned)guard);
    a[4] = 0x12345678;               // 越界 1 个 int，很可能正好压在 guard 上
    printf("  写越界后: guard = 0x%08X  %s\n", (unsigned)guard,
           (guard == (int)0xAAAAAAAA) ? "（这次没被改到，编译器排布不同）" : "← 被越界写改掉了！");
}

// 越界 3：堆上越界，破坏堆管理块，延迟到下次 malloc/free 才崩
void heap_overflow()
{
    char* p = (char*)malloc(8);
    printf("  malloc(8) 得到 %p\n", (void*)p);
    memset(p, 'A', 24);              // 写了 24 字节，超出 8 字节
    printf("  往 8 字节的块里写了 24 字节，现在还没报错\n");
    strcpy(p, "0123456789ABCDEF");   // 再写一次，依然可能没报错
    printf("  内容: %.20s\n", p);
    free(p);                         // ← 崩往往发生在这里：堆管理结构已被写坏
    printf("  free 也没崩 —— 说明堆块之间的空隙「侥幸」吸收了这次越界\n");
}

int main()
{
    printf("[1] 差一错误（off-by-one）：\n");
    off_by_one();

    printf("\n[2] 越界写覆盖相邻变量：\n");
    clobber_neighbour();

    printf("\n[3] 堆越界（写坏堆管理块，症状延迟出现）：\n");
    heap_overflow();

    printf("\n[4] 结论：越界写不报错，是因为 C 不做边界检查；\n");
    printf("    它只是「往那个地址写数据」而已。写到哪里、破坏了什么，取决于内存布局：\n");
    printf("      - 写到没人用的填充区  → 完全没症状\n");
    printf("      - 写到别的变量      → 那个变量莫名变化（结果不对）\n");
    printf("      - 写到栈保护/堆头  → 函数返回或 free 时才崩\n");
    printf("      - 写到只读页        → 立刻访问冲突\n");
    return 0;
}
