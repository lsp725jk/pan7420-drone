// ch13_ex18.cpp —— 综合实战：多态 + 虚析构 + 组合的完整小例子
// 编译：check.bat ch13_ex18.cpp c++latest

// 场景：一个"传感器管理器"，可以装不同类型的传感器，
// 统一读取、统一销毁（全部通过基类接口）。
#include <cstdio>
#include <cstring>

// ---------------- 接口 ----------------
class Sensor {
public:
    virtual const char* name() const { return "Sensor"; }
    virtual double      read()       = 0;      // 读一次（纯虚：派生类必须实现）
    virtual ~Sensor()
    {
                // 注意：析构函数里不要调用纯虚函数（会链接失败，见 ch13_ex19.cpp）
        std::printf("  [销毁] %s\n", type_name_);
    }
protected:
    explicit Sensor(const char* type_name)
    {
        std::snprintf(type_name_, sizeof(type_name_), "%s", type_name);
    }
private:
    char type_name_[16];
};

class ImuSensor : public Sensor {
public:
    ImuSensor() : Sensor("IMU") {}
    const char* name() const override { return "IMU"; }
    double read() override { return 9.81 + step_++ * 0.01; }
private:
    int step_ = 0;
};

class BaroSensor : public Sensor {
public:
    BaroSensor() : Sensor("BARO") {}
    const char* name() const override { return "BARO"; }
    double read() override { return 1013.25 - step_++ * 0.5; }
private:
    int step_ = 0;
};

// ---------------- 管理者：组合 + 多态 ----------------
class SensorManager {
public:
    static const int MAX = 4;
    SensorManager() : count_(0) {}
    ~SensorManager()
    {
        for (int i = 0; i < count_; ++i) delete sensors_[i];   // 通过基类指针删除
        std::printf("  [管理器的析构完成]\n");
    }

        // 禁止拷贝：管理器独占这些传感器（三法则/零法则的思路）
    SensorManager(const SensorManager&) = delete;
    SensorManager& operator=(const SensorManager&) = delete;

    void add(Sensor* s)
    {
        if (count_ < MAX) sensors_[count_++] = s;
    }

    void read_all()
    {
        for (int i = 0; i < count_; ++i)
            std::printf("  %-6s = %.3f\n", sensors_[i]->name(), sensors_[i]->read());
    }

private:
    Sensor* sensors_[MAX];
    int     count_;
};

int main()
{
    std::printf("1) 装配传感器：\n");
    {
        SensorManager mgr;
        mgr.add(new ImuSensor());
        mgr.add(new BaroSensor());

        std::printf("2) 第一次读取：\n");
        mgr.read_all();
        std::printf("3) 第二次读取：\n");
        mgr.read_all();
        std::printf("4) 作用域结束，管理器析构（会看到每个传感器的虚析构被调用）：\n");
    }

    std::printf("5) 这个例子里用到的知识点：\n");
    std::printf("   - 抽象类定义接口（Sensor）\n");
    std::printf("   - 多态：read_all() 只认基类指针，运行期调到各自实现\n");
    std::printf("   - 虚析构：delete 基类指针能正确释放派生类\n");
    std::printf("   - 组合：管理器持有传感器，而不是继承它们\n");
    std::printf("   - 现代写法会用 std::vector<std::unique_ptr<Sensor>> 取代裸数组（第 14 章）\n");
    return 0;
}
