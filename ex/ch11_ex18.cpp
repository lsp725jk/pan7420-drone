// ch11_ex18.cpp —— 函数重载：同名不同参
// 编译：check.bat ch11_ex18.cpp c++latest
#include <cstdio>

// E 里不允许重名，C++ 允许：靠"参数列表"区分
int    area(int side)      { return side * side; }
double area(double r)      { return 3.14159265358979323846 * r * r; }
int    area(int w, int h)  { return w * h; }

// 只有返回类型不同 -> 不允许！这和 C 标准库冲突时的报错是同一类问题
// int  value() { return 1; }
// double value() { return 1.0; }        // 错误：无法重载（仅返回类型不同）

// 重载 + 默认参数组合
void log_msg(const char* msg, int level = 0)
{
    std::printf("  [level %d] %s\n", level, msg);
}
void log_msg(const char* msg, const char* tag)
{
    std::printf("  [%s] %s\n", tag, msg);
}

int main()
{
        // 编译器根据实参类型/个数挑最合适的那个
    std::printf("area(5)      = %d   （int 版本）\n", area(5));
    std::printf("area(2.0)    = %.3f（double 版本）\n", area(2.0));
    std::printf("area(3, 4)   = %d   （两个参数版本）\n", area(3, 4));

        // 字面量的类型决定选哪个：5 是 int，5.0 是 double
    std::printf("area(5.0f)   = %.3f（float 会转成 double）\n", (double)area(5.0f));

    log_msg("no tag");
    log_msg("with level", 3);
    log_msg("with tag", "IMU");

        // 坑：隐式转换会造成"意外选中"
        // area('A');    // char 会提升为 int，调用 area(int)
    std::printf("area('A')    = %d（char 提升为 int，选中了 int 版本）\n", area('A'));

    return 0;
}
