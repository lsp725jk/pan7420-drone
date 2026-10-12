// 附录A 探针 11：编译期能力对照（#define / enum vs constexpr / consteval / 模板）
#include <cstdio>
#include <array>
#include <type_traits>

// ---------- 1. 常量定义：宏 vs constexpr ----------
#define  MACRO_MAX 100
constexpr int kMax = 100;
static_assert(MACRO_MAX == kMax, "两者数值相同");

// 宏没有类型、没有作用域；constexpr 有
namespace inner {
    constexpr int kMax = 200;      // 与全局的 kMax 不冲突
}
static_assert(inner::kMax == 200, "命名空间里的同名常量互不干扰");

// ---------- 2. 宏的经典坑：参数被多次求值 ----------
#define SQUARE_BAD(x) ((x) * (x))
constexpr int squareGood(int x) { return x * x; }

static int gCalls = 0;
int counted() { return ++gCalls; }   // 每次调用都会让计数器 +1，用来数"被求值几次"

// ---------- 3. 编译期计算：模板（C++11 之前的写法）----------
template <int N> struct FactorialT { static constexpr int value = N * FactorialT<N - 1>::value; };
template <>      struct FactorialT<0> { static constexpr int value = 1; };

// ---------- 4. 编译期计算：constexpr 函数（C++11 起，C++14 起可以写循环）----------
constexpr int factorial(int n) {
    int r = 1;
    for (int i = 2; i <= n; ++i) r *= i;
    return r;
}

// ---------- 5. consteval（C++20）：必须编译期求值 ----------
consteval int mustBeCompileTime(int n) { return n * 2; }

// ---------- 6. if constexpr 与类型分支 ----------
template <typename T>
constexpr const char* category() {
    if constexpr (std::is_pointer_v<T>)        return "指针";
    else if constexpr (std::is_integral_v<T>)  return "整数";
    else if constexpr (std::is_floating_point_v<T>) return "浮点";
    else                                       return "其它";
}

// ---------- 7. 用模板做"编译期数组长度" ----------
template <typename T, std::size_t N>
constexpr std::size_t arrayLen(const T (&)[N]) { return N; }   // C 里只能靠 sizeof/sizeof

int main() {
    std::printf("kMax=%d inner::kMax=%d\n", kMax, inner::kMax);

    std::puts("=== 宏 vs constexpr 函数：参数被求值几次 ===");
    gCalls = 0;
    int r1 = SQUARE_BAD(counted());
    std::printf("  SQUARE_BAD(counted())  = %d, counted() 被调用 %d 次\n", r1, gCalls);
    gCalls = 0;
    int r2 = squareGood(counted());
    std::printf("  squareGood(counted())  = %d, counted() 被调用 %d 次\n", r2, gCalls);
    std::puts("  宏把参数展开了两遍，带副作用的实参就被执行了两次 —— 这是宏最典型的坑");

    std::puts("=== 编译期计算：模板 vs constexpr ===");
    std::printf("  模板 FactorialT<5>::value   = %d\n", FactorialT<5>::value);
    std::printf("  constexpr factorial(5)      = %d\n", factorial(5));
    static_assert(FactorialT<5>::value == factorial(5), "两种写法结果一致");

    std::puts("=== consteval 必须编译期可算 ===");
    constexpr int cv = mustBeCompileTime(21);
    std::printf("  mustBeCompileTime(21) = %d\n", cv);
    int runtimeValue = 7;
    std::printf("  用运行期变量调用会编译报错，所以下面这行被注释掉了：\n");
    std::printf("      mustBeCompileTime(runtimeValue);\n");
    (void)runtimeValue;

    std::puts("=== if constexpr：只有被选中的分支才需要合法 ===");
    std::printf("  int -> %s ; double -> %s ; int* -> %s\n",
                category<int>(), category<double>(), category<int*>());

    std::puts("=== 数组长度：模板推导 vs sizeof 技巧 ===");
    int raw[7] = {0};
    std::array<int, 7> arr{};
    std::printf("  模板 arrayLen(raw) = %zu ; sizeof/sizeof = %zu ; arr.size() = %zu\n",
                arrayLen(raw), sizeof(raw) / sizeof(raw[0]), arr.size());
    return 0;
}
