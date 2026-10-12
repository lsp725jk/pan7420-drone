// ch16_ex06.cpp —— 全特化与偏特化
#include <cstdio>
#include <cstring>
#include <string>
#include <type_traits>
#include <vector>

// ---------------- 全特化（full specialization）：把 T 钉死成一个具体类型 ----------------
template <class T>
struct TypeInfo {
    static const char* name() { return "未知类型（通用模板）"; }
    static constexpr int size = sizeof(T);
};

template <>
struct TypeInfo<int> {                                  // 全特化：T = int
    static const char* name() { return "int（全特化）"; }
    static constexpr int size = sizeof(int);
};

template <>
struct TypeInfo<double> {
    static const char* name() { return "double（全特化）"; }
    static constexpr int size = sizeof(double);
};

template <>
struct TypeInfo<std::string> {
    static const char* name() { return "std::string（全特化）"; }
    static constexpr int size = sizeof(std::string);
};

// ---------------- 偏特化（partial specialization）：只固定一部分 ----------------
// 1) 固定"是指针"
template <class T>
struct TypeInfo<T*> {
    static const char* name() { return "指针（偏特化 T*）"; }
    static constexpr int size = sizeof(void*);
};

// 2) 固定"是数组"
template <class T, std::size_t N>
struct TypeInfo<T[N]> {
    static const char* name() { return "数组（偏特化 T[N]）"; }
    static constexpr int size = sizeof(T) * static_cast<int>(N);
};

// 3) 固定容器的元素类型：只留下 vector 的内层类型做参数
template <class T>
struct TypeInfo<std::vector<T>> {
    static const char* name() { return "std::vector（偏特化）"; }
    static constexpr int size = sizeof(std::vector<T>);
};

// ---------------- 用偏特化做"编译期分类"：这是标准库 type_traits 的手法 ----------------
template <class T>
struct IsPointer {
    static constexpr bool value = false;
};
template <class T>
struct IsPointer<T*> {                 // 只有 T* 才匹配这个偏特化
    static constexpr bool value = true;
};

// ---------------- 函数模板的全特化（注意：函数模板不能偏特化） ----------------
template <class T>
const char* describe(T)
{
    return "通用版本";
}
template <>                            // 函数模板的全特化：可以
const char* describe<int>(int)
{
    return "int 专用版本（全特化）";
}
// template <class T> const char* describe(T*) { ... }   // 这不是偏特化，是**重载**！

int main()
{
    std::printf("--- 1) 全特化：把类型钉死 ---\n");
    std::printf("    TypeInfo<char>::name()        = %s\n", TypeInfo<char>::name());
    std::printf("    TypeInfo<int>::name()         = %s\n", TypeInfo<int>::name());
    std::printf("    TypeInfo<double>::name()      = %s\n", TypeInfo<double>::name());
    std::printf("    TypeInfo<std::string>::name() = %s\n", TypeInfo<std::string>::name());

    std::printf("--- 2) 偏特化：只固定一部分，剩下的还是模板 ---\n");
    std::printf("    指针   : TypeInfo<int*>::name()          = %s, size=%d\n",
                TypeInfo<int*>::name(), TypeInfo<int*>::size);
    std::printf("    指针   : TypeInfo<double*>::name()       = %s, size=%d\n",
                TypeInfo<double*>::name(), TypeInfo<double*>::size);
    std::printf("    数组   : TypeInfo<int[10]>::name()       = %s, size=%d\n",
                TypeInfo<int[10]>::name(), TypeInfo<int[10]>::size);
    std::printf("    vector : TypeInfo<std::vector<int>>::name()    = %s\n",
                TypeInfo<std::vector<int>>::name());
    std::printf("    vector : TypeInfo<std::vector<double>>::name() = %s\n",
                TypeInfo<std::vector<double>>::name());

    std::printf("--- 3) 匹配优先级：特化程度越高越优先 ---\n");
    std::printf("    TypeInfo<std::vector<int>*> 会匹配哪个？\n");
    std::printf("      name() = %s\n", TypeInfo<std::vector<int>*>::name());
    std::printf("    答：T* 偏特化（因为它能匹配任何指针，而 vector 特化匹配不了 vector<int>*）\n");

    std::printf("--- 4) 偏特化做编译期分类（标准库 type_traits 的核心手法）---\n");
    std::printf("    IsPointer<int>::value    = %d\n", IsPointer<int>::value);
    std::printf("    IsPointer<int*>::value   = %d\n", IsPointer<int*>::value);
    std::printf("    IsPointer<double*>::value= %d\n", IsPointer<double*>::value);
    std::printf("    标准库的 std::is_pointer 就是这么写的，只是更严谨\n");
    std::printf("    对照：std::is_pointer_v<int*> = %d\n", std::is_pointer_v<int*>);

    std::printf("--- 5) 函数模板的特化：全特化可以，偏特化不行 ---\n");
    std::printf("    describe(1)     -> %s\n", describe(1));
    std::printf("    describe(1.5)   -> %s\n", describe(1.5));
    std::printf("    describe('a')   -> %s\n", describe('a'));
    std::printf("    函数模板想\"偏特化\"只能靠**重载**：\n");

    std::printf("--- 6) 偏特化的一个实用场景：把 C 数组当容器用 ---\n");
    {
        int  arr[5] = {1, 2, 3, 4, 5};
        std::printf("    TypeInfo<decltype(arr)>::name() = %s\n", TypeInfo<decltype(arr)>::name());
        std::printf("    TypeInfo<decltype(arr)>::size = %d（整个数组的字节数）\n",
                    TypeInfo<decltype(arr)>::size);
        std::printf("    注意 decltype(arr) 是 int[5]（不是 int*），所以匹配到数组偏特化\n");
    }

    return 0;
}
