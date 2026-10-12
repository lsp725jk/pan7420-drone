// ch05_ex6b.cpp —— 把地址 0 和野地址解引用（对照例）
// 编译：check.bat ch05_ex6b.cpp c++latest
// 本程序故意制造访问冲突，运行会崩溃（退出码非 0），这是预期结果。
#include <cstdio>

int main()
{
    int* nullp = nullptr;
    printf("准备写 *nullp ...\n");
    fflush(stdout);
    *nullp = 1;              // 写 0 号地址：Windows 上必然"访问冲突"
    printf("不会执行到这里\n");
    return 0;
}
