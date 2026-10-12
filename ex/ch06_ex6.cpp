// ch06_ex6.cpp —— 悬垂指针（dangling pointer）：指向已经交还的内存
// 编译（用调试堆，问题更容易暴露）：cl /nologo /utf-8 /std:c++latest /EHsc /W4 /MDd ch06_ex6.cpp
// 说明：本程序故意演示访问已释放内存，运行结果可能崩溃或输出垃圾值，都是预期。
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <cstdio>
#include <cstring>

// 典型 bug 1：函数返回了指向局部数组的指针
char* bad_return_local()
{
    char buf[32] = "I am on the stack";
    return buf;             // ← 返回后 buf 的栈帧就作废了
}

// 随便干点活：再调用一层函数，就会把刚才那块栈内存重新用掉
int occupy_stack(int seed)
{
    volatile int pad[16];
    for (int i = 0; i < 16; ++i) pad[i] = seed + i;
    return pad[15];
}

// 典型 bug 2：free 之后没有置空，别处又用
void use_after_free()
{
    int* p = (int*)malloc(4 * sizeof(int));
    p[0] = 0x11223344;
    printf("  free 前: p[0]=0x%08X  p=%p\n", (unsigned)p[0], (void*)p);
    free(p);
        // 此刻 p 是"悬垂指针"：它还记得那个地址，但内存已经交还
    printf("  free 后: p 还是 %p（不是 NULL！）\n", (void*)p);
    printf("  再读 p[0] = 0x%08X（读到的是被回收后的垃圾，UB）\n", (unsigned)p[0]);
    p[0] = 0x55667788;      // 写已释放内存：可能悄悄破坏堆管理结构
    printf("  往已释放的内存写 0x55667788 也没立刻报错 —— 这类 bug 最难查\n");
        // p = nullptr;         // ← 正确做法：free 之后立刻置空
}

// 正确写法 1：调用者提供缓冲区（谁申请谁释放）
void good_fill(char* out, size_t n)
{
    snprintf(out, n, "filled by callee");
}

// 正确写法 2：在堆上申请并明确"由调用者 free"
char* good_return_heap()
{
    char* p = (char*)malloc(32);
    if (p) strcpy(p, "I am on the heap");
    return p;               // 合法：这块内存的生命周期由调用者接管
}

int main()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    printf("[1] 返回局部数组的地址（栈内存，函数一返回就作废）:\n");
    char* bad = bad_return_local();
    printf("  函数里写进去的是 \"I am on the stack\"\n");
    printf("  函数返回的地址是 %p\n", (void*)bad);
    printf("  从这个地址读出来是 \"%s\"\n", bad);
    occupy_stack(7);                 // 只是调用一个普通函数，就会重用那块栈
    printf("  再调一次 occupy_stack() 之后读: \"%s\"\n", bad);
    printf("  ← 两次都是垃圾。字符串早就没了，读到什么完全看栈被谁踩过。\n");
    printf("    这就是「未定义行为」的真面目：不是随机数，而是别人写进去的数据；\n");
    printf("    它甚至可能看起来「很正常」，让你以为代码是对的。\n");

    printf("\n[2] use-after-free 演示:\n");
    use_after_free();

    printf("\n[3] 正确写法 A：调用方给缓冲区\n");
    char mine[64];
    good_fill(mine, sizeof(mine));
    printf("  good_fill 填好: \"%s\"\n", mine);

    printf("\n[4] 正确写法 B：返回堆内存，并在文档/命名上说清谁负责 free\n");
    char* heapstr = good_return_heap();
    printf("  good_return_heap 返回: \"%s\"  (地址 %p)\n", heapstr, (void*)heapstr);
    free(heapstr);
    heapstr = nullptr;
    printf("  调用者已 free 并置空\n");
    return 0;
}
