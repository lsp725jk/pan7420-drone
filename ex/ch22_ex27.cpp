// ch22_ex27.cpp —— 例 22-27：std::conditional 与 std::common_type
// conditional：编译期的三目运算符，用来"按条件挑一个类型"。
// common_type：多个类型放在一起运算时，结果该是什么类型。
#include <cstdint>
#include <cstdio>
#include <string>
#include <type_traits>

// -------- 1) conditional：按条件挑类型 --------
// 小于 4 字节的整数统一加宽到 int32_t，其余原样
template <typename T>
using Widened = std::conditional_t<(sizeof(T) < 4), std::int32_t, T>;

static_assert(std::is_same<Widened<char>, std::int32_t>::value, "char -> int32_t");
static_assert(std::is_same<Widened<std::int64_t>, std::int64_t>::value, "int64_t 不动");

// -------- 2) 真实用法：存储策略 --------
// 平凡可复制的类型直接存值；否则只存指针，省掉拷贝构造/析构的开销
template <typename T>
struct Storage {
    using type = std::conditional_t<std::is_trivially_copyable<T>::value, T, T*>;
};

struct Trivial {
    int a;
};
struct NonTrivial {
    std::string s;
    NonTrivial() : s("x") {}
};

// -------- 3) common_type：混合运算的结果类型 --------
static_assert(std::is_same<std::common_type_t<int, double>, double>::value, "");
static_assert(std::is_same<std::common_type_t<char, short>, int>::value, "整型提升");

template <typename A, typename B>
std::common_type_t<A, B> add(A a, B b) {
    return static_cast<std::common_type_t<A, B>>(a) + b;
}

// -------- 4) 实际使用：找一个"能被所有类型表示"的累加器类型 --------
template <typename... Ts>
using Accumulator = std::common_type_t<Ts..., std::int64_t>;

int main() {
    std::printf("sizeof(char)=%zu  Widened<char> 是 int32_t(%zu 字节)\n", sizeof(char),
                sizeof(Widened<char>));
    std::printf("sizeof(int64_t)=%zu Widened<int64_t> 不变\n", sizeof(std::int64_t));

    std::printf("Storage<Trivial>::type    是值类型   (%zu 字节)\n",
                sizeof(Storage<Trivial>::type));
    std::printf("Storage<NonTrivial>::type 是指针类型 (%zu 字节)\n",
                sizeof(Storage<NonTrivial>::type));

    std::printf("add(1, 2.5)   = %g   （返回 double）\n", add(1, 2.5));
    std::printf("add('a', 1)   = %d   （返回 int，字符参与整型提升）\n", add('a', 1));

    std::printf("Accumulator<int, short> 是 %zu 字节\n", sizeof(Accumulator<int, short>));

        // 坑：conditional 的两个分支**都会被写出来**，不像 if constexpr 那样会丢弃。
        // 下面这行即使条件为真也编不过，因为编译器仍要处理 T::no_such_type：
        // template <typename T>
        // using Bad = std::conditional_t<true, int, typename T::no_such_type>;
        // -> error C2039: "no_such_type": 不是 "T" 的成员
    std::printf("选择类型用 conditional；选择**代码**用 if constexpr\n");
    return 0;
}
