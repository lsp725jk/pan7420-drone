// ch05_ex9.cpp —— sizeof 数组 vs sizeof 指针：同一个函数两种写法差别巨大
// 编译：check.bat ch05_ex9.cpp c++latest
#include <cstdio>
#include <cstring>

// 错的写法：形参是数组语法，实际是指针，sizeof 得到指针大小
int bad_sum(int arr[], int n)
{
    int total = 0;
    size_t cnt = sizeof(arr) / sizeof(arr[0]);   // 这里算出来是 8/4 = 2，不是 5！
    printf("  bad_sum 里 sizeof(arr)=%zu, 自算元素个数=%zu（错误）\n",
           sizeof(arr), cnt);
    for (size_t i = 0; i < cnt && (int)i < n; ++i) total += arr[i];
    return total;
}

// 对的写法 1：额外传长度
int good_sum(const int* arr, size_t n)
{
    int total = 0;
    for (size_t i = 0; i < n; ++i) total += arr[i];
    return total;
}

// 对的写法 2：C++ 模板按引用收数组，长度信息不丢
template <size_t N>
int tmpl_sum(const int (&arr)[N])
{
    int total = 0;
    for (size_t i = 0; i < N; ++i) total += arr[i];
    return total;
}

int main()
{
    int a[5] = { 1, 2, 3, 4, 5 };
    int* p = a;

    printf("sizeof(a)          = %zu  （数组：5*4）\n", sizeof(a));
    printf("sizeof(p)          = %zu  （指针：64 位下 8）\n", sizeof(p));
    printf("sizeof(a)/sizeof(a[0]) = %zu  ← 只有数组能这么算长度\n", sizeof(a) / sizeof(a[0]));
    printf("sizeof(p)/sizeof(p[0]) = %zu  ← 指针算出来是错的\n", sizeof(p) / sizeof(p[0]));

    printf("\n调用三个求和的函数：\n");
    printf("  bad_sum(a,5)  = %d\n", bad_sum(a, 5));
    printf("  good_sum(a,5) = %d\n", good_sum(a, 5));
    printf("  tmpl_sum(a)   = %d\n", tmpl_sum(a));

        // sizeof 不求值：表达式根本不会被执行
    int x = 0;
    size_t s = sizeof(x++);
    printf("\nsizeof(x++) = %zu，但之后 x = %d  ← sizeof 里的表达式不求值\n", s, x);
    return 0;
}
