// ch22_ex10.cpp —— 例 22-10：static_assert 在模板里的两个经典坑
#include <cstddef>
#include <cstdio>

// ------------------------------------------------------------------
// 坑 1：条件**不依赖**模板参数时，static_assert 在模板"定义"处就检查，
// 哪怕这个模板从来没被实例化过，也会直接报错。
// ------------------------------------------------------------------
// template <typename T>
// struct Bad {
// static_assert(false, "永远不会通过");   // 直接 error C2338，模板没用到也报
// };

// 正确写法：让条件依赖 T，检查就被推迟到"实例化那一刻"
template <typename T>
struct Deferred {
    static_assert(sizeof(T) == 0, "只有真的实例化 Deferred<T> 时才检查");
};

// 指针不需要完整类型，所以下面这行不会触发实例化
Deferred<int>* g_never_instantiated = nullptr;

// ------------------------------------------------------------------
// 坑 2：写在 if constexpr 的"废分支"里。
// C++23 起，废分支里的 static_assert(false) 被允许（P2593）；
// C++20 及以前，它仍然会在模板定义处立刻报错。
// ------------------------------------------------------------------
template <typename T>
void only_int() {
    if constexpr (sizeof(T) == sizeof(int)) {
        std::printf("T 是 4 字节类型\n");
    } else {
                // 本机实测：c++latest(C++23) 编译通过；c++20 下这行报 error C2338
        static_assert(false, "only_int 只能用于 4 字节类型");
    }
}

// 跨标准都安全的写法：把条件也写成依赖 T 的表达式
template <typename T>
void only_int_safe() {
    static_assert(sizeof(T) == sizeof(int), "only_int_safe 只能用于 4 字节类型");
    std::printf("T 是 4 字节类型（安全版）\n");
}

int main() {
    only_int<int>();
    only_int_safe<int>();

        // 下面这行取消注释就会走进废分支
        // only_int<double>();      // C++23: 编译通过，什么也不做
        // only_int_safe<double>(); // 任何标准下都会报 static_assert 失败

    std::printf("sizeof(int)=%zu\n", sizeof(int));
    std::printf("坑 1 的 Bad 模板被注释掉了，否则本文件根本编不过\n");
    return 0;
}
