// ch23_ex13.cpp —— 例 23-13：noexcept 对性能的真实影响
// noexcept 本身不会让函数变快。它影响的是**别人怎么用你**：
// 标准库在"移动可能抛异常"时会退回拷贝，以保证强异常安全。
// 所以一个没标 noexcept 的移动构造函数，可能让你的 vector 扩容全是拷贝。
#include <cstdio>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

struct Counters {
    static int copies;
    static int moves;
    static void reset() { copies = moves = 0; }
};
int Counters::copies = 0;
int Counters::moves = 0;

// ---- 类型 A：移动构造没标 noexcept ----
struct Throwy {
    int v = 0;
    Throwy() = default;
    explicit Throwy(int x) : v(x) {}
    Throwy(const Throwy& o) : v(o.v) { ++Counters::copies; }
    Throwy(Throwy&& o) noexcept(false) : v(o.v) { ++Counters::moves; }  // 明确"可能抛"
    Throwy& operator=(const Throwy&) = default;
    Throwy& operator=(Throwy&&) = default;
};

// ---- 类型 B：移动构造标了 noexcept ----
struct NotThrowy {
    int v = 0;
    NotThrowy() = default;
    explicit NotThrowy(int x) : v(x) {}
    NotThrowy(const NotThrowy& o) : v(o.v) { ++Counters::copies; }
    NotThrowy(NotThrowy&& o) noexcept : v(o.v) { ++Counters::moves; }
    NotThrowy& operator=(const NotThrowy&) = default;
    NotThrowy& operator=(NotThrowy&&) = default;
};

static_assert(!std::is_nothrow_move_constructible<Throwy>::value, "Throwy 的移动可能抛");
static_assert(std::is_nothrow_move_constructible<NotThrowy>::value, "NotThrowy 的移动不抛");

int main() {
    const int N = 1000;

        // ---- Throwy：vector 扩容时会用**拷贝** ----
    Counters::reset();
    {
        std::vector<Throwy> v;
        for (int i = 0; i < N; ++i) {
            v.emplace_back(i);  // emplace 就地构造，本身不触发拷贝/移动
        }
    }
    const int copy_a = Counters::copies;
    const int move_a = Counters::moves;

        // ---- NotThrowy：vector 扩容时会用**移动** ----
    Counters::reset();
    {
        std::vector<NotThrowy> v;
        for (int i = 0; i < N; ++i) {
            v.emplace_back(i);
        }
    }
    const int copy_b = Counters::copies;
    const int move_b = Counters::moves;

    std::printf("往 vector 里放 %d 个元素（只 emplace，不主动拷贝）：\n", N);
    std::printf("  移动构造没标 noexcept : 拷贝 %5d 次, 移动 %5d 次\n", copy_a, move_a);
    std::printf("  移动构造标了 noexcept : 拷贝 %5d 次, 移动 %5d 次\n", copy_b, move_b);
    std::printf("\n差别就在 vector 扩容那一瞬间：\n");
    std::printf("  没有 noexcept -> 标准库不敢移动（怕搬到一半抛异常没法回滚），改成拷贝\n");
    std::printf("  有 noexcept   -> 放心移动\n");

    std::printf("\nis_nothrow_move_constructible:\n");
    std::printf("  Throwy    = %d\n", (int)std::is_nothrow_move_constructible<Throwy>::value);
    std::printf("  NotThrowy = %d\n", (int)std::is_nothrow_move_constructible<NotThrowy>::value);
    std::printf("  std::string = %d（标准库自己的类型都标好了）\n",
                (int)std::is_nothrow_move_constructible<std::string>::value);

        // 别的收益：
        // 1) 代码体积：调用方不用生成栈展开（unwind）代码，MCU 上很值钱；
        // 2) 有些标准库算法会走更短的路径；
        // 3) 文档价值：告诉调用者"这个函数不会抛"，接口更清楚。
        // 坑：noexcept 是**承诺**。函数里真抛了异常，程序直接 std::terminate，
        // 没有栈展开、没有 catch 的机会。不确定就别写。
        // 坑：析构函数默认就是 noexcept（C++11 起）。如果析构里可能抛，
        // 要么自己吞掉，要么显式写 noexcept(false) 并想清楚后果。
    std::printf("\n（本机实测，N=1000，用的是 check.bat 的 /Od）\n");
    return 0;
}
