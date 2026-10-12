// ch05_ex2.cpp —— 取地址 &a 与解引用 *p
// 编译：check.bat ch05_ex2.cpp c++latest
#include <cstdio>

int main()
{
    int a = 42;

    printf("a 的值      : %d\n", a);
    printf("a 的地址    : %p\n", (void*)&a);   // &a 的类型是 int*

    int* p = &a;             // p 里装的就是 a 的地址
    printf("p 的值(即地址): %p\n", (void*)p);
    printf("*p 的值     : %d\n", *p);        // *p 就是 a 本身

    *p = 100;                // 通过指针改内存
    printf("改完之后 a  : %d\n", a);

        // 指针自己也有地址
    printf("p 自己的地址: %p\n", (void*)&p);

    int** pp = &p;           // 指向"指针"的指针
    **pp = 7;                // 两次解引用才到 int
    printf("**pp 改完 a : %d\n", a);
    return 0;
}
