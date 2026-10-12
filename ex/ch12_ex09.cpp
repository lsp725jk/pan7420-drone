// ch12_ex09.cpp —— 浅拷贝的灾难：char* 成员 + double free（会崩溃）
// 编译：check.bat ch12_ex09.cpp c++latest

// 症状 -> 原因 -> 修法 中的"症状"和"原因"在这个例子里。
// 修法见 ch12_ex10.cpp。
#include <cstdio>
#include <cstring>

class Buffer {
public:
    explicit Buffer(const char* text)
    {
        len_ = (int)std::strlen(text);
        data_ = new char[len_ + 1];            // 堆上分配
        std::memcpy(data_, text, len_ + 1);
        std::printf("  [构造] Buffer(%s) data_=%p\n", data_, (void*)data_);
    }
    ~Buffer()
    {
        std::printf("  [析构] delete[] data_=%p\n", (void*)data_);
        delete[] data_;                        // 释放
        data_ = nullptr;
    }
        // 注意：这里没有写拷贝构造和拷贝赋值！
        // 编译器会生成"逐成员拷贝"的版本，也就是把 data_ 这个指针原样复制。
    const char* text() const { return data_; }
private:
    char* data_;
    int   len_;
};

int main()
{
    std::printf("1) 拷贝构造出的两个对象，data_ 指向同一块内存：\n");
    {
        Buffer a("hello");
        Buffer b(a);                            // 浅拷贝：b.data_ == a.data_
        std::printf("  a.data_=%p  b.data_=%p  -> %s\n",
                    (void*)a.text(), (void*)b.text(),
                    a.text() == b.text() ? "同一块内存！" : "不同内存");
        std::printf("  离开这个作用域时，两个对象都会 delete[] 同一块内存\n");
    }

    std::printf("2) 现在演示真正的后果：对象析构两次同一块内存\n");
    std::printf("   程序接下来很可能会崩溃（abort）\n");
    std::fflush(stdout);
    {
        Buffer c("world");
        Buffer d = c;                           // 又一次浅拷贝
        (void)d;
    }   // <- c 和 d 依次析构，第二次 delete[] 是重复释放
    std::printf("3) 如果这一行打印出来了，说明这次没崩（属于运气，不是正确）\n");
    return 0;
}
