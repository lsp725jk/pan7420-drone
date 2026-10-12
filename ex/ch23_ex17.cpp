// ch23_ex17.cpp —— 例 23-17：容器的连续性——vector 为什么几乎总是比 list 快
// 教科书说"链表插入是 O(1)，数组插入是 O(n)"，
// 但真实机器上，**遍历**一个 100 万元的 vector 比 list 快好几倍，
// 因为 vector 的内存是连续的，每次预取都能命中。
#include <chrono>
#include <cstdio>
#include <list>
#include <numeric>
#include <vector>

constexpr int N = 1000000;

int main() {
    std::vector<int> v(N);
    std::iota(v.begin(), v.end(), 0);
    std::list<int> l(v.begin(), v.end());

    std::printf("元素个数 = %d\n", N);
    std::printf("sizeof(std::vector<int>) = %zu 字节（3 个指针）\n", sizeof(std::vector<int>));
    std::printf("sizeof(std::list<int>)   = %zu 字节（也是 2~3 个指针）\n", sizeof(std::list<int>));
    std::printf("但每个 list 节点额外要 2 个指针 + 堆分配头 -> 每个元素实际约 %zu 字节\n",
                sizeof(int) + 2 * sizeof(void*) + 16);

    const int ROUNDS = 200;

        // ---- 遍历 vector ----
    const auto t0 = std::chrono::steady_clock::now();
    long long s1 = 0;
    for (int r = 0; r < ROUNDS; ++r) {
        for (int x : v) {
            s1 += x;
        }
    }
    const auto t1 = std::chrono::steady_clock::now();

        // ---- 遍历 list ----
    long long s2 = 0;
    for (int r = 0; r < ROUNDS; ++r) {
        for (int x : l) {
            s2 += x;
        }
    }
    const auto t2 = std::chrono::steady_clock::now();

    const double ms_v = std::chrono::duration<double, std::milli>(t1 - t0).count();
    const double ms_l = std::chrono::duration<double, std::milli>(t2 - t1).count();
    std::printf("\n遍历 %d 轮：\n", ROUNDS);
    std::printf("  vector : %8.2f ms  (%.2f ns/元素)\n", ms_v, ms_v * 1e6 / ((double)N * ROUNDS));
    std::printf("  list   : %8.2f ms  (%.2f ns/元素)   慢 %.1f 倍\n", ms_l,
                ms_l * 1e6 / ((double)N * ROUNDS), ms_l / ms_v);
    std::printf("  校验和 %lld / %lld\n", s1, s2);

        // ---- 中间插入：这里 list 才扳回一城 ----
    {
        std::vector<int> vv(N, 0);
        const auto t3 = std::chrono::steady_clock::now();
        vv.insert(vv.begin() + N / 2, 42);
        const auto t4 = std::chrono::steady_clock::now();
        std::list<int> ll(N, 0);
        auto it = ll.begin();
        std::advance(it, N / 2);
        const auto t5 = std::chrono::steady_clock::now();
        ll.insert(it, 42);
        const auto t6 = std::chrono::steady_clock::now();
        std::printf("\n在正中间插入一个元素：\n");
        std::printf("  vector : %.1f us （要搬走 %d 个元素）\n",
                    std::chrono::duration<double, std::micro>(t4 - t3).count(), N / 2);
        std::printf("  list   : %.1f us （但要先走 %d 步找位置，这步就已经很贵了）\n",
                    std::chrono::duration<double, std::micro>(t6 - t5).count(), N / 2);
    }

    std::printf("\n结论（Bjarne Stroustrup 的经典实验也是这个结论）：\n");
    std::printf("  - 默认用 vector。它的连续内存让 CPU 预取器工作得很好。\n");
    std::printf("  - 只有【频繁在已知位置插入删除 + 元素很大】时才考虑 list。\n");
    std::printf("  - 元素少（几十个）时，vector 几乎总是更快，别想太多。\n");

        // 坑 1：vector 的迭代器在扩容后**全部失效**，list 的不会。
        // 这是 vector 真正的代价，不是速度。
        // 坑 2：vector<bool> 是特化，不是真正的容器，别当数组用。
        // 坑 3：std::deque 介于两者之间，头尾插入 O(1) 且分段连续，
        // 适合做队列——但如果只在一端操作，vector + reserve 仍然更划算。
    return 0;
}
