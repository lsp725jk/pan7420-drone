// ch11_ex27.cpp —— "最令人头疼的解析"（most vexing parse）
// 编译：check.bat ch11_ex27.cpp c++latest
#include <cstdio>

struct Timer {
    int ms;
    Timer() { ms = 0; std::printf("  调用了 Timer() 默认构造\n"); }
    explicit Timer(int m) { ms = m; std::printf("  调用了 Timer(int) 构造\n"); }
};

// 返回一个 Timer 的函数
Timer make_timer() { return Timer(50); }

int main()
{
        // ---- 头疼现场：下面这行不是"定义一个对象"，而是"声明一个函数 ----
        // Timer t1();          // 声明：函数 t1，无参数，返回 Timer
        // t1.ms;               // 错误：t1 是函数，没有成员 ms

        // 正确写法 1：去掉括号（C++11 起推荐 {}）
    Timer t2;
    std::printf("1) Timer t2;      -> ms=%d\n", t2.ms);

        // 正确写法 2：花括号（最推荐，同时避开两种歧义）
    Timer t3{};
    Timer t4{ 50 };
    std::printf("2) Timer t3{} / t4{50} -> %d / %d\n", t3.ms, t4.ms);

        // 正确写法 3：多写一层括号（老代码里常见）
    Timer t5((Timer(70)));
    std::printf("3) Timer t5((Timer(70))) -> %d\n", t5.ms);

        // ---- 第二个头疼现场：函数声明 vs 对象定义 ----
        // Timer t6(Timer());   // 声明：函数 t6，参数是"返回 Timer 的函数指针"
        // // 而不是"用一个临时 Timer 构造 t6"
    Timer t7{ Timer(80) };  // 用 {} 就明确了：这是初始化
    std::printf("4) Timer t7{ Timer(80) } -> %d\n", t7.ms);

        // ---- 解决套路总结 ----
        // 1) 能用 {} 就用 {}；2) 不行就加一层括号；3) 实在绕就先定义再赋值
    Timer t8 = make_timer();
    std::printf("5) Timer t8 = make_timer() -> %d\n", t8.ms);

    return 0;
}
