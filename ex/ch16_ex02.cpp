// ch16_ex02.cpp —— 模板参数推导：按值 / 按引用 / 按 const 引用 三种情况的差别
#include <cstdio>
#include <typeinfo>
#include <type_traits>

// 把推导出来的 T "翻译"成人能看懂的形式
// 说明：typeid(T).name() 返回的名字是实现定义的（MSVC 对 int 返回 "int"），
// 所以下面这个函数只在 MSVC 上看起来最舒服；非 MSVC 会打印别的名字。
template <class T>
void show(const char* who, const char* what)
{
    using NoRef = std::remove_reference_t<T>;
    const char* suffix = std::is_lvalue_reference_v<T> ? "&"
                       : std::is_rvalue_reference_v<T> ? "&&"
                                                       : "";
    std::printf("    %-8s T = %s%-4s%s   <- %s\n",
                who,
                std::is_const_v<NoRef> ? "const " : "",
                typeid(NoRef).name(),
                suffix,
                what);
}

template <class T> void by_value(T v)       { (void)v; show<T>("按值", "传进来的是拷贝，const/引用都被丢掉"); }
template <class T> void by_ref(T& v)        { (void)v; show<T>("T&",   "保留引用，但只能接左值"); }
template <class T> void by_cref(const T& v) { (void)v; show<T>("const T&", "万能接收，T 本身不带 const"); }

template <class T> void ptr_arg(T* v)       { (void)v; show<T>("T*",   "从指针类型反推元素类型"); }
template <class T> void arr_arg(T v)        { (void)v; show<T>("T（数组）", "按值接数组会退化成指针"); }
template <class T, std::size_t N> void arr_ref(T (&v)[N])
{
    (void)v;
    show<T>("T(&)[N]", "按引用接数组能保住长度 N");
    std::printf("             并且 N 推导为 %zu\n", N);
}

int main()
{
    std::printf("--- 1) 按值传递：const 和引用都会被剥掉 ---\n");
    int        n  = 1;
    const int  cn = 2;
    int&       rn = n;
    by_value(n);
    by_value(cn);       // const 被丢掉
    by_value(rn);       // 引用被丢掉
    by_value(3);        // 字面量也能推

    std::printf("--- 2) 按引用传递：T 就是实参的精确类型 ---\n");
    by_ref(n);
    by_ref(cn);         // T = const int
        // by_ref(3);       // 编译错误：非常量引用不能绑右值
    std::printf("    by_ref(3) 会报错：无法将左值引用绑定到右值\n");
    by_ref(rn);         // T = int（引用会折叠掉）

    std::printf("--- 3) const T& ：T 不带 const，const 在参数上 ---\n");
    by_cref(n);
    by_cref(cn);
    by_cref(3);         // 右值也能接

    std::printf("--- 4) 从指针反推 ---\n");
    int  arr[3] = {1, 2, 3};
    int* p      = arr;
    ptr_arg(p);         // T = int
    ptr_arg(&n);        // T = int
    const int* cp = &cn;
    ptr_arg(cp);        // T = const int

    std::printf("--- 5) 数组：按值会退化成指针，按引用不会 ---\n");
    arr_arg(arr);                 // T = int*
    arr_ref(arr);                 // T = int, N = 3

    std::printf("--- 6) 推导失败与歧义的两种情况 ---\n");
    std::printf("    模板只有一个 T、实参类型不一致 -> 推导失败\n");
    std::printf("      例如 template<class T> T add(T,T);  add(1, 2.5) 编译错误\n");
    std::printf("    模板对 T 有矛盾要求 -> 推导失败\n");
    std::printf("      例如 T& 参数却传了右值\n");
    std::printf("    修法：显式写模板参数 add<double>(1, 2.5)，或用两个参数（见例 16-3）\n");

    return 0;
}
