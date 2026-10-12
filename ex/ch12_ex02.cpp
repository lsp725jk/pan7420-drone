// ch12_ex02.cpp —— 访问控制：public / private / protected 与封装
// 编译：check.bat ch12_ex02.cpp c++latest
#include <cstdio>

// 一个"有不变量的类型"：转速必须落在 [0, 2000]
class Motor {
public:
        // 公开接口：调用者只能用这几个
    void set_rpm(int rpm)
    {
        if (rpm < 0 || rpm > 2000) {          // 检查放在这里，外部绕不过去
            std::printf("  [拒绝] rpm=%d 超出范围，保持原值 %d\n", rpm, rpm_);
            return;
        }
        rpm_ = rpm;
    }
    int  rpm() const { return rpm_; }

private:
    int rpm_ = 0;          // 私有：外部无法直接改，不变量就能守住
    int temperature_ = 25; // 私有：暂时没提供接口，外部看不到
};

// protected：只有自己和派生类能访问（详见第 13 章）
class Base {
public:    int pub  = 1;
protected: int prot = 2;
private:   int priv = 3;
public:
    int read_priv() const { return priv; }
};

class Derived : public Base {
public:
    int read_prot() const { return prot; }      // 派生类能访问 protected
        // int read_priv2() const { return priv; }  // 错误：private 谁都不能碰
};

int main()
{
    Motor m;
    m.set_rpm(1200);
    std::printf("1) 合法设置后 rpm=%d\n", m.rpm());
    m.set_rpm(5000);                              // 被拒绝
    std::printf("   非法设置后 rpm=%d（不变量没被破坏）\n", m.rpm());

    Base b;
    std::printf("2) public 成员: b.pub=%d\n", b.pub);
        // std::printf("%d", b.prot);                 // 错误：protected 不能从外部访问
    std::printf("   private 成员要通过公开函数: b.read_priv()=%d\n", b.read_priv());

    Derived d;
    std::printf("3) 派生类能读 protected: d.read_prot()=%d\n", d.read_prot());
    std::printf("   派生类也能读 public: d.pub=%d\n", d.pub);
    return 0;
}
