// 附录A 探针 2：extern / static / const / volatile / constexpr / inline（单编译单元内可见的部分）
#include <cstdio>
#include <type_traits>

// ---- 1. const vs constexpr ----
const     int  kRuntime = 10;              // 编译期也能用，但语义上只保证"只读"
constexpr int  kCompile = 20;              // 保证编译期常量

static_assert(kCompile == 20, "constexpr 可用于 static_assert");
// static_assert(kRuntime == 10, "");      // 取消注释会编不过：const 不保证是编译期常量

// ---- 2. constexpr 函数 ----
constexpr int square(int x) { return x * x; }
static_assert(square(5) == 25, "constexpr 函数可在编译期求值");

// ---- 3. consteval（C++20）：必须编译期求值 ----
consteval int mustCompileTime(int x) { return x + 1; }
constexpr int kFromConsteval = mustCompileTime(41);

// ---- 4. 数组维度必须是编译期常量 ----
int arrConst[kCompile];
// int arrRuntime[kRuntime];               // 取消注释：C++ 里这是错误的（C 里 VLA 允许）

// ---- 5. inline 变量（C++17）：头文件里定义全局变量不再需要 extern 拆分 ----
inline int gInlineCounter = 0;

// ---- 6. static 的三种含义（这里是"文件作用域 static"= 内部链接）----
static int sFileLocal = 100;               // 只在本编译单元可见（C/C++ 相同）

// ---- 7. static 局部变量：只初始化一次 ----
int hitCount() {
    static int n = 0;                      // 首次调用时初始化，生命周期到程序结束
    return ++n;
}

// ---- 8. volatile 与 const 的四种组合 ----
volatile int           vRegLike   = 1;     // 每次都真读内存（不会被优化掉）
const volatile int     cvReadOnlyReg = 2;  // 只读但每次真读
const int              cPlain     = 3;

// ---- 9. constexpr if（C++17）：编译期分支 ----
template <typename T>
const char* kindOf() {
    if constexpr (std::is_integral_v<T>) {
        return "整数类型";
    } else if constexpr (std::is_floating_point_v<T>) {
        return "浮点类型";
    } else {
        return "其它类型";
    }
}

// ---- 10. 对齐 ----
struct Align1 { char c; };
struct Align8 { double d; char c; };
struct alignas(16) Align16 { int x; };   // alignas 放在 struct 关键字之后

int main() {
    std::printf("kRuntime=%d kCompile=%d square(7)=%d kFromConsteval=%d\n",
                kRuntime, kCompile, square(7), kFromConsteval);
    std::printf("sizeof(arrConst)=%zu (=%d*%zu)\n", sizeof(arrConst), kCompile, sizeof(int));
    std::printf("hitCount 三次调用: %d %d %d\n", hitCount(), hitCount(), hitCount());
    std::printf("kindOf<int>()=%s kindOf<double>()=%s kindOf<const char*>()=%s\n",
                kindOf<int>(), kindOf<double>(), kindOf<const char*>());
    std::printf("sizeof(Align1)=%zu sizeof(Align8)=%zu alignof(Align16)=%zu sizeof(Align16)=%zu\n",
                sizeof(Align1), sizeof(Align8), alignof(Align16), sizeof(Align16));
    std::printf("gInlineCounter=%d sFileLocal=%d vRegLike=%d cvReadOnlyReg=%d cPlain=%d\n",
                gInlineCounter, sFileLocal, vRegLike, cvReadOnlyReg, cPlain);
    return 0;
}
