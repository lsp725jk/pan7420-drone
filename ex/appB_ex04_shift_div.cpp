// 附录B 探针 4：移位超位宽、除零、浮点除零
#include <cstdio>
#include <climits>

int main(int argc, char** argv) {
    unsigned int u = 1u;
    int shift = 32 + (argc - 1);      // 32 位类型移 32 位以上就是 UB

    std::printf("=== 1. 移位超位宽 ===");
    std::printf("1u << 31 = %u （合法）\n", 1u << 31);
    std::printf("1u << %d 会执行（UB），本机结果 = %u\n", shift, (u << shift) & 0xFFFFFFFFu);

    std::printf("=== 2. 负数左移（C++20 起才明确合法）===\n");
    int neg = -8;
    std::printf("-8 << 1 = %d\n", neg << 1);

    std::printf("=== 3. 有符号右移负数（实现定义）===\n");
    std::printf("-8 >> 1 = %d （算术右移；标准只说这是「实现定义」行为）\n", neg >> 1);

    std::printf("=== 4. 浮点除零不是 UB ===\n");
    volatile double zero = 0.0, one = 1.0;
    std::printf("1.0 / 0.0 = %f  （IEEE-754: inf）\n", one / zero);
    std::printf("0.0 / 0.0 = %f  （nan）\n", zero / zero);

    std::printf("=== 5. 整数除零是 UB（通常会真的崩）===\n");
    int a = 10;
    int b = argc - 1;                 // 不带参数时 b == 0
    std::printf("准备计算 10 / %d ...\n", b);
    std::fflush(stdout);
    int c = a / b;                    // 整数除零：UB，x86 上触发 #DE 异常 -> 进程崩
    std::printf("10 / %d = %d （能打出来说明这次没崩）\n", b, c);

    std::printf("=== 6. 整数取余零同样是 UB ===\n");
    std::printf("10 %% %d = %d\n", b, a % b);
    return 0;
}
