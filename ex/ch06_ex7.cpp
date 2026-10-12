// ch06_ex7.cpp —— 重复释放（double free）：同一块内存 free 两次
// 编译（用调试堆，能直接指出错误位置）：cl /nologo /utf-8 /std:c++latest /EHsc /W4 /MDd ch06_ex7.cpp
// 说明：本程序后半段故意 double free，会在运行期报错并退出，这是预期结果。
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <cstdio>

// 常见成因 1：两个指针指向同一块内存，各 free 一次
void double_free_two_pointers()
{
    int* a = (int*)malloc(sizeof(int));
    int* b = a;                 // b 和 a 指向同一块内存（浅拷贝）
    free(a);
        // free(b);                 // ← 打开这行：double free
    printf("  free(a) 之后 b 也是悬垂指针（free(b) 会 double free）\n");
    a = nullptr; b = nullptr;
}

// 常见成因 2：free 之后忘了置空，清理函数又被调了一次
void cleanup(int* p)
{
    printf("  cleanup: free(%p)\n", (void*)p);
    free(p);
}

// 常见成因 3：结构体里的指针被"浅拷贝"（赋值），两个结构体都去 free
struct Buf { char* data; int len; };

int main()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    printf("[1] 两个指针指向同一块:\n");
    double_free_two_pointers();

    printf("\n[2] 结构体浅拷贝导致的 double free:\n");
    struct Buf b1;
    b1.data = (char*)malloc(16);
    b1.len = 16;
    struct Buf b2 = b1;          // 浅拷贝：b2.data 和 b1.data 是同一个地址
    printf("  b1.data=%p  b2.data=%p  相同? %s\n",
           (void*)b1.data, (void*)b2.data, (b1.data == b2.data) ? "是" : "否");
    free(b1.data);
    printf("  free(b1.data) 之后，如果再 free(b2.data) 就是 double free\n");
    b2.data = nullptr;           // 把"另一个持份者"也置空

    printf("\n[3] 正确的成对释放模式：\n");
    int* p = (int*)malloc(sizeof(int));
    cleanup(p);
    p = nullptr;                 // ← 关键：置空之后，第二次 cleanup(p) 只会 free(NULL)
    cleanup(p);                  // free(NULL) 是合法的空操作
    printf("  第二次 cleanup(nullptr) 什么都没做 —— 这就是防 double free 的要点\n");

    printf("\n[4] 现在故意 double free：\n");
    fflush(stdout);
    int* q = (int*)malloc(sizeof(int));
    printf("  q=%p\n", (void*)q);
    free(q);
    printf("  第一次 free 完成，准备第二次 free...\n");
    fflush(stdout);
    free(q);                     // ← double free：调试堆会在这里报错
    printf("  这行不会被执行到\n");
    return 0;
}
