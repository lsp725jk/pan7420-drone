// ch13_ex08.cpp —— 纯虚函数与抽象类
// 编译：check.bat ch13_ex08.cpp c++latest
#include <cstdio>
#include <cmath>

// 抽象类：含有纯虚函数（= 0）的类不能直接创建对象
class Shape {
public:
    virtual double area() const = 0;            // 纯虚函数
    virtual const char* name() const = 0;
    virtual ~Shape() = default;                 // 抽象类也要虚析构

        // 抽象类可以有普通成员函数（公共逻辑放在基类，避免重复）
    void describe() const
    {
        std::printf("  %s 的面积是 %.3f\n", name(), area());   // 调用虚函数
    }
};

class Circle : public Shape {
public:
    explicit Circle(double r) : r_(r) {}
    double area() const override { return 3.14159265358979323846 * r_ * r_; }
    const char* name() const override { return "圆"; }
private:
    double r_;
};

class Rect : public Shape {
public:
    Rect(double w, double h) : w_(w), h_(h) {}
    double area() const override { return w_ * h_; }
    const char* name() const override { return "矩形"; }
private:
    double w_, h_;
};

// 一个还没实现 area() 的中间类：仍然是抽象类，不能实例化
class Ellipse : public Shape {
public:
    explicit Ellipse(double a, double b) : a_(a), b_(b) {}
    const char* name() const override { return "椭圆"; }
        // area() 没实现 -> Ellipse 依旧是抽象类
protected:
    double a_, b_;
};

class RealEllipse : public Ellipse {
public:
    RealEllipse(double a, double b) : Ellipse(a, b) {}
    double area() const override { return 3.14159265358979323846 * a_ * b_; }
};

int main()
{
        // 1) 抽象类不能直接创建对象
        // Shape s;                       // 错误：不允许使用抽象类类型 Shape 的对象
    std::printf("1) Shape s; 会编译失败（抽象类）\n");

        // 2) 通过基类指针/引用使用派生类：面向接口编程
    Circle c(1.0);
    Rect   r(2.0, 3.0);
    Shape* list[2] = { &c, &r };
    std::printf("2) 统一遍历：\n");
    for (Shape* s : list) s->describe();

        // 3) 引用版本
    Shape& ref = c;
    std::printf("3) 通过引用调用: %s\n", ref.name());

        // 4) 中间抽象类：派生到实现了纯虚函数才能实例化
    RealEllipse e(2.0, 1.0);
    e.describe();
    std::printf("4) Ellipse（中间类）仍是抽象类，RealEllipse 才能实例化\n");
    return 0;
}
