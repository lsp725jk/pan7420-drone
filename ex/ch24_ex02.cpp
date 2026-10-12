// ch24_ex02.cpp —— 例 24-2：策略模式的三种变式，什么时候用哪个
// 变式 1：虚函数（例 24-1）—— 运行期可换，有间接调用开销
// 变式 2：模板参数（编译期策略）—— 零开销，但类型固定
// 变式 3：std::function / 函数对象 —— 折中，能存 lambda 和闭包
#include <cstdio>
#include <functional>
#include <memory>

// ==================================================================
// 变式 2：模板策略（编译期多态，也叫"策略作为模板参数"）
// ==================================================================
struct LowPass {
    double operator()(double x) const { return x * 0.5; }
};
struct Gain2x {
    double operator()(double x) const { return x * 2.0; }
};

template <typename Policy>
class Pipeline {
public:
    explicit Pipeline(Policy p) : policy_(p) {}
    double run(double x) { return policy_(x); }

private:
    Policy policy_;
};

// ==================================================================
// 变式 3：std::function（运行期可换，且能带状态/闭包）
// ==================================================================
class PipelineFn {
public:
    explicit PipelineFn(std::function<double(double)> f) : f_(std::move(f)) {}
    double run(double x) const { return f_(x); }

private:
    std::function<double(double)> f_;
};

// ==================================================================
// 变式 3b：C 风格函数指针（最小开销的运行期可换）
// ==================================================================
using FnPtr = double (*)(double);
class PipelineRaw {
public:
    explicit PipelineRaw(FnPtr f) : f_(f) {}
    double run(double x) const { return f_(x); }

private:
    FnPtr f_;
};
double raw_lowpass(double x) { return x * 0.5; }

int main() {
        // ---- 变式 2 ----
    Pipeline<LowPass> p1{LowPass{}};
    Pipeline<Gain2x> p2{Gain2x{}};
    std::printf("模板策略:\n");
    std::printf("  Pipeline<LowPass>::run(10) = %g\n", p1.run(10.0));
    std::printf("  Pipeline<Gain2x>::run(10)  = %g\n", p2.run(10.0));
    std::printf("  sizeof(Pipeline<LowPass>)=%zu（策略是空类，被优化成 0 开销）\n",
                sizeof(Pipeline<LowPass>));

        // ---- 变式 3：lambda + 捕获 ----
    double k = 3.0;
    PipelineFn p3{[k](double x) { return x * k; }};  // 闭包捕获 k
    std::printf("\nstd::function + 捕获变量的 lambda:\n");
    std::printf("  run(10) = %g（k=%g 被闭包带进来了）\n", p3.run(10.0), k);
    std::printf("  sizeof(PipelineFn)=%zu（比函数指针大得多）\n", sizeof(PipelineFn));

        // ---- 运行期从配置里选 ----
    std::printf("\n运行期切换：\n");
    const int mode = 2;
    if (mode == 1) {
        PipelineFn p{[](double x) { return x * 0.5; }};
        std::printf("  mode=1 -> %g\n", p.run(10.0));
    } else {
        PipelineFn p{[k](double x) { return x * k; }};
        std::printf("  mode=2 -> %g\n", p.run(10.0));
    }

        // ---- 变式 3b ----
    PipelineRaw p4{&raw_lowpass};
    std::printf("\n函数指针: run(10) = %g, sizeof=%zu\n", p4.run(10.0), sizeof(PipelineRaw));

    std::printf("\n怎么选：\n");
    std::printf("  编译期就定好、追求极致性能        -> 模板策略（变式 2）\n");
    std::printf("  需要捕获状态、想写 lambda         -> std::function（变式 3）\n");
    std::printf("  只有一层逻辑、不想堆分配          -> 函数指针（变式 3b）\n");
    std::printf("  策略有多个方法、是个【对象】       -> 虚函数（例 24-1）\n");
    std::printf("\n坑：模板策略会让每个不同的策略生成一份 Pipeline 代码，\n");
    std::printf("   策略多了 Flash 会涨；虚函数则只有一份。\n");
    return 0;
}
