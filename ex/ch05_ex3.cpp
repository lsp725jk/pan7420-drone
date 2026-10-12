// ch05_ex3.cpp —— 指针类型决定"步长"：+1 到底走多远
// 编译：check.bat ch05_ex3.cpp c++latest
#include <cstdio>
#include <cstddef>

int main()
{
    int    ia[4] = { 0, 1, 2, 3 };
    char   ca[4] = { 0, 1, 2, 3 };
    double da[4] = { 0, 1, 2, 3 };
    short  sa[4] = { 0, 1, 2, 3 };

    int*    pi = ia;
    char*   pc = ca;
    double* pd = da;
    short*  ps = sa;

    printf("sizeof(int)=%zu   int*  +1 前进 %td 字节\n", sizeof(int),    (char*)(pi + 1) - (char*)pi);
    printf("sizeof(char)=%zu  char* +1 前进 %td 字节\n", sizeof(char),   (char*)(pc + 1) - (char*)pc);
    printf("sizeof(double)=%zu double* +1 前进 %td 字节\n", sizeof(double), (char*)(pd + 1) - (char*)pd);
    printf("sizeof(short)=%zu short* +1 前进 %td 字节\n", sizeof(short),  (char*)(ps + 1) - (char*)ps);

    printf("\npi=%p  pi+1=%p\n", (void*)pi, (void*)(pi + 1));
    printf("pd=%p  pd+1=%p\n", (void*)pd, (void*)(pd + 1));

        // 反过来说：只有同类型指针相减，结果才是"元素个数"而不是字节数
    printf("\n(int*)相减: (ia+3)-(ia+1) = %td  （元素个数）\n", (ia + 3) - (ia + 1));
    printf("(char*)相减:(char*)(ia+3)-(char*)(ia+1) = %td （字节数）\n",
           (char*)(ia + 3) - (char*)(ia + 1));
    return 0;
}
