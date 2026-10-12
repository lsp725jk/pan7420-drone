// ch12_ex05.cpp —— 成员初始化顺序陷阱：初始化列表的顺序不影响真实顺序
// 编译：check.bat ch12_ex05.cpp c++latest

// 规则：成员按"在类里的声明顺序"初始化，与初始化列表里写的顺序无关。
// 本程序故意把列表顺序写反，验证实际执行顺序。
#include <cstdio>

class Bad {
public:
        // 初始化列表里先写 b_ 再写 a_，但真实顺序由下面的声明顺序决定
    Bad(int v) : b_(v), a_(b_ + 1)   // <- 顺序写反了（b_ 在 a_ 前面）
    {
        std::printf("  Bad: 列表顺序 b_ 在前，但声明顺序 a_ 在前\n");
    }
    int a() const { return a_; }
    int b() const { return b_; }

private:
    int a_;        // <- 先初始化这个
    int b_;        // <- 再初始化这个
};

class Good {
public:
        // 把列表顺序改成和声明一致，就不会看错
    Good(int v) : a_(v + 1), b_(v) {}
    int a() const { return a_; }
    int b() const { return b_; }

private:
    int a_;
    int b_;
};

// 更明显的例子：用"前一个成员"算"后一个成员"
class Chain {
public:
        // 想把 len_ 设成 size_ 的两倍，但 size_ 声明在后面 -> 读到的是未初始化值
    Chain() : len_(size_ * 2), size_(10) {}
    int size() const { return size_; }
    int len() const { return len_; }
private:
    int len_;      // 声明在前 -> 先初始化，此时 size_ 还没初始化
    int size_;
};

int main()
{
        // 1) 反着写列表顺序
    Bad bad(5);
    std::printf("1) Bad(5): a()=%d b()=%d\n", bad.a(), bad.b());
    std::printf("   预期（如果按列表顺序）: a()=6 b()=5\n");
    std::printf("   实际：a_ 先初始化，读到的 b_ 还是未初始化的值\n");

        // 2) 顺序写对就没问题
    Good good(5);
    std::printf("2) Good(5): a()=%d b()=%d\n", good.a(), good.b());

        // 3) 跨成员依赖：len_ 用 size_ 算，读取未初始化成员
    Chain ch;
    std::printf("3) Chain: size()=%d len()=%d（len_ 想等于 size_*2，但没算对）\n",
                ch.size(), ch.len());

        // 4) 怎么避开这个坑
    std::printf("4) 三条规则：\n");
    std::printf("   - 初始化列表的顺序永远和声明顺序写成一致\n");
    std::printf("   - 不要让一个成员依赖另一个成员的值\n");
    std::printf("   - 需要依赖时，在构造函数体里算，或者提供专门的成员函数\n");
    std::printf("   （MSVC 的 /W4 会对顺序不一致给出 warning C5038）\n");
    return 0;
}
