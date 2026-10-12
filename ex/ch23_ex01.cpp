// ch23_ex01.cpp —— 例 23-1：编译期 / 运行期的边界在哪里
// 三个关键字，三种"必须程度"：
// constexpr —— 允许编译期求值（也可能在运行期）
// consteval —— 必须编译期求值
// constinit —— 这个全局变量必须编译期初始化（但之后可以改）
#include <cstdio>

// 1) constexpr：两条路都能走
constexpr int max_c(int a, int b) {
    return a > b ? a : b;
}

// 2) consteval：只有一条路。C++20 起。
consteval int twice(int x) {
    return x * 2;
}

// 3) constinit：要求"值在程序开始跑之前就写好"，避免静态初始化顺序问题
constinit int g_buf_size = twice(64);

// 4) 反例：这个函数不可能是 constexpr，因为它要读硬件
int sensor_read();  // 只在别处定义，内容无关紧要

int main() {
        // ---- 编译期求值 ----
    constexpr int a = max_c(3, 7);
    static_assert(a == 7, "编译期就算好了");

        // ---- 运行期求值：同一个函数 ----
    volatile int x = 3;  // volatile 挡住常量传播
    const int b = max_c(x, 7);

        // ---- consteval 只能编译期 ----
    constexpr int c = twice(21);
    static_assert(c == 42, "");

    std::printf("max_c(3,7) 编译期 = %d\n", a);
    std::printf("max_c(x,7) 运行期 = %d\n", b);
    std::printf("twice(21)  编译期 = %d\n", c);
    std::printf("g_buf_size        = %d（constinit，编译期就写好了）\n", g_buf_size);

    std::printf("\n三个关键字的分工：\n");
    std::printf("  constexpr  函数/变量  可以编译期，也可以运行期\n");
    std::printf("  consteval  只能修饰函数  必须编译期；运行期调用直接编不过(C7595)\n");
    std::printf("  constinit  只能修饰静态变量  必须编译期初始化；变量本身可写\n");

        // 坑：把"运行期才知道的值"硬塞进编译期
        // int t = sensor_read();
        // constexpr int bad = t;          // error C2131: 表达式的计算结果不是常数
        // constexpr int bad2 = twice(t);  // error C7595: 对即时函数的调用不是常量表达式
    std::printf("边界就是一句话：编译期只能用编译期已知的东西\n");
    return 0;
}
