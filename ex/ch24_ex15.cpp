// ch24_ex15.cpp —— 例 24-15：从 assert 到一个 30 行的测试框架
// assert 是"我确信这里一定成立"的断言，不是错误处理。
// 单元测试是把这些"确信"固定下来，每次改代码自动检查一遍。
#include <cassert>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

// ==================================================================
// 被测代码：一个 PID 控制器
// ==================================================================
class Pid {
public:
    Pid(double kp, double ki, double kd) : kp_(kp), ki_(ki), kd_(kd) {}

    double update(double error, double dt) {
        integral_ += error * dt;
        const double deriv = (error - prev_) / dt;
        prev_ = error;
        return kp_ * error + ki_ * integral_ + kd_ * deriv;
    }
    void reset() {
        integral_ = 0.0;
        prev_ = 0.0;
    }

private:
    double kp_, ki_, kd_;
    double integral_ = 0.0;
    double prev_ = 0.0;
};

// ==================================================================
// 极简测试框架（30 行，够用了）
// ==================================================================
namespace tinytest {
int g_checks = 0;
int g_failed = 0;

void check(bool ok, const char* expr, const char* file, int line) {
    ++g_checks;
    if (!ok) {
        ++g_failed;
        std::printf("  [失败] %s:%d  %s\n", file, line, expr);
    }
}

template <typename A, typename B>
void check_eq(const A& a, const B& b, const char* expr, const char* file, int line) {
    check(a == b, expr, file, line);
}

int report() {
    std::printf("\n共 %d 项检查，失败 %d 项\n", g_checks, g_failed);
    return g_failed == 0 ? 0 : 1;
}
}  // namespace tinytest

#define CHECK(expr) tinytest::check((expr), #expr, __FILE__, __LINE__)
#define CHECK_EQ(a, b) tinytest::check_eq((a), (b), #a " == " #b, __FILE__, __LINE__)

// ---- 精确比较用 ==，浮点比较要允许误差 ----
bool near(double a, double b, double eps = 1e-9) {
    return std::fabs(a - b) < eps;
}

// ==================================================================
// 测试用例
// ==================================================================
void test_proportional_only() {
    std::printf("test_proportional_only\n");
    Pid pid(2.0, 0.0, 0.0);  // 只有 P
    CHECK_EQ(pid.update(3.0, 0.1), 6.0);  // 2.0 * 3.0
}

void test_integral_accumulates() {
    std::printf("test_integral_accumulates\n");
    Pid pid(0.0, 1.0, 0.0);  // 只有 I
    const double a = pid.update(1.0, 0.1);  // 积分 = 0.1
    const double b = pid.update(1.0, 0.1);  // 积分 = 0.2
    CHECK(near(a, 0.1));
    CHECK(near(b, 0.2));
    std::printf("  第一次输出 %.6f，第二次输出 %.6f\n", a, b);
}

void test_reset_clears_state() {
    std::printf("test_reset_clears_state\n");
    Pid pid(0.0, 1.0, 0.0);
    pid.update(5.0, 0.1);
    pid.reset();
    CHECK(near(pid.update(1.0, 0.1), 0.1));  // reset 之后积分应该从 0 重新开始
}

int main() {
    std::printf("=== 运行测试 ===\n");
    test_proportional_only();
    test_integral_accumulates();
    test_reset_clears_state();

    std::printf("\n=== assert 的定位 ===\n");
        // assert 用来表达"程序内部的不变量"，不是用来处理外部输入的
    Pid pid(1.0, 0.0, 0.0);
    const double dt = 0.01;
    assert(dt > 0.0);  // 这是内部约定：调用者必须给正的时间步长
    pid.update(1.0, dt);
    std::printf("assert(dt > 0.0) 通过\n");

    std::printf("\nassert 和错误处理的区别：\n");
    std::printf("  assert     -> 检查【不可能发生】的事（自己的 bug）\n");
    std::printf("                失败就 abort；NDEBUG 下会被去掉，所以不能有副作用\n");
    std::printf("  错误处理   -> 检查【可能发生】的事（用户输入、硬件故障、通信错误）\n");
    std::printf("                必须始终有效，用错误码/optional/异常\n");
    std::printf("\n坑：绝对不要写 assert(do_something());\n");
    std::printf("   因为 Release 构建里这行会被整个删掉，do_something() 就不执行了。\n");

    const int rc = tinytest::report();
    std::printf("（把失败项改成 0 就是绿，返回码 %d 可以直接被 CI 用）\n", rc);
    return rc;
}
