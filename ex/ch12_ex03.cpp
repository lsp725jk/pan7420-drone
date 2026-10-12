// ch12_ex03.cpp —— 构造函数：默认、带参、委托、explicit
// 编译：check.bat ch12_ex03.cpp c++latest
#include <cstdio>

class Pid {
public:
        // 1) 默认构造函数：不写的话编译器会"合成"一个（前提是没有别的构造函数）
    Pid() { std::printf("  [构造] Pid() 默认\n"); kp_ = ki_ = kd_ = 0.0; }

        // 2) 带参构造函数
    Pid(double kp, double ki, double kd)
    {
        std::printf("  [构造] Pid(%.1f, %.1f, %.2f)\n", kp, ki, kd);
        kp_ = kp; ki_ = ki; kd_ = kd;
    }

        // 3) 委托构造函数（C++11 起）：把活交给上面那个，避免重复代码
    Pid(double kp) : Pid(kp, 0.0, 0.0)
    {
        std::printf("  [构造] Pid(%.1f) 委托给了三参数版本\n", kp);
    }

        // 4) explicit：禁止"隐式转换"，必须显式写类型
    explicit Pid(int gain)
    {
        std::printf("  [构造] explicit Pid(int %d)\n", gain);
        kp_ = gain; ki_ = kd_ = 0.0;
    }

    void dump(const char* who) const
    {
        std::printf("  %s: kp=%.1f ki=%.1f kd=%.2f\n", who, kp_, ki_, kd_);
    }

private:
    double kp_, ki_, kd_;
};

void use_pid(Pid p) { p.dump("按值传参"); }

int main()
{
        // 默认构造
    Pid a;
    a.dump("a");
        // 注意：Pid b(); 是"声明一个函数"，不是默认构造！详见 11.15 节
    Pid b{};                 // 正确写法
    b.dump("b（用 {} 默认构造）");

        // 带参构造
    Pid c(1.0, 0.1, 0.01);
    c.dump("c");

        // 委托构造
    Pid d(2.0);
    d.dump("d");

        // explicit：必须显式转换
    Pid e(5);
    e.dump("e（explicit int）");
        // Pid f = 5;            // 错误：explicit 构造不能用于隐式转换
        // use_pid(5);           // 错误：同上

    use_pid(Pid(9));         // 显式构造一个临时对象再传，合法
    std::printf("一共构造了几个对象？数一数上面的 [构造] 行\n");
    return 0;
}
