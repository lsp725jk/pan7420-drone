// ch05_ex13.cpp —— 行指针当函数参数：二维数组怎么传
// 编译：check.bat ch05_ex13.cpp c++latest
#include <cstdio>

// 写法 1：必须写出列数（除第一维外都要给）
void print_v1(int m[][4], int rows)
{
    printf("写法1 int m[][4]: ");
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < 4; ++j) printf("%d ", m[i][j]);
    printf("\n");
}

// 写法 2：显式写成"指向 4 个 int 的数组的指针"（和写法 1 完全等价）
void print_v2(int (*m)[4], int rows)
{
    printf("写法2 int (*m)[4]: ");
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < 4; ++j) printf("%d ", m[i][j]);
    printf("\n");
}

// 写法 3：退化成一级指针 + 自己算下标（最通用，但易错）
void print_v3(const int* flat, int rows, int cols)
{
    printf("写法3 const int* + 手算下标: ");
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j) printf("%d ", flat[i * cols + j]);
    printf("\n");
}

// 写法 4：形参里连"第一维"都写出来，编译器照样忽略它
void print_v4(int m[3][4])          // 等价于 int (*m)[4]
{
    printf("写法4 int m[3][4]: sizeof(m)=%zu ← 第一维被忽略，仍是指针\n", sizeof(m));
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 4; ++j) printf("%d ", m[i][j]);
    printf("\n");
}

int main()
{
    int a[3][4] = { { 1,2,3,4 }, { 5,6,7,8 }, { 9,10,11,12 } };
    int b[2][4] = { { 1,1,1,1 }, { 2,2,2,2 } };

    print_v1(a, 3);
    print_v2(a, 3);
    print_v3(&a[0][0], 3, 4);       // 传首元素地址，退化成 int*
    print_v4(a);

    printf("\n同一函数能处理不同行数的数组（列数必须一样）:\n");
    print_v1(b, 2);
    print_v2(b, 2);

        // 注意：下面这种"列数在运行期决定"的变长数组（VLA, variable-length array）
        // 是 C99 的特性，C++ 标准里没有，MSVC 也不支持，编译会报 C2131。
        // 需要处理任意列数时，用 print_v3 那种"一维指针 + 手算下标"的写法。
    return 0;
}
