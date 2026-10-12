// ch17_ex07.cpp —— 常用算法：sort / find / find_if / transform / accumulate / count_if / all_of
#include <algorithm>
#include <cstdio>
#include <functional>
#include <iterator>
#include <numeric>
#include <string>
#include <vector>

struct Student {
    std::string name;
    int         score;
};

int main()
{
    std::printf("--- 1) sort：默认升序，可以给比较函数 ---\n");
    {
        std::vector<int> v{ 5, 2, 8, 1, 9, 3 };
        std::printf("    原始：        ");
        for (int x : v) std::printf("%d ", x);
        std::printf("\n");

        std::sort(v.begin(), v.end());
        std::printf("    默认升序：    ");
        for (int x : v) std::printf("%d ", x);
        std::printf("\n");

        std::sort(v.begin(), v.end(), std::greater<int>());
        std::printf("    greater<int>：");
        for (int x : v) std::printf("%d ", x);
        std::printf("\n");

        std::sort(v.begin(), v.end(), [](int a, int b) { return a > b; });
        std::printf("    lambda 降序： ");
        for (int x : v) std::printf("%d ", x);
        std::printf("（和 greater 等价，但更灵活）\n");

        std::vector<Student> st{ { "Tom", 85 }, { "Amy", 92 }, { "Bob", 78 } };
        std::sort(st.begin(), st.end(), [](const Student& a, const Student& b) {
            return a.score > b.score;              // 按分数降序
        });
        std::printf("    按分数降序：  ");
        for (const auto& s : st) std::printf("%s=%d ", s.name.c_str(), s.score);
        std::printf("\n");

        std::printf("    sort 要求**随机访问迭代器**，所以 list 不能用\n");
        std::printf("    stable_sort：相等元素的相对顺序保持（sort 不保证）\n");
    }

    std::printf("--- 2) find / find_if：找元素 ---\n");
    {
        std::vector<int> v{ 10, 20, 30, 40, 50 };

        auto it = std::find(v.begin(), v.end(), 30);
        std::printf("    find(30)     -> 位置 %td，值 %d\n", it - v.begin(), *it);

        auto it2 = std::find(v.begin(), v.end(), 99);
        std::printf("    find(99)     -> 找到了吗？%d（返回 end()）\n", it2 != v.end() ? 1 : 0);

        auto it3 = std::find_if(v.begin(), v.end(), [](int x) { return x > 25; });
        std::printf("    find_if(>25) -> 第一个是 %d（位置 %td）\n", *it3, it3 - v.begin());

        auto it4 = std::find_if(v.begin(), v.end(), [](int x) { return x > 999; });
        std::printf("    find_if(>999)-> 找到了吗？%d\n", it4 != v.end() ? 1 : 0);

        std::printf("    检查返回值是必须的：*v.end() 是 UB\n");
    }

    std::printf("--- 3) transform：对每个元素做变换 ---\n");
    {
        std::vector<int> src{ 1, 2, 3, 4, 5 };
        std::vector<int> dst(src.size());          // 注意：必须**先开好空间**

        std::transform(src.begin(), src.end(), dst.begin(), [](int x) { return x * x; });
        std::printf("    src:      ");
        for (int x : src) std::printf("%d ", x);
        std::printf("\n    transform 平方后 dst: ");
        for (int x : dst) std::printf("%d ", x);
        std::printf("\n");

                // 原地变换
        std::transform(src.begin(), src.end(), src.begin(), [](int x) { return -x; });
        std::printf("    原地取负后 src:       ");
        for (int x : src) std::printf("%d ", x);
        std::printf("\n");

                // 两路输入
        std::vector<int> a{ 1, 2, 3 }, b{ 10, 20, 30 }, c(3);
        std::transform(a.begin(), a.end(), b.begin(), c.begin(),
                       [](int x, int y) { return x + y; });
        std::printf("    两路相加 c:           ");
        for (int x : c) std::printf("%d ", x);
        std::printf("\n");

        std::printf("    坑：dst 必须已经够大。用 back_inserter 可以自动 push_back：\n");
        std::vector<int> dst2;
        std::transform(a.begin(), a.end(), std::back_inserter(dst2), [](int x) { return x * 100; });
        std::printf("    back_inserter 版本：  ");
        for (int x : dst2) std::printf("%d ", x);
        std::printf("\n");
    }

    std::printf("--- 4) accumulate：累加（注意它在 <numeric> 里）---\n");
    {
        std::vector<int> v{ 1, 2, 3, 4, 5 };
        std::printf("    accumulate 求和  = %d\n", std::accumulate(v.begin(), v.end(), 0));
        std::printf("    初始值改成 100   = %d\n", std::accumulate(v.begin(), v.end(), 100));
        std::printf("    accumulate 求积  = %d\n",
                    std::accumulate(v.begin(), v.end(), 1, std::multiplies<int>()));
        std::printf("    用 lambda 拼字符串：%s\n",
                    std::accumulate(v.begin() + 1, v.end(), std::to_string(v[0]),
                                    [](const std::string& acc, int x) {
                                        return acc + "-" + std::to_string(x);
                                    }).c_str());
        std::printf("    坑：初始值类型决定累加类型\n");
        std::printf("      accumulate(v.begin(), v.end(), 0)      -> int\n");
        std::printf("      accumulate(v.begin(), v.end(), 0.0)    -> double\n");
        std::printf("      实测：%.2f\n", std::accumulate(v.begin(), v.end(), 0.0) / v.size());
    }

    std::printf("--- 5) count / count_if / all_of / any_of / none_of ---\n");
    {
        std::vector<int> v{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
        std::printf("    count(5)         = %td\n", std::count(v.begin(), v.end(), 5));
        std::printf("    count_if(偶数)   = %td\n",
                    std::count_if(v.begin(), v.end(), [](int x) { return x % 2 == 0; }));
        std::printf("    all_of(>0)       = %d\n",
                    std::all_of(v.begin(), v.end(), [](int x) { return x > 0; }) ? 1 : 0);
        std::printf("    all_of(>5)       = %d\n",
                    std::all_of(v.begin(), v.end(), [](int x) { return x > 5; }) ? 1 : 0);
        std::printf("    any_of(>9)       = %d\n",
                    std::any_of(v.begin(), v.end(), [](int x) { return x > 9; }) ? 1 : 0);
        std::printf("    any_of(>99)      = %d\n",
                    std::any_of(v.begin(), v.end(), [](int x) { return x > 99; }) ? 1 : 0);
        std::printf("    none_of(<0)      = %d\n",
                    std::none_of(v.begin(), v.end(), [](int x) { return x < 0; }) ? 1 : 0);
        std::printf("    这三个配合 lambda 比手写循环清楚得多，而且不会写错边界\n");
    }

    std::printf("--- 6) min/max_element、reverse、fill、iota ---\n");
    {
        std::vector<int> v{ 3, 1, 4, 1, 5, 9, 2, 6 };
        auto mn = std::min_element(v.begin(), v.end());
        auto mx = std::max_element(v.begin(), v.end());
        std::printf("    min=%d（位置 %td），max=%d（位置 %td）\n",
                    *mn, mn - v.begin(), *mx, mx - v.begin());

        std::reverse(v.begin(), v.end());
        std::printf("    reverse 后：");
        for (int x : v) std::printf("%d ", x);
        std::printf("\n");

        std::fill(v.begin(), v.begin() + 3, 0);
        std::printf("    fill 前 3 个为 0：");
        for (int x : v) std::printf("%d ", x);
        std::printf("\n");

        std::vector<int> seq(5);
        std::iota(seq.begin(), seq.end(), 10);      // 10, 11, 12, ...
        std::printf("    iota 从 10 开始：");
        for (int x : seq) std::printf("%d ", x);
        std::printf("\n");
    }

    std::printf("--- 7) 算法能用的前提：迭代器类别足够强 ---\n");
    std::printf("    sort / reverse / min_element 等需要**随机访问**迭代器\n");
    std::printf("      -> vector / deque / array 可以，list / map 不行\n");
    std::printf("    find / count / for_each 只需要**输入/前向**迭代器\n");
    std::printf("      -> 所有容器都可以\n");
    std::printf("    list 想排序用成员函数 l.sort()；map 本身就是有序的，不用排\n");

    return 0;
}
