// ch17_ex09.cpp —— std::function 与类型擦除
#include <cstdio>
#include <functional>
#include <map>
#include <string>
#include <vector>

int free_func(int x) { return x + 1; }

struct Functor {
    int k;
    int operator()(int x) const { return x * k; }
};

// 一个"注册回调"的典型接口：用 std::function 就能接受任何可调用对象
class Button {
public:
    using Handler = std::function<void(const std::string&)>;
    void on_click(Handler h) { handlers_.push_back(std::move(h)); }
    void click(const std::string& name) const
    {
        for (const auto& h : handlers_) {
            h(name);                       // 不管它原本是什么，都能这么调
        }
    }
private:
    std::vector<Handler> handlers_;
};

// 计算器：用一个 map 存不同运算 —— 类型擦除的经典用法
class Calculator {
public:
    void set(const std::string& op, std::function<double(double, double)> f)
    {
        ops_[op] = std::move(f);
    }
    double run(const std::string& op, double a, double b) const
    {
        auto it = ops_.find(op);
        return it == ops_.end() ? 0.0 : it->second(a, b);
    }
private:
    std::map<std::string, std::function<double(double, double)>> ops_;
};

int main()
{
    std::printf("--- 1) std::function 能装四种东西 ---\n");
    {
        std::function<int(int)> f1 = free_func;                       // 1) 普通函数
        int base = 10;
        std::function<int(int)> f2 = [base](int x) { return x + base; };  // 2) 带捕获的 lambda
        std::function<int(int)> f3 = Functor{ 3 };                    // 3) 函数对象
        std::function<int(int)> f4 = std::negate<int>();               // 4) 标准库函数对象

        std::printf("    f1(5) 普通函数     = %d\n", f1(5));
        std::printf("    f2(5) 带捕获 lambda= %d\n", f2(5));
        std::printf("    f3(5) 函数对象     = %d\n", f3(5));
        std::printf("    f4(5) negate       = %d\n", f4(5));
        std::printf("    它们的类型都是 std::function<int(int)> —— 这就是\"类型擦除\"\n");
    }

    std::printf("--- 2) 对比 auto：auto 保留真实类型，function 擦掉它 ---\n");
    {
        auto lam = [](int x) { return x * 2; };
        std::function<int(int)> fn = lam;

        std::printf("    sizeof(lambda)           = %zu（无捕获，最小 1 字节）\n", sizeof(lam));
        std::printf("    sizeof(std::function)    = %zu（典型实现：一个指针 + 一个内部缓冲）\n",
                    sizeof(fn));
        int captured = 7;
        auto lam2 = [captured](int x) { return x + captured; };
        std::function<int(int)> fn2 = lam2;
        std::printf("    sizeof(带捕获 lambda)    = %zu\n", sizeof(lam2));
        std::printf("    sizeof(对应的 function)  = %zu\n", sizeof(fn2));
        std::printf("    function 更重（可能堆分配），但可以存进容器、可以当参数类型\n");
    }

    std::printf("--- 3) 空 function 的陷阱 ---\n");
    {
        std::function<int(int)> f;                 // 默认构造：空的
        std::printf("    默认构造后 (bool)f = %d\n", f ? 1 : 0);
        std::printf("    调用空的 std::function 会抛 std::bad_function_call：\n");
        try {
            std::printf("    f(1) = %d\n", f(1));
        }
        catch (const std::bad_function_call& e) {
            std::printf("    捕获到 std::bad_function_call: %s\n", e.what());
        }
        f = free_func;
        std::printf("    赋值之后 (bool)f = %d，f(1) = %d\n", f ? 1 : 0, f(1));
    }

    std::printf("--- 4) 实战一：注册回调（事件/中断服务）---\n");
    {
        Button btn;
        btn.on_click([](const std::string& n) {
            std::printf("      [日志] 按钮 %s 被点击\n", n.c_str());
        });
        int click_count = 0;
        btn.on_click([&click_count](const std::string&) {
            ++click_count;
            std::printf("      [计数] 这是第 %d 次点击\n", click_count);
        });
        btn.on_click([](const std::string& n) {
            std::printf("      [业务] 处理 %s 的动作\n", n.c_str());
        });
        btn.click("OK");
        std::printf("    三个回调按注册顺序执行，彼此类型完全不同\n");
    }

    std::printf("--- 5) 实战二：把函数当数据存进 map（策略表）---\n");
    {
        Calculator calc;
        calc.set("+", [](double a, double b) { return a + b; });
        calc.set("-", [](double a, double b) { return a - b; });
        calc.set("*", [](double a, double b) { return a * b; });
        calc.set("/", [](double a, double b) { return b != 0 ? a / b : 0.0; });

        std::printf("    10 + 3 = %.2f\n", calc.run("+", 10, 3));
        std::printf("    10 - 3 = %.2f\n", calc.run("-", 10, 3));
        std::printf("    10 * 3 = %.2f\n", calc.run("*", 10, 3));
        std::printf("    10 / 3 = %.2f\n", calc.run("/", 10, 3));
        std::printf("    10 / 0 = %.2f（lambda 里处理了除零）\n", calc.run("/", 10, 0));
        std::printf("    这就是\"用数据驱动行为\"—— 不用写 switch/case\n");
    }

    std::printf("--- 6) 实战三：把 function 放进 vector 做\"命令队列\" ---\n");
    {
        std::vector<std::function<void()>> queue;
        queue.push_back([] { std::printf("      命令 1: 初始化\n"); });
        queue.push_back([] { std::printf("      命令 2: 采集\n"); });
        queue.push_back([] { std::printf("      命令 3: 上报\n"); });
        std::printf("    队列里有 %zu 个命令，依次执行：\n", queue.size());
        for (auto& cmd : queue) cmd();
        queue.clear();
        std::printf("    清空后大小 = %zu\n", queue.size());
    }

    std::printf("--- 7) 类型擦除的代价与替代方案 ---\n");
    {
        std::printf("    代价：\n");
        std::printf("      - 多一层间接调用（通常不能内联）\n");
        std::printf("      - 对象更大（MSVC 上 64 字节左右），小 lambda 走内部缓冲，大的会堆分配\n");
        std::printf("      - 可能抛异常（空调用）\n");
        std::printf("    嵌入式替代方案：\n");
        std::printf("      - 如果类型在编译期已知：用模板参数（零开销）\n");
        std::printf("      - 如果只是一组固定操作：用函数指针（8 字节，无分配）\n");
        std::printf("      - 如果是中断/回调且数量固定：用一个静态数组存函数指针\n");
        std::printf("      - C++20 起可以用 std::move_only_function（可装只能移动的对象）\n");
    }

    return 0;
}
