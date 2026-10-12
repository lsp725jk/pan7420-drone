// ch13_ex09.cpp —— 纯虚函数的变式：接口类、纯虚析构、虚函数带默认实现
// 编译：check.bat ch13_ex09.cpp c++latest
#include <cstdio>

// ---- 变式 1：纯虚析构函数（有函数体，但类仍然是抽象的）----
class AbstractBase {
public:
    virtual void run() = 0;
    virtual ~AbstractBase() = 0;          // 声明为纯虚
};
AbstractBase::~AbstractBase()             // 但必须提供定义（派生类析构会调用它）
{
    std::printf("  AbstractBase 的纯虚析构函数体被执行了\n");
}

class Worker : public AbstractBase {
public:
    void run() override { std::printf("  Worker::run()\n"); }
    ~Worker() override { std::printf("  Worker 析构\n"); }
};

// ---- 变式 2：接口类（只有纯虚函数 + 虚析构，没有数据成员）----
class ILogger {
public:
    virtual void log(const char* msg) = 0;
    virtual ~ILogger() = default;
};

class ConsoleLogger : public ILogger {
public:
    void log(const char* msg) override { std::printf("  [控制台] %s\n", msg); }
};

class NullLogger : public ILogger {       // 空实现：测试时不想看日志就用它
public:
    void log(const char*) override { }
};

// ---- 变式 3：纯虚函数也可以有函数体（C++ 允许），派生类可以显式调用它 ----
class WithDefault {
public:
    virtual void step() = 0;              // 纯虚
};
void WithDefault::step()                  // 提供默认实现（不是纯虚独占的）
{
    std::printf("  WithDefault::step() 默认实现\n");
}
class UseDefault : public WithDefault {
public:
    void step() override
    {
        WithDefault::step();              // 显式调用基类实现
        std::printf("  UseDefault::step() 的自己那部分\n");
    }
};

int main()
{
        // 1) 纯虚析构：类仍是抽象的，但析构函数体照样会被调用
    std::printf("1) 纯虚析构函数实验：\n");
    {
        Worker w;
        w.run();
    }

        // 2) 接口类的多态使用
    std::printf("2) 接口类（依赖注入的样子）：\n");
    ConsoleLogger cl;
    NullLogger    nl;
    ILogger* logger = &cl;
    logger->log("启动完成");
    logger = &nl;
    logger->log("这行不会显示");

        // 3) 带默认实现的纯虚函数
    std::printf("3) 纯虚函数也能有实现：\n");
    UseDefault u;
    u.step();

    std::printf("4) 要点：\n");
    std::printf("   - 纯虚析构函数必须写定义，否则派生类链接失败\n");
    std::printf("   - 接口类 = 只有纯虚函数 + 虚析构，用来描述这个类型能做什么\n");
    std::printf("   - 纯虚函数带实现可以做默认行为，但派生类必须自己声明 override\n");
    return 0;
}
