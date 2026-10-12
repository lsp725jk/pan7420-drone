// ch22_ex33.cpp —— 例 22-33：consteval 与 constinit（C++20）
// constexpr  = 可以编译期算，也可以运行期算
// consteval  = 必须编译期算（立即函数，immediate function）
// constinit  = 这个变量必须"常量初始化"（编译期就把值放好），但变量本身可变
#include <cstdio>

// -------- consteval：编译期强制 --------
consteval int square(int x) {
    return x * x;
}

constexpr int cube(int x) {  // 对比：constexpr 两条路都行
    return x * x * x;
}

// -------- constinit 解决的问题：静态初始化顺序 --------
// 同一个编译单元里，全局变量的"动态初始化"按声明顺序执行。
// 如果 B 依赖 A，而 A 的动态初始化排在 B 后面，B 拿到的就是 0。
int runtime_seed() {  // 非 constexpr：所以它必须在运行期执行
    return 41;
}

extern int g_a;
int g_b = g_a + 1;  // 声明在前面：此刻 g_a 还没被动态初始化，读到的是零初始化值 0
int g_a = runtime_seed();

// 用 constinit：值在编译期就放好，跟声明顺序彻底无关
constexpr int kSeed = 41;
constinit int g_ok = kSeed + 1;  // 42，编译期完成，没有动态初始化代码
constinit int g_table[4] = {kSeed, kSeed + 1, kSeed + 2, kSeed + 3};

int main() {
        // ---- consteval 的编译期调用 ----
    constexpr int a = square(5);
    static_assert(a == 25, "");
    std::printf("square(5) = %d（编译期）\n", a);

        // 运行期调用 consteval 会直接编译失败（实测）：
        // volatile int n = 5;
        // std::printf("%d\n", square(n));
        // -> error C7595: "square": 对即时函数的调用不是常量表达式
        // note: 失败原因是读取了常量表达式中不可使用的变量
    volatile int n = 5;
    std::printf("cube(n)   = %d（constexpr 允许运行期）\n", cube(n));

        // ---- 静态初始化顺序 ----
    std::printf("g_b = %d   <-- 声明顺序靠前，读到的 g_a 还是 0\n", g_b);
    std::printf("g_a = %d   <-- 它的动态初始化排在 g_b 之后\n", g_a);
    std::printf("g_ok = %d  <-- constinit，编译期就定好了，不受顺序影响\n", g_ok);
    std::printf("g_table = {%d,%d,%d,%d}\n", g_table[0], g_table[1], g_table[2], g_table[3]);

        // 坑：constinit 不是 const。g_ok 是可写的：
    g_ok += 1;
    std::printf("g_ok 改成 %d 也能编译——constinit 只管【怎么初始化】，不管【能不能改】\n", g_ok);

        // 坑：constinit 的初值必须是常量表达式。
        // constinit int g_bad = g_a + 1;
        // -> error C2127: "g_bad": 使用非常量表达式初始化"constinit"实体是非法的
        // 这其实是个**优点**：编译器替你把"初始化顺序地雷"当场挑出来。
    return 0;
}
