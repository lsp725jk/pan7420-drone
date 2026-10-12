// 附录B 探针 3：有符号整数溢出——编译器优化会把"UB"变成完全不同的结果
#include <cstdio>
#include <climits>

// 编译器有权假设"有符号加法永不溢出"，于是把 x+1 > x 直接优化成 true
bool alwaysTrue(int x) { return x + 1 > x; }

// 循环边界依赖有符号溢出时，/O2 下可能被优化成死循环或直接删掉循环
int countUp(int start) {
    int n = 0;
    for (int i = start; i > 0; i += 1000000000) {   // i 迟早会溢出
        ++n;
        if (n > 100) return -1;                     // 保险丝，避免真的转不出来
    }
    return n;
}

int main() {
    std::printf("INT_MAX = %d\n", INT_MAX);
    std::printf("alwaysTrue(INT_MAX) = %d   （O0 下应为 0，优化后可能变成 1）\n",
                (int)alwaysTrue(INT_MAX));
    std::printf("countUp(1) = %d\n", countUp(1));

        // 无符号溢出是"定义良好"的回绕，可以放心用
    unsigned u = UINT_MAX;
    std::printf("无符号: UINT_MAX + 1u = %u （标准保证回绕到 0）\n", u + 1u);
    return 0;
}
