// ch18_ex10_expected.cpp —— std::expected（C++23）：不抛异常的错误传递
// 编译: check.bat ch18_ex10_expected.cpp c++latest
#include <iostream>
#include <expected>          // C++23；MSVC 14.51 已支持
#include <string>
#include <charconv>
#include <system_error>

// ---------------- 1) 最基础：返回"值或错误"，调用者必须自己决定 ----------------
enum class ParseErr { Empty, NotNumber, TooBig };

std::string describe(ParseErr e)
{
    switch (e) {
    case ParseErr::Empty:     return "输入是空的";
    case ParseErr::NotNumber: return "包含了非数字字符";
    case ParseErr::TooBig:    return "数值超过 1000";
    }
    return "?";
}

std::expected<int, ParseErr> parseSmall(const std::string& s)
{
    if (s.empty()) return std::unexpected(ParseErr::Empty);     // 失败：包一个 unexpected
    int v = 0;
    for (char c : s) {
        if (c < '0' || c > '9') return std::unexpected(ParseErr::NotNumber);
        v = v * 10 + (c - '0');
    }
    if (v > 1000) return std::unexpected(ParseErr::TooBig);
    return v;                                                   // 成功：直接返回 int
}

// ---------------- 2) 和 std::to_chars 组合（标准库自带 expected 的地方）----------------
void fromCharsDemo(const std::string& s)
{
    int v = 0;
        // std::from_chars 就是"不用异常"的解析接口，比 stoi 更适合嵌入式
    auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), v);
    if (ec == std::errc{}) {
        std::cout << "  from_chars(\"" << s << "\") = " << v
                  << "  (消耗了 " << (ptr - s.data()) << " 个字符)\n";
    }
    else {
        std::cout << "  from_chars(\"" << s << "\") 失败: "
                  << std::make_error_code(ec).message() << "\n";
    }
}

// ---------------- 3) 用 and_then / transform 串起来（不用一层层 if）----------------
std::expected<int, ParseErr> addTen(int v)
{
    if (v > 990) return std::unexpected(ParseErr::TooBig);
    return v + 10;
}

int main()
{
    std::cout << "=== A. 成功与失败两种返回 ===\n";
    for (const std::string s : {std::string("42"), std::string(""), std::string("4x2"), std::string("9999")}) {
        auto r = parseSmall(s);
        if (r) {                                   // has_value() 的简写
            std::cout << "  \"" << s << "\" -> 成功, 值 = " << *r << "\n";
        }
        else {
            std::cout << "  \"" << s << "\" -> 失败, 原因 = " << describe(r.error()) << "\n";
        }
    }

    std::cout << "\n=== B. value_or / value() / error() ===\n";
    auto good = parseSmall("7");
    auto bad  = parseSmall("abc");
    std::cout << "  good.value()      = " << good.value() << "\n";
    std::cout << "  good.value_or(0)  = " << good.value_or(0) << "\n";
    std::cout << "  bad.value_or(-1)  = " << bad.value_or(-1) << "   <- 失败时给默认值\n";
    std::cout << "  bad.error()       = " << describe(bad.error()) << "\n";
    std::cout << "  bool 转换: good=" << static_cast<bool>(good)
              << ", bad=" << static_cast<bool>(bad) << "\n";
    std::cout << "  （bad.value() 会抛 std::bad_expected_access，和 std::bad_optional_access 一个套路）\n";

    std::cout << "\n=== C. 无异常的解析接口 std::from_chars ===\n";
    fromCharsDemo("1234");
    fromCharsDemo("12ab");
    fromCharsDemo("");

    std::cout << "\n=== D. 用 and_then / transform 把步骤串起来 ===\n";
    for (const std::string s : {std::string("5"), std::string("995"), std::string("xx")}) {
        auto r = parseSmall(s)
                     .and_then(addTen)                   // 成功才继续；失败直接短路传下去
                     .transform([](int v) { return v * 2; });   // 成功时再加工
        std::cout << "  \"" << s << "\" -> "
                  << (r ? "结果 " + std::to_string(*r)
                        : "短路于: " + describe(r.error()))
                  << "\n";
    }

    std::cout << "\nmain 正常结束\n";
    return 0;
}
