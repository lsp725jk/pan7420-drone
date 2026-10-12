// ch17_ex10.cpp —— emplace_back vs push_back、reserve 与性能
#include <chrono>
#include <cstdio>
#include <string>
#include <utility>
#include <vector>

// 一个"能观察到构造/拷贝/移动"的类型
struct Item {
    std::string name;
    int         value;

    explicit Item(std::string n, int v) : name(std::move(n)), value(v)
    {
        std::printf("      [构造]      name=%s\n", name.c_str());
    }
    Item(const Item& o) : name(o.name), value(o.value)
    {
        std::printf("      [拷贝构造]  name=%s\n", name.c_str());
    }
    Item(Item&& o) noexcept : name(std::move(o.name)), value(o.value)
    {
        std::printf("      [移动构造]  name=%s\n", name.c_str());
    }
    ~Item() = default;
};

int main()
{
    std::printf("--- 1) push_back(临时对象)：构造 + 移动 ---\n");
    {
        std::vector<Item> v;
        v.reserve(4);
        std::printf("    v.push_back(Item(\"A\", 1));\n");
        v.push_back(Item("A", 1));
        std::printf("    共 1 次构造 + 1 次移动构造（临时对象被搬进容器）\n");
    }

    std::printf("--- 2) emplace_back(参数)：只有构造 ---\n");
    {
        std::vector<Item> v;
        v.reserve(4);
        std::printf("    v.emplace_back(\"B\", 2);\n");
        v.emplace_back("B", 2);
        std::printf("    只有 1 次构造，没有移动 —— 这就是 emplace 的价值\n");
    }

    std::printf("--- 3) push_back 一个已存在的对象：拷贝 ---\n");
    {
        std::vector<Item> v;
        v.reserve(4);
        Item exist("C", 3);
        std::printf("    v.push_back(exist);      // 左值 -> 拷贝\n");
        v.push_back(exist);
        std::printf("    v.push_back(std::move(exist));  // 右值 -> 移动\n");
        v.push_back(std::move(exist));
    }

    std::printf("--- 4) 【必做实验】不 reserve：观察扩容搬移 ---\n");
    {
        std::vector<Item> v;                 // 不 reserve
        for (int i = 0; i < 5; ++i) {
            std::printf("    -- emplace_back(#%d)，size=%zu capacity=%zu --\n",
                        i, v.size(), v.capacity());
            v.emplace_back("x" + std::to_string(i), i);
        }
        std::printf("    最终 size=%zu capacity=%zu\n", v.size(), v.capacity());
        std::printf("    看到那些\"移动构造\"了吗？每次扩容都要把已有元素搬走\n");
    }

    std::printf("--- 5) 对比：先 reserve ---\n");
    {
        std::vector<Item> v;
        v.reserve(5);
        std::printf("    reserve(5) 之后 capacity=%zu\n", v.capacity());
        for (int i = 0; i < 5; ++i) {
            std::printf("    -- emplace_back(#%d)，size=%zu capacity=%zu --\n",
                        i, v.size(), v.capacity());
            v.emplace_back("y" + std::to_string(i), i);
        }
        std::printf("    全程没有出现任何\"移动构造\"（除了临时 string，那是另一个故事）\n");
    }

    std::printf("--- 6) 实测：reserve 与否的时间差（元素是较大的结构）---\n");
    {
        struct Big {
            char data[256];
            int  id;
            explicit Big(int i) : data{}, id(i) {}
            Big(const Big& o) : id(o.id) { for (int k = 0; k < 256; ++k) data[k] = o.data[k]; }
            Big(Big&& o) noexcept : id(o.id) { for (int k = 0; k < 256; ++k) data[k] = o.data[k]; }
        };
        const int N = 20000;
        std::vector<Big> a;
        std::vector<Big> b;
        b.reserve(N);

        auto t0 = std::chrono::steady_clock::now();
        for (int i = 0; i < N; ++i) a.emplace_back(i);
        auto t1 = std::chrono::steady_clock::now();
        for (int i = 0; i < N; ++i) b.emplace_back(i);
        auto t2 = std::chrono::steady_clock::now();

        auto us = [](auto x, auto y) {
            return std::chrono::duration_cast<std::chrono::microseconds>(y - x).count();
        };
        std::printf("    不 reserve：%lld us（capacity=%zu）\n", (long long)us(t0, t1), a.capacity());
        std::printf("    先 reserve：%lld us（capacity=%zu）\n", (long long)us(t1, t2), b.capacity());
        std::printf("    省下的是扩容时搬移已有元素的时间\n");
        std::printf("    （本机数字，仅供感受量级；元素越大、N 越大，差距越明显）\n");
    }

    std::printf("--- 7) emplace_back 的坑：隐式转换不再被检查 ---\n");
    {
        std::vector<std::string> v;
        v.emplace_back(5, 'a');        // 调的是 string(size_t, char) -> "aaaaa"
        std::printf("    v.emplace_back(5, 'a') 得到 \"%s\"（长度 %zu）\n",
                    v[0].c_str(), v[0].size());
        std::printf("    如果写 v.push_back(5, 'a') 直接编译错误（push_back 只收一个参数）\n");
        std::printf("    另一个坑：emplace 会调用 explicit 构造函数，push_back 不会 —— \n");
        std::printf("              这既是优点（更少临时对象），也可能让不该编过的代码编过\n");
    }

    std::printf("--- 8) 选择建议 ---\n");
    std::printf("    已知大致规模时：先 reserve（这是最划算的一步优化）\n");
    std::printf("    能用 emplace_back 就用（省一次移动）\n");
    std::printf("    需要一个个传入多个构造参数时：必须用 emplace_back\n");
        std::printf("    要放\"已经存在的对象\"时：push_back(x) 或 push_back(std::move(x))\n");
    return 0;
}
