// ch18_ex01_throw_catch.cpp —— try / catch / throw 的最小完整用法
// 编译: check.bat ch18_ex01_throw_catch.cpp c++latest
#include <iostream>
#include <string>
#include <stdexcept>

// 一个"会失败"的函数：返回商。除数非法时，用 throw 把错误"抛"出去。
double divide(double a, double b)
{
    if (b == 0.0) {
        throw std::runtime_error("divide: 除数为 0");   // 抛出即"函数立刻结束"
    }
    return a / b;
}

// 抛出任意类型：这里抛的是 int
int parsePositive(int x)
{
    if (x < 0) throw -1;          // 抛出一个 int
    return x;
}

int main()
{
        // ---- 1) 正常路径：try 里没抛，catch 全都不执行 ----
    try {
        std::cout << "[1] 10/4 = " << divide(10.0, 4.0) << "\n";
    }
    catch (const std::runtime_error& e) {
        std::cout << "[1] 不该走到这里: " << e.what() << "\n";
    }

        // ---- 2) 异常路径：throw 之后，throw 后面那句根本不执行 ----
    try {
        std::cout << "[2] 准备计算 1/0 ...\n";
        double r = divide(1.0, 0.0);            // 这一句不会返回
        std::cout << "[2] 这行永远不会打印, r = " << r << "\n";
    }
    catch (const std::runtime_error& e) {       // 按类型匹配
        std::cout << "[2] 捕获到 runtime_error: " << e.what() << "\n";
    }

        // ---- 3) 就地捕获：try 块可以包住任意多句 ----
    std::cout << "[3] 逐个计算: ";
    for (double b : {2.0, 0.0, 5.0}) {
        try {
            std::cout << divide(100.0, b) << " ";
        }
        catch (const std::runtime_error& e) {
            std::cout << "<失败:" << e.what() << "> ";
        }
    }
    std::cout << "\n";

        // ---- 4) 可以抛非类类型（不推荐，但语法允许） ----
    try {
        parsePositive(-5);
    }
    catch (int code) {                          // 按值捕获一个 int
        std::cout << "[4] 捕获到 int 异常, code = " << code << "\n";
    }

        // ---- 5) 同一个 try 配多个 catch：按顺序匹配，只进第一个匹配的 ----
    try {
        std::string s = "abc";
        std::cout << "[5] s.at(10) 之前\n";
        char c = s.at(10);                      // 越界 -> 抛 std::out_of_range
        std::cout << "[5] 不会打印 " << c << "\n";
    }
    catch (const std::out_of_range& e) {         // 更"具体"的类型写在前面
        std::cout << "[5] out_of_range: " << e.what() << "\n";
    }
    catch (const std::exception& e) {            // 更"笼统"的类型写在后面
        std::cout << "[5] 其它标准异常: " << e.what() << "\n";
    }

    std::cout << "main 正常结束\n";
    return 0;
}
