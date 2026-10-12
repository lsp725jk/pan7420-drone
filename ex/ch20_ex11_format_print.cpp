// ch20_ex11_format_print.cpp —— std::format / std::print / std::source_location
// 编译: check.bat ch20_ex11_format_print.cpp c++latest
// 说明：std::print 在 MSVC 14.51 上需要 /std:c++latest，本脚本已用该标准。
#include <iostream>
#include <format>
#include <print>
#include <string>
#include <vector>
#include <source_location>
#include <chrono>
#include <numbers>

// ============================================================
// A. std::source_location：不用再写 __FILE__ / __LINE__ 宏
// ============================================================
void logWithLocation(const std::string& msg,
                     const std::source_location& loc = std::source_location::current())
{
    std::cout << "  [" << loc.file_name() << ":" << loc.line()
              << " " << loc.function_name() << "] " << msg << "\n";
}

// 对比老写法：宏才能拿到调用点的行号
#define LOG_OLD(msg) \
    std::cout << "  [" << __FILE__ << ":" << __LINE__ << "] " << msg << "\n"

// ============================================================
// B. std::format：类型安全的格式化
// ============================================================
struct MotorState {
    int   id;
    float rpm;
    bool  armed;
};

int main()
{
    std::cout << "=== A. std::format：把 printf 语法和类型安全结合 ===\n";
    std::cout << "  " << std::format("普通替换: {} 和 {}\n", 1, "abc");
    std::cout << "  " << std::format("指定位置: {1} {0} {1}\n", "A", "B");
    std::cout << "  " << std::format("宽度对齐: [{:>8}] [{:<8}] [{:^8}]\n", "右", "左", "中");
    std::cout << "  " << std::format("数值进制: {:#x} {:o} {:b}\n", 255, 8, 5);
    std::cout << "  " << std::format("浮点控制: {:.3f} {:e} {:g}\n", 3.14159, 12345.678, 0.000012345);
    std::cout << "  （注意：Python 风格的百分号 {:.1%} 在 MSVC 14.51 上**不支持**，\n";
    std::cout << "    写进去会报 C7595「对即时函数的调用不是常量表达式」—— 实测如此。\n";
    std::cout << "    要百分号就自己乘 100 再加 '%'。）\n";
    std::cout << "  " << std::format("补零: {:06.2f}\n", 3.14159);
    std::cout << "  " << std::format("bool/指针: {} {}\n", true, static_cast<const void*>(nullptr));
    std::cout << "  和 printf 的区别：\n";
    std::cout << "    - {} 不需要写 %d/%s，类型自动匹配（写错不会崩，最多编译失败）；\n";
    std::cout << "    - 返回 std::string，可以直接 + 或存起来；\n";
    std::cout << "    - 越界/格式错误在**编译期**检查（格式串是编译期常量时）。\n";

    std::cout << "\n=== B. 格式化自定义类型 ===\n";
    MotorState m{3, 1523.7f, true};
        // C++20 起：特化 std::formatter 就能让自己的类型支持 {}
    std::cout << "  " << std::format("电机 {}: rpm={:.1f}, armed={}\n", m.id, m.rpm, m.armed);
    std::cout << "  更省事的办法：C++23 的 std::format 支持直接传范围，\n";
    std::cout << "  " << std::format("vector = {}\n", std::vector<int>{1, 2, 3});
    std::cout << "  " << std::format("range with spec = {:>4}\n", std::vector<int>{1, 2, 3});
    std::cout << "  这对调试输出很方便，但嵌入式慎用（会拖进一大堆模板代码）。\n";

    std::cout << "\n=== C. std::print / std::println：直接输出 ===\n";
    std::print("  std::print 不换行: {} + {} = {}", 2, 3, 5);
    std::println("");
    std::println("  std::println 自动换行: {} + {} = {}", 2, 3, 5);
    std::println("  支持颜色标记（Windows 终端需支持 ANSI）: \x1b[32m绿色\x1b[0m 普通");
    std::println("  格式串和 std::format 完全一样: {:.2f} {:#06x}", 3.14159, 255);
    std::cout << "  std::print 内部是直接写 stdout，通常比 cout << 快。\n";
    std::cout << "  也可以指定流: std::print(cerr, \"...\");\n";

    std::cout << "\n=== D. std::source_location ===\n";
    logWithLocation("这行日志自动带上文件、行号、函数名");
    logWithLocation("不需要传任何位置参数");
    LOG_OLD("老写法（宏）也能拿到位置，但宏无法作为参数传递、不能放进命名空间");
    std::cout << "  区别：source_location 是**真正的对象**，可以当默认参数、\n";
    std::cout << "        可以存进日志结构体、可以跨函数传递；宏做不到。\n";

    std::cout << "\n=== E. 常见坑 ===\n";
    std::cout << "  坑 1：std::format 的格式串**必须是编译期常量**才能享受编译期检查。\n";
    std::cout << "        用 std::vformat(运行时字符串, ...) 会退回运行期检查，可能抛异常。\n";
    std::cout << "  坑 2：{} 里的类型必须能格式化，自定义类型要写 std::formatter 特化。\n";
    std::cout << "  坑 3：std::print 需要 <print>；MSVC 下 c++latest 可用（实测本机 OK）。\n";
    std::cout << "  坑 4：嵌入式基本用不了 —— std::format 会引入大量模板和动态分配，\n";
    std::cout << "        64KB Flash 装不下。单片机上还是用自己写的 uart_printf。\n";

    std::cout << "\n=== F. 数字常量 ===\n";
    std::cout << "  std::numbers::pi = " << std::format("{:.10f}", std::numbers::pi) << "\n";
    std::cout << "  std::numbers::e  = " << std::format("{:.10f}", std::numbers::e) << "\n";
    std::cout << "  std::numbers::sqrt2 = " << std::format("{:.10f}", std::numbers::sqrt2) << "\n";
    std::cout << "  比手写 3.14159265358979 更不容易写错。\n";

    std::cout << "\nmain 正常结束\n";
    return 0;
}
