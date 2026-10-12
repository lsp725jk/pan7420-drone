// ch11_ex04.cpp —— 编译单元、inline 与"重复定义"（ODR）
// 编译：check.bat ch11_ex04.cpp c++latest
// 这个文件本身可以独立编译运行；下面同时说明"头文件里放定义"的两种下场。
#include <cstdio>

// ---- 情况 A：函数定义只能出现在一个翻译单元里 ----
// 如果把这个定义搬到头文件里，两个 .cpp 都包含它，链接期会报：
// error LNK2005: "int __cdecl square(int)" (?square@@YAHH@Z) 已经在 xxx.obj 中定义
// （完整复现见 ch11_ex05_bad_math.h + ch11_ex05_bad_a.cpp + ch11_ex05_bad_b.cpp）
int square(int x) { return x * x; }

// ---- 情况 B：加 inline，同一个函数定义可以出现在多个翻译单元里 ----
// inline 的真正含义不是"一定把代码展开"，而是
// "允许在多个翻译单元里出现同一份定义，链接器只保留一份"。
// 所以小函数可以直接写在头文件里。
inline int cube(int x) { return x * x * x; }

// ---- 情况 C：inline 函数里的 static 局部量，全局只有一份 ----
inline int counter_next()
{
    static int n = 0;      // 所有翻译单元共享同一个 n
    return ++n;
}

int main()
{
    std::printf("square(5)=%d\n", square(5));
    std::printf("cube(5)=%d\n", cube(5));

        // 注意：不要把多个 counter_next() 写进同一条语句。
        // 函数实参的求值顺序在 C++ 里是"未指定"的（见 11.15 节），
        // 那种写法在 MSVC 上会从右往左算，看起来像 bug。
    int a = counter_next();
    int b = counter_next();
    int c = counter_next();
    std::printf("counter: %d %d %d\n", a, b, c);
    return 0;
}
