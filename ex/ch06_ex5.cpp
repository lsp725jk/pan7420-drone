// ch06_ex5.cpp —— 内存泄漏（memory leak）：丢了地址，内存收不回来
// 编译（必须带调试运行时 /MDd，Crt 调试堆才生效）：
// cl /nologo /utf-8 /std:c++latest /EHsc /W4 /MDd ch06_ex5.cpp
// 说明：_CrtDumpMemoryLeaks() 会把报告写到"调试输出通道"，控制台看不到；
// 所以本示例同时用返回值 + _CrtMemDifference 把泄漏量打印到 stdout。
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <cstdio>

// 泄漏 1：申请了从来不 free
void leak_simple()
{
    int* p = (int*)malloc(100 * sizeof(int));
    p[0] = 1;                  // 用完就丢了地址
}

// 泄漏 2：中途 return，忘了释放
int leak_early_return(int n)
{
    char* buf = (char*)malloc(64);
    if (n < 0) {
        return -1;             // ← 这里直接返回，buf 永远没人 free
    }
    free(buf);
    return 0;
}

// 泄漏 3：指针被覆盖，原地址丢失（这是最难查的一种）
void leak_lost_pointer()
{
    int* p = (int*)malloc(50 * sizeof(int));
    p = (int*)malloc(50 * sizeof(int));   // 第一次申请的地址被覆盖 → 泄漏
    free(p);
}

// 正确写法：所有出口都经过统一清理（goto cleanup 是 C 里最常见的做法）
int ok_function(int n)
{
    char* buf = (char*)malloc(64);
    if (buf == nullptr) return -1;
    int ret = 0;
    if (n < 0) { ret = -1; goto cleanup; }
    buf[0] = 'x';
cleanup:
    free(buf);
    return ret;
}

int main()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

        // 先记一个"干净"的基线
    _CrtMemState base, now, diff;
    _CrtMemCheckpoint(&base);

    printf("反复调用会泄漏的函数，模拟「长期运行的程序」:\n");
    for (int i = 0; i < 5; ++i) {
        leak_simple();          // 每次漏 400 字节
        leak_early_return(-1);  // 每次漏 64 字节
        leak_lost_pointer();    // 每次漏 200 字节
    }
    printf("  5 次循环，累计泄漏 %d 字节（5 * (400 + 64 + 200)）\n", 5 * (400 + 64 + 200));
    printf("  单次看不出来，跑 10000 次就是 %d 字节 ≈ %.1f MB\n",
           10000 * 664, 10000 * 664 / 1024.0 / 1024.0);

    ok_function(5);
    printf("\n正确的函数 ok_function 没有泄漏\n");

        // 把这一段的净增量打出来（这就是"泄漏了多少"）
    _CrtMemCheckpoint(&now);
    if (_CrtMemDifference(&diff, &base, &now)) {
        printf("\n_CrtMemDifference 报告：新增分配块 %ld 个，净增 %ld 字节\n",
               (long)diff.lCounts[1], (long)diff.lSizes[1]);
    }

    printf("\n_CrtDumpMemoryLeaks() 的返回值（非 0 = 有泄漏）: ");
    fflush(stdout);
    int leaked = _CrtDumpMemoryLeaks();
    printf("%d\n", leaked);
    printf("（报告正文走「调试输出」通道，控制台看不到；用 VS 调试器或 DebugView 才能看到明细）\n");
    return 0;
}
