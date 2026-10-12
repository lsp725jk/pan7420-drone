// ch11_ex28.cpp —— 初始化列表的最后两个坑 + 汇总检查表
// 编译：check.bat ch11_ex28.cpp c++latest
#include <cstdio>
#include <vector>

struct Vec2 { double x, y; };

int main()
{
        // 坑 1：vector<int> v{10, 20} 是"两个元素"，不是"长度 10，初值 20"
    std::vector<int> a{ 10, 20 };
    std::vector<int> b(10, 20);        // 这个才是"10 个元素，都是 20"
    std::printf("1) v{10,20} 大小=%llu（两个元素）, v(10,20) 大小=%llu（十个元素都是 20）\n",
                (unsigned long long)a.size(), (unsigned long long)b.size());
    std::printf("   a[0]=%d a[1]=%d, b[0]=%d b[9]=%d\n",
                a[0], a[1], b[0], b[9]);

        // 坑 2：聚合类型里成员顺序写反了不会报错（类型恰好能转）
    Vec2 p1{ 1.0, 2.0 };      // 想表达 x=1 y=2
    Vec2 p2{ 2.0, 1.0 };      // 顺序写反，编译器不管
    std::printf("2) p1=(%.1f, %.1f) p2=(%.1f, %.1f) 顺序写反不会报错\n",
                p1.x, p1.y, p2.x, p2.y);

        // 坑 3：{} 和 () 在"有 initializer_list 构造函数"的类上语义不同
        // 详见第 12 章构造函数一节；这里只记结论：
        // 想调"初始化列表构造函数" -> 用 {}
        // 想调"普通构造函数"       -> 用 ()
    std::printf("3) vector 就是典型例子：{} 走 initializer_list，() 走普通构造\n");

        // 汇总：本节的检查表
    std::printf("\n--- 从 C 到 C++ 的初始化检查表 ---\n");
    std::printf("  变量        : int x{0};             优先用 {}\n");
    std::printf("  数组        : int a[3]{};           等价于 {0,0,0}\n");
    std::printf("  结构体      : Pid p{1.0, 0.1, 0.0}; 顺序必须对\n");
    std::printf("  容器        : vector<int> v{1,2,3}; 是元素列表\n");
    std::printf("  无参对象    : Timer t{};            绝不要写 Timer t();\n");
    return 0;
}
