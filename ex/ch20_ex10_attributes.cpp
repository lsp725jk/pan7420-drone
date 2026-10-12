// ch20_ex10_attributes.cpp —— 属性（attributes）：[[nodiscard]] / [[maybe_unused]] 等
// 编译: check.bat ch20_ex10_attributes.cpp c++latest
// 注意：本文件故意使用一些"会被编译器警告"的写法，请看编译输出里的 warning。
#include <iostream>
#include <string>
#include <vector>

// ============================================================
// A. [[nodiscard]]：返回值不许被丢掉
// ============================================================
[[nodiscard]] int readSensorRaw()
{
    return 4095;
}

// 带说明文字的版本（C++20），警告信息里会带上这句话
[[nodiscard("忽略设备初始化结果会导致后续未定义行为")]]
bool initDevice()
{
    return true;
}

// 也可以标在整个类型上：凡是返回这个类型的函数都自动 nodiscard
struct [[nodiscard]] ErrorCode {
    int code;
};

ErrorCode openPort() { return ErrorCode{0}; }

// ============================================================
// B. [[maybe_unused]]：我知道它没用到，别警告
// ============================================================
void logLevels([[maybe_unused]] int verboseLevel)
{
#ifdef DEBUG_BUILD
    std::cout << "  verbose = " << verboseLevel << "\n";
#endif
    std::cout << "  logLevels 被调用（verboseLevel 在 Release 下用不到）\n";
}

// ============================================================
// C. [[fallthrough]]：switch 里故意不 break
// ============================================================
std::string classify(int x)
{
    std::string s;
    switch (x) {
    case 0:
        s += "zero ";
        [[fallthrough]];                 // 明确告诉编译器和读者：我是故意的
    case 1:
        s += "small ";
        break;
    default:
        s += "big ";
        break;
    }
    return s;
}

// ============================================================
// D. [[deprecated]] 与 [[noreturn]]
// ============================================================
[[deprecated("请改用 readSensorRaw()，旧接口在 v2.0 会被删除")]]
int readSensorOld() { return 4095; }

[[noreturn]] void fatalError(const std::string& msg)
{
    std::cerr << "fatal: " << msg << "\n";
    std::exit(1);                        // 真的不返回，编译器可以据此优化
}

int main()
{
    std::cout << "=== A. [[nodiscard]]：编译期抓\"忘了接收返回值\" ===\n";
    int v = readSensorRaw();             // 正确：接收了
    std::cout << "  readSensorRaw() -> " << v << "\n";
    std::cout << "  如果写 readSensorRaw(); 不接收返回值：\n";
    std::cout << "    warning C4834: 放弃具有 \"nodiscard\" 属性的函数的返回值\n";
    std::cout << "  initDevice() 不接收 -> warning 里会带上你写的那句话：\n";
    std::cout << "    warning C4858: 忽略设备初始化结果会导致后续未定义行为\n";
    std::cout << "  ErrorCode 类型标了 nodiscard，所以 openPort(); 也会警告。\n";
    std::cout << "  这个属性价值极高：硬件初始化/写寄存器失败是最容易被忽略的错误。\n";

    std::cout << "\n=== B. [[maybe_unused]] ===\n";
    logLevels(3);
    std::cout << "  没有这个属性时，Release 构建会报 warning C4100（未引用的形参），\n";
    std::cout << "  于是大家只好写 (void)verboseLevel; —— 现在有正规写法了。\n";

    std::cout << "\n=== C. [[fallthrough]] ===\n";
    std::cout << "  classify(0) = \"" << classify(0) << "\"\n";
    std::cout << "  classify(1) = \"" << classify(1) << "\"\n";
    std::cout << "  classify(9) = \"" << classify(9) << "\"\n";
    std::cout << "  没有 [[fallthrough]] 时，编译器会报 warning C26819（隐式贯穿）。\n";

    std::cout << "\n=== D. [[deprecated]] 与 [[noreturn]] ===\n";
    std::cout << "  调用 readSensorOld() -> " << readSensorOld() << "\n";
    std::cout << "  编译时会报：warning C4996: \"readSensorOld\": 请改用 readSensorRaw()，\n";
    std::cout << "              旧接口在 v2.0 会被删除\n";
    std::cout << "  这是给库作者做平滑迁移的工具，比写注释靠谱 —— 注释没人看，警告会拦人。\n";
    std::cout << "  [[noreturn]] 让编译器知道 fatalError 之后不会继续执行，\n";
    std::cout << "  从而不再警告\"缺少 return\"，还能省掉多余的清理代码。\n";

    std::cout << "\n=== E. 常用属性一览 ===\n";
    std::cout << "  [[nodiscard]]       返回值不可丢弃 —— 最值得用\n";
    std::cout << "  [[maybe_unused]]    我知道没用，别警告\n";
    std::cout << "  [[fallthrough]]     switch 故意贯穿\n";
    std::cout << "  [[deprecated]]      标记废弃接口\n";
    std::cout << "  [[noreturn]]        函数不返回\n";
    std::cout << "  [[likely]] / [[unlikely]]  分支预测提示（C++20）\n";
    std::cout << "  [[no_unique_address]]      空成员不占空间（C++20，嵌入式省 RAM 有用）\n";
    std::cout << "  嵌入式提醒：这些属性都是\"给编译器的建议/约束\"，\n";
    std::cout << "              全部零运行时开销，可以放心用在 Cortex-M0+ 上。\n";

    std::cout << "\nmain 正常结束\n";
    return 0;
}
