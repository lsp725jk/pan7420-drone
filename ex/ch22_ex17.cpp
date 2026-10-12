// ch22_ex17.cpp —— 例 22-17：SFINAE 的边界——只有"直接上下文"才享受豁免
// SFINAE 保护的是"把模板实参代进**声明**"这一步。
// 一旦代码进了函数体，替换早就成功了，里面的错误就是硬错误。
#include <cstddef>
#include <cstdio>
#include <string>
#include <type_traits>
#include <utility>

// ---------------- 版本 A：操作写在签名里（返回类型） ----------------
// 对 int：decltype(v.size()) 替换失败 -> 候选被剔除 -> 自动改用下面的兜底版本
template <typename T>
auto size_of_a(const T& v, int) -> decltype(v.size()) {
    return v.size();
}
template <typename T>
std::size_t size_of_a(const T&, ...) {
    return 0;
}

// ---------------- 版本 B：操作写在函数体里 ----------------
// 签名是 (const T&) -> std::size_t，对任何 T 都能替换成功，
// 所以它**永远**是可用候选；真调用 int 时才会在函数体里报错。
template <typename T>
std::size_t size_of_b(const T& v) {
    return v.size();  // 对 int：error C2039: "size": 不是 "int" 的成员
}

// 探测器：它只能看到 B 的**声明**，看不到函数体
template <typename T, typename = void>
struct callable_b : std::false_type {};
template <typename T>
struct callable_b<T, std::void_t<decltype(size_of_b(std::declval<const T&>()))>>
    : std::true_type {};

int main() {
    std::string s = "hello";
    int n = 42;

    std::printf("A 版 string : %zu\n", size_of_a(s, 0));
    std::printf("A 版 int    : %zu  （走兜底版本，没有报错）\n", size_of_a(n, 0));

    std::printf("callable_b<std::string> = %d\n", (int)callable_b<std::string>::value);
    std::printf("callable_b<int>         = %d  <-- 陷阱：声明看着能用\n",
                (int)callable_b<int>::value);

        // 下面这行一取消注释就编译失败 —— 因为函数体里的错误不是 SFINAE：
        // std::printf("%zu\n", size_of_b(n));
        // -> error C2039: "size": 不是 "int" 的成员
    std::printf("结论：想被 SFINAE 保护，把表达式放进签名；放进函数体就没人救得了\n");
    return 0;
}
