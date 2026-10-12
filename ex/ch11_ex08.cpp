// ch11_ex08.cpp —— 引用做参数：为什么 C++ 更喜欢它
// 编译：check.bat ch11_ex08.cpp c++latest
#include <cstdio>
#include <cstring>

struct Big {                  // 假装是个很大的数据结构
    double data[128];
    int    id;
};

// C 时代最常见的三种写法
void by_value(Big b)            { b.id = 1; }                   // 整份拷贝
void by_pointer(Big* b)         { if (b) b->id = 2; }           // 传地址，要判空
void by_reference(Big& b)       { b.id = 3; }                   // 引用，不用判空也不拷贝

// 只读的引用：const Big& 既省拷贝，又保证函数内改不动
void by_const_reference(const Big& b)
{
    std::printf("  by_const_reference 读到 id=%d, data[0]=%.1f\n", b.id, b.data[0]);
        // b.id = 9;   // 去掉注释：编译错误，表达式必须是可修改的左值
}

// C 里交换两个数得传指针；C++ 里直接传引用，调用处更干净
void swap_c(int* a, int* b) { int t = *a; *a = *b; *b = t; }
void swap_cpp(int& a, int& b) { int t = a; a = b; b = t; }

int main()
{
    Big big;
    big.id = 0;
    std::memset(big.data, 0, sizeof(big.data));
    big.data[0] = 9.5;

    by_value(big);
    std::printf("by_value     之后 big.id=%d（改的是副本，没生效）\n", big.id);

    by_pointer(&big);
    std::printf("by_pointer   之后 big.id=%d\n", big.id);

    by_reference(big);
    std::printf("by_reference 之后 big.id=%d\n", big.id);

    by_const_reference(big);

    int x = 1, y = 2;
    swap_c(&x, &y);
    std::printf("swap_c  : x=%d y=%d\n", x, y);
    swap_cpp(x, y);
    std::printf("swap_cpp: x=%d y=%d（调用处没有 & 和 *，更干净）\n", x, y);

        // 引用不能绑定到"空"，所以省掉了 C 里到处写的 if (p != NULL)
        // by_reference(nullptr);   // 编译错误：无法从 nullptr 初始化 Big&
    return 0;
}
