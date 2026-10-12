// ch12_ex22.cpp —— 类实战：一个飞控里常见的 PID 类（含五法则讨论）
// 编译：check.bat ch12_ex22.cpp c++latest
#include <cstdio>

class Pid {
public:
        // 构造函数 + 初始化列表（顺序与声明一致）
    Pid(double kp, double ki, double kd, double limit)
        : kp_(kp), ki_(ki), kd_(kd), limit_(limit),
          integral_(0.0), prev_err_(0.0), first_(true)
    {
    }

        // 默认参数版本（第 11.12 节）
    explicit Pid(double kp = 1.0) : Pid(kp, 0.0, 0.0, 100.0) {}   // 委托构造

        // 核心：算一次输出
    double update(double target, double actual, double dt)
    {
        double err = target - actual;

        integral_ += err * dt;
        double deriv = first_ ? 0.0 : (err - prev_err_) / dt;
        first_ = false;
        prev_err_ = err;

        double out = kp_ * err + ki_ * integral_ + kd_ * deriv;

                // 输出限幅
        if (out >  limit_) out =  limit_;
        if (out < -limit_) out = -limit_;
        return out;
    }

    void reset() { integral_ = 0.0; prev_err_ = 0.0; first_ = true; }

        // 只读接口：加 const
    double kp() const { return kp_; }
    double integral() const { return integral_; }

        // 参数整定：返回值用 void（失败时才需要报错）
    void set_gains(double kp, double ki, double kd)
    {
        if (kp < 0 || ki < 0 || kd < 0) { std::printf("   非法增益，忽略\n"); return; }
        kp_ = kp; ki_ = ki; kd_ = kd;
    }

private:
        // 成员声明顺序 = 初始化顺序（见 12.5 节）
    double kp_, ki_, kd_;
    double limit_;
    double integral_;
    double prev_err_;
    bool   first_;
};

int main()
{
        // 1) 用带参构造
    Pid pid(0.8, 0.05, 0.01, 50.0);
    std::printf("1) kp=%.2f\n", pid.kp());

        // 2) 模拟几步控制：目标 100，初始 0
    double actual = 0.0;
    std::printf("2) 控制过程（目标 100）:\n");
    for (int step = 0; step < 6; ++step) {
        double out = pid.update(100.0, actual, 0.1);
        actual += out * 0.1;             // 假装被控对象按输出变化
        std::printf("   step=%d out=%7.3f actual=%7.3f integral=%7.3f\n",
                    step, out, actual, pid.integral());
    }

        // 3) 重置
    pid.reset();
    std::printf("3) reset 之后 integral=%.3f\n", pid.integral());

        // 4) 委托构造 + 默认参数
    Pid simple;
    std::printf("4) Pid simple; 用默认参数: kp=%.2f\n", simple.kp());

        // 5) 这个类要不要写拷贝构造/析构？
        // 成员都是 double/bool -> 编译器生成的就对 -> 零法则
    Pid copy = pid;
    std::printf("5) 拷贝一份: kp=%.2f（零法则：不用自己写拷贝/析构）\n", copy.kp());
    return 0;
}
