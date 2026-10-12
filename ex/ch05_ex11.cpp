// ch05_ex11.cpp —— 多维数组的内存布局：二维其实是"一行接一行"的一维
// 编译：check.bat ch05_ex11.cpp c++latest
#include <cstdio>
#include <cstring>

int main()
{
    int m[3][4] = {
        {  1,  2,  3,  4 },
        { 11, 12, 13, 14 },
        { 21, 22, 23, 24 }
    };

    printf("sizeof(m)        = %zu  （3*4*4 = 48 字节，一整块连续内存）\n", sizeof(m));
    printf("sizeof(m[0])     = %zu  （一行 4 个 int = 16 字节）\n", sizeof(m[0]));
    printf("sizeof(m[0][0])  = %zu\n", sizeof(m[0][0]));
    printf("行数 = %zu   列数 = %zu\n", sizeof(m) / sizeof(m[0]), sizeof(m[0]) / sizeof(m[0][0]));

    printf("\n逐字节看布局（用 char* 走一遍）:\n");
    char* raw = (char*)m;
    for (size_t i = 0; i < sizeof(m); i += sizeof(int)) {
        int v;
                // 用 memcpy 避免未对齐访问的麻烦
        memcpy(&v, raw + i, sizeof(int));
        printf("  偏移 %2zu 字节: %3d\n", i, v);
    }

    printf("\n换成 (int*) 走一遍（这就是 m 退化成 int* 的效果）:\n");
    int* flat = (int*)m;              // 实际上 m 退化后是 int(*)[4]，这里强制展平
    for (int i = 0; i < 12; ++i) printf("%d ", flat[i]);
    printf("\n");

    printf("\n用指针访问：m[i][j] == *(*(m+i)+j)\n");
    printf("  m[1][2]        = %d\n", m[1][2]);
    printf("  *(*(m+1)+2)    = %d\n", *(*(m + 1) + 2));
        // m+1 前进一整行（16 字节），*(m+1) 是第 1 行的数组名，再 +2 前进 2 个 int
    printf("  (char*)(m+1)-(char*)m = %td 字节  （+1 跳过一整行）\n", (char*)(m + 1) - (char*)m);
    printf("  (char*)(m[1]+1)-(char*)m[1] = %td 字节 （+1 跳一个 int）\n",
           (char*)(m[1] + 1) - (char*)m[1]);
    return 0;
}
