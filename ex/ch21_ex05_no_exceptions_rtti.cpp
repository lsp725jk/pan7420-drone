// ch21_ex05_no_exceptions_rtti.cpp —— 单片机能不能用 C++：关掉异常与 RTTI 的代价账
// 编译: check.bat ch21_ex05_no_exceptions_rtti.cpp c++latest
// 说明：本机 MSVC 关不掉异常（/EHs- 也不彻底），所以这里讲清"关了会怎样"，
// 并给出 GCC/Clang 上真正的开关，以及你自己怎么验证。
#include <iostream>
#include <typeinfo>
#include <type_traits>
#include <cstddef>
#include <new>
#include <string>

// ============================================================
// A. 怎么判断当前编译环境有没有开异常 / RTTI
// ============================================================
void reportFeatures()
{
#ifdef __cpp_exceptions
    std::cout << "  __cpp_exceptions = " << __cpp_exceptions
              << "   -> 异常**可用**\n";
#else
    std::cout << "  __cpp_exceptions 未定义 -> 异常**已关闭**\n";
#endif

#ifdef __cpp_rtti
    std::cout << "  __cpp_rtti       = " << __cpp_rtti
              << "   -> RTTI**可用**\n";
#else
    std::cout << "  __cpp_rtti       未定义   -> RTTI**已关闭**\n";
#endif

#if defined(_CPPUNWIND)
    std::cout << "  _CPPUNWIND 已定义 -> MSVC 认为异常已启用\n";
#else
    std::cout << "  _CPPUNWIND 未定义 -> MSVC 认为异常已关闭\n";
#endif
}

// ============================================================
// B. RTTI 关掉之后，哪些代码会失效
// ============================================================
struct Base { virtual ~Base() = default; };
struct Derived : Base { int extra = 42; };

// 编译期类型判断：关掉 RTTI 也能用（这是纯模板机制）
template <class T>
const char* kindOf()
{
    if constexpr (std::is_integral_v<T>) return "整数";
    else if constexpr (std::is_floating_point_v<T>) return "浮点";
    else return "其它";
}

int main()
{
    std::cout << "=== A. 本机（PC / MSVC）的开关状态 ===\n";
    reportFeatures();
    std::cout << "  MSVC 下 /EHs /EHsc 是默认开的；想彻底关掉很麻烦，\n";
    std::cout << "  所以\"关异常\"这件事主要在 GCC/Clang 工具链上做。\n";

    std::cout << "\n=== B. 嵌入式工具链上关异常/RTTI 的开关 ===\n";
    std::cout << "  GCC / Clang（arm-none-eabi-g++）：\n";
    std::cout << "    -fno-exceptions         关异常（try/throw/catch 会编译失败）\n";
    std::cout << "    -fno-rtti               关 RTTI（typeid / dynamic_cast 会编译失败）\n";
    std::cout << "    -fno-threadsafe-statics 关\"函数内 static 的线程安全 guard\"\n";
    std::cout << "    -fno-use-cxa-atexit     简化全局对象析构（嵌入式常不需要）\n";
    std::cout << "    -ffunction-sections -fdata-sections  +  -Wl,--gc-sections\n";
    std::cout << "                            让没用的函数/数据被链接器删掉（省 Flash 利器）\n";
    std::cout << "  典型 PY32F030 Makefile：\n";
    std::cout << "    CXXFLAGS = -mcpu=cortex-m0plus -mthumb -Os -std=c++20 \\\n";
    std::cout << "               -fno-exceptions -fno-rtti -fno-threadsafe-statics \\\n";
    std::cout << "               -ffunction-sections -fdata-sections -fno-use-cxa-atexit\n";
    std::cout << "    LDFLAGS  = -Wl,--gc-sections -specs=nano.specs -specs=nosys.specs\n";

    std::cout << "\n=== C. 关掉异常之后，哪些代码会编译失败 ===\n";
    std::cout << "  ✘ try { } catch (...) { }   -> 直接编译错误\n";
    std::cout << "  ✘ throw std::runtime_error(...)\n";
    std::cout << "  ✘ 标准库里凡是会抛的接口：\n";
    std::cout << "      std::vector::at()        -> 抛 out_of_range\n";
    std::cout << "      std::string::substr(越界) -> 抛 out_of_range\n";
    std::cout << "      std::stoi / std::stod     -> 抛 invalid_argument\n";
    std::cout << "      std::optional::value()   -> 抛 bad_optional_access\n";
    std::cout << "      new（内存不足）           -> 抛 bad_alloc\n";
    std::cout << "  ✔ 替代方案（这些都不抛）：\n";
    std::cout << "      vector::operator[] / data() + size()\n";
    std::cout << "      std::from_chars（不抛，返回 errc）—— 见第 18 章 ex10\n";
    std::cout << "      std::optional::value_or() / operator*\n";
    std::cout << "      std::expected<T,E>（C++23）\n";
    std::cout << "      自己的错误码枚举 + 输出参数\n";
    std::cout << "  重要：libstdc++ / libc++ 的某些实现里，即使你不写 try，\n";
    std::cout << "        内部也可能有 try（例如 iostream 的异常掩码逻辑）。\n";
    std::cout << "        所以嵌入式往往连 <iostream> / <string> 都换掉，\n";
    std::cout << "        只留 <array> / <type_traits> / <cstdint> 这类纯头文件库。\n";

    std::cout << "\n=== D. 关掉 RTTI 之后，哪些代码会编译失败 ===\n";
    std::cout << "  ✘ typeid(x)                -> 编译错误\n";
    std::cout << "  ✘ dynamic_cast<Derived*>(p) -> 编译错误\n";
    std::cout << "  ✔ 虚函数本身**不受影响**！多态照样工作，因为 vtable 里\n";
    std::cout << "     只有函数指针，RTTI 只是额外附加的类型信息。\n";
    std::cout << "  本机实测（RTTI 可用时）：\n";
    std::cout << "    typeid(Derived).name() = " << typeid(Derived).name() << "\n";
    Base b;
    Derived d;
    Base* p = &d;
    std::cout << "    虚函数调用 p->~Base() 之类照常（sizeof(Base) = " << sizeof(Base)
              << ", sizeof(Derived) = " << sizeof(Derived) << "）\n";
    std::cout << "    dynamic_cast 能用：" << (dynamic_cast<Derived*>(p) != nullptr) << "\n";
    std::cout << "  关掉 RTTI 后，想替代 dynamic_cast 的做法：\n";
    std::cout << "    ① 自己加一个 enum class Kind 成员，用 switch 分派；\n";
    std::cout << "    ② 用 std::variant + std::visit（不依赖 RTTI）；\n";
    std::cout << "    ③ 用编译期的 std::is_same_v / if constexpr（纯模板）。\n";
    std::cout << "  非 RTTI 的模板判断照样工作：kindOf<int>() = " << kindOf<int>()
              << ", kindOf<double>() = " << kindOf<double>() << "\n";

    std::cout << "\n=== E. 代价账：为什么单片机上要关它们 ===\n";
    std::cout << "  异常打开的代价：\n";
    std::cout << "    1) 每个可能抛的函数都要生成 unwind 信息（.pdata/.eh_frame 之类），\n";
    std::cout << "       实测常见结果：固件体积增加 10%~30%（视代码量而定）；\n";
    std::cout << "    2) 抛异常本身要\"查表 + 逐帧展开\"，在 M0+ 上是几百到上千周期，\n";
    std::cout << "       飞控的控制周期里根本不能接受；\n";
    std::cout << "    3) 运行时还要一个\"异常对象分配 + 终止处理\"的小运行时。\n";
    std::cout << "  RTTI 打开的代价：\n";
    std::cout << "    1) 每个多态类都要额外的 type_info 结构，放在 Flash 里；\n";
    std::cout << "    2) vtable 里多一项指向 type_info 的指针。\n";
    std::cout << "  关掉之后的代价：\n";
    std::cout << "    1) 只能用错误码 / std::expected 传错误 —— 代码更啰嗦（见第 18 章 ex09）；\n";
    std::cout << "    2) 只能用自定义 tag 代替 dynamic_cast；\n";
    std::cout << "    3) 出问题时没有异常信息可以打，只能靠断言 + 重启。\n";
    std::cout << "  注：上面\"10%~30%\"是业界常见经验值，**不是本机实测** ——\n";
    std::cout << "      本机没有 arm-none-eabi-g++，无法替你量。要精确数字请自己\n";
    std::cout << "      对同一份代码分别加/不加 -fno-exceptions 编译，比较 arm-none-eabi-size 的输出。\n";

    std::cout << "\n=== F. 实践建议（PY32F030 飞控）===\n";
    std::cout << "  1) 开 -fno-exceptions -fno-rtti，这是嵌入式 C++ 的默认姿势；\n";
    std::cout << "  2) 断言用自己写的 ASSERT(条件)（进 HardFault 或点亮 LED），别用 assert 抛异常；\n";
    std::cout << "  3) 需要\"可能失败\"的接口一律返回错误码或 std::expected；\n";
    std::cout << "  4) 完全不用动态分配（不 new / 不用 std::vector / std::string）；\n";
    std::cout << "     固定大小用 std::array + 手写环形缓冲；\n";
    std::cout << "  5) 不用的功能靠 --gc-sections 删，而不是靠\"不用\"；\n";
    std::cout << "  6) 每次发版都看一遍 .map 文件里 .text/.data/.bss 的大小。\n";

    std::cout << "\nmain 正常结束\n";
    return 0;
}
