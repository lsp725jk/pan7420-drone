// ch14_ex12.cpp —— 异常安全的三个高频坑
#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

struct Res {
    int id;
    Res() : id(0) {}                              // 数组版 make_unique 需要默认构造函数
    explicit Res(int i) : id(i) { std::printf("  Res(%d) 构造\n", id); }
    ~Res() { std::printf("  Res(%d) 析构\n", id); }
};

// 坑 A：裸指针 + 会抛异常的第二步 -> 第一步申请的内存漏掉
void raw_leak()
{
    Res* p = new Res(1);
    std::printf("  抛异常前 p = %p\n", static_cast<void*>(p));
    throw std::runtime_error("第二步失败");     // p 永远不会被 delete
}

// 正确做法：先交给 unique_ptr，再抛
void safe_no_leak()
{
    auto p = std::make_unique<Res>(2);
    std::printf("  抛异常前 p.get() = %p\n", static_cast<void*>(p.get()));
    throw std::runtime_error("第二步失败");     // 栈展开，p 自动 delete
}

int main()
{
    std::printf("--- 坑 A：裸指针 + 抛异常 = 泄漏 ---\n");
    try { raw_leak(); }
    catch (const std::exception& e) {
        std::printf("  捕获: %s（注意上面没有 Res(1) 的析构）\n", e.what());
    }

    std::printf("--- 修法：交给智能指针 ---\n");
    try { safe_no_leak(); }
    catch (const std::exception& e) {
        std::printf("  捕获: %s（Res(2) 已经析构了）\n", e.what());
    }

    std::printf("--- 坑 B：new[] 必须配 delete[]，智能指针要用数组特化版 ---\n");
    {
        auto arr = std::make_unique<Res[]>(3);      // 注意：参数是"元素个数"
        std::printf("  arr[0].id = %d, arr[2].id = %d\n", arr[0].id, arr[2].id);
        std::printf("  离开作用域会析构 3 次 —— 等价于 delete[]\n");
    }

    std::printf("--- 坑 C：容器里存裸指针，异常时漏一片 ---\n");
    {
        std::vector<Res*> raw;
        try {
            for (int i = 10; i < 13; ++i) {
                raw.push_back(new Res(i));
                if (i == 11) {
                    throw std::runtime_error("第 2 个失败");
                }
            }
        }
        catch (const std::exception& e) {
            std::printf("  捕获: %s\n", e.what());
        }
        std::printf("  raw 里有 %zu 个裸指针，一个都没释放（vector 不删元素）\n", raw.size());
        for (Res* p : raw) delete p;                // 只能手工补
        std::printf("  手工补 delete 完成\n");
    }

    std::printf("--- 修法：容器里存 unique_ptr ---\n");
    {
        std::vector<std::unique_ptr<Res>> own;
        try {
            for (int i = 20; i < 23; ++i) {
                own.push_back(std::make_unique<Res>(i));
                if (i == 21) {
                    throw std::runtime_error("第 2 个失败");
                }
            }
        }
        catch (const std::exception& e) {
            std::printf("  捕获: %s\n", e.what());
        }
        std::printf("  own 里有 %zu 个元素，离开作用域时全部自动释放\n", own.size());
    }

    std::printf("main 结束\n");
    return 0;
}
