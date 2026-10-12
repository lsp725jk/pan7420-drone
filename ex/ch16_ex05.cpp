// ch16_ex05.cpp —— 类模板：写一个能装任何类型的"环形缓冲区"
#include <cstddef>
#include <cstdio>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

// 类模板：成员函数在**被调用时**才实例化（这一点很重要）
template <class T, std::size_t N>
class RingBuffer {
public:
    RingBuffer() : head_(0), tail_(0), count_(0) {}

    bool push(const T& v)
    {
        if (count_ == N) return false;          // 满了
        buf_[tail_] = v;
        tail_ = (tail_ + 1) % N;
        ++count_;
        return true;
    }
    bool pop(T& out)
    {
        if (count_ == 0) return false;          // 空了
        out = buf_[head_];
        head_ = (head_ + 1) % N;
        --count_;
        return true;
    }

    std::size_t size() const  { return count_; }
    std::size_t cap() const   { return N; }
    bool        empty() const { return count_ == 0; }
    bool        full() const  { return count_ == N; }

        // 成员函数里也能用模板参数 N 做编译期判断
    static constexpr bool is_tiny() { return N <= 4; }

private:
    T           buf_[N];        // 注意：T 必须有默认构造函数（数组要求）
    std::size_t head_, tail_, count_;
};

// 类模板的成员函数定义写在类外面时的写法
template <class T>
class Wrapper {
public:
    explicit Wrapper(T v);
    T get() const;
private:
    T v_;
};

template <class T>
Wrapper<T>::Wrapper(T v) : v_(v) {}         // 要写 Wrapper<T>::

template <class T>
T Wrapper<T>::get() const { return v_; }    // 返回类型也写 T

int main()
{
    std::printf("--- 1) 类模板的基本用法 ---\n");
    {
        RingBuffer<int, 4> rb;
        std::printf("    cap=%zu size=%zu empty=%d full=%d\n",
                    rb.cap(), rb.size(), rb.empty() ? 1 : 0, rb.full() ? 1 : 0);
        for (int i = 1; i <= 4; ++i) {
            bool ok = rb.push(i * 10);
            std::printf("    push(%d) -> %d，size=%zu\n", i * 10, ok ? 1 : 0, rb.size());
        }
        std::printf("    再 push(50) -> %d（满了，推不进去）\n", rb.push(50) ? 1 : 0);

        int v = 0;
        while (rb.pop(v)) {
            std::printf("    pop -> %d\n", v);
        }
        std::printf("    空了吗？empty=%d，再 pop -> %d\n",
                    rb.empty() ? 1 : 0, rb.pop(v) ? 1 : 0);
    }

    std::printf("--- 2) 同一个模板，换类型就能用 ---\n");
    {
        RingBuffer<std::string, 3> rs;
        rs.push("one");
        rs.push("two");
        std::string s;
        while (rs.pop(s)) {
            std::printf("    string pop -> %s\n", s.c_str());
        }
    }

    std::printf("--- 3) 不同实例化参数 = 不同类型 ---\n");
    {
        std::printf("    RingBuffer<int,4> 与 RingBuffer<int,8> 同类型? %d\n",
                    std::is_same_v<RingBuffer<int, 4>, RingBuffer<int, 8>>);
        std::printf("    RingBuffer<int,4> 与 RingBuffer<double,4> 同类型? %d\n",
                    std::is_same_v<RingBuffer<int, 4>, RingBuffer<double, 4>>);
        std::printf("    is_tiny(): 4 元素 -> %d，8 元素 -> %d\n",
                    RingBuffer<int, 4>::is_tiny() ? 1 : 0,
                    RingBuffer<int, 8>::is_tiny() ? 1 : 0);
        std::printf("    sizeof(RingBuffer<int,4>)   = %zu\n", sizeof(RingBuffer<int, 4>));
        std::printf("    sizeof(RingBuffer<int,64>)  = %zu\n", sizeof(RingBuffer<int, 64>));
        std::printf("    sizeof(RingBuffer<double,4>)= %zu\n", sizeof(RingBuffer<double, 4>));
    }

    std::printf("--- 4) 成员函数定义写在类外 ---\n");
    {
        Wrapper<int>         wi(42);
        Wrapper<std::string> ws(std::string("hello"));
        std::printf("    Wrapper<int>.get()         = %d\n", wi.get());
        std::printf("    Wrapper<std::string>.get() = %s\n", ws.get().c_str());
    }

    std::printf("--- 5) 关键特性：成员函数只在被调用时才实例化 ---\n");
    {
        std::printf("    RingBuffer<int,4> 里所有成员都被调用过 -> 全部实例化\n");
        std::printf("    如果某个成员函数从没被调用，它就不会被实例化，\n");
        std::printf("    所以即使它对某个 T 编不过，只要你不用它，程序照样能编过\n");
        std::printf("    （这是模板被称为\"鸭子类型\"的原因：看起来像就够用）\n");
    }

    std::printf("--- 6) 类模板做参数：模板模板参数 ---\n");
    {
        std::printf("    类模板本身也可以当模板参数传（template<template<class> class C>）\n");
        std::printf("    标准库里的 stack/queue 就是这么实现的：\n");
        std::printf("      template<class T, class Container = std::deque<T>> class queue;\n");
    }

    return 0;
}
