// ch13_ex17.cpp —— 组合优于继承：一个可运行的对照
// 编译：check.bat ch13_ex17.cpp c++latest

// 需求：一个"飞控日志器"，要能输出到控制台/文件，还要能加时间戳。
// 写法一：用继承把功能堆起来（类数量爆炸、改一处动全身）
// 写法二：用组合 + 接口（每个功能一个独立小类，随取随用）
#include <cstdio>
#include <cstring>

// -------------------- 写法二用到的接口 --------------------
class Sink {                                  // 输出目标（抽象）
public:
    virtual void write(const char* line) = 0;
    virtual ~Sink() = default;
};

class ConsoleSink : public Sink {
public:
    void write(const char* line) override { std::printf("   [控制台] %s\n", line); }
};

class MemorySink : public Sink {               // 收集到内存里，便于测试
public:
    void write(const char* line) override
    {
        std::snprintf(buf_ + used_, sizeof(buf_) - used_, "%s|", line);
        used_ += (int)std::strlen(line) + 1;
    }
    const char* all() const { return buf_; }
private:
    char buf_[256] = { 0 };
    int  used_ = 0;
};

// -------------------- 写法二的"组合"版本 --------------------
class Logger {                                 // 只做一件事：把行交给 sink
public:
    explicit Logger(Sink* sink) : sink_(sink) {}
    void log(const char* msg)
    {
        char line[128];
        std::snprintf(line, sizeof(line), "%s", msg);
        sink_->write(line);
    }
    void set_sink(Sink* s) { sink_ = s; }      // 运行期随时换输出目标
private:
    Sink* sink_;                               // 组合：持有接口指针，不继承实现
};

// 时间戳可以再套一层装饰（这里用最简单的方式演示"叠加"）
class PrefixedSink : public Sink {
public:
    PrefixedSink(const char* prefix, Sink* next) : next_(next)
    {
        std::snprintf(prefix_, sizeof(prefix_), "%s", prefix);
    }
    void write(const char* line) override
    {
        char tmp[160];
        std::snprintf(tmp, sizeof(tmp), "%s%s", prefix_, line);
        next_->write(tmp);
    }
private:
    Sink* next_;
    char  prefix_[16];
};

int main()
{
        // 1) 组合：日志器 + 控制台输出
    ConsoleSink console;
    Logger logger(&console);
    logger.log("imu ok");

        // 2) 运行期换成内存收集（不用改 Logger 一行代码）
    MemorySink mem;
    logger.set_sink(&mem);
    logger.log("baro ok");
    logger.log("gps ok");
    std::printf("1) 内存里的内容: %s\n", mem.all());

        // 3) 叠加前缀：装饰器模式（还是组合）
    PrefixedSink prefixed("[T=1.23] ", &console);
    logger.set_sink(&prefixed);
    logger.log("attitude 12.5");

    std::printf("4) 为什么组合更好：\n");
    std::printf("   - 继承是编译期绑定：ConsoleLoggerWithTime / FileLoggerWithTime 这类类会爆炸\n");
    std::printf("   - 组合是运行期可换：一个 Logger 可以配任意 Sink\n");
    std::printf("   - 继承的层次一深，改基类会影响所有后代（脆弱基类问题）\n");
    std::printf("   - 经验：接口继承（is-a）可以，实现继承尽量换成组合（has-a）\n");
    std::printf("   - 详见《C++ Core Guidelines》C.129 / C.130 一节的讨论\n");
    return 0;
}
