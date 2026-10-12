// ch17_ex02.cpp —— 五种序列容器的取舍：vector / array / deque / list / forward_list
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdio>
#include <deque>
#include <forward_list>
#include <list>
#include <vector>

int main()
{
    std::printf("--- 1) vector：连续内存，随机访问 O(1)，尾部增删均摊 O(1) ---\n");
    {
        std::vector<int> v{ 1, 2, 3, 4, 5 };
        std::printf("    size=%zu capacity=%zu\n", v.size(), v.capacity());
        std::printf("    v[3]=%d（O(1)），&v[0]=%p，&v[4]-&v[0]=%td\n",
                    v[3], static_cast<const void*>(v.data()),
                    &v[4] - &v[0]);
        std::printf("    元素在内存里连续 -> 对 CPU 缓存最友好（这就是它默认最快的根本原因）\n");
        std::printf("    中间插入/删除是 O(n)（要搬后面所有元素）\n");
        v.insert(v.begin(), 0);
        std::printf("    insert 到头部后：");
        for (int x : v) std::printf("%d ", x);
        std::printf("\n");
    }

    std::printf("--- 2) array：编译期固定大小，无堆分配 ---\n");
    {
        std::array<int, 5> a{ 1, 2, 3, 4, 5 };
        std::printf("    size()=%zu（自己知道多长，不像 C 数组）\n", a.size());
        std::printf("    sizeof=%zu，没有任何额外开销（就是一个包了数组的结构）\n", sizeof(a));
        std::printf("    a.at(1)=%d, a.front()=%d, a.back()=%d\n", a.at(1), a.front(), a.back());
        std::printf("    支持整体赋值：");
        std::array<int, 5> b = a;
        b[0] = 99;
        std::printf("b = a; b[0]=99 之后 a[0]=%d\n", a[0]);
        std::printf("    能和 STL 算法配合：std::sort(a.begin(), a.end())\n");
        std::sort(a.begin(), a.end(), [](int x, int y) { return x > y; });
        std::printf("    降序排序后：");
        for (int x : a) std::printf("%d ", x);
        std::printf("\n");
    }

    std::printf("--- 3) deque：双端队列，两端增删都是 O(1) ---\n");
    {
        std::deque<int> d{ 2, 3, 4 };
        d.push_front(1);          // vector 做不到 O(1)
        d.push_back(5);
        std::printf("    push_front(1) + push_back(5) 后：");
        for (int x : d) std::printf("%d ", x);
        std::printf("\n");
        std::printf("    d[2]=%d（也支持 O(1) 随机访问）\n", d[2]);
        std::printf("    代价：内存是分块（chunk）的，不是一整块连续\n");
        std::printf("    典型用途：需要一个\"先进先出\"的队列（任务队列、串口缓冲）\n");
    }

    std::printf("--- 4) list：双向链表，任意位置插入/删除 O(1) ---\n");
    {
        std::list<int> l{ 1, 2, 3, 4, 5 };
        auto it = l.begin();
        std::advance(it, 2);              // 走到第 3 个元素（O(n)，因为 list 不能随机跳）
        l.insert(it, 99);                 // 在这里插入：O(1)，不搬移任何元素
        std::printf("    在中间插入 99 后：");
        for (int x : l) std::printf("%d ", x);
        std::printf("\n");
        std::printf("    list 没有 operator[]（不能 l[2]），要访问第 n 个得走 n 步\n");
        std::printf("    每个元素额外要存两个指针（前驱+后继）-> 内存开销大、缓存不友好\n");
        std::printf("    list 有自己的 sort（成员函数），不能用 std::sort：\n");
        l.sort([](int a, int b) { return a > b; });
        std::printf("    l.sort() 降序后：");
        for (int x : l) std::printf("%d ", x);
        std::printf("\n");
    }

    std::printf("--- 5) forward_list：单向链表，只往前遍历 ---\n");
    {
        std::forward_list<int> f{ 1, 2, 3 };
        f.push_front(0);
        std::printf("    push_front(0) 后：");
        for (int x : f) std::printf("%d ", x);
        std::printf("\n");
        std::printf("    只有 push_front / insert_after（没有 push_back、没有 size()）\n");
        std::printf("    每个元素只存一个指针 -> 比 list 更省内存，但能力也更弱\n");
        std::printf("    实用场景：嵌入式里内存极紧张、只需要前向遍历的链表\n");
    }

    std::printf("--- 6) 实测：尾部插入的性能差别（vector vs list，各 20000 个）---\n");
    {
        const int N = 20000;
        auto t0 = std::chrono::steady_clock::now();
        std::vector<int> v;
        for (int i = 0; i < N; ++i) v.push_back(i);
        auto t1 = std::chrono::steady_clock::now();
        std::list<int> l;
        for (int i = 0; i < N; ++i) l.push_back(i);
        auto t2 = std::chrono::steady_clock::now();

        auto us = [](auto a, auto b) {
            return std::chrono::duration_cast<std::chrono::microseconds>(b - a).count();
        };
        std::printf("    vector push_back %d 次：%lld us\n", N, (long long)us(t0, t1));
        std::printf("    list   push_back %d 次：%lld us\n", N, (long long)us(t1, t2));
        std::printf("    list 每次都要申请一个节点 -> 慢很多（还受分配器影响）\n");
    }

    std::printf("--- 7) 复杂度速查 ---\n");
    std::printf("                 访问[i]  头部插入  尾部插入  中间插入  查找\n");
    std::printf("    vector       O(1)     O(n)      O(1)*     O(n)      O(n)\n");
    std::printf("    array        O(1)     不支持    O(1)*     不支持    O(n)\n");
    std::printf("    deque        O(1)     O(1)      O(1)      O(n)      O(n)\n");
    std::printf("    list         O(n)     O(1)      O(1)      O(1)      O(n)\n");
    std::printf("    forward_list O(n)     O(1)      不支持    O(1)      O(n)\n");
    std::printf("    * 均摊；vector 尾部插入偶尔要扩容。array 的\"尾部\"指末尾之后，实际不支持\n");
    std::printf("    经验：先用 vector；确实需要频繁头插用 deque；\n");
    std::printf("          确实需要频繁在已知位置插删、且元素很大，才考虑 list\n");

    return 0;
}
