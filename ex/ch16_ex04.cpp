// ch16_ex04.cpp —— 非类型模板参数：template <int N>、template <class T, std::size_t N>
#include <array>
#include <cstddef>
#include <cstdio>
#include <string>
#include <type_traits>

// ---- 1) 非类型模板参数 N：编译期常量，可以直接当数组大小 ----
template <int N>
struct FixedArray {
    int data[N];              // N 必须是编译期常量
    int size() const { return N; }
    void fill(int v)
    {
        for (int i = 0; i < N; ++i) data[i] = v;
    }
};

// ---- 2) 类型 + 非类型参数混用 ----
template <class T, std::size_t N>
std::size_t count_of(const T (&)[N])
{
    return N;
}

// ---- 3) 非类型参数也能出现在函数签名里 ----
template <int Times>
void repeat(const char* msg)
{
    for (int i = 0; i < Times; ++i) {
        std::printf("    %s\n", msg);
    }
}

// ---- 4) 非类型参数可用于编译期计算 ----
template <int N>
struct Factorial {
    static constexpr long long value = N * Factorial<N - 1>::value;
};

template <>
struct Factorial<0> {                 // 递归终止：全特化（见例 16-6）
    static constexpr long long value = 1;
};

// ---- 5) 指针/引用也可以做非类型参数（要指向有静态生命期的对象）----
const char* const kNames[] = { "alpha", "beta", "gamma" };

template <const char* const* Table, int N>
void print_table()
{
    for (int i = 0; i < N; ++i) {
        std::printf("    [%d] %s\n", i, Table[i]);
    }
}

int main()
{
    std::printf("--- 1) template<int N>：N 决定数组大小 ---\n");
    {
        FixedArray<3>  a3;
        FixedArray<10> a10;
        a3.fill(7);
        a10.fill(9);
        std::printf("    FixedArray<3>::size()  = %d，sizeof = %zu\n", a3.size(), sizeof(a3));
        std::printf("    FixedArray<10>::size() = %d，sizeof = %zu\n", a10.size(), sizeof(a10));
        std::printf("    a3.data[0] = %d, a10.data[9] = %d\n", a3.data[0], a10.data[9]);
        std::printf("    FixedArray<3> 和 FixedArray<10> 是**两个不同的类型**：%d\n",
                    std::is_same_v<FixedArray<3>, FixedArray<10>>);
    }

    std::printf("--- 2) 类型 + 非类型参数混用：自动数出数组长度 ---\n");
    {
        int  arr[]  = {1, 2, 3, 4, 5};
        double darr[] = {1.0, 2.0};
        std::printf("    count_of(arr)  = %zu\n", count_of(arr));
        std::printf("    count_of(darr) = %zu\n", count_of(darr));
        std::printf("    这比 C 的 sizeof(arr)/sizeof(arr[0]) 更安全（不会算错元素大小）\n");
    }

    std::printf("--- 3) 非类型参数用在函数里 ---\n");
    repeat<3>("重复三次");

    std::printf("--- 4) 编译期计算：阶乘 ---\n");
    std::printf("    Factorial<5>::value  = %lld\n", Factorial<5>::value);
    std::printf("    Factorial<10>::value = %lld\n", Factorial<10>::value);
    std::printf("    这些值是在**编译期**算好的，运行时零开销\n");
    {
        int runtime_n = 5;
        std::printf("    对比：如果 N 是运行时变量 (%d)，就不能做模板参数\n", runtime_n);
        std::printf("    模板参数必须是编译期常量表达式\n");
    }

    std::printf("--- 5) 指针/数组也能当非类型参数 ---\n");
    print_table<kNames, 3>();

    std::printf("--- 6) C++17 起的 auto 非类型参数 ---\n");
    {
                // C++17 起允许 template <auto N>，让编译器自动推导非类型参数的类型
        auto show = []<auto V>() { std::printf("    auto 非类型参数 V = %d\n", (int)V); };
        show.template operator()<42>();
        std::printf("    （这是 C++20 的模板 lambda 语法，本机 c++latest 支持）\n");
    }

    std::printf("--- 7) 标准库里的非类型参数：std::array ---\n");
    {
        std::array<int, 4> a{1, 2, 3, 4};
        std::printf("    std::array<int,4> 大小 = %zu，sizeof = %zu\n", a.size(), sizeof(a));
        std::printf("    与 int[4] 的区别：array 知道自己多长（size()），也能整体赋值\n");
        std::array<int, 4> b = a;      // 可以拷贝赋值
        b[0] = 100;
        std::printf("    b = a 之后 b[0] = %d，a[0] = %d（独立的对象）\n", b[0], a[0]);
    }

    return 0;
}
