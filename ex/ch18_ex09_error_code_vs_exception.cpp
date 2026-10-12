// ch18_ex09_error_code_vs_exception.cpp —— 异常 vs 错误码：同一条 6 层调用链，两种写法
// 编译: check.bat ch18_ex09_error_code_vs_exception.cpp c++latest
#include <iostream>
#include <string>
#include <stdexcept>
#include <optional>
#include <utility>

// ---------------- 公共业务层 ----------------
enum class Err { Ok = 0, NoData, CrcBad, TooLarge };

const char* name(Err e)
{
    switch (e) {
    case Err::Ok:       return "OK";
    case Err::NoData:   return "无数据";
    case Err::CrcBad:   return "校验错";
    case Err::TooLarge: return "超长";
    }
    return "?";
}

int payloadSize(int n)                 // 模拟"读一帧"
{
    if (n == 0) return 0;
    if (n == 1) return 99;             // 正常
    if (n == 2) return -1;             // 用它代表 CrcBad
    return 9999;                       // 用它代表 TooLarge
}

// ---------------- 写法 1：错误码层层手工上传 ----------------
// 症状：每一层都要写 if，而且"忘了检查"编译器不报错
Err layer3_code(int n, int* out)
{
    *out = payloadSize(n);
    if (*out < 0)        return Err::CrcBad;
    if (*out == 0)       return Err::NoData;
    if (*out > 4096)     return Err::TooLarge;
    return Err::Ok;
}
Err layer2_code(int n, int* out)
{
    Err e = layer3_code(n, out);
    if (e != Err::Ok) return e;        // ← 忘了写这一行，错误就静默消失
    *out += 1;
    return Err::Ok;
}
Err layer1_code(int n, int* out)
{
    Err e = layer2_code(n, out);
    if (e != Err::Ok) return e;
    *out += 1;
    return Err::Ok;
}

// ---------------- 写法 2：异常，中间层一个字都不用加 ----------------
void layer3_ex(int n)
{
    int s = payloadSize(n);
    if (s < 0)    throw std::runtime_error(std::string("校验错 (size=") + std::to_string(s) + ")");
    if (s == 0)   throw std::runtime_error("无数据");
    if (s > 4096) throw std::length_error("帧超长: " + std::to_string(s));
}
int layer2_ex(int n) { layer3_ex(n); return payloadSize(n) + 1; }   // 中间层：什么都不用做
int layer1_ex(int n) { return layer2_ex(n) + 1; }

// ---------------- 写法 3：std::optional，表示"可能没有值"，但不解释原因 ----------------
std::optional<int> layer3_opt(int n)
{
    int s = payloadSize(n);
    if (s <= 0 || s > 4096) return std::nullopt;    // 失败：没有值
    return s;
}
std::optional<int> layer1_opt(int n)
{
    auto v = layer3_opt(n);
    if (!v) return std::nullopt;
    return *v + 2;
}

int main()
{
    std::cout << "=== A. 错误码写法：每层都要 if，全链 3 层 ===\n";
    for (int n = 0; n <= 3; ++n) {
        int v = 0;
        Err e = layer1_code(n, &v);
        if (e == Err::Ok) std::cout << "  n=" << n << " -> 成功, size=" << v << "\n";
        else              std::cout << "  n=" << n << " -> 失败: " << name(e) << "\n";
    }
    std::cout << "  代价：3 个函数 × 每层 2 行样板代码 = 6 行纯噪声；\n"
                 "        而且漏写任何一处 if，错误就被静默吞掉（编译器不吭声）。\n";

    std::cout << "\n=== B. 异常写法：中间层零样板代码 ===\n";
    for (int n = 0; n <= 3; ++n) {
                // 先算结果再打印：否则"计算抛异常"会留下半行输出（下面 A 段那种错位）
        try {
            int size = layer1_ex(n);
            std::cout << "  n=" << n << " -> 成功, size=" << size << "\n";
        }
        catch (const std::length_error& e) {          // 可以只处理"我关心的那类"
            std::cout << "  n=" << n << " -> 需要特殊处理: " << e.what() << "\n";
        }
        catch (const std::exception& e) {
            std::cout << "  n=" << n << " -> 失败: " << e.what() << "\n";
        }
    }
    std::cout << "  代价：中间层真的什么都没写；错误信息自带上下文（帧超长: 9999）。\n";

    std::cout << "\n=== C. std::optional：只关心有没有值 ===\n";
    for (int n = 0; n <= 3; ++n) {
        auto v = layer1_opt(n);
        std::cout << "  n=" << n << " -> "
                  << (v ? "有值 size=" + std::to_string(*v) : std::string("空 (nullopt)"))
                  << "\n";
    }

    std::cout << "\n=== D. 选型对照表 ===\n";
    std::cout <<
        "  场景                              选哪个\n"
        "  ---------------------------------  -----------------------------\n"
        "  \"没找到\" 且不是错误（查表没命中）   std::optional\n"
        "  频繁调用、本来就常失败（解析一个字节） 错误码 / std::expected\n"
        "  失败罕见、且必须被处理（分配失败）    异常\n"
        "  构造函数里没法返回错误码              异常（唯一选择）\n"
        "  运算符重载里没法返回错误码            异常\n"
        "  硬实时 / 单片机 / -fno-exceptions     错误码 / std::expected\n"
        "  noexcept 函数内部                      错误码（抛就直接 terminate）\n";

    std::cout << "\nmain 正常结束\n";
    return 0;
}
