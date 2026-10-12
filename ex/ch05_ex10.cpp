// ch05_ex10.cpp —— a[i] 到底是怎么算出来的：四种等价写法
// 编译：check.bat ch05_ex10.cpp c++latest
#include <cstdio>

int main()
{
    int a[5] = { 10, 20, 30, 40, 50 };
    int* p = a;

    printf("四种写法取 a[2]：\n");
    printf("  a[2]       = %d\n", a[2]);
    printf("  *(a + 2)   = %d\n", *(a + 2));
    printf("  p[2]       = %d\n", p[2]);
    printf("  *(p + 2)   = %d\n", *(p + 2));
    printf("  *(2 + a)   = %d   ← 加法可交换，所以 2[a] 也合法\n", *(2 + a));
    printf("  2[a]       = %d   ← 合法但不要这么写\n", 2[a]);

    printf("\n为什么 a[i] == *(a+i)：下标运算符的定义就是后者。\n");

        // p[-1] 这种"负下标"在指针指向数组中间时是合法的
    int* mid = &a[3];
    printf("mid=&a[3]; mid[-1]=%d  mid[0]=%d  mid[1]=%d\n", mid[-1], mid[0], mid[1]);

        // 典型用法：用负下标访问"前面一个"
    printf("\n典型场景：遍历时看前一个元素\n");
    for (int* it = a + 1; it < a + 5; ++it) {
        printf("  当前 %d, 前一个 %d\n", *it, it[-1]);
    }

        // 和字符串一起用的例子
    const char* s = "hello";
    printf("\n字符串也可以用这套: s[1]='%c'  *(s+1)='%c'  1[s]='%c'\n", s[1], *(s + 1), 1[s]);
    return 0;
}
