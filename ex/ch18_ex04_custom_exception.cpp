// ch18_ex04_custom_exception.cpp —— 自定义异常：带上错误码和上下文
// 编译: check.bat ch18_ex04_custom_exception.cpp c++latest
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

// ---- 错误码枚举：给机器看的 ----
enum class SensorErr {
    None = 0,
    Timeout,
    CrcFail,
    OutOfRange,
};

const char* toString(SensorErr e)
{
    switch (e) {
    case SensorErr::None:       return "无错误";
    case SensorErr::Timeout:    return "超时";
    case SensorErr::CrcFail:    return "校验失败";
    case SensorErr::OutOfRange: return "数值超范围";
    }
    return "未知";
}

// ---- 自定义异常：继承 std::runtime_error ----
// 为什么继承 runtime_error 而不是 exception？
// 1) 它已经帮你实现了 what() 和拷贝构造；
// 2) 想区分"逻辑错误"和"运行时错误"时，把类别也带上了；
// 3) 任何只写 catch(std::exception&) 的地方照样能兜住。
class SensorError : public std::runtime_error {
public:
    SensorError(SensorErr code, int rawValue)
        : std::runtime_error(buildMessage(code, rawValue))   // 先构造基类
        , code_(code)
        , raw_(rawValue)
    {}

    SensorErr code() const noexcept { return code_; }   // 机器可判别的错误码
    int rawValue() const noexcept { return raw_; }      // 现场数据，方便打日志

private:
    static std::string buildMessage(SensorErr code, int rawValue)
    {
        return std::string("传感器错误[") + toString(code) + "] 原始值=" + std::to_string(rawValue);
    }
    SensorErr code_;
    int raw_;
};

// 一个会抛自定义异常的读取函数
int readSensor(int attempt)
{
    if (attempt == 0) throw SensorError(SensorErr::Timeout, -1);
    if (attempt == 1) throw SensorError(SensorErr::CrcFail, 0x5A3C);
    if (attempt == 2) throw SensorError(SensorErr::OutOfRange, 4095);
    return 100 + attempt;
}

int main()
{
    std::cout << "=== A. 精确捕获，取出错误码分支处理 ===\n";
    for (int i = 0; i < 3; ++i) {
        try {
            std::cout << "  第 " << i << " 次读取 -> " << readSensor(i) << "\n";
        }
        catch (const SensorError& e) {                 // 按引用捕获！
            std::cout << "  异常: " << e.what() << "\n";
            switch (e.code()) {
            case SensorErr::Timeout:
                std::cout << "    处理策略: 重试\n"; break;
            case SensorErr::CrcFail:
                std::cout << "    处理策略: 丢弃这一帧\n"; break;
            case SensorErr::OutOfRange:
                std::cout << "    处理策略: 标定或裁剪到 " << e.rawValue() << "\n"; break;
            default: break;
            }
        }
    }

    std::cout << "\n=== B. 派生类能被基类 catch 捕获（多态）===\n";
    try {
        readSensor(0);
    }
    catch (const std::runtime_error& e) {     // 用基类捕获派生类，OK
        std::cout << "  作为 runtime_error 捕获: " << e.what() << "\n";
    }

    std::cout << "\n=== C. 按值捕获会发生对象切片，错误信息丢失 ===\n";
    try {
        try {
            readSensor(1);
        }
        catch (SensorError e) {               // 反例：按值捕获 —— 会拷贝，且类型信息被截断
            std::cout << "  按值捕获: " << e.what() << "  code=" << static_cast<int>(e.code()) << "\n";
            throw;                            // 重新抛出原始异常对象（不是这个副本）
        }
    }
    catch (const std::exception& e) {
        std::cout << "  外层仍看到原始类型信息: " << e.what() << "\n";
    }

    std::cout << "\n=== D. catch 顺序：派生类必须写在基类前面 ===\n";
    auto test = [](int which) {
        try {
            if (which == 0) throw SensorError(SensorErr::Timeout, 7);
            throw std::runtime_error("普通运行时错误");
        }
        catch (const SensorError& e) {                    // 派生在前 ✔
                        // 注意：enum class 不会隐式转成 int，要显式 static_cast
            std::cout << "  命中 SensorError: " << static_cast<int>(e.code()) << "\n";
        }
        catch (const std::runtime_error& e) {              // 基类在后 ✔
            std::cout << "  命中 runtime_error: " << e.what() << "\n";
        }
    };
    test(0);
    test(1);
    std::cout << "  （若把两个 catch 调换顺序，SensorError 会被基类抢走，\n"
                 "    编译器还会给 warning C4286）\n";

    std::cout << "\nmain 正常结束\n";
    return 0;
}
