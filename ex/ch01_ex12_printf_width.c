/**
 * ch01_ex12_printf_width.c —— printf 的宽度与对齐：做一张表
 */
#include <stdio.h>

typedef struct {
    const char *name;
    int         raw;
    double      volt;
} sample_t;

int main(void)
{
    static const sample_t rows[4] = {
        { "bat",   1234,  3.700 },
        { "gyro",     7,  1.234 },
        { "baro", 98765, 101.325 },
        { "mag",   -42,  0.125 },
    };

    printf("---- 1. 不做任何对齐（很难看）----\n");
    for (int i = 0; i < 4; i++) {
        printf("%s raw=%d volt=%f\n", rows[i].name, rows[i].raw, rows[i].volt);
    }

    printf("---- 2. 加上宽度与精度（整齐）----\n");
    printf("%-6s %8s %10s\n", "name", "raw", "volt");
    printf("%-6s %8s %10s\n", "----", "---", "----");
    for (int i = 0; i < 4; i++) {
        printf("%-6s %8d %10.3f\n", rows[i].name, rows[i].raw, rows[i].volt);
    }

    printf("---- 3. 同一个数的五种宽度写法 ----\n");
    int v = 42;
    printf("[%d] [%5d] [%-5d] [%05d] [%+5d]\n", v, v, v, v, v);
    int n = -7;
    printf("[%d] [%5d] [%-5d] [%05d] [%+5d]\n", n, n, n, n, n);

    printf("---- 4. 十六进制日志的固定宽度（嵌入式最常用）----\n");
    unsigned int reg = 0x0000A5u;
    printf("  %%08X   = %08X\n", reg);
    printf("  %%#010X = %#010X\n", reg);
    printf("  %%04X   = %04X\n", reg);
    printf("  【为什么固定宽度重要】多行日志的十六进制列能对齐，肉眼比对快得多。\n");

    printf("---- 5. 浮点的宽度与精度组合 ----\n");
    double pi = 3.14159265358979;
    printf("  [%f] [%12.4f] [%-12.4f] [%+.2f] [%.0f]\n",
           pi, pi, pi, pi, pi);
    printf("  【提醒】%%.2f 是四舍五入显示，不改变变量本身的值。\n");

    printf("---- 6. 字符串的宽度与截断 ----\n");
    const char *s = "hello";
    printf("  [%s] [%10s] [%-10s] [%.3s] [%10.3s]\n", s, s, s, s, s);
    printf("  【坑】字符串不能补零：%%010s 里的 0 会被忽略（补的是空格）。\n");
    return 0;
}
