// ch11_ex02.cpp —— 把"声明"抽到头文件里
// 编译：check.bat ch11_ex02.cpp c++latest（本文件自包含，含头文件内容的等价展开）

// 真实工程里会拆成三个文件：
// ch11_ex02_math.h   只有声明（+ 头文件保护）
// ch11_ex02_math.cpp 放定义（#include "ch11_ex02_math.h"）
// ch11_ex02_main.cpp 使用者（#include "ch11_ex02_math.h"）
// 这里用一个文件模拟"头文件 + 实现"的边界，方便单文件编译运行。

#include <cstdio>

// ===================== 相当于 ch11_ex02_math.h =====================
#ifndef CH11_EX02_MATH_H          // 头文件保护（include guard）：
#define CH11_EX02_MATH_H        // 防止同一个头文件被重复展开
namespace mathx {
    int  add(int a, int b);      // 只有声明，没有函数体
    int  mul(int a, int b);
    extern const double PI;      // const 全局量：声明要写 extern
}
#endif // CH11_EX02_MATH_H

// ===================== 相当于 ch11_ex02_math.cpp =====================
namespace mathx {
    int add(int a, int b) { return a + b; }
    int mul(int a, int b) { return a * b; }
    const double PI = 3.14159265358979323846;   // 定义处不写 extern
}

// ===================== 相当于 ch11_ex02_main.cpp =====================
int main()
{
    std::printf("add(3,4)=%d\n", mathx::add(3, 4));
    std::printf("mul(3,4)=%d\n", mathx::mul(3, 4));
    std::printf("PI=%.6f\n", mathx::PI);
    return 0;
}
