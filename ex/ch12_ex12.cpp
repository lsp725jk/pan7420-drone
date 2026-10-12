// ch12_ex12.cpp —— this 指针：成员函数里那个"隐藏参数"
// 编译：check.bat ch12_ex12.cpp c++latest
#include <cstdio>

class Counter {
public:
    void add(int n)
    {
                // 每个非静态成员函数都隐含一个 this 指针，指向调用它的对象
        std::printf("  add: this=%p\n", (void*)this);
        value_ += n;
    }

        // 返回 *this 的引用：可以链式调用
    Counter& inc()
    {
        ++value_;
        return *this;                 // 返回"我自己"
    }

        // 返回 *this 的值：会拷贝一份（不要拿它做链式调用）
    Counter  clone() const { return *this; }

        // 参数名与成员名相同时，用 this-> 区分
    void set_value(int value)
    {
        this->value_ = value;         // this->value_ 是成员，value 是参数
    }

    int value() const { return value_; }

private:
    int value_ = 0;
};

int main()
{
    Counter a, b;
    std::printf("1) 不同对象的 this 不同\n");
    std::printf("   &a=%p &b=%p\n", (void*)&a, (void*)&b);
    a.add(1);
    b.add(1);

    std::printf("2) 链式调用：inc() 返回 *this 的引用\n");
    a.inc().inc().inc();
    std::printf("   a.value()=%d（连加三次）\n", a.value());

    std::printf("3) 同名参数的区分\n");
    a.set_value(100);
    std::printf("   a.value()=%d\n", a.value());

    std::printf("4) 静态成员函数没有 this（详见 ex13）\n");
    std::printf("   clone() 返回的是拷贝: %d\n", a.clone().value());

    std::printf("5) 常见坑：返回局部对象的引用/指针\n");
    std::printf("   Counter& bad() { Counter tmp; return tmp; }  // 悬垂引用\n");
    return 0;
}
