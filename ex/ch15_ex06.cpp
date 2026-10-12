// ch15_ex06.cpp —— 【必做实验】vector 扩容时的拷贝/移动次数：noexcept 决定了用哪个
#include <cstdio>
#include <type_traits>
#include <utility>
#include <vector>

// 两个只有一处差别的类型：MoveThrowing 的移动构造不标 noexcept，
// MoveNoThrow 的移动构造标了 noexcept。其他完全一样。
struct MoveThrowing {
    int id;
    int payload[8];
    explicit MoveThrowing(int i) : id(i), payload{} { std::printf("    构造 #%d\n", id); }
    MoveThrowing(const MoveThrowing& o) : id(o.id), payload{}
    {
        std::printf("    拷贝构造 #%d  <<< 拷贝了！\n", id);
    }
    MoveThrowing(MoveThrowing&& o)         /* 没有 noexcept */
        : id(o.id), payload{}
    {
        o.id = -1;
        std::printf("    移动构造 #%d\n", id);
    }
};

struct MoveNoThrow {
    int id;
    int payload[8];
    explicit MoveNoThrow(int i) : id(i), payload{} { std::printf("    构造 #%d\n", id); }
    MoveNoThrow(const MoveNoThrow& o) : id(o.id), payload{}
    {
        std::printf("    拷贝构造 #%d  <<< 拷贝了！\n", id);
    }
    MoveNoThrow(MoveNoThrow&& o) noexcept
        : id(o.id), payload{}
    {
        o.id = -1;
        std::printf("    移动构造 #%d\n", id);
    }
};

template <class T>
void test_realloc(const char* title)
{
    std::printf("=== %s ===\n", title);
    std::printf("  is_nothrow_move_constructible = %d\n",
                std::is_nothrow_move_constructible_v<T>);
    std::vector<T> v;
    std::printf("  依次 push_back 5 个元素（不 reserve），每次扩容都要搬走已有元素：\n");
    for (int i = 0; i < 5; ++i) {
        std::printf("  -- push_back(#%d)，当前 size=%zu capacity=%zu --\n",
                    i, v.size(), v.capacity());
        v.push_back(T(i));
    }
    std::printf("  最终 size=%zu capacity=%zu\n\n", v.size(), v.capacity());
}

int main()
{
    std::printf("--- 1) 移动构造**没有** noexcept ---\n");
    test_realloc<MoveThrowing>("MoveThrowing：move 不标 noexcept");

    std::printf("--- 2) 移动构造**标了** noexcept ---\n");
    test_realloc<MoveNoThrow>("MoveNoThrow：move 标了 noexcept");

    std::printf("--- 3) 明明白白地观察 std::move_if_noexcept ---\n");
    {
        MoveThrowing a(100);
        MoveNoThrow  b(200);

        std::printf("  std::move_if_noexcept(a) 的类型带 && 吗？\n");
        std::printf("    MoveThrowing: is_rvalue_reference = %d\n",
                    std::is_rvalue_reference_v<decltype(std::move_if_noexcept(a))>);
        std::printf("    结果：拷贝构造可用且移动不 noexcept -> 返回左值引用 -> 走拷贝\n");
        auto a2 = std::move_if_noexcept(a);
        std::printf("    a.id 仍然是 %d（没被搬空）\n", a.id);

        std::printf("  MoveNoThrow: is_rvalue_reference = %d\n",
                    std::is_rvalue_reference_v<decltype(std::move_if_noexcept(b))>);
        std::printf("    结果：移动是 noexcept -> 返回右值引用 -> 走移动\n");
        auto b2 = std::move_if_noexcept(b);
        std::printf("    b.id 变成 %d（被搬空了），b2.id = %d\n", b.id, b2.id);
    }

    std::printf("--- 4) reserve 之后就没有搬移了 ---\n");
    {
        std::vector<MoveNoThrow> v;
        v.reserve(5);
        std::printf("  reserve(5) 之后 capacity=%zu\n", v.capacity());
        std::printf("  再 push_back 5 个：\n");
        for (int i = 0; i < 5; ++i) {
            v.push_back(MoveNoThrow(i));
        }
        std::printf("  全程没有出现\"拷贝构造\"或\"移动构造\"（除了临时对象的构造）\n");
    }

    return 0;
}
