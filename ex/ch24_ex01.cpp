// ch24_ex01.cpp —— 例 24-1：策略模式（Strategy）——运行时多态版
// 场景：同一套姿态解算流程，可以换不同的滤波器（互补滤波 / 卡尔曼 / 滑动平均）。
// 需求：算法可替换，而且调用方不需要改代码。
#include <cmath>
#include <cstdio>
#include <memory>
#include <vector>

// ---- 1) 抽象接口：只描述"能做什么"，不含实现 ----
class Filter {
public:
    virtual ~Filter() = default;                     // 多态基类必须有虚析构
    virtual const char* name() const = 0;
    virtual double update(double raw) = 0;           // 每次采样调用一次
    virtual void reset() = 0;
};

// ---- 2) 具体策略 ----
class PassthroughFilter : public Filter {
public:
    const char* name() const override { return "直通"; }
    double update(double raw) override { return raw; }
    void reset() override {}
};

class LowPassFilter : public Filter {
public:
    explicit LowPassFilter(double alpha) : alpha_(alpha) {}
    const char* name() const override { return "一阶低通"; }
    double update(double raw) override {
        y_ = y_ + alpha_ * (raw - y_);
        return y_;
    }
    void reset() override { y_ = 0.0; }

private:
    double alpha_;
    double y_ = 0.0;
};

class MovingAverageFilter : public Filter {
public:
    explicit MovingAverageFilter(int n) : buf_(static_cast<std::size_t>(n), 0.0) {}
    const char* name() const override { return "滑动平均"; }
    double update(double raw) override {
        sum_ -= buf_[idx_];
        buf_[idx_] = raw;
        sum_ += raw;
        idx_ = (idx_ + 1) % buf_.size();
        return sum_ / static_cast<double>(buf_.size());
    }
    void reset() override {
        std::fill(buf_.begin(), buf_.end(), 0.0);
        sum_ = 0.0;
        idx_ = 0;
    }

private:
    std::vector<double> buf_;
    std::size_t idx_ = 0;
    double sum_ = 0.0;
};

// ---- 3) 上下文：持有策略，自己不关心具体是哪个 ----
class AttitudeEstimator {
public:
    explicit AttitudeEstimator(std::unique_ptr<Filter> f) : filter_(std::move(f)) {}
    void feed(double raw) { last_ = filter_->update(raw); }
    double value() const { return last_; }
    const char* filterName() const { return filter_->name(); }
    void swapFilter(std::unique_ptr<Filter> f) {
        f->reset();
        filter_ = std::move(f);
    }

private:
    std::unique_ptr<Filter> filter_;
    double last_ = 0.0;
};

int main() {
    const double samples[6] = {10.0, 12.0, 30.0, 11.0, 10.5, 10.0};

    AttitudeEstimator est(std::make_unique<LowPassFilter>(0.3));
    std::printf("策略: %s\n", est.filterName());
    for (double s : samples) {
        est.feed(s);
        std::printf("  输入 %5.1f -> 输出 %6.3f\n", s, est.value());
    }

    est.swapFilter(std::make_unique<MovingAverageFilter>(3));
    std::printf("\n换策略: %s\n", est.filterName());
    for (double s : samples) {
        est.feed(s);
        std::printf("  输入 %5.1f -> 输出 %6.3f\n", s, est.value());
    }

    est.swapFilter(std::make_unique<PassthroughFilter>());
    std::printf("\n换策略: %s\n", est.filterName());
    est.feed(42.0);
    std::printf("  输入  42.0 -> 输出 %6.3f\n", est.value());

    std::printf("\n优点：调用方（AttitudeEstimator）完全不知道算法的存在，运行期可换；\n");
    std::printf("代价：每个虚调用一次间接跳转，且对象要堆分配。\n");
    std::printf("      控制循环 1kHz 时，虚调用的开销通常可以忽略；\n");
    std::printf("      但如果这段在 10kHz 中断里，就要考虑例 24-2 的静态多态了。\n");
    return 0;
}
