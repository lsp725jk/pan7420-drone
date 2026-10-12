// ch06_ex1.cpp —— malloc：唯一需要记的参数是"字节数"
// 编译：check.bat ch06_ex1.cpp c++latest
#include <cstdio>
#include <cstdlib>
#include <cstring>

int main()
{
        // 1) 最朴素的用法：申请 n 个元素的字节数
    int* a = (int*)malloc(5 * sizeof(int));
    if (a == nullptr) {                    // 申请可能失败，必须检查
        printf("malloc 失败\n");
        return 1;
    }
    for (int i = 0; i < 5; ++i) a[i] = i * 10;
    printf("[1] malloc(5*sizeof(int)) = %p  （堆地址，和栈的地址差很远）\n", (void*)a);
    for (int i = 0; i < 5; ++i) printf("    a[%d]=%d\n", i, a[i]);

        // 2) 堆内存不会自动清零：里面是上次使用的垃圾值
    int* b = (int*)malloc(3 * sizeof(int));
    printf("\n[2] 刚 malloc 出来、还没写过的内容（不保证是 0）:\n");
    for (int i = 0; i < 3; ++i) printf("    b[%d]=%d\n", i, b[i]);

        // 3) 经典错误写法：sizeof(指针) 而不是 sizeof(元素)
    int* wrong = (int*)malloc(5 * sizeof(int*));   // 在 64 位上反而申请多了
    printf("\n[3] malloc(5*sizeof(int*)) = %zu 字节，以为在申请 5 个 int（%zu 字节）\n",
           5 * sizeof(int*), 5 * sizeof(int));
    printf("    反过来 malloc(5*sizeof(char)) 给 int* 用就会少申请 4 倍，一写就崩\n");

        // 4) malloc(0)：合法，但返回值不能解引用
    void* z = malloc(0);
    printf("\n[4] malloc(0) 返回 %p（实现相关，可能是 NULL 也可能是可 free 的指针）\n", z);

        // 5) 用完必须 free，且 free 之后指针要置空
    free(a); free(b); free(wrong); free(z);
    a = b = wrong = nullptr;   // 防止悬垂指针
    printf("\n[5] free 之后把指针置为 nullptr，这样误用会被立刻发现（解引用 nullptr 必崩）\n");
    printf("    a=%p b=%p\n", (void*)a, (void*)b);
    return 0;
}
