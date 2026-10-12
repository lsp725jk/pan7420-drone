// ch05_ex6.cpp —— 为什么 int *p; *p = 1; 是 UB（未定义行为）
// 编译：check.bat ch05_ex6.cpp c++latest
// 注意：本程序故意访问非法地址，运行会崩溃（退出码非 0），这是预期结果。
#include <cstdio>

int g_ok = 0;          // 全局变量：地址由链接器决定，可以放心取地址

int main()
{
        // ---- 情况 1：指针未初始化，里面是栈上的垃圾值 ----
    int* bad;                    // 里面是上一次函数调用留下的数据，不是"没有值"
    printf("[1] 未初始化指针 bad  = %p  （垃圾值，每次运行都可能不同）\n", (void*)bad);
    printf("    它是否等于 NULL? %s\n", bad == nullptr ? "是" : "否");
    printf("    所以 if (p == NULL) 检查不出这种错误\n");

        // ---- 情况 2：把一个小整数当地址用 ----
    int* tiny = (int*)8;
    printf("[2] tiny = %p   （低地址，操作系统禁止访问）\n", (void*)tiny);

        // ---- 正确做法 ----
    int* good = &g_ok;           // 指向真实存在的对象
    *good = 1;                   // 合法
    printf("[3] good = &g_ok, *good = %d  （合法写入）\n", g_ok);

        // ---- 下面这一行是 UB：写进一个不属于本程序的内存 ----
    printf("\n即将执行 *bad = 1;  →  预期崩溃\n");
    fflush(stdout);
    *bad = 1;                    // UB！不是语法错误，编译器不拦，运行期可能崩
    printf("这行通常不会被执行到：*bad = %d\n", *bad);
    return 0;
}
