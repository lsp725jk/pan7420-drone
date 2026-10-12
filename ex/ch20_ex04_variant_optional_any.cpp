// ch20_ex04_variant_optional_any.cpp —— std::optional / std::variant / std::any
// 编译: check.bat ch20_ex04_variant_optional_any.cpp c++latest
#include <iostream>
#include <optional>
#include <variant>
#include <any>
#include <string>
#include <vector>
#include <map>

// 一个小查找表：用来演示"找不到"这个语义
const std::map<std::string, int> g_table{{"a", 1}, {"b", 2}};

// ============================================================
// A. std::optional：可能没有值
// ============================================================
std::optional<int> lookup(const std::string& key)
{
    auto it = g_table.find(key);
    if (it == g_table.end()) return std::nullopt;   // 没有值
    return it->second;
}

// ============================================================
// B. std::variant：类型安全的联合体（union），同一时刻存一种
// ============================================================
using Value = std::variant<int, double, std::string>;

std::string describeVariant(const Value& v)
{
        // std::visit：把"当前存的是哪种"交给编译器分派
    return std::visit([](const auto& x) -> std::string {
        using T = std::decay_t<decltype(x)>;
        if constexpr (std::is_same_v<T, int>)          return "int: " + std::to_string(x);
        else if constexpr (std::is_same_v<T, double>)  return "double: " + std::to_string(x);
        else                                           return "string: " + x;
    }, v);
}

// ============================================================
// C. std::any：什么都能装（但有代价）
// ============================================================
std::any makeAny(int which)
{
    if (which == 0) return 42;
    if (which == 1) return std::string("hello");
    if (which == 2) return 3.14;
    return {};                                   // 空的 any
}

int main()
{
    std::cout << "=== A. std::optional：把\"可能没有\"写进类型里 ===\n";
    for (const char* k : {"a", "b", "z"}) {
        auto r = lookup(k);
        if (r) std::cout << "  lookup(\"" << k << "\") = " << *r << "\n";
        else   std::cout << "  lookup(\"" << k << "\") = 没有这个键（nullopt）\n";
    }
    std::cout << "  value_or(-1) 的用法: lookup(\"z\").value_or(-1) = "
              << lookup("z").value_or(-1) << "\n";
    std::cout << "  对比旧写法：返回 -1 表示找不到 —— 如果 -1 也是合法值就完蛋了。\n";
    std::cout << "  也可以返回 bool + 输出参数，但调用方容易忘记检查返回值。\n";
    std::cout << "  optional 把\"检查\"变成了类型系统的一部分。\n";
    std::cout << "  坑：optional<bool> 有三态（true / false / 空），容易和 bool 混淆。\n";
    std::cout << "  坑：optional<int> o; *o; 是 UB（未定义行为），要先 if (o) 或用 value()。\n";

    std::cout << "\n=== B. std::variant：类型安全的 union ===\n";
    std::vector<Value> vs{Value{42}, Value{3.14}, Value{std::string("hi")}};
    for (const auto& v : vs) {
        std::cout << "  " << describeVariant(v)
                  << "   (index = " << v.index() << ")\n";
    }
    std::cout << "  内置的辅助工具：\n";
    Value v{10};
    std::cout << "    holds_alternative<int>(v)     = " << std::holds_alternative<int>(v) << "\n";
    std::cout << "    get<int>(v)                   = " << std::get<int>(v) << "\n";
    std::cout << "    get_if<double>(&v) 返回空指针 = "
              << (std::get_if<double>(&v) == nullptr ? "是" : "否") << "\n";
    std::cout << "  坑：get<错误的类型> 会抛 std::bad_variant_access（不是返回 0）。\n";
    std::cout << "      valueless_by_exception() 可能为真 —— 切换类型时抛异常会进入\"无值\"状态。\n";
    std::cout << "  嵌入式价值：variant 的大小 = 最大成员 + 一个 tag，\n";
    std::cout << "              比手写 union + enum 更容易写对，且没有额外开销。\n";

    std::cout << "\n=== C. std::any：任意类型，但代价不小 ===\n";
    for (int i = 0; i < 4; ++i) {
        std::any a = makeAny(i);
        std::cout << "  makeAny(" << i << "): has_value = " << a.has_value()
                  << ", type = " << (a.has_value() ? a.type().name() : "(空)") << "\n";
        if (a.type() == typeid(int)) {
            std::cout << "    any_cast<int> = " << std::any_cast<int>(a) << "\n";
        }
        else if (a.type() == typeid(std::string)) {
            std::cout << "    any_cast<std::string> = " << std::any_cast<std::string>(a) << "\n";
        }
    }
    std::cout << "  any 的代价：\n";
    std::cout << "    1) 可能堆分配（大对象）；\n";
    std::cout << "    2) any_cast 失败会抛 std::bad_any_cast；\n";
    std::cout << "    3) 类型信息在运行期查，没有编译期检查。\n";
    std::cout << "  结论：嵌入式基本不用 any；能用 variant 就用 variant。\n";

    std::cout << "\n=== D. 三者选型 ===\n";
    std::cout << "  optional<T> : \"有值或没有值\"两种情况 —— 最轻，默认选它\n";
    std::cout << "  variant<A,B,C> : \"多于两种可能，且要编译期检查\" —— 事件/消息/传感器读数\n";
    std::cout << "  any         : \"类型完全未知\" —— 只在写库、写插件接口时用\n";

    std::cout << "\nmain 正常结束\n";
    return 0;
}
