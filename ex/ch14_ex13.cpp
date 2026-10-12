// ch14_ex13.cpp —— 析构函数里抛异常会发生什么（单独一个文件，因为它会终止程序）
#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <stdexcept>

struct BadDtor {
    BadDtor() { std::printf("  BadDtor 构造\n"); }
    ~BadDtor() noexcept(false)          // 显式把析构函数声明成"可能抛"
    {
        std::printf("  BadDtor 析构，即将抛出异常\n");
        std::fflush(stdout);            // 程序会被强杀，先把缓冲刷出去，否则看不到这行
        throw std::runtime_error("析构函数里抛异常");
    }
};

int main()
{
    std::printf("--- 正常离开作用域，析构抛异常没人接 ---\n");
    {
        BadDtor bad;
        std::printf("  bad 已创建，准备离开作用域\n");
    }
    std::printf("这一行永远不会打印 —— 上面已经 std::terminate 了\n");
    return 0;
}
