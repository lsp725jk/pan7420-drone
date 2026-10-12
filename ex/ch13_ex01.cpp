// ch13_ex01.cpp —— 继承的基本语法与三种继承方式
// 编译：check.bat ch13_ex01.cpp c++latest
#include <cstdio>

// ---- 基类 ----
class Sensor {
public:
    void set_id(int id) { id_ = id; }
    int  id() const { return id_; }
    void common() const { std::printf("   Sensor::common()\n"); }

protected:
    int id_ = 0;                 // protected：派生类能用，外部不能用

private:
    int secret_ = 42;            // private：连派生类也不能用
};

// ---- 1) public 继承（最常用）：is-a 关系，"派生类是一种基类" ----
class Imu : public Sensor {
public:
    void read() const
    {
        std::printf("   Imu::read() id=%d（能访问 protected 成员）\n", id_);
                // std::printf("%d", secret_);   // 错误：private 成员派生类也看不见
    }
};

// ---- 2) protected 继承：外部看不到"我是 Sensor"，派生类内部可以 ----
class Baro : protected Sensor {
public:
    void read() const { std::printf("   Baro::read() id=%d\n", id_); }
};

// ---- 3) private 继承：只有自己知道继承了谁（实现复用，不是 is-a）----
class Gps : private Sensor {
public:
    void read() const { std::printf("   Gps::read() id=%d\n", id_); }
};

int main()
{
        // 1) public 继承：派生类对象就是基类对象（能当 Sensor 用）
    Imu imu;
    imu.set_id(1);
    imu.read();
    imu.common();                       // 继承来的公开成员
    Sensor* p = &imu;                   // 合法：public 继承允许向上转型
    std::printf("1) 通过基类指针访问: id=%d\n", p->id());

        // 2) protected 继承：外面不能当 Sensor 用
    Baro baro;
    baro.read();
        // Sensor* q = &baro;               // 错误：protected 继承不允许外部转型

        // 3) private 继承：同样不能向上转型
    Gps gps;
    gps.read();
        // Sensor* r = &gps;                // 错误：private 继承不允许外部转型

        // 4) 三种继承方式对"派生类内部"和"外部"的可见性总结
    std::printf("4) 继承方式的影响（基类成员在派生类里的最高可见性）：\n");
    std::printf("   public 继承    : public->public, protected->protected\n");
    std::printf("   protected 继承 : public->protected, protected->protected\n");
    std::printf("   private 继承   : public->private, protected->private\n");
    std::printf("   无论哪种方式，基类的 private 成员都不可直接访问\n");
    return 0;
}
