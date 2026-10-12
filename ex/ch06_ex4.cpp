// ch06_ex4.cpp —— free 的规则：只能 free 一次，只能 free 堆上的起始地址
// 编译：check.bat ch06_ex4.cpp c++latest
#include <cstdio>
#include <cstdlib>

int main()
{
        // 1) 正确：malloc 返回的地址原样交给 free
    int* a = (int*)malloc(4 * sizeof(int));
    printf("[1] malloc 返回 a=%p，原样 free(a) 正确\n", (void*)a);
    free(a);

        // 2) free(nullptr) 是合法的空操作（所以不用写 if (p) free(p)）
    int* b = nullptr;
    free(b);
    printf("[2] free(nullptr) 合法且什么都不做\n");

        // 3) 数组要用 free，不能用 delete；malloc 配 free，new 配 delete（不要混）
    int* arr = (int*)malloc(4 * sizeof(int));
    printf("[3] malloc 出来的必须用 free（混用 delete 是 UB）\n");
    free(arr);

        // 4) 不能 free 栈上的地址 —— 下面这行会崩，已注释
    int stack_var = 5;
    printf("[4] &stack_var = %p 在栈上，free(&stack_var) 会崩，不能这么写\n", (void*)&stack_var);
        // free(&stack_var);   // UB：运行时"堆已损坏"或直接崩

        // 5) 不能 free 指针中间的位置 —— 必须是最初 malloc 返回的地址
    int* mid = (int*)malloc(8 * sizeof(int));
    printf("[5] mid=%p，free(mid+1) 会崩：free 要求传「最初的地址」\n", (void*)mid);
        // free(mid + 1);      // UB
    free(mid);

        // 6) free 之后那块内存的内容可能还在（清空是"未定义"，不要依赖）
    int* d = (int*)malloc(2 * sizeof(int));
    d[0] = 123; d[1] = 456;
    free(d);
    printf("[6] free 之后原来的值读出来还是 %d（未定义，MSVC 调试堆可能填 0xDD）\n", d[0]);
    printf("    结论：free 之后绝不能再读、再写、再 free\n");
    return 0;
}
