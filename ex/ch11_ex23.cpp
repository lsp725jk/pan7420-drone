// ch11_ex23.cpp —— enum class 实战：模式机、switch、位标志
// 编译：check.bat ch11_ex23.cpp c++latest
#include <cstdio>

// 状态机：底层类型用 unsigned char，省内存（单片机场景）
enum class Mode : unsigned char { Idle, Calib, Run, Failsafe };

// 位标志：用 unsigned 做底层，配合 1u << n 的写法
enum class Flag : unsigned {
    None    = 0u,
    Armed   = 1u << 0,
    Logging = 1u << 1,
    GpsFix  = 1u << 2,
};

// 位运算要自己写（enum class 不自动支持）
constexpr Flag operator|(Flag a, Flag b)
{
    return static_cast<Flag>(static_cast<unsigned>(a) | static_cast<unsigned>(b));
}
constexpr bool has_flag(Flag all, Flag one)
{
    return (static_cast<unsigned>(all) & static_cast<unsigned>(one)) != 0u;
}

const char* mode_name(Mode m)
{
    switch (m) {                       // switch 里可以直接用枚举名
    case Mode::Idle:     return "Idle";
    case Mode::Calib:    return "Calib";
    case Mode::Run:      return "Run";
    case Mode::Failsafe: return "Failsafe";
    }
    return "?";
}

int main()
{
        // 1) 状态机
    Mode m = Mode::Calib;
    std::printf("1) mode=%s, sizeof(Mode)=%llu\n",
                mode_name(m), (unsigned long long)sizeof(Mode));

        // 2) 遍历所有取值（enum class 没有内建遍历，需要自己写数组或递增）
    std::printf("2) 全部模式: ");
    for (int i = 0; i <= (int)Mode::Failsafe; ++i)
        std::printf("%s ", mode_name(static_cast<Mode>(i)));
    std::printf("\n");

        // 3) 位标志组合
    Flag f = Flag::Armed | Flag::GpsFix;
    std::printf("3) raw=%u, Armed=%d, Logging=%d, GpsFix=%d\n",
                (unsigned)f, has_flag(f, Flag::Armed),
                has_flag(f, Flag::Logging), has_flag(f, Flag::GpsFix));

        // 4) 从整数恢复枚举：本枚举有固定底层类型（unsigned char），
// 所以在底层类型范围（0~255）内的转换是**良定义**的，不是 UB。
// 仍然建议校验，理由不是「避免 UB」，而是「合法值 ≠ 有意义的业务值」。
    int raw = 42;
    if (raw >= 0 && raw <= (int)Mode::Failsafe) {
        Mode safe = static_cast<Mode>(raw);
        std::printf("4) 校验后转换: %s\n", mode_name(safe));
    } else {
        std::printf("4) raw=42 超出 Mode 范围，拒绝转换"
                "（本枚举有固定底层类型，转换本身良定义；拒绝是因为 42 不代表任何状态）\n");
    }

    return 0;
}
