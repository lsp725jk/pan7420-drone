// ch11_ex15.cpp —— 范围 for：C 的 for 循环终于可以退休了
// 编译：check.bat ch11_ex15.cpp c++latest
#include <cstdio>

int main()
{
    int a[5] = { 1, 2, 3, 4, 5 };

        // ---- 写法一：C 风格下标 ----
    std::printf("C 下标    : ");
    for (int i = 0; i < 5; ++i) std::printf("%d ", a[i]);
    std::printf("\n");

        // ---- 写法二：范围 for，拷贝每个元素（只读时浪费）----
    std::printf("范围for 值: ");
    for (int v : a) std::printf("%d ", v);
    std::printf("\n");

        // ---- 写法三：auto 推导元素类型 ----
    std::printf("auto 值   : ");
    for (auto v : a) std::printf("%d ", v);
    std::printf("\n");

        // ---- 写法四：auto& 改元素 ----
    for (auto& v : a) v *= 10;              // 就地乘 10
    std::printf("auto& 改后: ");
    for (const auto& v : a) std::printf("%d ", v);   // 只读遍历用 const auto&
    std::printf("\n");

        // ---- 陷阱：auto v（不带 &）改不了原数组 ----
    for (auto v : a) v = 0;                 // 改的是副本
    std::printf("auto 值改后: %d %d %d %d %d（原数组没变）\n",
                a[0], a[1], a[2], a[3], a[4]);

        // ---- 只想读又想省拷贝：const auto& ----
    long long sum = 0;
    for (const auto& v : a) sum += v;
    std::printf("求和 = %lld\n", sum);

    return 0;
}
