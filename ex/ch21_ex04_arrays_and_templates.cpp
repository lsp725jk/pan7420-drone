// ch21_ex04_arrays_and_templates.cpp —— std::array vs C 数组；模板的零开销与代码膨胀
// 编译: check.bat ch21_ex04_arrays_and_templates.cpp c++latest
#include <iostream>
#include <array>
#include <cstdint>
#include <cstddef>
#include <algorithm>

// ============================================================
// 1. C 数组的问题：传参会退化成指针，长度丢失
// ============================================================
void sumCPointer(const int* p, std::size_t n)      // 必须额外传 n
{
    int s = 0;
    for (std::size_t i = 0; i < n; ++i) s += p[i];
    std::cout << "    (指针版) sum = " << s << "\n";
}

template <std::size_t N>
void sumTemplate(const int (&arr)[N])              // 长度进了类型，不用传 n
{
    int s = 0;
    for (std::size_t i = 0; i < N; ++i) s += arr[i];
    std::cout << "    (模板引用版) sum = " << s << ", N = " << N << "\n";
}

void sumArray(const std::array<int, 5>& a)         // std::array 自带 size()
{
    int s = 0;
    for (int v : a) s += v;
    std::cout << "    (std::array 版) sum = " << s << ", size = " << a.size() << "\n";
}

// ============================================================
// 2. 模板零开销抽象：编译后和手写展开一样的机器码
// ============================================================
template <typename T>
T clampValue(T v, T lo, T hi)
{
    return v < lo ? lo : (v > hi ? hi : v);
}

// 手写版本，用来对比"模板没有额外代价"
std::uint16_t clampManual(std::uint16_t v, std::uint16_t lo, std::uint16_t hi)
{
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

// ============================================================
// 3. 模板的代价：每个实例化一份代码（代码膨胀）
// ============================================================
template <typename T>
struct Filter {
    T state = T{};
    T step(T input, T alpha)
    {
        state = static_cast<T>(state + alpha * (input - state));
        return state;
    }
};

// ============================================================
// 4. 用 static_assert 在模板里做"编译期契约"
// ============================================================
template <typename T>
T adcToMillivolts(T raw, T vref_mv, T fullScale)
{
    static_assert(std::is_integral_v<T>, "ADC 原始值必须是整数类型");
    static_assert(sizeof(T) <= 4, "别用 64 位类型，M0+ 上太慢");
    return static_cast<T>(static_cast<std::uint32_t>(raw) * vref_mv / fullScale);
}

int main()
{
    std::cout << "=== A. 数组传参：三种写法对比 ===\n";
    int carr[5] = {1, 2, 3, 4, 5};
    std::array<int, 5> sarr{1, 2, 3, 4, 5};
    sumCPointer(carr, 5);
    sumTemplate(carr);
    sumArray(sarr);
    std::cout << "  C 数组的问题：\n";
    std::cout << "    sizeof(carr) 在 main 里 = " << sizeof(carr) << " 字节\n";
    std::cout << "    但传进函数后 sizeof(p) = " << sizeof(const int*) << " 字节（退化成指针！）\n";
    std::cout << "    所以必须额外传长度，而长度和数组不匹配时编译器不管。\n";
    std::cout << "  std::array 的好处：\n";
    std::cout << "    sizeof 完全一样（" << sizeof(sarr) << " 字节），零开销；\n";
    std::cout << "    但 size() 永远正确，可以整体赋值 a = b，能当返回值返回。\n";
    std::cout << "    .at(i) 会检查越界（代价：越界抛异常，嵌入式关了异常就别用）。\n";

    std::cout << "\n=== B. 模板是零开销抽象（zero-overhead abstraction）===\n";
    std::cout << "  clampValue<uint16_t>(1500, 1000, 2000) = "
              << clampValue<std::uint16_t>(1500, 1000, 2000) << "\n";
    std::cout << "  clampManual(1500, 1000, 2000)          = "
              << clampManual(1500, 1000, 2000) << "\n";
    std::cout << "  两者编译出来的机器码**完全一样** —— 模板只存在于编译期，\n";
    std::cout << "  运行时不存在\"模板\"这个东西，没有虚函数、没有类型判断。\n";
    std::cout << "  这就是为什么 Cortex-M0+（48MHz、无 FPU、无缓存）也敢用模板。\n";

    std::cout << "\n=== C. 模板的代价：代码膨胀（code bloat）===\n";
    std::cout << "  Filter 模板被实例化成几种类型：\n";
    Filter<float>        ff;
    Filter<double>       fd;
    Filter<std::int32_t> fi;
    std::cout << "    sizeof(Filter<float>)        = " << sizeof(ff) << " 字节\n";
    std::cout << "    sizeof(Filter<double>)       = " << sizeof(fd) << " 字节\n";
    std::cout << "    sizeof(Filter<std::int32_t>) = " << sizeof(fi) << " 字节\n";
    std::cout << "  每个不同的 T 都会生成**一份独立的机器码**。\n";
    std::cout << "  用 3 种类型 -> 3 份 step() 代码；用 7 种 -> 7 份。\n";
    std::cout << "  在 64KB Flash 的 PY32F030 上，这是真实压力：\n";
    std::cout << "    - 一个中等复杂的模板类，每种实例化几百字节到几 KB；\n";
    std::cout << "    - 递归模板、变参模板展开后可能爆炸性增长。\n";
    std::cout << "  控制手段：\n";
    std::cout << "    ① 只实例化真正需要的类型，别为了\"通用\"全试一遍；\n";
    std::cout << "    ② 把不依赖类型参数的逻辑抽到非模板基类/普通函数里；\n";
    std::cout << "       （业界叫 type erasure / 提取公共部分）\n";
    std::cout << "    ③ 用 extern template 抑制重复实例化（多文件项目）：\n";
    std::cout << "       extern template class Filter<float>;   // 别的 cpp 里已实例化\n";
    std::cout << "    ④ 编译后一定看 map 文件，确认 .text 没有意外膨胀。\n";
    std::cout << "       arm-none-eabi-size 或 arm-none-eabi-nm --size-sort 都能看。\n";

    std::cout << "\n=== D. 模板 + static_assert：编译期契约 ===\n";
    std::cout << "  adcToMillivolts<std::uint16_t>(2048, 3300, 4095) = "
              << adcToMillivolts<std::uint16_t>(2048, 3300, 4095) << " mV\n";
    std::cout << "  模板里写了：\n";
    std::cout << "    static_assert(std::is_integral_v<T>, \"ADC 原始值必须是整数类型\");\n";
    std::cout << "    static_assert(sizeof(T) <= 4, \"别用 64 位类型，M0+ 上太慢\");\n";
    std::cout << "  于是 adcToMillivolts<double>(...) 会在**编译期**被拒绝，\n";
    std::cout << "  而不是等你烧进板子跑出奇怪结果才发现。\n";

    std::cout << "\n=== E. 嵌入式选型建议 ===\n";
    std::cout << "  ✔ 该用模板：\n";
    std::cout << "     - 固定大小的容器（std::array）、编译期已知的环形缓冲\n";
    std::cout << "     - 寄存器访问封装（模板参数给地址，编译期常量 -> 直接寻址）\n";
    std::cout << "     - 数学工具（clamp / map / 定点运算）\n";
    std::cout << "     - 状态机跳转表\n";
    std::cout << "  ✘ 慎用模板：\n";
    std::cout << "     - 为\"通用\"而通用的容器库（std::map/std::function 会拖进大量代码）\n";
    std::cout << "     - 深层递归模板（编译慢、代码大）\n";
    std::cout << "     - 虚函数 + 模板混用（虚函数有 vtable 和间接跳转开销）\n";
    std::cout << "  判断标准很简单：**看 map 文件**。Flash 涨了多少就是多少。\n";

    std::cout << "\nmain 正常结束\n";
    return 0;
}
