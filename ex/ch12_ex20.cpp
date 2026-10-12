// ch12_ex20.cpp —— mutable：在 const 函数里也能改的成员
// 编译：check.bat ch12_ex20.cpp c++latest
#include <cstdio>
#include <cstring>

class Cache {
public:
    explicit Cache(int key) : key_(key), value_(0), hits_(0), dirty_(false) {}

        // 逻辑上是"只读查询"，但内部要更新缓存统计 -> mutable
    int get() const
    {
        ++hits_;                     // mutable：const 函数里允许修改
        if (!dirty_) {
            value_ = key_ * 2;       // 模拟"算一次、缓存起来"
            dirty_ = true;
        }
        return value_;
    }

    int hits() const { return hits_; }

        // 反面教材：不要用 mutable 掩盖"这个函数其实改了对象状态"
        // 如果一个函数改了对象的"业务状态"，它就不该是 const。

private:
    int  key_;
    mutable int  value_;      // 缓存：不算对象的外部状态
    mutable int  hits_;       // 统计：不算对象的外部状态
    mutable bool dirty_;
};

// 参数是 const 引用，也能调用 get()（因为 get 是 const 函数）
void query(const Cache& c, int times)
{
    for (int i = 0; i < times; ++i) (void)c.get();
}

int main()
{
    Cache c(21);

        // 1) const 对象上调用 const 函数，mutable 成员照样被改
    const Cache& r = c;
    std::printf("1) r.get()=%d（第一次算出来并缓存）\n", r.get());
    std::printf("   r.get()=%d（第二次直接用缓存）\n", r.get());

        // 2) 通过 const 引用调用
    query(c, 3);
    std::printf("2) 查询次数 hits=%d（mutable 计数器生效）\n", c.hits());

        // 3) mutable 的合理用途：缓存、统计、日志、互斥量
    std::printf("3) mutable 的合理用途：缓存、计数器、日志、mutex\n");

        // 4) 坑：mutable 会让 const 成员函数不再是线程安全的
    std::printf("4) 坑：多个线程同时调用 const 函数会并发修改 mutable 成员，\n");
    std::printf("   所以 const 函数看起来只读，并不等于线程安全\n");
    return 0;
}
