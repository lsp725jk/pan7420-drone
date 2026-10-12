// ch05_ex18.cpp —— void*：无类型指针，malloc 和 qsort 都靠它
// 编译：check.bat ch05_ex18.cpp c++latest
#include <cstdio>
#include <cstdlib>
#include <cstring>

// qsort 的比较函数签名是固定的 int(const void*, const void*)
int cmp_int_asc(const void* a, const void* b)
{
    int x = *(const int*)a;      // void* 必须先转成具体类型才能解引用
    int y = *(const int*)b;
    return (x > y) - (x < y);    // 返回 -1 / 0 / 1
}

int main()
{
        // 1) void* 可以接任何对象指针，不需要强制转换
    int i = 42;
    double d = 3.14;
    char c = 'A';
    void* v1 = &i;
    void* v2 = &d;
    void* v3 = &c;
    printf("void* 能装: &int=%p  &double=%p  &char=%p\n", v1, v2, v3);
    printf("sizeof(void*) = %zu  （和普通指针一样大）\n", sizeof(void*));

        // 2) 但解引用前必须转回具体类型
    printf("\n转回具体类型再解引用:\n");
    printf("  *(int*)v1    = %d\n", *(int*)v1);
    printf("  *(double*)v2 = %.2f\n", *(double*)v2);
    printf("  *(char*)v3   = '%c'\n", *(char*)v3);
    printf("  用 char* 走字节: ");
    for (size_t k = 0; k < sizeof(int); ++k) printf("%02X ", (unsigned char)((char*)v1)[k]);
    printf("\n");

        // 3) 指针运算：void* 不能直接做算术（C++ 里），要先转成 char*
        // v1 + 1;             // 编译错误：void* 上的算术是非法的
    char* bytes = (char*)v1;
    printf("\nvoid* 不能 +1，转成 char* 才行: bytes+1 前进 %td 字节\n", (bytes + 1) - bytes);
        // printf("sizeof(void) = %zu\n", sizeof(void));
        // ↑ C++ 标准不允许 sizeof(void)；MSVC 会报 error C2070，别写。
    printf("注意：sizeof(void) 在 C++ 里非法（MSVC 报 C2070），sizeof(void*) 才是 %zu\n",
           sizeof(void*));

        // 4) 真实用途：malloc 返回 void*，C 里不用转，C++ 里必须显式转
    int* arr = (int*)malloc(5 * sizeof(int));
    for (int k = 0; k < 5; ++k) arr[k] = (5 - k) * 10;
    printf("\nmalloc 返回 void*，C++ 里要 (int*) 转一下。排序前: ");
    for (int k = 0; k < 5; ++k) printf("%d ", arr[k]);

        // qsort 用 void* 实现"泛型"：能排任意类型的数组
    qsort(arr, 5, sizeof(int), cmp_int_asc);
    printf("\nqsort 排序后: ");
    for (int k = 0; k < 5; ++k) printf("%d ", arr[k]);
    printf("\n");

        // 同一个 qsort 也能排 double
    double dd[4] = { 2.5, 0.5, 9.0, 1.5 };
        // 这里只演示"能传"，比较函数得自己写（略）
    printf("qsort 的 void* 参数能接 double 数组: %p\n", (void*)dd);

    free(arr);
    return 0;
}
