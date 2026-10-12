// 附录B 探针 3b：编译器究竟会不会利用"有符号溢出是 UB"
// 用同一份源码分别以 /Od 与 /O2 编译，对比结果。
// 标准允许编译器假设有符号运算永不溢出；是否真的这么做，取决于编译器与优化级别。
#include <cstdio>
#include <climits>

// 写法 A：x + 1 > x 恒真（若假设不溢出）
bool formA(int x) { return x + 1 > x; }

// 写法 B：x * 2 / 2 == x 恒真（若假设不溢出）
bool formB(int x) { return x * 2 / 2 == x; }

// 写法 C：x + 10 < x 恒假
bool formC(int x) { return x + 10 < x; }

// 写法 D：循环计数器溢出后应当回绕，编译器可以认为循环永不结束
int formD(int start, int step, int limit) {
    int n = 0;
    for (int i = start; i > 0; i += step) {
        if (++n > limit) return -1;      // 保险丝
    }
    return n;
}

// 写法 E：把两个数"比较"的差改成减法判正负（经典的有符号溢出陷阱）
bool formE(int a, int b) { return (a - b) > 0; }

int main() {
    std::printf("formA(INT_MAX) = %d\n", (int)formA(INT_MAX));
    std::printf("formB(INT_MAX) = %d\n", (int)formB(INT_MAX));
    std::printf("formC(INT_MIN) = %d\n", (int)formC(INT_MIN));
    std::printf("formD(1, 1000000000, 5) = %d\n", formD(1, 1000000000, 5));
    std::printf("formE(INT_MIN, 1) = %d\n", (int)formE(INT_MIN, 1));
    std::printf("（把结果与 /Od 的输出对照，就能看出优化器是否利用了 UB）\n");
    return 0;
}
