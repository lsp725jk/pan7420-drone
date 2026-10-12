// ch12_ex04.cpp —— 成员初始化列表（member initializer list）基础
// 编译：check.bat ch12_ex04.cpp c++latest
#include <cstdio>
#include <cstring>

// 模拟一个没有默认构造函数的成员类型
class NoDefault {
public:
    explicit NoDefault(int v) : v_(v) {}
    int v() const { return v_; }
private:
    int v_;
};

class Rigid {
public:
        // ---- 错误写法：在函数体里赋值 ----
        // Rigid(int a) { nd_ = NoDefault(a); }    // 错误：nd_ 必须先构造，但没有默认构造函数

        // ---- 正确写法：初始化列表 ----
    Rigid(int a, const char* nm) : nd_(a), value_(a * 2), len_(0)
    {
        std::snprintf(name_, sizeof(name_), "%s", nm);
        len_ = (int)std::strlen(name_);
        std::printf("  Rigid 构造完成: nd=%d value=%d len=%d\n", nd_.v(), value_, len_);
    }

    void dump() const
    {
        std::printf("  nd=%d value=%d name=%s len=%d\n", nd_.v(), value_, name_, len_);
    }

private:
    NoDefault nd_;          // 必须靠初始化列表构造
    int       value_;
    int       len_;
    char      name_[16];
};

class WithReference {       // 引用成员也只能在初始化列表里初始化
public:
    WithReference(int& target) : ref_(target) { }
    void set(int v) { ref_ = v; }
private:
    int& ref_;
};

int main()
{
        // 1) 必须用初始化列表的两种情况：无默认构造的成员、引用成员、const 成员
    Rigid r(7, "imu");
    r.dump();

        // 2) 初始化列表 vs 函数体赋值：效率与正确性的差别
    std::printf("2) 初始化列表 = 一步构造到位；函数体赋值 = 先默认构造再赋值\n");

        // 3) 引用成员
    int shared = 0;
    WithReference w(shared);
    w.set(42);
    std::printf("3) 引用成员绑定的外部变量被改成 %d\n", shared);
    return 0;
}
