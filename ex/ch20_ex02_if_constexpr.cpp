// ch20_ex02_if_constexpr.cpp —— if constexpr：编译期分支
// 编译: check.bat ch20_ex02_if_constexpr.cpp c++latest
#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

// ============================================================
// A. 最基础：打印任意类型，按类型走不同分支
// ============================================================
template <class T>
std::string describe(const T&)
{
    if constexpr (std::is_integral_v<T>) {          // 整数
        return "整数类型";
    }
    else if constexpr (std::is_floating_point_v<T>) { // 浮点
        return "浮点类型";
    }
    else if constexpr (std::is_same_v<T, std::string>) {
        return "std::string";
    }
    else {
        return "其它类型";
    }
}

// ============================================================
// B. 对比：用普通 if（运行期）会怎样
// ============================================================
template <class T>
std::string describeWithRuntimeIf(const T&)
{
    if (std::is_integral_v<T>) return "整数类型";
        // 下面这句在 T=int 时也会被**编译**，只是运行时不执行
        // 对于不存在 .size() 的类型，这会造成编译错误 —— 所以普通 if 不行
        // return "长度 " + std::to_string(std::declval<T>().size());
    return "非整数";
}

// ============================================================
// C. 真正体现价值：只对某些类型编译某些代码
// ============================================================
struct HasSize {
    std::size_t size() const { return 42; }
};
struct NoSize {
    int value = 7;
};

template <class T>
std::string sizeOf(const T& v)
{
    if constexpr (requires { v.size(); }) {        // C++20 requires 表达式做探测
        return "有 size() = " + std::to_string(v.size());
    }
    else {
        return "没有 size()，当标量处理";
    }
}

// ============================================================
// D. if constexpr 和 static_assert 配合：精确的编译期报错
// ============================================================
template <class T>
T multiplyAll(const std::vector<T>& v)
{
    static_assert(!std::is_same_v<T, bool>,
                  "不能用 vector<bool> 做数值累乘（它是位压缩的特化）");
    T acc = T{1};
    for (const auto& x : v) acc *= x;
    return acc;
}

int main()
{
    std::cout << "=== A. if constexpr 按类型选分支 ===\n";
    std::cout << "  describe(42)        -> " << describe(42) << "\n";
    std::cout << "  describe(3.14)      -> " << describe(3.14) << "\n";
    std::cout << "  describe(std::string(\"x\")) -> " << describe(std::string("x")) << "\n";
    std::cout << "  describe('c')       -> " << describe('c') << "\n";
    std::cout << "  关键：只有被选中的那个分支会被**实例化**，其余分支被丢弃。\n";
    std::cout << "        所以未选中分支里的代码即使对当前类型非法也没关系。\n";

    std::cout << "\n=== B. 普通 if 做不到这一点 ===\n";
    std::cout << "  describeWithRuntimeIf(42) -> " << describeWithRuntimeIf(42) << "\n";
    std::cout << "  普通 if 的两个分支**都必须能编译**，所以不能用来做类型分派。\n";

    std::cout << "\n=== C. 探测成员是否存在（requires 表达式）===\n";
    HasSize hs;
    NoSize  ns;
    std::cout << "  sizeOf(HasSize) -> " << sizeOf(hs) << "\n";
    std::cout << "  sizeOf(NoSize)  -> " << sizeOf(ns) << "\n";
    std::cout << "  这是 C++20 写法；C++17 里要用 void_t 或 SFINAE，读起来痛苦得多。\n";

    std::cout << "\n=== D. static_assert 把运行期错误提前到编译期 ===\n";
    std::vector<int> vi{2, 3, 4};
    std::cout << "  multiplyAll({2,3,4}) = " << multiplyAll(vi) << "\n";
    std::cout << "  若写 vector<bool> vb{true,false}; multiplyAll(vb);\n";
    std::cout << "  编译器直接报：不能用 vector<bool> 做数值累乘……\n";
    std::cout << "  这就是 static_assert 的用法：把\"用错了\"变成编译失败，而不是运行出错。\n";

    std::cout << "\n=== E. if constexpr vs 普通 if 选型 ===\n";
    std::cout << "  if constexpr : 条件依赖模板参数、且未选中分支对当前类型非法时用\n";
    std::cout << "  普通 if      : 条件依赖运行时数据时用（例如 if (x > 0)）\n";
    std::cout << "  两者都能写时优先普通 if —— if constexpr 会让编译产物代码量翻倍\n";
    std::cout << "  （每个分支都生成一份），嵌入式的 Flash 会很敏感。\n";

    std::cout << "\nmain 正常结束\n";
    return 0;
}
