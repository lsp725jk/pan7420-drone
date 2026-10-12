// ch05_ex12.cpp —— int* a[5] 和 int (*a)[5]：一眼分不清的两种类型
// 编译：check.bat ch05_ex12.cpp c++latest
#include <cstdio>

int main()
{
    int x = 1, y = 2, z = 3;
    int row0[5] = { 10, 11, 12, 13, 14 };
    int row1[5] = { 20, 21, 22, 23, 24 };
    int grid[2][5] = { { 100, 101, 102, 103, 104 },
                       { 200, 201, 202, 203, 204 } };

        // 1) 指针数组：5 个元素的数组，每个元素是 int*
    int* parr[5] = { &x, &y, &z, nullptr, nullptr };
    printf("int* parr[5]  sizeof(parr)=%zu  （5 个指针 = 40 字节）\n", sizeof(parr));
    printf("  parr[0]=%p *parr[0]=%d\n", (void*)parr[0], *parr[0]);

        // 2) 数组指针：一个指针，指向"5 个 int 的数组"
    int (*prow)[5] = &row0;       // 注意括号不能少
    printf("int (*prow)[5] sizeof(prow)=%zu （一个指针 = 8 字节）\n", sizeof(prow));
    printf("  (*prow)[2] = %d   （必须先解引用拿到整行，再取元素）\n", (*prow)[2]);
    printf("  (char*)(prow+1)-(char*)prow = %td 字节 （+1 跳一整行）\n",
           (char*)(prow + 1) - (char*)prow);

        // 3) 二维数组名退化后，正好就是"数组指针"
    int (*prow2)[5] = grid;       // grid 退化成 int(*)[5]
    printf("\n用数组指针遍历二维数组：\n");
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 5; ++j) printf("%4d", prow2[i][j]);
        printf("\n");
    }
    printf("  注意 prow2[i] 的类型是 int[5]，再 [j] 才到 int\n");
    printf("  prow2=%p  prow2+1=%p  差 %td 字节\n",
           (void*)prow2, (void*)(prow2 + 1), (char*)(prow2 + 1) - (char*)prow2);

        // 4) 另一种等价写法：用行指针的指针
    int* flat[2] = { row0, row1 };   // 指针数组装两行
    printf("\n指针数组也能当「二维」用：flat[1][3]=%d\n", flat[1][3]);
    return 0;
}
