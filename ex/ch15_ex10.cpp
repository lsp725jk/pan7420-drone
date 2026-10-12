// ch15_ex10.cpp —— std::move_if_noexcept：容器的"保底"策略
#include <cstdio>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

// 拷贝可用 + 移动不 noexcept -> move_if_noexcept 选择"拷贝"
struct CopyableThrowingMove {
    std::string s;
    explicit CopyableThrowingMove(std::string v) : s(std::move(v)) {}
    CopyableThrowingMove(const CopyableThrowingMove& o) : s(o.s)
    {
        std::printf("    拷贝构造 s=%s\n", s.c_str());
    }
    CopyableThrowingMove(CopyableThrowingMove&& o)         /* 无 noexcept */ : s(std::move(o.s))
    {
        std::printf("    移动构造 s=%s\n", s.c_str());
    }
};

// 拷贝可用 + 移动 noexcept -> move_if_noexcept 选择"移动"
struct CopyableNoThrowMove {
    std::string s;
    explicit CopyableNoThrowMove(std::string v) : s(std::move(v)) {}
    CopyableNoThrowMove(const CopyableNoThrowMove& o) : s(o.s)
    {
        std::printf("    拷贝构造 s=%s\n", s.c_str());
    }
    CopyableNoThrowMove(CopyableNoThrowMove&& o) noexcept : s(std::move(o.s))
    {
        std::printf("    移动构造 s=%s\n", s.c_str());
    }
};

// 拷贝被删除（只能移动）-> move_if_noexcept 只能选择"移动"，哪怕它会抛
struct MoveOnlyThrowingMove {
    std::string s;
    explicit MoveOnlyThrowingMove(std::string v) : s(std::move(v)) {}
    MoveOnlyThrowingMove(const MoveOnlyThrowingMove&) = delete;
    MoveOnlyThrowingMove& operator=(const MoveOnlyThrowingMove&) = delete;
    MoveOnlyThrowingMove(MoveOnlyThrowingMove&& o)         /* 无 noexcept */ : s(std::move(o.s))
    {
        std::printf("    移动构造 s=%s\n", s.c_str());
    }
};

template <class T>
void probe(const char* title)
{
    std::printf("=== %s ===\n", title);
    T a("x");
    std::printf("  is_nothrow_move_constructible = %d\n",
                std::is_nothrow_move_constructible_v<T>);
    std::printf("  is_copy_constructible         = %d\n",
                std::is_copy_constructible_v<T>);
    std::printf("  decltype(move_if_noexcept(a)) 是右值引用? %d\n",
                std::is_rvalue_reference_v<decltype(std::move_if_noexcept(a))>);
    T b = std::move_if_noexcept(a);
    std::printf("  结果：a.s = \"%s\"，b.s = \"%s\"\n", a.s.c_str(), b.s.c_str());
    std::printf("\n");
}

int main()
{
    std::printf("--- 1) vector 扩容：不标 noexcept 的元素会被拷贝 ---\n");
    {
        std::vector<CopyableThrowingMove> v;
        std::printf("  逐个 push_back（观察扩容时的动作）：\n");
        for (int i = 0; i < 4; ++i) {
            std::printf("  -- push #%d --\n", i);
            v.push_back(CopyableThrowingMove(std::to_string(i)));
        }
        std::printf("  最终 size=%zu capacity=%zu\n\n", v.size(), v.capacity());
    }

    std::printf("--- 2) vector 扩容：标了 noexcept 就会移动 ---\n");
    {
        std::vector<CopyableNoThrowMove> v;
        std::printf("  逐个 push_back（观察扩容时的动作）：\n");
        for (int i = 0; i < 4; ++i) {
            std::printf("  -- push #%d --\n", i);
            v.push_back(CopyableNoThrowMove(std::to_string(i)));
        }
        std::printf("  最终 size=%zu capacity=%zu\n\n", v.size(), v.capacity());
    }

    std::printf("--- 3) move_if_noexcept 的三条决策规则（实测）---\n");
    probe<CopyableThrowingMove>("拷贝可用 + 移动会抛  -> 选拷贝（换异常安全）");
    probe<CopyableNoThrowMove>("拷贝可用 + 移动 noexcept -> 选移动（换性能）");
    probe<MoveOnlyThrowingMove>("拷贝不可用 + 移动会抛 -> 只能移动（否则编不过）");

    std::printf("--- 4) 规则总结 ---\n");
    std::printf("  move_if_noexcept(x) 返回什么，取决于类型 T：\n");
    std::printf("    T 不可拷贝            -> 返回 T&&（只能移动）\n");
    std::printf("    T 可拷贝且移动 noexcept -> 返回 T&&（移动更快且安全）\n");
    std::printf("    T 可拷贝且移动会抛     -> 返回 const T&（退回拷贝，保住强异常保证）\n");
    std::printf("  标准库的 vector::resize / reserve 的搬移就是用这个函数实现的\n");

    return 0;
}
