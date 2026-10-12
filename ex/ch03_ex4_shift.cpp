/**
 * ch03_ex4_shift.c —— 移位运算，以及它的未定义行为
 */
#include <stdio.h>
#include <stdint.h>
#include <limits.h>

int main(void)
{
                /**
         * ---- 1. 左移 = 乘 2，右移 = 除 2 ----
         */
    printf("---- 1. 移位就是乘除 2 的幂 ----\n");
    uint32_t v = 3;
    for (int n = 0; n <= 5; n++) {
        printf("3 << %d = %2u   (3 * 2^%d = %u)     3 >> %d = %u\n",
               n, (unsigned)(v << n), n, (unsigned)(v << n), n, (unsigned)(v >> n));
    }

                /* ---- 2. 右移：无符号补 0，有符号补符号位 ---- */
    printf("---- 2. 右移补什么 ----\n");
    uint8_t  u8  = 0x80;                        /* 1000 0000 */
    int8_t   s8  = -128;                        /* 同样的位模式，作为有符号数是 -128 */
    printf("uint8_t 0x80 >> 1 = 0x%02X  = %u   (补 0)\n",
           (unsigned)(uint8_t)(u8 >> 1), (unsigned)(uint8_t)(u8 >> 1));
    printf("int8_t  0x80 = %d\n", (int)s8);
    printf("(int)s8 >> 1 = %d   (MSVC/x86 补符号位，即 -128/2 = -64)\n",
           (int)(s8 >> 1));
    printf("有符号右移是'实现定义'行为：本机补符号位，换个平台不保证。\n");
    printf("【实践建议】只对无符号数做移位，需要时先转成 unsigned。\n");

                /* ---- 3. 移位量越界：UB，编译期有警告，运行期结果不可依赖 ---- */
    printf("---- 3. 移位量越界（UB）----\n");
    uint32_t one = 1u;
    volatile int amount = 32;                   /* 用 volatile 让它是"运行期的值" */
    printf("1u << 32 (运行期移位量) = %u\n", (unsigned)(one << amount));
    printf("  实测 MSVC/x64 得到 1：x86 的 SHL 只取移位量的低 5 位(32&31=0)，\n");
    printf("  所以 1u<<32 变成了 1u<<0。这是硬件巧合，语言层面是 UB。\n");
                /* 如果移位量写成常量 32，编译期就会警告： warning C4293: '<<': Shift 计数为负或过大，其行为未定义 这类警告千万不要忽略。 */

                /* ---- 4. 有符号左移进符号位：也是 UB ---- */
    printf("---- 4. 有符号左移的边界 ----\n");
    int pos = 1;
    printf("1 << 30  = %d   (安全，还在正数范围)\n", pos << 30);
    printf("1 << 31  对 int 是 UB（移进了符号位）；本机实测 = %d\n", pos << 31);
    printf("正确做法：用无符号写掩码 —— 1u << 31 = 0x%08X\n", 1u << 31);
    printf("INT_MAX  = 0x%08X\n", (unsigned)INT_MAX);

                /* ---- 5. 用移位拼装/拆解多字节数据（通信协议必备）---- */
    printf("---- 5. 拼装与拆解 32 位数据 ----\n");
    uint8_t buf[4] = { 0x12, 0x34, 0x56, 0x78 };
                /* 大端（big-endian）：高位字节在前面，网络序就是大端 */
    uint32_t be = ((uint32_t)buf[0] << 24) | ((uint32_t)buf[1] << 16) |
                  ((uint32_t)buf[2] << 8)  |  (uint32_t)buf[3];
                /* 小端（little-endian）：低位字节在前面，x86/STM32/PY32 是小端 */
    uint32_t le = ((uint32_t)buf[3] << 24) | ((uint32_t)buf[2] << 16) |
                  ((uint32_t)buf[1] << 8)  |  (uint32_t)buf[0];
    printf("字节 12 34 56 78\n");
    printf("  按大端拼 = 0x%08X\n", be);
    printf("  按小端拼 = 0x%08X\n", le);
    printf("  拆回去：be>>24=0x%02X be>>16&0xFF=0x%02X be>>8&0xFF=0x%02X be&0xFF=0x%02X\n",
           (unsigned)((be >> 24) & 0xFF), (unsigned)((be >> 16) & 0xFF),
           (unsigned)((be >> 8) & 0xFF), (unsigned)(be & 0xFF));
    printf("【坑】拼装时每个 buf[i] 都要先转成 uint32_t，\n");
    printf("     否则 buf[i] << 24 是在 int 上做，0x78<<24 会溢出成 UB。\n");
                /* 印证一下：不转型会发生什么 */
    uint8_t b0 = 0x80;
    int bad = b0 << 24;                         /* b0 先提升为 int，0x80<<24 已经越界 */
    printf("  对比：不转型 (int)(0x80) << 24 = %d（负数，已经错了）\n", bad);
    printf("  正确： (uint32_t)0x80 << 24 = 0x%08X\n", (uint32_t)0x80 << 24);

                /* ---- 6. 嵌入式实战：用移位做分频/位带/紧凑存储 ---- */
    printf("---- 6. 直接用移位算分频系数 ----\n");
    uint32_t sysclk = 48000000u;                /* 48MHz，PY32F030 常见主频 */
    for (int div = 0; div <= 3; div++) {
        printf("  分频 2^%d -> %u Hz\n", div, (unsigned)(sysclk >> div));
    }
    printf("右移代替除法：编译器对无符号除 2^n 本来就会优化成移位，\n");
    printf("自己写移位只是让意图更明显，性能上通常没差别。\n");
    return 0;
}
