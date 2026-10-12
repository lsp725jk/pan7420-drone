// ch14_ex01.cpp —— RAII（资源获取即初始化）：C 手工管理 vs C++ 自动管理
#include <cstdio>
#include <cstdlib>
#include <memory>

// ------------------- C 风格 -------------------
struct CArray {
    int* data;
    int  n;
};

CArray c_array_new(int n)
{
    CArray a;
    a.n = n;
    a.data = static_cast<int*>(malloc(sizeof(int) * static_cast<size_t>(n)));
    if (a.data == nullptr) {
        std::printf("malloc 失败\n");
        std::exit(1);
    }
    for (int i = 0; i < n; ++i) {
        a.data[i] = i * i;
    }
    return a;
}

void c_array_delete(CArray& a)   // 必须记得手工调用
{
    free(a.data);
    a.data = nullptr;
    a.n = 0;
}

// ------------------- C++ 风格（RAII） -------------------
std::unique_ptr<int[]> cpp_array_new(int n)
{
    auto p = std::make_unique<int[]>(n);   // 拿内存的同时拿到"所有权"
    for (int i = 0; i < n; ++i) {
        p[i] = i * i;
    }
    return p;
}

int main()
{
    std::printf("=== C 风格：谁申请、谁释放，全靠人记住 ===\n");
    CArray a = c_array_new(5);
    for (int i = 0; i < a.n; ++i) {
        std::printf("%d ", a.data[i]);
    }
    std::printf("\n");
    c_array_delete(a);                              // 忘了这行就是内存泄漏
    std::printf("c_array_delete 之后 a.data = %p\n", static_cast<void*>(a.data));

    std::printf("=== C++ 风格：析构函数负责释放（RAII）===\n");
    {
        std::unique_ptr<int[]> b = cpp_array_new(5);
        for (int i = 0; i < 5; ++i) {
            std::printf("%d ", b[i]);
        }
        std::printf("\n");
        std::printf("作用域结束前 b.get() = %p\n", static_cast<void*>(b.get()));
    }   // <-- 这里自动 delete[]，一行释放代码都不用写
    std::printf("作用域已结束，内存自动归还\n");

    return 0;
}
