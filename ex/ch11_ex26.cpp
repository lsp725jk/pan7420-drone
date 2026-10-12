// ch11_ex26.cpp —— 列表初始化 {}：统一写法 + 挡住窄化
// 编译：check.bat ch11_ex26.cpp c++latest
#include <cstdio>
#include <string>
#include <vector>

struct Pid {           // 聚合类型
    double kp, ki, kd;
};

int main()
{
        // ---- 写法对照：C++ 里同一个初始化有好几种语法 ----
    int a1 = 1;            // 拷贝初始化（从 C 继承）
    int a2(2);             // 直接初始化
    int a3{ 3 };           // 列表初始化（C++11 起，推荐）
    int a4 = { 4 };        // 拷贝列表初始化
    std::printf("1) %d %d %d %d\n", a1, a2, a3, a4);

        // ---- 好处 1：所有类型都能用同一套 {} 写法 ----
    std::string  s{ "hello" };
    std::vector<int> v{ 1, 2, 3 };
    Pid p{ 1.0, 0.1, 0.01 };
    std::printf("2) s=%s v.size()=%llu pid=(%.1f, %.1f, %.2f)\n",
                s.c_str(), (unsigned long long)v.size(), p.kp, p.ki, p.kd);

        // ---- 好处 2：{} 会挡住窄化转换（这是最实用的一条）----
        // int bad1{ 3.9 };          // 错误：从 double 到 int 的窄化转换
        // int bad2{ 300 };          // 视类型而定：int 到 char 之类会报错
    int  ok1 = 3.9;              // 合法：悄悄截断成 3（/W4 会给 warning C4244）
    int  ok2(3.9);               // 合法：同样截断
    std::printf("3) int ok1 = 3.9 -> %d, int ok2(3.9) -> %d（老写法不报错，值被截断）\n",
                ok1, ok2);

        // ---- 好处 3：聚合类型可以直接花括号给成员 ----
    Pid q{};
    std::printf("4) Pid q{} 全部零初始化: %.1f %.1f %.1f\n", q.kp, q.ki, q.kd);

        // ---- 好处 4：C++20 起可以按名字指定成员（指定初始化器）----
    Pid r{ .kp = 2.0, .kd = 0.5 };      // C++20 起
    std::printf("5) C++20 指定初始化: kp=%.1f ki=%.1f kd=%.1f\n", r.kp, r.ki, r.kd);

    return 0;
}
