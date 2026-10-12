// 附录B 探针 8：求值顺序未指定、迭代器失效、数据竞争
#include <cstdio>
#include <vector>
#include <thread>
#include <atomic>
#include <string>

static int gSharedCounter = 0;                  // 非原子：多线程同时改就是数据竞争
static std::atomic<int> gAtomicCounter{0};

int main() {
    std::printf("=== 1. 同一表达式里多次修改同一个变量（无序列点/未序列化）===\n");
    {
        int i = 5;
        int r = i++ + i++;                      // C++17 起仍是 UB（未序列化的两次修改）
        std::printf("  i++ + i++ 的结果 = %d, i = %d （结果不可依赖）\n", r, i);
    }
    {
        int i = 0;
        int a[4] = {10, 20, 30, 40};
        int v = a[i] + i++;                     // 同一表达式中读 a[i] 又改 i
        std::printf("  a[i] + i++ = %d, i = %d （不同编译器/优化级别结果不同）\n", v, i);
    }
    {
        std::vector<int> v{1, 2, 3};
        std::size_t k = 0;
        v[k] = k++;                             // C++17 起：不确定但不再是 UB
        std::printf("  v[k] = k++ 后 v = %d %d %d\n", v[0], v[1], v[2]);
    }

    std::printf("=== 2. 函数实参的求值顺序未指定 ===\n");
    {
                // MSVC 习惯从右往左求值，GCC 常常从左往右 —— 不要依赖
        auto show = [](int a, int b, int c) {
            std::printf("  实参依次是 %d %d %d\n", a, b, c);
        };
        int n = 0;
        show(n++, n++, n++);                    // 实参之间无序列关系
        std::printf("  调用后 n = %d\n", n);
    }

    std::printf("=== 3. 迭代器失效 ===\n");
    {
        std::vector<int> v{1, 2, 3};
        auto it = v.begin();
        std::printf("  扩容前 *it = %d, capacity = %zu\n", *it, v.capacity());
        for (int k = 0; k < 10; ++k) v.push_back(k);   // 触发扩容，it 失效
        std::printf("  扩容后 capacity = %zu。it 已经在旧内存上 -> 解引用它是 UB\n", v.capacity());
        std::printf("  这里不真的去解引用 it，避免当场崩\n");
    }
    {
        std::vector<int> v{1, 2, 3, 4};
        for (auto it = v.begin(); it != v.end(); ) {
            if (*it % 2 == 0) it = v.erase(it);        // erase 返回下一个有效迭代器
            else              ++it;
        }
        std::printf("  erase 的正确写法（接住返回值）后 v.size() = %zu\n", v.size());
    }

    std::printf("=== 4. 数据竞争（data race）===\n");
    {
        gSharedCounter = 0;
        auto bump = [] { for (int k = 0; k < 200000; ++k) ++gSharedCounter; };
        std::thread t1(bump), t2(bump);
        t1.join(); t2.join();
        std::printf("  两个线程各加 200000 次，普通 int 结果 = %d（应为 400000）\n", gSharedCounter);

        auto bumpAtomic = [] { for (int k = 0; k < 200000; ++k) ++gAtomicCounter; };
        std::thread t3(bumpAtomic), t4(bumpAtomic);
        t3.join(); t4.join();
        std::printf("  换成 std::atomic<int> 结果 = %d（应为 400000）\n", gAtomicCounter.load());
    }

    std::printf("=== 5. 迭代器/指针指向的容器被销毁 ===\n");
    {
        std::string* p = nullptr;
        {
            std::string s = "临时";
            p = &s;
        }                                        // s 销毁，p 悬空
        std::printf("  悬空 string 指针 size() = %zu （UB；这里只读长度不再解引用内容）\n", p->size());
    }
    return 0;
}
