// ch21_ex03_embedded_traps.cpp —— 嵌入式常见陷阱合集（症状 -> 原因 -> 修法）
// 编译: check.bat ch21_ex03_embedded_traps.cpp c++latest
// 注意：本文件故意包含若干"有问题的写法"，把它们的症状用输出展示出来。
#include <iostream>
#include <cstdint>
#include <cstddef>
#include <array>
#include <string>
#include <vector>
#include <cstring>

// ============================================================
// 陷阱 1：最令人头疼的解析（most vexing parse）
// ============================================================
struct Motor {
    int id = 0;
    Motor() { std::cout << "    [Motor 默认构造]\n"; }
    explicit Motor(int i) : id(i) { std::cout << "    [Motor(int) 构造 id=" << i << "]\n"; }
};

// ============================================================
// 陷阱 2：对象切片（object slicing）
// ============================================================
struct Sensor {                       // 基类
    int raw = 100;
    virtual int value() const { return raw; }
    virtual ~Sensor() = default;
};
struct TemperatureSensor : Sensor {   // 派生类
    int value() const override { return raw - 40; }
};

void showByValue(Sensor s) { std::cout << "    按值传: " << s.value() << "（派生信息已丢）\n"; }
void showByRef(const Sensor& s) { std::cout << "    按引用: " << s.value() << "（正确）\n"; }

// ============================================================
// 陷阱 4：auto 推导出意外类型
// ============================================================
std::vector<int> makeVec() { return {1, 2, 3}; }
const std::string& getGlobalName()
{
    static const std::string name = "global";
    return name;
}
std::string buildName() { return "temp"; }

// ============================================================
// 陷阱 9：char 的符号性
// ============================================================
int byteAsIntSigned(std::int8_t b)   { return b; }      // 明确有符号
int byteAsIntFromChar(char c)        { return c; }      // 实现定义！

// ============================================================
// 陷阱 10：size_t 与负数比较
// ============================================================
bool isEmptyBuggy(const std::vector<int>& v, int n)
{
    return v.size() < n;                 // n 是 int，v.size() 是 size_t
}

int main()
{
    std::cout << "=== 陷阱 1：最令人头疼的解析（most vexing parse）===\n";
    {
        Motor good1;                     // ✔ 默认构造一个对象
        (void)good1;
        Motor good2(7);                  // ✔ 调用 Motor(int)
        (void)good2;
        Motor good3{9};                  // ✔ C++11 起最推荐的写法
        (void)good3;
        std::cout << "  上面三行都正常。下面这一行有问题：\n";
        std::cout << "      Motor bad();   // ← 这不是定义对象！\n";
        std::cout << "  编译器把它解析成\"声明一个返回 Motor、无参数的函数 bad\"。\n";
        std::cout << "  症状：后面写 bad.id = 1; 报错 \"bad 不是类类型\" / \"需要对象\"。\n";
        std::cout << "  修法：① 去掉括号 Motor bad;  ② 或加大括号 Motor bad{};  (推荐)\n";
        std::cout << "        ③ 或 C++11 起用 Motor bad{}; ④ 或写成 Motor bad = Motor();\n";
        std::cout << "  同类坑：std::vector<int> v(std::istream_iterator<int>(cin), ...);\n";
        std::cout << "          这里第二个参数需要额外加括号才能避开同样的解析。\n";
    }

    std::cout << "\n=== 陷阱 2：对象切片（object slicing）===\n";
    {
        TemperatureSensor t;
        std::cout << "  派生对象自身: t.value() = " << t.value() << "\n";
        showByRef(t);
        showByValue(t);                  // 切片发生在这里
        std::cout << "  原因：按值传参时只拷贝了基类那部分（Sensor 子对象），\n";
        std::cout << "        派生类的成员和数据被\"切掉\"，虚函数表也变成基类的。\n";
        std::cout << "  修法：多态对象一律用引用或指针传参：const Sensor&\n";
        std::cout << "  同类坑：vector<Sensor> v; v.push_back(t); 也会切片，\n";
        std::cout << "          要存派生对象就用 vector<unique_ptr<Sensor>>。\n";
    }

    std::cout << "\n=== 陷阱 3：返回局部变量的引用（悬垂引用）===\n";
    {
        std::cout << "  错误写法：\n";
        std::cout << "      const std::string& bad() { std::string s = \"x\"; return s; }\n";
        std::cout << "  症状：函数返回后 s 已析构，调用方拿到的是野引用；\n";
        std::cout << "        可能\"看起来正常\"（栈没被覆盖），也可能随机崩溃。\n";
        std::cout << "  原因：返回类型是引用，没有拷贝发生，局部对象生命周期已结束。\n";
        std::cout << "  修法：① 返回 std::string（按值，靠 RVO/移动，几乎零开销）；\n";
        std::cout << "        ② 返回 static 局部变量（注意线程安全）；\n";
        std::cout << "        ③ 让调用方传引用进来做输出参数。\n";
        std::cout << "  编译器警告：MSVC 会对明显的这种写法给 warning C4172。\n";
        std::cout << "  正确例子：getGlobalName() 返回的是 static，安全。\n";
        std::cout << "    getGlobalName() = " << getGlobalName() << "\n";
    }

    std::cout << "\n=== 陷阱 4：auto 推导出意外类型 ===\n";
    {
        std::cout << "  ① auto 会丢掉 const 和引用：\n";
        std::cout << "       auto name = getGlobalName();   // 得到 std::string 的**拷贝**\n";
        auto name = getGlobalName();
        std::cout << "       类型是 std::string（拷贝）而不是 const std::string&\n";
        std::cout << "       想保留引用要写 auto& 或 const auto&\n";
        std::cout << "  ② auto&& 才是\"完美转发\"式的写法（万能引用）\n";
        std::cout << "  ③ 表达式里的\"隐形代理类型\"：\n";
        std::cout << "       std::vector<bool> vb{true, false};\n";
        std::cout << "       auto b = vb[0];   // 不是 bool！是 std::vector<bool>::reference 代理\n";
        std::vector<bool> vb{true, false};
        auto b = vb[0];
        std::cout << "       sizeof(vb[0]) = " << sizeof(vb[0])
                  << ", sizeof(bool) = " << sizeof(bool) << "\n";
        std::cout << "       decltype(b) 是代理类型，放进别的容器或跨线程用会出问题。\n";
        std::cout << "  ④ 整数常量：auto x = 0; 是 int；auto y = 0u; 是 unsigned；\n";
        std::cout << "       auto z = 1'000'000'000'000; 才是 long long。\n";
        std::cout << "       嵌入式里寄存器操作千万别用 auto 猜宽度，写清 uint32_t。\n";
    }

    std::cout << "\n=== 陷阱 5：隐式转换（implicit conversion）===\n";
    {
        double d = 3.9;
        int i = d;                       // 隐式截断，编译器可能只给 warning
        std::cout << "  int i = 3.9;      -> i = " << i << "（截断，不是四舍五入）\n";
        unsigned u = 10;
        int neg = -1;
        std::cout << "  (u > neg) 的结果 = " << (u > static_cast<unsigned>(neg))
                  << "   <- -1 被转成 4294967295，所以 10 > 4294967295 是假的\n";
        std::cout << "  这正是 while (i < v.size()) 死循环的根源。\n";
        std::cout << "  修法：\n";
        std::cout << "    ① 开 /W4 并关注 C4245 / C4018 这类\"有符号/无符号不匹配\"警告；\n";
        std::cout << "    ② 显式写 static_cast，让意图可见；\n";
        std::cout << "    ③ 需要显式构造就用 explicit 构造函数；\n";
        std::cout << "    ④ 嵌入式里 ADC/寄存器运算尽量统一用 uint32_t，避免混用 int。\n";
    }

    std::cout << "\n=== 陷阱 6：std::move 之后还用那个对象 ===\n";
    {
        std::string a = "被移动的字符串";
        std::string c = std::move(a);
        std::cout << "  std::string c = std::move(a); 之后 c = \"" << c << "\"\n";
        std::cout << "  a.size() = " << a.size() << "（实现定义，MSVC 上通常变成空）\n";
        std::cout << "  症状：move 之后继续读 a，值\"看起来还行\"或\"莫名其妙空了\"，\n";
        std::cout << "        取决于标准库实现，换编译器/换 STL 版本就可能变。\n";
        std::cout << "  原因：move 只是\"允许搬走资源\"，源对象只剩\"有效但未指定\"的状态。\n";
        std::cout << "  修法：move 之后不要再读它；要复用就先 a.clear() 再赋值。\n";
        std::cout << "        唯一的合法操作是\"析构\"或\"重新赋值\"。\n";
    }

    std::cout << "\n=== 陷阱 7：迭代器失效（iterator invalidation）===\n";
    {
        std::vector<int> v{1, 2, 3};
        auto it = v.begin();
        std::cout << "  *it = " << *it << "\n";
        v.push_back(4);                  // 可能触发扩容：所有迭代器失效
        std::cout << "  push_back 之后迭代器已可能失效（扩容导致内存搬迁）\n";
        std::cout << "  本机 capacity=" << v.capacity() << " 所以这次侥幸还有效，但**不可依赖**。\n";
        std::cout << "  症状：越界读、读到旧数据、崩溃，且常常只在数据量变大时出现。\n";
        std::cout << "  修法：\n";
        std::cout << "    ① push_back 之后重新取 begin()/end()；\n";
        std::cout << "    ② 先 reserve() 预留容量，避免中途扩容；\n";
        std::cout << "    ③ 循环中删除元素用 it = v.erase(it) 的返回值；\n";
        std::cout << "    ④ 需要稳定下标就用 std::deque 或自己管理数组。\n";
        std::cout << "  嵌入式提示：**根本别在中断里操作 vector**，\n";
        std::cout << "              中断只用固定大小的环形缓冲区（数组 + 读写指针）。\n";
    }

    std::cout << "\n=== 陷阱 8：整数溢出是 UB（有符号）/ 回绕（无符号）===\n";
    {
        std::cout << "  无符号溢出：回绕（well-defined）\n";
        std::uint8_t u = 255;
        u = static_cast<std::uint8_t>(u + 1);
        std::cout << "    uint8_t 255 + 1 = " << static_cast<int>(u) << "（回绕到 0）\n";
        std::cout << "  有符号溢出：UB（未定义行为）\n";
        int maxv = 2147483647;
        std::cout << "    若写 int x = 2147483647; x = x + 1;  -> UB\n";
        std::cout << "    编译器可以假设它不发生，从而把 if (x + 1 < x) 优化掉。\n";
        std::cout << "  症状：优化开 -O2 后行为突变、死循环、判断永远不成立。\n";
        std::cout << "  修法：\n";
        std::cout << "    ① 加法前先检查：if (a > INT_MAX - b) 溢出处理；\n";
        std::cout << "    ② 传感器差值/时间差一律用无符号并在文档里写明回绕语义；\n";
        std::cout << "    ③ C++20 的 std::cmp_less/std::in_range 做安全比较；\n";
        std::cout << "    ④ 嵌入式定时器回绕是常态，用 (uint32_t)(now - last) 自动正确。\n";
        std::cout << "       —— 前提是两边都是无符号！有符号减法回绕是 UB。\n";
    }

    std::cout << "\n=== 陷阱 9：char 的符号性是实现定义的 ===\n";
    {
        char c = static_cast<char>(0xFF);
        std::int8_t s8 = static_cast<std::int8_t>(0xFF);
        std::cout << "  (int)(char)0xFF          = " << byteAsIntFromChar(c) << "\n";
        std::cout << "  (int)(int8_t)0xFF        = " << byteAsIntSigned(s8) << "\n";
        std::cout << "  x86 上 char 默认**有符号**，所以得到 -1；\n";
        std::cout << "  ARM 上 char 默认**无符号**，同样的代码会得到 255！\n";
        std::cout << "  症状：在 PC 上调通的\"把字节转成整数\"代码，烧进 ARM 就变号了。\n";
        std::cout << "  修法：\n";
        std::cout << "    ① 存原始字节用 uint8_t，别用 char；\n";
        std::cout << "    ② 需要显式指定就加编译选项 -funsigned-char / /J；\n";
        std::cout << "    ③ 转换时写清楚 static_cast<uint8_t> 还是 static_cast<int8_t>。\n";
    }

    std::cout << "\n=== 陷阱 10：size_t 与负数比较 ===\n";
    {
        std::vector<int> v{1, 2, 3};
        std::cout << "  v.size() = " << v.size() << "（类型 size_t，无符号）\n";
        std::cout << "  v.size() < -1 的结果 = " << isEmptyBuggy(v, -1)
                  << "   <- 应该是 true（3 不小于 -1）\n";
        std::cout << "  原因：-1 被隐式转成 SIZE_MAX（18446744073709551615），\n";
        std::cout << "        于是 3 < 18446744073709551615 为真。\n";
        std::cout << "  更常见的症状：\n";
        std::cout << "      for (int i = v.size() - 1; i >= 0; --i)   // v 为空时死循环\n";
        std::cout << "      if (v.size() - 1 < 0)                     // 永远为假\n";
        std::cout << "  修法：\n";
        std::cout << "    ① 倒序循环用 size_t 并判 i-- > 0：for (size_t i = v.size(); i-- > 0; )\n";
        std::cout << "    ② 比较前显式转换：static_cast<long long>(v.size()) < n\n";
        std::cout << "    ③ C++20 用 std::ssize(v) 得到有符号长度（推荐）\n";
        std::cout << "       std::ssize(v) = " << std::ssize(v) << "\n";
        std::cout << "    ④ 开 /W4 并处理 C4018（有符号/无符号不匹配）警告。\n";
    }

    std::cout << "\nmain 正常结束\n";
    return 0;
}
