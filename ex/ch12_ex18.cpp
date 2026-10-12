// ch12_ex18.cpp —— 运算符重载 3：赋值运算符与类型转换运算符
// 编译：check.bat ch12_ex18.cpp c++latest
#include <cstdio>

class Meters {                       // 长度：米
public:
    explicit Meters(double v) : v_(v) {}

        // 赋值运算符：把别的类型赋给本类型（隐式转换 + 赋值一步完成，C++ 里叫"转换赋值"）
    Meters& operator=(double v) { v_ = v; return *this; }

        // 类型转换运算符：把本类型转成 double
        // explicit 可以防止隐式转换（C++11 起支持对转换运算符用 explicit）
    operator double() const { return v_; }

    double value() const { return v_; }

private:
    double v_;
};

class Strict {                       // 故意把两个转换都做成 explicit
public:
    explicit Strict(double v) : v_(v) {}
    explicit operator bool() const { return v_ != 0.0; }
    double value() const { return v_; }
private:
    double v_;
};

int main()
{
        // 1) 赋值运算符重载
    Meters m(3.5);
    m = 7.25;                        // 走 operator=(double)
    std::printf("1) m = 7.25 之后 m.value()=%.2f\n", m.value());

        // 2) 类型转换运算符
    double d = m;                    // 走 operator double()
    std::printf("2) double d = m -> %.2f\n", d);
    std::printf("   也可以直接参与运算: m + 1.0 = %.2f\n", (double)m + 1.0);

        // 3) explicit operator bool：必须显式判断
    Strict s(0.0);
    if (static_cast<bool>(s)) std::printf("3) s 非零\n");
    else                      std::printf("3) s 是零（explicit operator bool 需要显式转换）\n");

        // 4) 常见坑
    std::printf("4) 两个坑：\n");
    std::printf("   - 转换运算符太多会让重载解析产生歧义，非必要不加\n");
    std::printf("   - 转换运算符配 explicit 构造函数是安全组合，能防止意外转换\n");
    return 0;
}
