// ch12_ex13.cpp —— 静态成员：变量与函数属于"类"，不属于"对象"
// 编译：check.bat ch12_ex13.cpp c++latest
#include <cstdio>

class Device {
public:
    Device(const char* nm)
    {
        std::snprintf(name_, sizeof(name_), "%s", nm);
        ++count_;                       // 每个对象构造时计数
        std::printf("  [构造] %s，当前对象数=%d\n", name_, count_);
    }
    ~Device()
    {
        --count_;
        std::printf("  [析构] %s，当前对象数=%d\n", name_, count_);
    }

        // 静态成员函数：没有 this，只能访问静态成员
    static int  count() { return count_; }
    static void report() { std::printf("  [静态函数] 对象数=%d\n", count_); }

    const char* name() const { return name_; }

private:
    char name_[16];
    static int count_;                  // 声明：静态成员变量
};

int Device::count_ = 0;                 // 定义：必须在类外写一次（C++17 起可写 inline）

int main()
{
    std::printf("1) 还没有对象时就能调用静态函数\n");
    std::printf("   Device::count()=%d\n", Device::count());
    Device::report();

    std::printf("2) 构造对象时静态变量被共享\n");
    Device d1("imu");
    Device d2("baro");
    std::printf("   Device::count()=%d\n", Device::count());

    std::printf("3) 两种调用方式都行（推荐用类名）\n");
    std::printf("   d1.count()=%d  Device::count()=%d\n", d1.count(), Device::count());

    std::printf("4) 作用域结束，计数回落\n");
    {
        Device d3("gps");
        std::printf("   Device::count()=%d\n", Device::count());
    }
    std::printf("   Device::count()=%d\n", Device::count());

    std::printf("5) 静态函数里不能用 this / 不能访问非静态成员\n");
    std::printf("   static int f() { return name_[0]; }  // 错误\n");
    (void)d1;
    return 0;
}
