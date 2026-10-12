// ch00_evolution_c_vs_cpp.cpp —— 《技术演进总纲》实测脚本 4/4

// 回答的问题：**"C 的特性"和"C++ 的特性"到底怎么区分？**

// 【重要】这个文件会**编译失败**，失败就是它的结论。
// 同样的两样东西，在 C 里是**合法且有用**的，在 C++ 里**根本不存在**：

// ┌──────────────────┬────────────────────────┬──────────────────────────┐
// │ 特性              │ 当 .c 用 /std:c17 编译  │ 当 .cpp 用 c++latest 编译 │
// ├──────────────────┼────────────────────────┼──────────────────────────┤
// │ _Generic         │ ✅ 通过，实测输出        │ ❌ error C2062:          │
// │ （C11 起）        │   _Generic(int) = int   │    意外的类型"int"        │
// │                  │   _Generic(double)=double│                         │
// │ _Static_assert   │ ✅ 通过                 │ ❌ 同上（它不认识这个写法）│
// │ （C11 起）        │                         │   C++ 里对应的是          │
// │                  │                         │   static_assert（无下划线）│
// └──────────────────┴────────────────────────┴──────────────────────────┘

// 为什么必须知道这件事：教程里 **C 示例必须以 /std:c17 验证**。
// 如果错用 C++ 模式去编 .c 文件，你会得到一批**假的编译失败**，
// 然后花时间去找一个根本不存在的语法错误。

// 编译（会失败，这是预期的）：
// cmd /c "C:\py32-tools\check.bat <本文件> c++latest"

// 本机实测输出（MSVC 14.51 / VS 2026，2026 年核对）：
// ch00_evolution_c_vs_cpp.cpp(55): error C4430: 缺少类型说明符 - 假定为 int。
// 注意: C++ 不支持默认 int
// ch00_evolution_c_vs_cpp.cpp(55): error C2440: "初始化": 无法从"initializer list"
// 转换为"int"
// ch00_evolution_c_vs_cpp.cpp(55): note: 初始值设定项包含过多元素
// ch00_evolution_c_vs_cpp.cpp(59): error C2062: 意外的类型"int"
// ch00_evolution_c_vs_cpp.cpp(60): error C2062: 意外的类型"int"
// [BUILD FAILED]

// 把同一份源码存成 .c，用下面的命令编，**就是通过的**：
// cmd /c "C:\py32-tools\checkc.bat <同一份源码>.c c17"
// 实测输出：
// _Generic(int) = int
// _Generic(double) = double
// ---- exit code: 0 ----

// 对照写法（C++ 里该用什么）：
// · 想要 C 的 _Generic 那种"按类型分派" → C++ 用**函数重载**或**模板**
// · 想要 _Static_assert          → C++11 起直接写 static_assert（无下划线）
// · 想要 C 的 restrict           → C++ 没有对应关键字（靠 __restrict 扩展）

#include <stdio.h>

/* C11 的 _Generic：按表达式的类型选一个值。 这是"宏也能有类型意识"的唯一办法，C++ 里没有这个东西。 */
#define TYPENAME(x) _Generic((x), int: "int", double: "double", default: "other")

/* C11 的 _Static_assert：编译期断言（C23 起才去掉下划线写成 static_assert）。 */
_Static_assert(sizeof(int) == 4, "int must be 4 bytes");

int main(void)
{
    printf("_Generic(int) = %s\n", TYPENAME(1));
    printf("_Generic(double) = %s\n", TYPENAME(1.0));
    return 0;
}
