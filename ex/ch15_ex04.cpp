// ch15_ex04.cpp —— std::move 不是"移动"，只是一次类型转换（cast）
#include <chrono>
#include <cstdio>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

// 一个 1MB 的负载，用来让"拷贝"和"移动"的差别在时间上看得出来
struct BigPayload {
    std::vector<char> bytes;
    BigPayload() : bytes(1024 * 1024, 'x') {}          // 1 MiB
};

int main()
{
    std::printf("--- 1) std::move 不产生任何动作 ---\n");
    {
        std::string s = "hello";
        std::printf("  move 之前 s = \"%s\"（内容还在）\n", s.c_str());
        std::string&& r = std::move(s);                 // 只是绑了个引用
        std::printf("  std::move(s) 之后 s = \"%s\"（内容依然在！）\n", s.c_str());
        std::printf("  r 指向的还是同一个对象：r.data() == s.data() ? %d\n",
                    r.data() == s.data());
    }

    std::printf("--- 2) 真正搬走内容的是\"用这个右值去构造新对象\" ---\n");
    {
        std::string s = "hello world, this is a long string";
        const char* before = s.data();
        std::string t = std::move(s);                   // 这里才调用移动构造
        std::printf("  源对象 s   = \"%s\"（被搬空了）\n", s.c_str());
        std::printf("  新对象 t   = \"%s\"\n", t.c_str());
        std::printf("  缓冲区指针是否被\"偷\"给了 t：%d\n", t.data() == before);
        std::printf("  ——移动构造就是偷指针，所以是 O(1)\n");
    }

    std::printf("--- 3) 从类型上看：std::move 返回的只是一个右值引用 ---\n");
    {
        int x = 1;
        std::printf("  decltype(std::move(x))           是 int&&  ? %d\n",
                    std::is_same_v<decltype(std::move(x)), int&&>);
        std::printf("  decltype(std::move(std::move(x)))还是 int&&  ? %d\n",
                    std::is_same_v<decltype(std::move(std::move(x))), int&&>);
        std::printf("  decltype(std::move(1))           是 int&&  ? %d\n",
                    std::is_same_v<decltype(std::move(1)), int&&>);

        const int cx = 1;
        std::printf("  const 对象的 move 结果是 const int&& ? %d\n",
                    std::is_same_v<decltype(std::move(cx)), const int&&>);
        std::printf("  注意：结果是 const 右值引用，绑不进 T&& 参数 —— 这就是\"const 对象移不动\"的原因\n");

                // 看一下 move 的典型实现（标准库就是这么写的）：
                // template <class T>
                // constexpr std::remove_reference_t<T>&& move(T&& t) noexcept {
                // return static_cast<std::remove_reference_t<T>&&>(t);
                // }
        int&& viaCast = static_cast<int&&>(x);          // 和 std::move(x) 完全等价
        std::printf("  static_cast<int&&>(x) 等价写法也能编过，值 = %d\n", viaCast);

        struct Probe {
            int v;
            explicit Probe(int i) : v(i) {}
            Probe(const Probe&) { std::printf("    [拷贝构造] 被调用\n"); }
            Probe(Probe&&)      { std::printf("    [移动构造] 被调用\n"); }
        };
        Probe p(1);
        std::printf("  用 std::move(p) 造新对象：\n");
        Probe q(std::move(p));
        std::printf("  q.v = %d\n", q.v);
    }

    std::printf("--- 4) std::move 的运行时开销：0 ---\n");
    {
        const int kLoop = 2000;
        BigPayload payload;

        auto t0 = std::chrono::steady_clock::now();
        for (int i = 0; i < kLoop; ++i) {
            BigPayload copy(payload);                  // 真拷贝 1 MiB
            (void)copy;
        }
        auto t1 = std::chrono::steady_clock::now();
        for (int i = 0; i < kLoop; ++i) {
            BigPayload moved(std::move(payload));      // 移动：只偷指针
            payload = std::move(moved);                // 再搬回来，继续下一轮
        }
        auto t2 = std::chrono::steady_clock::now();

        auto copy_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
        auto move_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
        std::printf("  拷贝 %d 次 1 MiB：%lld ms\n", kLoop, static_cast<long long>(copy_ms));
        std::printf("  移动 %d 次 1 MiB：%lld ms\n", kLoop, static_cast<long long>(move_ms));
        if (move_ms == 0) {
            std::printf("  移动快到测不出来（0 ms）—— 它只是改了几个指针\n");
        }
        std::printf("  结论：std::move 本身不搬数据，搬数据的是移动构造/移动赋值\n");
    }

    return 0;
}
