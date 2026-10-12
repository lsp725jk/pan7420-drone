// ch12_ex08.cpp —— 拷贝构造与拷贝赋值：编译器默认给的是"浅拷贝"
// 编译：check.bat ch12_ex08.cpp c++latest

// 这个类是"安全的"：成员都是值类型，浅拷贝就够了。
// 有指针成员的版本见 ex09/ex10。
#include <cstdio>
#include <cstring>

class Card {
public:
    Card() { std::snprintf(name_, sizeof(name_), "%s", "empty"); id_ = 0; }
    Card(const char* nm, int id)
    {
        std::snprintf(name_, sizeof(name_), "%s", nm);
        id_ = id;
    }

        // 为了看清默认拷贝发生在哪一步，手动写出编译器会生成的版本
    Card(const Card& other)            // 拷贝构造函数
    {
        std::snprintf(name_, sizeof(name_), "%s", other.name_);
        id_ = other.id_;
        std::printf("  [拷贝构造] name=%s id=%d\n", name_, id_);
    }
    Card& operator=(const Card& other) // 拷贝赋值运算符
    {
        std::printf("  [拷贝赋值] %s <- %s\n", name_, other.name_);
        if (this != &other) {          // 自赋值检查（必须写）
            std::snprintf(name_, sizeof(name_), "%s", other.name_);
            id_ = other.id_;
        }
        return *this;
    }
    void dump(const char* who) const
    {
        std::printf("  %s: name=%s id=%d\n", who, name_, id_);
    }
private:
    char name_[16];
    int  id_;
};

int main()
{
    Card a("imu", 1);
    a.dump("a 原始");

        // 1) 拷贝构造：用一个已有对象初始化新对象
    Card b(a);
    b.dump("b 拷贝自 a");
    Card c = a;              // 同样是拷贝构造（不是赋值！）
    c.dump("c 拷贝自 a");

        // 2) 拷贝赋值：两个对象都已存在
    Card d;
    d = a;                   // 走 operator=
    d.dump("d 赋值自 a");

        // 3) 自赋值：不检查就会出问题（这里检查了）
    a = a;
    a.dump("a 自赋值后");

        // 4) 什么时候会隐式拷贝：按值传参、按值返回、放进容器
    std::printf("4) 按值传参也会拷贝构造：\n");
    auto by_value = [](Card x) { x.dump("  形参 x"); };
    by_value(a);

    return 0;
}
