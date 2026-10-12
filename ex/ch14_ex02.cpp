// ch14_ex02.cpp —— 为什么需要 RAII：提前 return / 出错分支 / 异常三条漏资源的路径
#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <stdexcept>

// ------------------- C 风格：三条路径都要手工释放 -------------------
int c_process(const char* text, int mode)
{
    char* buf = static_cast<char*>(malloc(64));
    if (buf == nullptr) {
        return -1;
    }
    std::strncpy(buf, text, 63);
    buf[63] = '\0';

    if (mode == 1) {
                // 坑 1：提前 return，忘了 free(buf) —— 泄漏
        std::printf("[C] mode=1 提前返回，buf 泄漏了\n");
        return 1;
    }
    if (mode == 2) {
                // 坑 2：出错也要 free，靠人记得
        std::printf("[C] mode=2 出错路径，这次记得 free\n");
        free(buf);
        return 2;
    }

    std::printf("[C] mode=%d 正常路径 buf=\"%s\"\n", mode, buf);
    free(buf);
    return 0;
}

// ------------------- C++ 风格：无论怎么走，析构都会执行 -------------------
int cpp_process(const char* text, int mode)
{
    auto buf = std::make_unique<char[]>(64);   // 出了这个函数一定会 delete[]
    std::strncpy(buf.get(), text, 63);
    buf[63] = '\0';

    if (mode == 1) {
        std::printf("[C++] mode=1 提前返回，buf 照样自动释放\n");
        return 1;                              // 不用写任何释放语句
    }
    if (mode == 2) {
        std::printf("[C++] mode=2 抛出异常，栈展开时也会释放\n");
        throw std::runtime_error("模拟出错");
    }

    std::printf("[C++] mode=%d 正常路径 buf=\"%s\"\n", mode, buf.get());
    return 0;
}

int main()
{
    for (int m = 0; m <= 1; ++m) {
        std::printf("c_process 返回 %d\n", c_process("hello", m));
    }
    for (int m = 0; m <= 1; ++m) {
        std::printf("cpp_process 返回 %d\n", cpp_process("hello", m));
    }

    try {
        cpp_process("hello", 2);
    }
    catch (const std::exception& e) {
        std::printf("捕获到异常: %s\n", e.what());
    }
    std::printf("main 结束\n");
    return 0;
}
