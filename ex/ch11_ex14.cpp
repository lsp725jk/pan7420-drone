// ch11_ex14.cpp —— auto 的陷阱：引用/const 会被"吃掉"
// 编译：check.bat ch11_ex14.cpp c++latest
#include <cstdio>
#include <string>

int main()
{
    std::string s = "hello";

        // ---- 陷阱 1：auto 拷贝，auto& 才是引用 ----
    auto  copy = s;          // 类型 std::string —— 拷贝了一份
    auto& ref  = s;          // 类型 std::string& —— 只是别名
    copy[0] = 'H';
    ref[1]  = 'E';
    std::printf("1) s=%s, copy=%s（ref 改到了 s，copy 没有）\n", s.c_str(), copy.c_str());

        // ---- 陷阱 2：顶层 const 会被 auto 丢掉 ----
    const int ci = 7;
    auto c2 = ci;            // 类型 int（const 没了）
    c2 = 8;                  // 所以这里能改 —— 如果你本想只读，就危险了
    const auto c3 = ci;      // 想保留 const 就显式写出来
    std::printf("2) c2=%d（const 被丢掉）, c3=%d（显式 const）\n", c2, c3);

        // ---- 陷阱 3：auto& 绑 const 对象时要写 const auto& ----
    const std::string cs = "const-str";
    auto& r1 = cs;           // 类型 const std::string&（const 被保留）
        // r1[0] = 'X';          // 错误：不能改
    const auto& r2 = cs;     // 等价写法，语义更明显
    std::printf("3) r1=%s, r2=%s\n", r1.c_str(), r2.c_str());

        // ---- 陷阱 4：auto 不能推导数组，会退化成指针 ----
    int arr[4] = { 1, 2, 3, 4 };
    auto  a1 = arr;          // 类型 int*
    auto& a2 = arr;          // 类型 int(&)[4]，保留了数组
    std::printf("4) sizeof(a1)=%llu（指针）, sizeof(a2)=%llu（整个数组）\n",
                (unsigned long long)sizeof(a1), (unsigned long long)sizeof(a2));

        // ---- 陷阱 5：auto 用于结构化绑定和范围 for 时的选择 ----
    std::printf("5) 遍历时 auto v（拷贝）/ auto& v（引用）/ const auto& v（只读引用）\n");
    return 0;
}
