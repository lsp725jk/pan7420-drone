// ch22_ex14.cpp —— 例 22-14：SFINAE 到底在解决什么问题
// 先看需求：写一个 describe(v)，
// - 如果 v 有 size()（容器、字符串），说出它的长度；
// - 如果 v 没有 size()（int、自定义点），走另一条路。
// 用重载做不到"有就选这个、没有就选那个"，因为重载只看参数类型，不看"能不能调 size()"。
// SFINAE 补的就是这一块：**当模板实参替换进签名失败时，把这个候选悄悄扔掉，
// 而不是报错**（Substitution Failure Is Not An Error）。
#include <cstdio>
#include <string>
#include <type_traits>
#include <vector>

// 候选 1：返回类型里写了 decltype(v.size())。
// 对 int 来说 v.size() 不存在 -> 替换失败 -> 这个候选被移出重载集合（不报错！）
// 第二个参数写 int，是为了让它比下面的 "..." 更优先。
template <typename T>
auto describe(const T& v, int) -> decltype(v.size(), std::string()) {
    return std::string("有 size() -> 长度 ") + std::to_string(v.size());
}

// 候选 2：兜底。C 风格可变参数 "..." 的匹配代价最高，永远排最后
template <typename T>
std::string describe(const T&, ...) {
    return std::string("没有 size()");
}

// 变式：不用 decltype 返回类型，改成在返回类型上做算术——同样靠替换失败
template <typename T>
auto is_small(const T& v, int) -> decltype(v.size(), bool()) {
    return v.size() < 10;
}
template <typename T>
bool is_small(const T&, ...) {
    return false;
}

struct Point {
    int x, y;
};

int main() {
    std::vector<int> v{1, 2, 3};
    std::string s = "hello";
    Point p{1, 2};
    int n = 42;

    std::printf("vector  : %s\n", describe(v, 0).c_str());
    std::printf("string  : %s\n", describe(s, 0).c_str());
    std::printf("Point   : %s\n", describe(p, 0).c_str());
    std::printf("int     : %s\n", describe(n, 0).c_str());

    std::printf("is_small(vector) = %d\n", (int)is_small(v, 0));
    std::printf("is_small(int)    = %d（走的兜底版本）\n", (int)is_small(n, 0));

        // 关键理解：
        // "替换失败"指的是把 T 代进**函数签名**（返回类型、参数类型）时失败；
        // 一旦错误出现在**函数体**里，那就是硬错误，SFINAE 救不了（见例 22-17）。
    std::printf("SFINAE = Substitution Failure Is Not An Error\n");
    return 0;
}
