// ch21_ex01_constexpr_flash.cpp —— 把计算搬到编译期：省 Flash / RAM / 启动时间
// 编译: check.bat ch21_ex01_constexpr_flash.cpp c++latest
// 说明：本例在 PC 上编译运行，但所有结论都对着 PY32F030（Cortex-M0+，64KB Flash / 8KB RAM）说。
#include <iostream>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstddef>

// ============================================================
// 1. 运行期算：代码在 Flash 里，栈上还要放中间变量
// ============================================================
float sinRuntime(float deg)
{
    return std::sin(deg * 3.14159265358979f / 180.0f);   // 会拖进整个 libm
}

// ============================================================
// 2. 编译期算：表直接以常量形式躺在 .rodata，运行时只读内存
// ============================================================
constexpr std::size_t kTableSize = 91;                   // 0..90 度
constexpr float deg2rad(float d) { return d * 3.14159265358979f / 180.0f; }

// 注意：C++20 起 std::sin 不是 constexpr，所以这里用泰勒/查表法自己写
constexpr float sinSeries(float x)
{
        // 只用到 7 次项，对 0..90 度精度约 1e-4，够做飞控限幅
    const float x2 = x * x;
    return x * (1.0f - x2 / 6.0f * (1.0f - x2 / 20.0f * (1.0f - x2 / 42.0f)));
}

constexpr std::array<std::int16_t, kTableSize> makeSinTable()
{
    std::array<std::int16_t, kTableSize> t{};
    for (std::size_t i = 0; i < kTableSize; ++i) {
                // 放大 10000 倍存成整数：M0+ 没有 FPU，整数查表最快
        t[i] = static_cast<std::int16_t>(sinSeries(deg2rad(static_cast<float>(i))) * 10000.0f);
    }
    return t;
}
constexpr auto kSinTable = makeSinTable();               // 编译期生成

// ============================================================
// 3. 编译期校验：把"算错了"变成编译失败
// ============================================================
static_assert(kSinTable[0]  == 0,     "sin(0) 必须是 0");
static_assert(kSinTable[30] > 4990 && kSinTable[30] < 5010, "sin(30) 应该约等于 0.5");
static_assert(kSinTable[90] > 9990,   "sin(90) 应该约等于 1");
static_assert(kTableSize == 91,       "表长必须正好 91 项");

// ============================================================
// 4. 用编译期常量做数组长度：RAM 占用一目了然
// ============================================================
struct PidState {
    float kp, ki, kd;
    float integral;
    float lastError;
};
constexpr std::size_t kAxisCount = 3;                     // roll / pitch / yaw
// 用 std::array 而不是 C 数组：长度是类型的一部分，sizeof 精确已知

int main()
{
    std::cout << "=== A. 编译期表 vs 运行期计算 ===\n";
    std::cout << "  查表: sin(30) = " << kSinTable[30] / 10000.0f << "（表里存的是 "
              << kSinTable[30] << "/10000）\n";
    std::cout << "  实算: sin(30) = " << sinRuntime(30.0f) << "\n";
    std::cout << "  误差 = " << (kSinTable[30] / 10000.0f - sinRuntime(30.0f)) << "\n";
    std::cout << "  表在二进制的 .rodata 段里，运行时不执行任何指令生成它。\n";
    std::cout << "  代价 = " << sizeof(kSinTable) << " 字节 Flash（"
              << kTableSize << " 项 × 2 字节 int16）\n";

    std::cout << "\n=== B. 换算成 PY32F030 的账（64KB Flash / 8KB RAM）===\n";
    std::cout << "  方案 1（运行期调 sinf）：\n";
    std::cout << "    - 链接进 libm 的 sinf 实现，典型 1~3KB Flash（含常量表）；\n";
    std::cout << "    - 每次调用要几十到上百条指令，M0+ 无 FPU，走软件浮点；\n";
    std::cout << "    - 软件浮点还要再拖进 __aeabi_fmul / fdiv 等运行时，又 1~2KB。\n";
    std::cout << "  方案 2（编译期整数查表，本例）：\n";
    std::cout << "    - Flash 只多 " << sizeof(kSinTable) << " 字节；\n";
    std::cout << "    - 运行时是 \"取下标 + 读 int16\"，约 4~6 条指令；\n";
    std::cout << "    - RAM 占用 0（constexpr 表是只读的）。\n";
    std::cout << "  在 8KB RAM 的芯片上，这个差别就是\"能跑\"和\"跑不动\"的区别。\n";

    std::cout << "\n=== C. 哪些东西能搬到编译期 ===\n";
    std::cout << "  ✔ 三角函数/开方/指数表（查表代替计算）\n";
    std::cout << "  ✔ CRC 表、FFT 旋转因子表、滤波器系数\n";
    std::cout << "  ✔ 单位换算系数（度->弧度、ADC->电压）\n";
    std::cout << "  ✔ PID 参数默认值、限幅值\n";
    std::cout << "  ✔ 状态机跳转表\n";
    std::cout << "  ✘ 依赖运行时输入的任何计算（传感器值）\n";
    std::cout << "  ✘ 标准库的 sinf/cosf/sqrtf（C++20 起它们不是 constexpr）\n";
    std::cout << "     要编译期算就只能自己写多项式/查表（像本例这样）。\n";

    std::cout << "\n=== D. 代价与坑 ===\n";
    std::cout << "  1) 编译时间变长（每次都要重算整张表）；\n";
    std::cout << "  2) 表越大 Flash 占用越大 —— 别把 3600 项的表放进去，\n";
    std::cout << "     用对称性只存 0..90 度（本例 91 项就够）。\n";
    std::cout << "     完整 0..360 存 int16 = 720 字节；0..90 只要 182 字节。\n";
    std::cout << "  3) static_assert 一定要写：编译期算错了不会有人告诉你，\n";
    std::cout << "     除非你把校验也写进 static_assert（见源码里的四条）。\n";
    std::cout << "  4) constexpr 函数的步数有上限（MSVC 默认约 100 万步），\n";
    std::cout << "     表太大或递归太深会报\"constexpr 求值步数超限\"。\n";

    std::cout << "\n=== E. std::array vs C 数组（嵌入式视角）===\n";
    std::array<PidState, kAxisCount> pid{};               // 3 个轴的 PID 状态
    std::cout << "  std::array<PidState,3> 的 sizeof = " << sizeof(pid)
              << " 字节（每轴 " << sizeof(PidState) << " 字节）\n";
    std::cout << "  和 C 数组 PidState pid[3] 的 sizeof 完全一样 —— std::array 零开销。\n";
    std::cout << "  但 std::array 多了 .size()、边界检查的 .at()、能整体拷贝赋值。\n";
    std::cout << "  注意：.at() 越界会抛 std::out_of_range，嵌入式若关了异常就别用 .at()。\n";

    std::cout << "\nmain 正常结束\n";
    return 0;
}
