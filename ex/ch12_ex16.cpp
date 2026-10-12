// ch12_ex16.cpp —— 运算符重载 1：成员函数 vs 非成员函数（+ 与 +=）
// 编译：check.bat ch12_ex16.cpp c++latest
#include <cstdio>

class Vec2 {
public:
    Vec2() : x_(0), y_(0) {}
    Vec2(double x, double y) : x_(x), y_(y) {}

        // += 会改自己 -> 习惯上做成成员函数，返回 *this 的引用
    Vec2& operator+=(const Vec2& o)
    {
        x_ += o.x_; y_ += o.y_;
        return *this;
    }

        // + 不改自己 -> 可以用成员函数（左操作数必须是 Vec2）
    Vec2 operator+(const Vec2& o) const
    {
        return Vec2(x_ + o.x_, y_ + o.y_);
    }

        // 前置 ++ / 后置 ++ 的写法区别（后置多一个 int 占位参数）
    Vec2& operator++()      { ++x_; ++y_; return *this; }        // ++v
    Vec2  operator++(int)   { Vec2 old = *this; ++x_; ++y_; return old; }  // v++

    double x() const { return x_; }
    double y() const { return y_; }
    void dump(const char* who) const
    {
        std::printf("  %s: (%.1f, %.1f)\n", who, x_, y_);
    }

private:
    double x_, y_;
};

int main()
{
    Vec2 a(1, 2), b(3, 4);

        // 1) 二元 +
    Vec2 c = a + b;
    c.dump("1) a + b");

        // 2) 复合赋值 +=
    a += b;
    a.dump("2) a += b");

        // 3) 链式：+= 返回引用才能连写
    Vec2 d(0, 0);
    (d += Vec2(1, 1)) += Vec2(2, 2);
    d.dump("3) (d += (1,1)) += (2,2)");

        // 4) 前置/后置 ++ 的区别
    Vec2 e(5, 5);
    Vec2 f = ++e;                 // 先加再用：e 和 f 都是 (6,6)
    e.dump("4) ++e 之后 e");
    f.dump("   f = ++e");
    Vec2 g(5, 5);
    Vec2 h = g++;                 // 先用再加：h 是 (5,5)，g 是 (6,6)
    g.dump("   g++ 之后 g");
    h.dump("   h = g++");

    std::printf("5) 注意：运算符重载不能改变优先级、结合性，也不能发明新运算符\n");
    return 0;
}
