// ch24_ex03.cpp —— 例 24-3：工厂模式
// 场景：飞控支持多种 IMU 型号（比如 ICM42688、BMI088、MPU6050），
// 上电时读 WHO_AM_I 决定用哪个驱动。调用方只想拿到"一个 IMU"，不想知道型号。
#include <cstdio>
#include <functional>
#include <map>
#include <memory>
#include <string>

// ---- 产品接口 ----
class Imu {
public:
    virtual ~Imu() = default;
    virtual const char* name() const = 0;
    virtual int readWhoAmI() const = 0;
};

class Icm42688 : public Imu {
public:
    const char* name() const override { return "ICM42688"; }
    int readWhoAmI() const override { return 0x47; }
};
class Bmi088 : public Imu {
public:
    const char* name() const override { return "BMI088"; }
    int readWhoAmI() const override { return 0x1E; }
};
class Mpu6050 : public Imu {
public:
    const char* name() const override { return "MPU6050"; }
    int readWhoAmI() const override { return 0x68; }
};

// ==================================================================
// 写法 A：简单工厂（if/switch 分支）
// ==================================================================
std::unique_ptr<Imu> make_imu_by_id(int who_am_i) {
    switch (who_am_i) {
        case 0x47:
            return std::make_unique<Icm42688>();
        case 0x1E:
            return std::make_unique<Bmi088>();
        case 0x68:
            return std::make_unique<Mpu6050>();
        default:
            return nullptr;
    }
}

// ==================================================================
// 写法 B：自注册工厂（表驱动）
// 加一款新 IMU 时，只要在新驱动文件里写一行 REGISTER_IMU，不用动工厂本身。
// 这就是"开闭原则"（对扩展开放，对修改关闭）的具体样子。
// ==================================================================
class ImuFactory {
public:
    using Creator = std::function<std::unique_ptr<Imu>()>;

    static ImuFactory& instance() {
        static ImuFactory f;
        return f;
    }

    void reg(int who_am_i, const char* label, Creator c) {
        table_[who_am_i] = Entry{label, std::move(c)};
    }

    std::unique_ptr<Imu> create(int who_am_i) const {
        const auto it = table_.find(who_am_i);
        if (it == table_.end()) {
            return nullptr;
        }
        return it->second.creator();
    }

    void list() const {
        for (const auto& kv : table_) {
            std::printf("  0x%02X -> %s\n", kv.first, kv.second.label.c_str());
        }
    }

private:
    struct Entry {
        std::string label;
        Creator creator;
    };
    std::map<int, Entry> table_;
};

// 注册一条：在真实项目里这一行放在各自驱动的 .cpp 里
struct ImuRegistrar {
    ImuRegistrar(int id, const char* label, ImuFactory::Creator c) {
        ImuFactory::instance().reg(id, label, std::move(c));
    }
};
#define REGISTER_IMU(ID, TYPE) \
    static ImuRegistrar g_reg_##TYPE(ID, #TYPE, [] { return std::make_unique<TYPE>(); })

REGISTER_IMU(0x47, Icm42688);
REGISTER_IMU(0x1E, Bmi088);
REGISTER_IMU(0x68, Mpu6050);

int main() {
    std::printf("简单工厂：\n");
    for (int id : {0x47, 0x1E, 0x68, 0x99}) {
        auto p = make_imu_by_id(id);
        if (p) {
            std::printf("  WHO_AM_I=0x%02X -> %s\n", id, p->name());
        } else {
            std::printf("  WHO_AM_I=0x%02X -> 不认识的器件\n", id);
        }
    }

    std::printf("\n自注册工厂（表里现在有）：\n");
    ImuFactory::instance().list();

    std::printf("\n按 ID 创建：\n");
    auto imu = ImuFactory::instance().create(0x1E);
    std::printf("  0x1E -> %s (读回 0x%02X)\n", imu->name(), imu->readWhoAmI());

    std::printf("\n两者怎么选：\n");
    std::printf("  型号只有两三种、逻辑简单  -> 简单工厂（一眼看得完，好调试）\n");
    std::printf("  型号多、驱动分散在多个文件 -> 自注册工厂（加驱动不用改工厂）\n");
    std::printf("\n坑：自注册依赖【静态对象构造】，如果工厂在另一个编译单元里，\n");
    std::printf("   注册顺序是不确定的。稳妥做法是在 main 开头显式调用一次初始化函数，\n");
    std::printf("   或者用 constinit / 函数内 static（C++11 起保证线程安全且只初始化一次）。\n");
    return 0;
}
