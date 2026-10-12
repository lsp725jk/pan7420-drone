// ch20_ex09_spaceship_designated.cpp —— 三路比较 <=> 与指定初始化
// 编译: check.bat ch20_ex09_spaceship_designated.cpp c++latest
#include <iostream>
#include <compare>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdint>

// ============================================================
// A. 一行 <=> 自动生成全部六个比较运算符
// ============================================================
struct Version {
    int major;
    int minor;
    int patch;

        // 编译器按成员声明顺序逐个比较，生成 == != < <= > >= 六个
    auto operator<=>(const Version&) const = default;
};

// ============================================================
// B. 只想比较某几个字段：手写 <=>
// ============================================================
struct SensorReading {
    std::uint32_t timestamp;
    int           value;
    bool          valid;

        // 只按 timestamp 排序；valid == false 的一律排最后
    std::strong_ordering operator<=>(const SensorReading& o) const
    {
        if (valid != o.valid) return valid ? std::strong_ordering::less
                                           : std::strong_ordering::greater;
        return timestamp <=> o.timestamp;
    }
    bool operator==(const SensorReading& o) const
    {
        return timestamp == o.timestamp && valid == o.valid;
    }
};

// ============================================================
// C. 三种比较类别（comparison category）
// ============================================================
struct Point {              // 强序：两个值要么相等要么可排序
    int x, y;
    auto operator<=>(const Point&) const = default;
};

struct CaseInsensitive {     // 弱序：可以有"等价但不相等"的值
    std::string s;
    std::weak_ordering operator<=>(const CaseInsensitive& o) const
    {
                // 忽略大小写比较
        auto lower = [](std::string t) {
            for (char& c : t) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            return t;
        };
        std::string a = lower(s), b = lower(o.s);
        if (a < b) return std::weak_ordering::less;
        if (a > b) return std::weak_ordering::greater;
        return std::weak_ordering::equivalent;   // 等价，但 "ABC" != "abc"
    }
    bool operator==(const CaseInsensitive& o) const { return s == o.s; }
};

// ============================================================
// D. 指定初始化（designated initializers，C++20）
// ============================================================
struct PidConfig {
    float kp = 1.0f;
    float ki = 0.0f;
    float kd = 0.0f;
    int   limit = 1000;
};

struct FrameHeader {
    std::uint8_t  type;
    std::uint16_t length;
    std::uint8_t  flags;
};

int main()
{
    std::cout << "=== A. <=> 默认生成六个比较 ===\n";
    Version a{1, 2, 3}, b{1, 3, 0};
    std::cout << "  a = {1,2,3}, b = {1,3,0}\n";
    std::cout << "  a == b : " << (a == b) << "\n";
    std::cout << "  a != b : " << (a != b) << "\n";
    std::cout << "  a <  b : " << (a <  b) << "\n";
    std::cout << "  a <= b : " << (a <= b) << "\n";
    std::cout << "  a >  b : " << (a >  b) << "\n";
    std::cout << "  a >= b : " << (a >= b) << "\n";
    std::cout << "  只写了 `auto operator<=>(const Version&) const = default;` 一行。\n";
    std::cout << "  注意：`= default` 的 <=> 也会自动生成 ==，不用另写。\n";

    std::cout << "\n=== B. 手写 <=>：只按 timestamp 排 ===\n";
    std::vector<SensorReading> rs{
        {300, 30, true}, {100, 10, false}, {200, 20, true}, {50, 5, true}
    };
    std::sort(rs.begin(), rs.end());
    std::cout << "  排序后（无效的排最后）:\n";
    for (const auto& r : rs) {
        std::cout << "    t=" << r.timestamp << " v=" << r.value
                  << (r.valid ? "" : " (无效)") << "\n";
    }

    std::cout << "\n=== C. 三种比较类别 ===\n";
    Point p1{1, 2}, p2{1, 3};
    auto r1 = (p1 <=> p2);
    std::cout << "  Point <=> 返回 std::strong_ordering\n";
    std::cout << "    (p1 <=> p2) < 0 ? " << (r1 < 0) << "\n";
    std::cout << "    (p1 <=> p2) == 0 ? " << (r1 == 0) << "\n";
    std::cout << "  类别速记：\n";
    std::cout << "    strong_ordering : a 等价 b => a == b（整数、字符串）\n";
    std::cout << "    weak_ordering   : a 等价 b 但 a != b（忽略大小写、按绝对值排）\n";
    std::cout << "    partial_ordering: 还可能\"无法比较\"（浮点的 NaN）\n";
    CaseInsensitive c1{"ABC"}, c2{"abc"};
    auto r2 = (c1 <=> c2);
    std::cout << "  CaseInsensitive \"ABC\" vs \"abc\":\n";
    std::cout << "    <=> 等价? " << (r2 == 0) << "\n";
    std::cout << "    == 相等? " << (c1 == c2) << "   <- 等价但不相等，这就是 weak_ordering\n";
    std::cout << "  浮点：double 的 <=> 返回 partial_ordering，因为有 NaN：\n";
    double nan = std::numeric_limits<double>::quiet_NaN();
    auto r3 = (nan <=> 1.0);
    std::cout << "    (NaN <=> 1.0) 与 0 的任何比较都是 false，\"无序\"状态: "
              << (r3 < 0) << (r3 == 0) << (r3 > 0)
              << "  <- 三个都是 0，说明不可比较\n";

    std::cout << "\n=== D. 指定初始化 ===\n";
    PidConfig onlyKp{.kp = 2.5f};
    std::cout << "  PidConfig onlyKp{.kp = 2.5f} -> kp=" << onlyKp.kp
              << " ki=" << onlyKp.ki << " kd=" << onlyKp.kd
              << " limit=" << onlyKp.limit << "（其余用成员默认值）\n";
    PidConfig full{.kp = 1.5f, .ki = 0.1f, .kd = 0.05f, .limit = 500};
    std::cout << "  全部指定 -> kp=" << full.kp << " ki=" << full.ki
              << " kd=" << full.kd << " limit=" << full.limit << "\n";
    std::cout << "  好处：读代码时一眼看出哪个值是给哪个字段的，\n";
    std::cout << "        再也不会把 ki 和 kd 传反（这是真实事故）。\n";
    std::cout << "  规则（务必记住）：\n";
    std::cout << "    1) 顺序必须与声明顺序一致 —— .kd=1, .kp=2 是编译错误；\n";
    std::cout << "    2) 不能跳跃后再回头，也不能重复指定同一个成员；\n";
    std::cout << "    3) 不能和普通位置初始化混用：PidConfig{.kp=1, 2.0f} 非法；\n";
    std::cout << "    4) 只能用于聚合类型（没有用户定义构造函数的 struct）。\n";
    FrameHeader hdr{.type = 0xAA, .length = 16, .flags = 0x01};
    std::cout << "  FrameHeader{.type=0xAA,.length=16,.flags=0x01} 也支持\n";
    std::cout << "  注意：这是 C++20 的指定初始化，规则比 C 的严格得多。\n";
    std::cout << "        C 允许乱序和跳过，C++ 不允许。\n";

    std::cout << "\n=== E. 嵌入式实用组合 ===\n";
    std::cout << "  定义外设寄存器描述表时，<=> 让\"按地址排序\"变成一行；\n";
    std::cout << "  初始化配置结构体时用指定初始化，可以避免位置参数传错。\n";
    std::cout << "  两者都是零运行时代价的语法糖。\n";

    std::cout << "\nmain 正常结束\n";
    return 0;
}
