// ch22_ex13.cpp —— 例 22-13：if constexpr 的三个坑
#include <cstdio>
#include <string>
#include <type_traits>

// ------------------------------------------------------------------
// 坑 1：丢弃的分支不是"不存在"。它的语法、名字查找、非依赖部分的类型
// 仍然要合法，只是不再检查"依赖模板参数"的语义。
// 在**非模板**函数里，if constexpr 的废分支会被完整检查。
// ------------------------------------------------------------------
void non_template_demo() {
    if constexpr (sizeof(int) == 4) {
        std::printf("坑 1：走的是这一支\n");
    } else {
                // 把下面这行的注释去掉就会编译失败，即使条件恒为真、这一支永远走不到：
                // int x = "字符串";     // error C2440: 无法从 const char[7] 转换为 int
        std::printf("永远走不到的分支\n");
    }
}

// ------------------------------------------------------------------
// 坑 2：模板基类里的成员是"依赖名"，必须写 this-> 或 Base<T>::
// ------------------------------------------------------------------
template <typename T>
struct Base {
    void hello() { std::printf("坑 2：Base<T>::hello() 被调用\n"); }
    using value_type = T;
};

template <typename T>
struct Derived : Base<T> {
    void call_ok() {
        this->hello();                       // 正确：告诉编译器"这是模板基类里的"
        typename Base<T>::value_type v{};    // 正确：类型名要加 typename
        (void)v;
    }
        // void call_bad() { hello(); }   // error C3861: "hello": 找不到标识符
};

// ------------------------------------------------------------------
// 坑 3：if constexpr 不能"绕过"非依赖的错误。
// 比如一个类型根本没有某个操作符，写在依赖位置才行。
// ------------------------------------------------------------------
template <typename T>
std::string kind() {
    if constexpr (std::is_pointer_v<T>) {
        return "指针";
    } else if constexpr (requires { std::declval<T>() + std::declval<T>(); }) {
        return "支持 +";
    } else {
        return "其它";
    }
}

struct NoPlus {
    int a = 0;
};
struct WithPlus {
    int a = 0;
};
WithPlus operator+(const WithPlus& x, const WithPlus& y) { return WithPlus{x.a + y.a}; }

int main() {
    non_template_demo();

    Derived<int> d;
    d.call_ok();

    std::printf("kind<int>()        = %s\n", kind<int>().c_str());
    std::printf("kind<int*>()       = %s\n", kind<int*>().c_str());
    std::printf("kind<WithPlus>()   = %s\n", kind<WithPlus>().c_str());
    std::printf("kind<NoPlus>()     = %s\n", kind<NoPlus>().c_str());
    return 0;
}
