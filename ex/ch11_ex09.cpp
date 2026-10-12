// ch11_ex09.cpp —— 返回值的引用坑：悬垂引用与返回内部成员的引用
// 编译：check.bat ch11_ex09.cpp c++latest
#include <cstdio>

// ---- 错误示范 A：返回局部变量的引用 ----
// 函数一返回，局部变量就不存在了，引用指向已经作废的栈空间。
// 这一版连编译都过不去（error C2440），属于"编译器帮你挡住了"。
// int& bad_ref()
// {
// int local = 42;
// return local;
// }

// ---- 错误示范 B：换成"能编过但结果不可靠"的版本 ----
// 这里返回的是函数内部临时对象的引用，函数结束时对象就销毁了。
struct Holder { int v; double pad[32]; };

int& bad_ref2()
{
    Holder h{ 42, {} };
    (void)h.pad[0];
    return h.v;            // 返回了 h.v 的引用，而 h 在函数返回时就死了
}

// ---- 正确示范 1：返回 static 变量的引用 ----
// 生命周期贯穿整个程序，安全；但它不是线程安全的。
int& counter_ref()
{
    static int n = 0;
    return ++n;
}

// ---- 正确示范 2：返回"传进来对象"的成员引用（谁的对象谁负责）----
struct Config {
    int baud = 115200;
    int& baud_ref() { return baud; }        // 对象还活着就安全
};

// ---- 正确示范 3：返回引用可以用来做左值 ----
struct Array {
    int a[4] = { 0, 0, 0, 0 };
    int& at(int i) { return a[i]; }         // 返回引用 -> 可以被赋值
    int  get(int i) const { return a[i]; }  // 返回值   -> 只能读
};

int main()
{
        // 1) 悬垂引用：能编过、能打印，但值是"已经作废的内存"
    int& dangling = bad_ref2();
    std::printf("1) bad_ref2() 读到的值 = %d（本次碰巧是 42，但不保证；属于未定义行为）\n",
                dangling);

        // 2) static 局部变量 + 引用返回
    int c1 = counter_ref();
    int c2 = counter_ref();
    std::printf("2) counter_ref: %d %d\n", c1, c2);

        // 3) 返回成员引用
    Config cfg;
    int& r = cfg.baud_ref();
    r = 9600;
    std::printf("3) 通过引用修改: cfg.baud=%d\n", cfg.baud);

        // 4) 引用返回做左值
    Array arr;
    arr.at(2) = 77;                 // at() 返回引用，所以能出现在 = 左边
    std::printf("4) arr.at(2)=%d（引用返回可以做左值）\n", arr.get(2));

    return 0;
}
