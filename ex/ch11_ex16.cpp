// ch11_ex16.cpp —— 范围 for 的变式与坑
// 编译：check.bat ch11_ex16.cpp c++latest
#include <cstdio>
#include <initializer_list>
#include <vector>

// 任何有 begin()/end() 的类型都能用范围 for
struct Range {
    int lo, hi;
    struct It {                       // 最小可用的迭代器
        int v;
        int& operator*() { static int tmp; tmp = v; return tmp; }
        It& operator++() { ++v; return *this; }
        bool operator!=(const It& o) const { return v != o.v; }
    };
    It begin() const { return It{ lo }; }
    It end()   const { return It{ hi }; }
};

int main()
{
        // ---- 变式 1：遍历 vector ----
    std::vector<int> v{ 10, 20, 30 };
    std::printf("1) vector: ");
    for (int x : v) std::printf("%d ", x);
    std::printf("\n");

        // ---- 变式 2：遍历初始化列表 ----
    std::printf("2) 初始化列表: ");
    for (int x : { 7, 8, 9 }) std::printf("%d ", x);
    std::printf("\n");

        // ---- 变式 3：遍历自定义类型（只要有 begin/end）----
    std::printf("3) 自定义 Range(2..6): ");
    for (int x : Range{ 2, 6 }) std::printf("%d ", x);
    std::printf("\n");

        // ---- 变式 4：C++20 起范围 for 可以带初始化语句 ----
        // 作用：把"要遍历的东西"算出来就地在循环里用，不污染外层作用域
    std::printf("4) C++20 带初始化的范围 for: ");
    for (int base = 100; int x : { 1, 2, 3 }) std::printf("%d ", base + x);
    std::printf("\n");

        // ---- 坑 1：遍历过程中改容器大小（对 vector 是未定义行为）----
        // for (int x : v) { if (x == 20) v.push_back(99); }   // 千万不要这么写

        // ---- 坑 2：遍历时删除元素也是坑（正确做法详见第 14 章）----
    std::printf("5) 遍历期间增删元素属于未定义行为，别这么干\n");

        // ---- 坑 3：范围 for 里的变量作用域只在循环体内 ----
    for (int x : v) { (void)x; }
        // std::printf("%d", x);    // 错误：x 未定义

    return 0;
}
