// 附录A 探针 13：术语落地——RAII / SFINAE / CRTP / AoS 与 SoA
// 每个术语配一个最小可运行例子，避免"只背名词"。
#include <cstdio>
#include <type_traits>
#include <vector>
#include <cstddef>

// ---------- 1. RAII（Resource Acquisition Is Initialization，资源获取即初始化）----------
// 资源在构造函数里拿，在析构函数里放；靠作用域自动收尾，不写 finally。
class FileLike {
public:
    explicit FileLike(const char* name) : name_(name) {
        std::printf("  [RAII] 打开 %s\n", name_);
    }
    ~FileLike() {
        std::printf("  [RAII] 关闭 %s\n", name_);
    }
    FileLike(const FileLike&)            = delete;   // 不许拷贝
    FileLike& operator=(const FileLike&) = delete;
private:
    const char* name_;
};

// ---------- 2. SFINAE（Substitution Failure Is Not An Error，替换失败不是错误）----------
// 模板实参替换失败时，这个重载被静默丢弃，而不是编译报错 —— 用它做"按类型选择重载"。
template <typename T>
typename std::enable_if<std::is_integral<T>::value, const char*>::type
kind() { return "整数版本"; }

template <typename T>
typename std::enable_if<std::is_floating_point<T>::value, const char*>::type
kind() { return "浮点版本"; }

// C++17 之后更推荐 if constexpr / concepts；SFINAE 主要用于读老代码
template <typename T>
const char* kind17() {
    if constexpr (std::is_integral_v<T>)          return "整数版本(if constexpr)";
    else if constexpr (std::is_floating_point_v<T>) return "浮点版本(if constexpr)";
    else                                          return "其它";
}

// ---------- 3. CRTP（Curiously Recurring Template Pattern，奇异递归模板模式）----------
// 派生类把自己当模板实参传给基类，于是基类能在编译期"静态多态"地调用派生类的函数，
// 没有虚函数表，也就没有运行期开销 —— 嵌入式里很常用。
template <typename Derived>
struct CounterBase {
    int countTimes(int n) {
        int total = 0;
        for (int i = 0; i < n; ++i) total += static_cast<Derived*>(this)->step();
        return total;
    }
};
struct StepOne : CounterBase<StepOne> { int step() { return 1; } };
struct StepTwo : CounterBase<StepTwo> { int step() { return 2; } };

// 对照：虚函数版本（有运行期开销）
struct VirtBase { virtual int step() = 0; virtual ~VirtBase() = default; };
struct VirtOne : VirtBase { int step() override { return 1; } };

// ---------- 4. AoS 与 SoA（Array of Structs / Struct of Arrays，结构体数组 / 数组结构体）----------
struct ParticleAoS { float x, y, z, vx, vy, vz; };      // 一个粒子 24 字节
struct ParticlesSoA {                                    // 所有粒子的同一字段连排
    std::vector<float> x, y, z, vx, vy, vz;
};

int main() {
    std::puts("=== RAII ===");
    {
        FileLike f("config.txt");
        std::printf("  [RAII] 在作用域里干活\n");
    }   // 离开作用域自动"关闭"
    std::puts("  （没有写任何 close 调用）");

    std::puts("=== SFINAE ===");
    std::printf("  kind<int>()    = %s\n", kind<int>());
    std::printf("  kind<double>() = %s\n", kind<double>());
    std::printf("  kind17<int>()  = %s\n", kind17<int>());

    std::puts("=== CRTP ===");
    StepOne s1; StepTwo s2;
    std::printf("  CRTP: StepOne 走 3 次 = %d ; StepTwo 走 3 次 = %d\n",
                s1.countTimes(3), s2.countTimes(3));
    VirtOne v1; VirtBase* pv = &v1;
    std::printf("  虚函数: 走 3 次 = %d ; sizeof(有虚函数的对象) = %zu, sizeof(CRTP 对象) = %zu\n",
                pv->step() * 3, sizeof(VirtOne), sizeof(StepOne));

    std::puts("=== AoS 与 SoA ===");
    std::printf("  sizeof(ParticleAoS) = %zu 字节（一个粒子）\n", sizeof(ParticleAoS));
    ParticlesSoA soa;
    for (int i = 0; i < 4; ++i) { soa.x.push_back((float)i); soa.y.push_back(0); soa.z.push_back(0);
                                  soa.vx.push_back(1); soa.vy.push_back(0); soa.vz.push_back(0); }
    std::puts("  AoS: 遍历所有粒子的 x 字段要跨 24 字节步长读（缓存利用率低）");
    std::puts("  SoA: x 全部连排，遍历 x 是线性扫描（缓存友好，也方便 SIMD）");
    std::printf("  SoA 里 x 的 4 个值: %.0f %.0f %.0f %.0f\n",
                soa.x[0], soa.x[1], soa.x[2], soa.x[3]);

    std::puts("=== 其它常见缩写 ===");
    std::puts("  ODR  (One Definition Rule)      单一定义规则：同一实体只能有一个定义");
    std::puts("  TU   (Translation Unit)         编译单元：一个 .cpp 加它 include 进来的头");
    std::puts("  RTTI (Run-Time Type Information) 运行期类型信息，dynamic_cast/typeid 依赖它");
    std::puts("  ADL  (Argument-Dependent Lookup) 实参依赖查找：swap(a,b) 能找到 a 所在命名空间的 swap");
    std::puts("  SSO  (Small String Optimization) 小字符串优化，本机 15 字符以内不分配堆内存");
    std::puts("  POD  (Plain Old Data)           平凡旧数据：可直接 memcpy 的类型");
    std::puts("  ABI  (Application Binary Interface) 二进制接口，MSVC 与 GCC 的 C++ ABI 不兼容");
    return 0;
}
