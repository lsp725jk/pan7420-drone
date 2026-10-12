// ch05_ex4.cpp —— 指针本身有多大？所有指针都一样大吗？
// 编译：check.bat ch05_ex4.cpp c++latest
#include <cstdio>

struct S { int a; double b; char c[7]; };

int main()
{
    printf("本程序是 %zu 位程序（指针 %zu 字节）\n", sizeof(void*) * 8, sizeof(void*));
    printf("sizeof(char*)      = %zu\n", sizeof(char*));
    printf("sizeof(int*)       = %zu\n", sizeof(int*));
    printf("sizeof(double*)    = %zu\n", sizeof(double*));
    printf("sizeof(struct S*)  = %zu\n", sizeof(S*));
    printf("sizeof(int(*)())   = %zu   （函数指针）\n", sizeof(int(*)()));
    printf("sizeof(int**)      = %zu\n", sizeof(int**));
    printf("sizeof(char[100])  = %zu   （数组不是指针！）\n", sizeof(char[100]));
    return 0;
}
