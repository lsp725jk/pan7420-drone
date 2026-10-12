// ch12_ex14.cpp —— 常量成员函数（const member function）
// 编译：check.bat ch12_ex14.cpp c++latest
#include <cstdio>

class Vector2 {
public:
    Vector2() : x_(0), y_(0) {}
    Vector2(double x, double y) : x_(x), y_(y) {}

        // 不修改对象的函数都加 const：这样 const 对象、const 引用都能调用
    double x() const { return x_; }
    double y() const { return y_; }
    double length() const { return x_ * x_ + y_ * y_; }   // 假长度（不开根号）
    void   dump(const char* who) const
    {
        std::printf("  %s: (%.1f, %.1f) len2=%.1f\n", who, x_, y_, length());
    }

        // 修改对象的函数不能加 const
    void add(const Vector2& o) { x_ += o.x_; y_ += o.y_; }

        // 想在 const 函数里改成员？只对 mutable 成员可行（见 ex17）
    void bump_counter() const { ++reads_; }
    int  reads() const { return reads_; }

private:
    double x_, y_;
    mutable int reads_ = 0;      // mutable：const 函数里也能改
};

// 参数是 const 引用 -> 只能调用 const 成员函数
void print_len(const Vector2& v)
{
    std::printf("  print_len: %.1f\n", v.length());
        // v.add(Vector2(1, 1));    // 错误：v 是 const
}

// 参数是非 const 引用 -> 什么都能调
void modify(Vector2& v)
{
    v.add(Vector2(1, 1));
    v.dump("modify 之后");
}

int main()
{
    Vector2 a(3, 4);
    a.dump("1) a");

    const Vector2 b(1, 2);
    b.dump("2) const 对象 b");
        // b.add(Vector2(1,1));     // 错误：const 对象不能调用非 const 成员

    print_len(a);               // 非 const 对象也能传给 const 引用
    modify(a);

    std::printf("3) mutable 成员在 const 函数里也能改：\n");
    b.bump_counter();
    b.bump_counter();
    std::printf("   b.reads()=%d（b 仍然是 const 对象）\n", b.reads());

    std::printf("4) 经验规则：不改对象的函数一律加 const，能省掉很多编译错误\n");
    return 0;
}
