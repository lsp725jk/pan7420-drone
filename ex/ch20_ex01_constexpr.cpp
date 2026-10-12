// ch20_ex01_constexpr.cpp —— constexpr / consteval / constinit：把计算搬到编译期
// 编译: check.bat ch20_ex01_constexpr.cpp c++latest
#include <iostream>
#include <array>
#include <string>
#include <cstddef>

// ============================================================
// A. const vs constexpr：一个只读，一个"编译期就知道"
// ============================================================
const     int a = 10;          // 只读；运行时初始化也合法
constexpr int b = 20;          // 必须是编译期常量

// constexpr 函数：既能编译期算，也能运行时调用
constexpr int square(int x) { return x * x; }

// ============================================================
// B. 编译期数组大小：只有 constexpr 能用
// ============================================================
constexpr std::size_t kSize = 4;
constexpr std::array<int, kSize> makeTable()      // constexpr 函数返回 array
{
    std::array<int, kSize> t{};
    for (std::size_t i = 0; i < kSize; ++i) t[i] = static_cast<int>(i * i);
    return t;                                     // C++14 起 constexpr 函数可以有循环
}
constexpr auto g_table = makeTable();             // 整张表在编译期生成

// ============================================================
// C. constexpr 递归：编译期算阶乘
// ============================================================
constexpr long long factorial(int n) { return n <= 1 ? 1 : n * factorial(n - 1); }

// ============================================================
// D. consteval：强制只能在编译期求值（C++20）
// ============================================================
consteval int compileTimeOnly(int x) { return x * 3; }

// ============================================================
// E. constinit：保证"静态初始化"，不保证只读（C++20）
// ============================================================
constinit int g_counter = 100;         // 在程序启动前就初始化好，不会"首次使用时才初始化"
int g_plain = 100;                     // 普通全局变量，可能是动态初始化

// 反例演示：想用 constinit 但初始化器不是常量表达式 -> 编译错误
// constinit int g_bad = std::rand();  // error: constinit 变量必须常量初始化

int main()
{
    std::cout << "=== A. const 与 constexpr 的区别 ===\n";
    std::cout << "  const int a = " << a << "\n";
    std::cout << "  constexpr int b = " << b << "\n";
    std::cout << "  square(6) 运行时调用 = " << square(6) << "\n";
    std::cout << "  square(b) 也能编译期用: constexpr int c = square(b) -> ";
    constexpr int c = square(b);
    std::cout << c << "\n";
    std::cout << "  要点：constexpr 变量隐含 const；const 变量不一定是编译期常量。\n";
    std::cout << "        const int n = std::rand(); 合法（运行时才知道），\n";
    std::cout << "        constexpr int n = std::rand(); 编译不过。\n";

    std::cout << "\n=== B. 编译期生成的查找表 ===\n";
    std::cout << "  g_table = { ";
    for (auto v : g_table) std::cout << v << " ";
    std::cout << "}\n";
    std::cout << "  这张表在 .rdata 段里是常量数据，运行时**零指令**生成它。\n";
    std::cout << "  单片机上这就是省 Flash 的关键：查表代替实时计算。\n";

    std::cout << "\n=== C. constexpr 递归（编译期阶乘）===\n";
    constexpr long long f10 = factorial(10);
    std::cout << "  factorial(10) 编译期 = " << f10 << "\n";
    std::cout << "  factorial(10) 运行时 = " << factorial(10) << "（同一个函数）\n";
    std::cout << "  注意：递归深度 = 编译器 constexpr 步数上限，太深会报错。\n";

    std::cout << "\n=== D. consteval：不给运行时留后路 ===\n";
    std::cout << "  compileTimeOnly(7) = " << compileTimeOnly(7) << "\n";
    std::cout << "  如果写 int n = 5; compileTimeOnly(n); -> 编译错误，\n";
    std::cout << "        因为 n 不是常量表达式，consteval 拒绝运行时调用。\n";
    std::cout << "  什么时候用 consteval 而不是 constexpr？\n";
    std::cout << "    当\"运行时求值\"毫无意义甚至会掩盖错误时（例如生成编译期字符串、"
                 "校验格式串）。\n";

    std::cout << "\n=== E. constinit：解决\"静态初始化顺序\"问题 ===\n";
    std::cout << "  g_counter (constinit) = " << g_counter << "\n";
    std::cout << "  g_plain   (普通)      = " << g_plain << "\n";
    std::cout << "  区别不在值，而在**何时初始化**：\n";
    std::cout << "    constinit 保证在程序启动时静态初始化好（编译期写入 .data 段）；\n";
    std::cout << "    普通全局变量可能延迟到\"首次使用\"才动态初始化，\n";
    std::cout << "    于是别的翻译单元的静态构造函数可能读到 0。\n";
    std::cout << "  constinit 可以改（不像 const），只保证初始化时机。\n";
    g_counter = 101;
    std::cout << "  修改之后 g_counter = " << g_counter << "（constinit 不禁止修改）\n";

    std::cout << "\n=== F. 三者关系速查 ===\n";
    std::cout << "  constexpr 变量 : 编译期初始化 + 隐含 const（不能改）\n";
    std::cout << "  constinit 变量 : 编译期初始化 + 可以改\n";
    std::cout << "  constexpr 函数 : 可以编译期求值；用在运行时也合法\n";
    std::cout << "  consteval 函数 : 只能编译期求值（C++20）\n";
    std::cout << "  嵌入式价值：把启动时要跑的计算移到编译期 -> 省 Flash、省启动时间、\n";
    std::cout << "              还避免运行时浮点（M0+ 没有 FPU）。\n";

    std::cout << "\nmain 正常结束\n";
    return 0;
}
