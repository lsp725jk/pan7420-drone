// ch11_ex01.cpp —— 单文件程序：C++ 的最小完整样子
// 编译：check.bat ch11_ex01.cpp c++latest

// 讲编译与链接之前，先记住这条流水线：
// 源文件(.cpp) --编译--> 目标文件(.obj) --链接--> 可执行文件(.exe)
// 编译只需要语法+声明，链接才要求"每个用到的名字真的存在"。
// 下面程序故意把"声明"和"定义"摆在一起，方便后面拆开看。

#include <cstdio>          // 头文件：只提供"声明"

// ---- 声明（declaration）：告诉编译器有这么个函数，签名长这样 ----
int  add(int a, int b);
void show(const char* who, int value);

// ---- 定义（definition）：真正生成机器码的地方 ----
int add(int a, int b)
{
    return a + b;
}

void show(const char* who, int value)
{
    std::printf("%s = %d\n", who, value);
}

int main()
{
    int s = add(3, 4);
    show("3 + 4", s);
    return 0;
}
