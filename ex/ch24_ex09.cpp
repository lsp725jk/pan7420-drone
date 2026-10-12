// ch24_ex09.cpp —— 例 24-9：类型擦除（type erasure）——"不写模板也能泛型"
// std::function 就是类型擦除的典型：它能装任意可调用对象，
// 但你在用它的时候不需要知道具体类型。
// 这里手写一个简化版，看清它的原理：**虚函数 + 模板构造函数 + 堆上的小壳子**。
#include <cstdio>
#include <memory>
#include <string>
#include <utility>

// ==================================================================
// 手写一个最小的 FunctionRef：装"能被调用的东西"
// ==================================================================
class Callable {
public:
        // 关键：用一个**模板构造函数**吃掉所有具体类型
    template <typename F>
    explicit Callable(F f) : self_(std::make_unique<Model<F>>(std::move(f))) {}

    Callable(Callable&&) noexcept = default;
    Callable& operator=(Callable&&) noexcept = default;

    int operator()(int x) const { return self_->call(x); }

private:
        // 抽象接口：只有虚函数，与 F 无关
    struct Concept {
        virtual ~Concept() = default;
        virtual int call(int) const = 0;
    };

        // 具体的壳子：知道 F 是什么
    template <typename F>
    struct Model final : Concept {
        explicit Model(F f) : f_(std::move(f)) {}
        int call(int x) const override { return f_(x); }
        F f_;
    };

    std::unique_ptr<Concept> self_;  // 唯一的成员，类型与 F 无关
};

// ==================================================================
// 用类型擦除写一个"通用传感器读取器"
// 要求：能装函数指针、lambda、仿函数；调用方不关心具体是哪种
// ==================================================================
struct ReadResult {
    int raw;
    std::string who;
};

class SensorReader {
public:
        // F 是"int -> int"的任意可调用对象；ReadResult 只是给调用者看的包装
    template <typename F>
    explicit SensorReader(const char* name, F f)
        : reader_([f](int ch) { return f(ch); }), name_(name) {}

        // 对外仍然可以返回更丰富的结果，类型擦除把 f 藏在了 reader_ 里
    ReadResult read_full(int channel) const { return ReadResult{reader_(channel), name_}; }
    int read(int channel) const { return reader_(channel); }
    const char* name() const { return name_.c_str(); }

private:
    Callable reader_;  // 类型与 F 无关
    std::string name_;
};

int main() {
        // 装函数对象
    struct Doubler {
        int operator()(int x) const { return x * 2; }
    };
    Callable c1{Doubler{}};
    std::printf("装仿函数   : c1(21) = %d\n", c1(21));

        // 装 lambda（带捕获）
    int offset = 100;
    Callable c2{[offset](int x) { return x + offset; }};
    std::printf("装 lambda  : c2(21) = %d（捕获了 offset=%d）\n", c2(21), offset);

        // 装函数指针
    Callable c3{+[](int x) { return x * x; }};
    std::printf("装函数指针 : c3(21) = %d\n", c3(21));

        // 它们的**类型完全相同**
    std::printf("sizeof(Callable) = %zu（不管装什么，都一样大）\n", sizeof(Callable));

    std::printf("\n标准库里的类型擦除例子：\n");
    std::printf("  std::function<int(int)>   —— 可调用对象\n");
    std::printf("  std::any                  —— 任意类型\n");
    std::printf("  std::shared_ptr<T> 的删除器 —— 擦除删除器的类型\n");
    std::printf("  std::coroutine_handle<>   —— 擦除协程的 promise 类型（C++20）\n");

    std::printf("\n怎么做出来的（三步）：\n");
    std::printf("  1) 定义一个不含模板参数的抽象基类（Concept）\n");
    std::printf("  2) 写一个模板派生类（Model<F>），把具体类型存进去\n");
    std::printf("  3) 外层类的模板构造函数里 new 一个 Model<F>，用基类指针持有\n");

    std::printf("\n权衡：\n");
    std::printf("  好处：接口统一、不用把模板写进头文件、可以放进容器\n");
    std::printf("  代价：一次堆分配 + 一次虚调用（比模板慢，比裸函数指针也慢）\n");
    std::printf("        => 热路径别用；冷路径（初始化、配置、事件分发）很合适\n");
    std::printf("\n嵌入式提示：小对象可以不堆分配，用 SBO（小缓冲优化）\n");
    std::printf("  在类里留一块固定大小的 buffer，够放就放里面（std::function 一般也这么做）。\n");
    return 0;
}
