// ch05_ex1.cpp —— 四种存储区：栈 / 堆 / 静态区 / 代码区
// 编译：check.bat ch05_ex1.cpp c++latest
#include <cstdio>
#include <cstdlib>

// 全局变量：静态存储区（static storage）
int g_var = 10;          // 已初始化 -> .data 段
int g_zero;              // 未初始化 -> .bss 段（被清零）

// 字符串字面量：只读数据区（通常和代码区在一起）
const char* g_msg = "hello";

int add(int a, int b) { return a + b; }   // 函数体 -> 代码区(text)

int main()
{
    int local = 1;                 // 栈
    static int s_var = 2;          // 静态区（生命周期同程序）
    int* heap = (int*)malloc(sizeof(int));   // 堆
    *heap = 3;

    printf("代码区(函数地址)  : %p\n", (void*)&add);
    printf("只读数据(字面量)  : %p\n", (void*)g_msg);
    printf("静态区 已初始化变量: %p  g_var=%d\n", (void*)&g_var, g_var);
    printf("静态区 未初始化变量: %p  g_zero=%d\n", (void*)&g_zero, g_zero);
    printf("静态区 static局部 : %p  s_var=%d\n", (void*)&s_var, s_var);
    printf("堆(malloc)        : %p  *heap=%d\n", (void*)heap, *heap);
    printf("栈(局部变量)      : %p  local=%d\n", (void*)&local, local);

    free(heap);
    return 0;
}
