// ch20_ex08_ranges.cpp —— 范围库（ranges / views）与管道 |
// 编译: check.bat ch20_ex08_ranges.cpp c++latest
#include <iostream>
#include <ranges>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>

struct Reading {
    int  rawValue;
    bool valid;
};

int main()
{
    std::vector<int> v{5, 2, 8, 1, 9, 3, 7, 4, 6};

    std::cout << "=== A. 老写法 vs ranges 写法 ===\n";
    std::cout << "  目标：取出所有偶数，乘以 10，打印。\n";
    std::cout << "  老写法（迭代器 + 算法 + lambda）：\n";
    {
        std::vector<int> tmp;
        std::copy_if(v.begin(), v.end(), std::back_inserter(tmp),
                     [](int x) { return x % 2 == 0; });
        std::vector<int> out;
        std::transform(tmp.begin(), tmp.end(), std::back_inserter(out),
                       [](int x) { return x * 10; });
        std::cout << "    ";
        for (int x : out) std::cout << x << " ";
        std::cout << "\n";
    }
    std::cout << "  ranges 写法（管道，零拷贝、不产生中间容器）：\n";
    std::cout << "    ";
    auto even10 = v | std::views::filter([](int x) { return x % 2 == 0; })
                    | std::views::transform([](int x) { return x * 10; });
    for (int x : even10) std::cout << x << " ";
    std::cout << "\n";

    std::cout << "\n=== B. 常用视图（view）速查 ===\n";
    std::cout << "  v | views::take(3)         取前 3 个        : ";
    for (int x : v | std::views::take(3)) std::cout << x << " ";
    std::cout << "\n";

    std::cout << "  v | views::drop(6)         跳过前 6 个      : ";
    for (int x : v | std::views::drop(6)) std::cout << x << " ";
    std::cout << "\n";

    std::cout << "  v | views::reverse         倒序            : ";
    for (int x : v | std::views::reverse) std::cout << x << " ";
    std::cout << "\n";

    std::cout << "  v | views::take(4) | views::reverse : ";
    for (int x : v | std::views::take(4) | std::views::reverse) std::cout << x << " ";
    std::cout << "\n";

    std::cout << "  v | views::drop(3) | views::take(3) 切片: ";
    for (int x : v | std::views::drop(3) | std::views::take(3)) std::cout << x << " ";
    std::cout << "\n";

    std::cout << "\n=== C. 复合：先过滤再取前 N 个再求和 ===\n";
    auto pipe = v | std::views::filter([](int x) { return x > 3; })
                  | std::views::transform([](int x) { return x * x; })
                  | std::views::take(3);
    std::cout << "  v 中 >3 的数平方后取前 3 个: ";
    for (int x : pipe) std::cout << x << " ";
    std::cout << "\n";
        // 注意：filter 产生的 view 的 begin/end 是**不同类型**（不是 common_range），
        // 所以 accumulate(pipe.begin(), pipe.end(), 0) 编译不过，
        // 连 vector(pipe.begin(), pipe.end()) 也不行（同一个原因）。
        // 修法：用 ranges 算法（它们接受整个 range），或者手写循环。
    long long sum = 0;
    for (int x : pipe) sum += x;
    std::cout << "  求和 = " << sum << "（用范围 for 遍历 view）\n";
    std::cout << "  std::ranges::fold_left(pipe, 0LL, std::plus{}) = "
              << std::ranges::fold_left(pipe, 0LL, std::plus<>{}) << "\n";

    std::cout << "\n=== D. ranges 版的算法：不用写 .begin()/.end() ===\n";
    std::cout << "  std::ranges::sort(v, std::greater{}) 之后: ";
    std::vector<int> w{5, 2, 8, 1, 9};
    std::ranges::sort(w, std::greater<>{});
    for (int x : w) std::cout << x << " ";
    std::cout << "  <- 整个容器直接传进去\n";

    std::cout << "  std::ranges::count_if(v, pred) = "
              << std::ranges::count_if(v, [](int x) { return x > 3; }) << "\n";
    std::cout << "  std::ranges::max(w) = " << std::ranges::max(w) << "\n";
    std::cout << "  C++20 的 ranges 算法接受**整个范围**，且能防住\"迭代器来自不同容器\"的错。\n";

    std::cout << "\n=== E. 实战：过滤掉无效的传感器读数 ===\n";
    std::vector<Reading> readings{
        {100, true}, {200, false}, {300, true}, {400, false}, {500, true}
    };
    auto validValues = readings
        | std::views::filter([](const Reading& r) { return r.valid; })
        | std::views::transform([](const Reading& r) { return r.rawValue; });
    std::cout << "  有效读数: ";
    for (int x : validValues) std::cout << x << " ";
    std::cout << "\n";
    std::cout << "  全程没有中间 vector，没有 begin/end，读起来就是\"过滤 -> 取字段\"。\n";

    std::cout << "\n=== F. 注意事项 ===\n";
    std::cout << "  1) view 是**惰性**的：不遍历就不干活；但每次遍历都会重新计算一遍。\n";
    std::cout << "     take 结尾的 view 可以写成 vector<int> r(pipe.begin(), pipe.end())；\n";
    std::cout << "     但 filter 结尾的 view 不行（begin/end 类型不同），要手写循环收集。\n";
    std::cout << "  2) view 不拥有数据！底层容器销毁后 view 就悬垂了。\n";
    std::cout << "  3) 管道里的 lambda 不要有副作用（可能被调用 0 次或多次）。\n";
    std::cout << "  4) 嵌入式现实中很少用 ranges：编译产物大、头文件重、\n";
    std::cout << "     而且 M0+ 的 Flash 只有 64KB。手写 for 循环更合适。\n";

    std::cout << "\nmain 正常结束\n";
    return 0;
}
