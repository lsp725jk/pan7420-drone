// ch11_ex25.cpp —— 结构化绑定的变式与坑
// 编译：check.bat ch11_ex25.cpp c++latest
#include <cstdio>
#include <map>
#include <string>

struct Cfg { int baud; const char* name; };

int main()
{
        // 1) 三种限定写法，改不改原对象看这个
    Cfg a{ 115200, "a" };
    auto        [x1, n1] = a;    // 拷贝：改 x1 不影响 a.baud
    auto&       [x2, n2] = a;    // 引用：改 x2 就是改 a.baud
    const auto& [x3, n3] = a;    // 只读引用：不拷贝，也改不了
    x1 = 1; x2 = 9600; (void)n1; (void)n2; (void)n3; (void)x3;
    std::printf("1) a.baud=%d（auto 改不动，auto& 能改）\n", a.baud);

        // 2) 配合字典遍历：C++17 之后最舒服的写法
    std::map<std::string, int> m{ { "imu", 1000 }, { "baro", 50 } };
    std::printf("2) 遍历 map:\n");
    for (const auto& [key, val] : m)
        std::printf("   %s -> %d\n", key.c_str(), val);

        // 3) 只想拿其中一部分：std::pair / std::tuple 用 std::get 或 std::tie
    std::pair<int, int> pr{ 1, (int)m.size() };
    auto [first, second] = pr;
    std::printf("3) first=%d second=%d\n", first, second);

        // 4) 坑：结构化绑定不能加类型说明，也不能先声明后绑定
        // int [p, q] = arr;               // 语法错误
        // auto [p, q];                    // 错误：必须有初始化器
    std::printf("4) 结构化绑定必须一步到位：auto [a,b] = ...;\n");

        // 5) 坑：结构化绑定的名字是"别名"，别对它们取地址传给 C 接口
    std::printf("5) 需要传地址时就老老实实拆成普通变量，别用结构化绑定\n");

    return 0;
}
