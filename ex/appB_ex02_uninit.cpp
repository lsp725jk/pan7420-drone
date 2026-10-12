// 附录B 探针 2：读取未初始化变量
// 期望现象：不同运行/不同优化级别下读到不同值，且编译器可能给出 C4700 警告。
#include <cstdio>

int gUninit;               // 全局变量：零初始化，读它是"定义良好"的

int main() {
    int local;             // 局部（自动存储期）：不初始化就是不确定的值
    int buf[3];
    char pad;
    static int sUninit;    // static：零初始化

    std::printf("gUninit(全局)   = %d\n", gUninit);
    std::printf("sUninit(static) = %d\n", sUninit);
    std::printf("local(未初始化) = %d\n", local);       // UB（读不确定值）
    std::printf("buf[0..2]       = %d %d %d\n", buf[0], buf[1], buf[2]);
    std::printf("pad             = %d\n", (int)pad);

        // 用未初始化的值做判断/寻址，危害更大
    if (local == 0x12345678) {
        std::puts("巧合命中了某个值");
    }
    std::puts("程序结束（读未初始化内存常常什么异常都看不到）");
    return 0;
}
