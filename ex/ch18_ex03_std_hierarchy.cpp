// ch18_ex03_std_hierarchy.cpp —— 标准异常层次：std::exception / logic_error / runtime_error
// 编译: check.bat ch18_ex03_std_hierarchy.cpp c++latest
#include <iostream>
#include <exception>
#include <stdexcept>
#include <string>
#include <vector>
#include <new>

// 把标准库的异常层次打印出来（只看我们要背的那几支）
void dumpTree()
{
    std::cout <<
        "std::exception\n"
        " |-- std::logic_error              <- 逻辑错误：程序自己写错了，理论上能提前查出来\n"
        " |    |-- std::invalid_argument\n"
        " |    |-- std::domain_error\n"
        " |    |-- std::length_error\n"
        " |    |-- std::out_of_range\n"
        " |-- std::runtime_error            <- 运行时错误：只有跑起来才知道（文件没了、网断了）\n"
        " |    |-- std::range_error\n"
        " |    |-- std::overflow_error\n"
        " |    |-- std::underflow_error\n"
        " |-- std::bad_alloc                 <- new 失败\n"
        " |-- std::bad_cast                  <- dynamic_cast 引用失败\n"
        " |-- std::bad_optional_access       <- optional::value() 空访问\n"
        " |-- std::bad_variant_access        <- variant::get 类型不对\n";
}

// 越界：logic_error 家族
void showOutOfRange()
{
    std::vector<int> v{1, 2, 3};
    try {
        std::cout << "  取 v.at(99) ...\n";
        std::cout << v.at(99) << "\n";
    }
    catch (const std::out_of_range& e) {         // 精确捕获
        std::cout << "  out_of_range: " << e.what() << "\n";
    }
}

// 用 std::stoi 演示 invalid_argument / out_of_range 两兄弟
void showStoi(const char* text)
{
    try {
        int n = std::stoi(text);
        std::cout << "  stoi(\"" << text << "\") = " << n << "\n";
    }
    catch (const std::invalid_argument& e) {
        std::cout << "  invalid_argument: " << e.what() << "  (输入根本没数字)\n";
    }
    catch (const std::out_of_range& e) {
        std::cout << "  out_of_range: " << e.what() << "  (数字太大装不进 int)\n";
    }
}

// 只写一个 catch(std::exception&) 就能兜住上面所有类型
// —— 前提是：用公有继承、并且按引用捕获
void catchAllByBase(int which)
{
    try {
        if (which == 1) throw std::runtime_error("运行时炸弹");
        if (which == 2) throw std::out_of_range("越界炸弹");
        if (which == 3) throw std::bad_alloc();
    }
    catch (const std::exception& e) {
        std::cout << "  统一捕获 -> " << e.what() << "\n";
    }
}

int main()
{
    std::cout << "=== A. 标准异常层次 ==\n";
    dumpTree();

    std::cout << "\n=== B. out_of_range（logic_error 家族）==\n";
    showOutOfRange();

    std::cout << "\n=== C. stoi 的两种失败，抛的是不同类型 ===\n";
    showStoi("123");
    showStoi("12abc");
    showStoi("99999999999999999999");

    std::cout << "\n=== D. 一个 catch(std::exception&) 兜住所有子孙 ===\n";
    for (int i = 1; i <= 3; ++i) catchAllByBase(i);

    std::cout << "\n=== E. what() 对 bad_alloc 说什么 ===\n";
    std::cout << "  std::bad_alloc().what() = \"" << std::bad_alloc().what() << "\"\n";
    std::cout << "  （what() 返回的指针只在异常对象活着时有效，别存下来）\n";

    std::cout << "\nmain 正常结束\n";
    return 0;
}
