// ch12_ex10.cpp —— 深拷贝：三法则（拷贝构造 + 拷贝赋值 + 析构）
// 编译：check.bat ch12_ex10.cpp c++latest

// 这是 ex09 的"修法"。同一份功能，把三个特殊成员函数都写出来。
#include <cstdio>
#include <cstring>
#include <utility>

class Buffer {
public:
    explicit Buffer(const char* text)
    {
        copy_from(text, std::strlen(text));
    }

        // ---- 拷贝构造：新分配一块自己的内存，再拷内容（深拷贝）----
    Buffer(const Buffer& other)
    {
        copy_from(other.data_, (size_t)other.len_);
        std::printf("  [深拷贝构造] 新地址=%p（源地址=%p）\n",
                    (void*)data_, (void*)other.data_);
    }

        // ---- 拷贝赋值：先释放自己的，再分配并拷内容 ----
    Buffer& operator=(const Buffer& other)
    {
        std::printf("  [拷贝赋值] this=%p <- other=%p\n", (void*)this, (void*)&other);
        if (this == &other) return *this;        // 自赋值保护，必须有
        delete[] data_;                          // 先放掉旧资源
        copy_from(other.data_, (size_t)other.len_);
        return *this;
    }

    ~Buffer()
    {
        std::printf("  [析构] delete[] %p\n", (void*)data_);
        delete[] data_;
    }

    const char* text() const { return data_; }
    int         len()  const { return len_; }

        // 为了演示移动后的状态，提供一个"改内容"的接口
    void set_text(const char* t)
    {
        delete[] data_;
        copy_from(t, std::strlen(t));
    }

private:
    void copy_from(const char* src, size_t n)
    {
        len_  = (int)n;
        data_ = new char[n + 1];
        std::memcpy(data_, src, n);
        data_[n] = '\0';
    }

    char* data_ = nullptr;
    int   len_  = 0;
};

int main()
{
    std::printf("1) 深拷贝：两个对象各有各的内存\n");
    {
        Buffer a("hello");
        Buffer b(a);                       // 深拷贝
        std::printf("   a=%s @%p, b=%s @%p\n",
                    a.text(), (void*)a.text(), b.text(), (void*)b.text());
        b.set_text("world");
        std::printf("   改 b 之后: a=%s, b=%s（互不影响）\n", a.text(), b.text());
    }   // 两个对象各自 delete[] 自己的内存，安全

    std::printf("2) 拷贝赋值 + 自赋值保护\n");
    {
        Buffer c("ccc");
        Buffer d("ddd");
        d = c;
        std::printf("   赋值后: c=%s, d=%s\n", c.text(), d.text());
        d = d;                             // 自赋值：没有保护就会先 delete 掉自己的数据
        std::printf("   自赋值后: d=%s（内容还在，说明保护生效）\n", d.text());
    }

    std::printf("3) 三法则：只要写了析构函数（需要手动释放资源），\n");
    std::printf("   就必须同时写拷贝构造和拷贝赋值。\n");
    return 0;
}
