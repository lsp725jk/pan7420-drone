// ch22_ex35.cpp —— 例 22-35：为什么说"模板是零开销抽象"
// 零开销 = 你不用为没用到的东西付费，用了的东西也不比手写差。
// 这里用三件事来验证：内存布局、编译期计算、算法性能。
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <vector>

// C 数组版
void fill_c(int* a, int n, int v) {
    for (int i = 0; i < n; ++i) {
        a[i] = v + i;
    }
}
// std::array 版：写法更"高级"，但生成的东西一样
void fill_cpp(std::array<int, 8>& a, int v) {
    for (std::size_t i = 0; i < a.size(); ++i) {
        a[i] = v + static_cast<int>(i);
    }
}

void bubble_sort(int* a, int n) {
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - 1 - i; ++j) {
            if (a[j] > a[j + 1]) {
                const int t = a[j];
                a[j] = a[j + 1];
                a[j + 1] = t;
            }
        }
    }
}

int main() {
        // ---- 1) 内存布局：std::array 一点额外开销都没有 ----
    static_assert(sizeof(std::array<int, 8>) == sizeof(int[8]), "std::array 不许多占一个字节");
    static_assert(alignof(std::array<int, 8>) == alignof(int[8]), "对齐也必须一样");
    std::array<int, 8> a{};
    std::printf("sizeof(std::array<int,8>) = %zu，sizeof(int[8]) = %zu\n", sizeof(a), sizeof(int[8]));
    std::printf("a.data() == &a[0] ? %d\n", (int)(a.data() == &a[0]));
    std::printf("a.size() 是编译期常量 : %zu\n", a.size());

        // ---- 2) 编译期计算：运行期只剩一个立即数 ----
    constexpr std::array<int, 6> kTable = [] {
        std::array<int, 6> t{};
        for (int i = 0; i < 6; ++i) {
            t[i] = i * i * i;
        }
        return t;
    }();
    static_assert(kTable[5] == 125, "立方表在编译期就算好了");
    std::printf("编译期算好的立方表: ");
    for (int v : kTable) {
        std::printf("%d ", v);
    }
    std::printf("\n");
    std::printf("表在 .rdata 里，运行期一次乘法都不用做\n");

        // ---- 3) 算法性能：std::sort 对 手写冒泡 ----
    const int N = 20000;
    std::vector<int> v1(N), v2(N);
    for (int i = 0; i < N; ++i) {
        const int x = (i * 7919) % N;  // 固定的伪随机序列，保证两次数据一样
        v1[i] = x;
        v2[i] = x;
    }

    const auto t0 = std::chrono::steady_clock::now();
    bubble_sort(v1.data(), N);
    const auto t1 = std::chrono::steady_clock::now();
    std::sort(v2.begin(), v2.end());
    const auto t2 = std::chrono::steady_clock::now();

    const double ms_bubble = std::chrono::duration<double, std::milli>(t1 - t0).count();
    const double ms_std = std::chrono::duration<double, std::milli>(t2 - t1).count();

    std::printf("冒泡排序 %d 个元素 : %.1f ms\n", N, ms_bubble);
    std::printf("std::sort 同样数据 : %.1f ms  （快 %.0f 倍）\n", ms_std, ms_bubble / ms_std);
    std::printf("两者结果一致: %d\n", (int)(v1 == v2));

        // 想说的话：
        // std::sort 内部是模板 + 迭代器 + 比较器，看上去"抽象层层叠叠"，
        // 但它把比较和交换都内联了，没有任何虚函数或函数指针开销。
        // 这就是"零开销抽象"：抽象只存在于源码里，不存在于机器码里。
        // 注意：这里同样是 /Od（不优化），所以两者都慢；
        // /O2 下 std::sort 的优势更明显。
    return 0;
}
