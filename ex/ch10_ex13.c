/**
 * ch10_ex13.c —— printf 重定向：fputc 与 _write 到底差在哪
 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <time.h>

/**
 * 第一部分：观测 C 库的缓冲行为
 */

/* 往缓冲区里填可打印字符，然后数"还没被刷出去"的字节数 */
static size_t pending_bytes(const char *buf, size_t n)
{
    size_t k = 0;
    while (k < n && buf[k] != '.') k++;              /* 用 '.' 当"未使用"的填充 */
    return k;
}

static void show_buffering(void)
{
    static char mybuf[64];

                /* 注意：这一段自己的输出不能用 stdout 打印 —— stdout 的缓冲区正被我们 拿来做实验。所以结论全部用 stderr 输出（stderr 默认不缓冲）。 */

                /* ---- 全缓冲 _IOFBF：攒着，直到满或者 fflush ---- */
    memset(mybuf, '.', sizeof(mybuf));
    setvbuf(stdout, mybuf, _IOFBF, sizeof(mybuf));
    printf("IMU ok");                                /* 没有 \n，6 字节 */
    printf(" PWM=1500");                             /* 再 9 字节 */
    size_t p1 = pending_bytes(mybuf, sizeof(mybuf));
    fprintf(stderr, "全缓冲 _IOFBF ：两次 printf 之后，缓冲区里积压了 %zu 字节\n", p1);
    fprintf(stderr, "               -> 这期间 _write 一次都没被调用\n");
    fflush(stdout);
    size_t p2 = pending_bytes(mybuf, sizeof(mybuf));
    fprintf(stderr, "               -> fflush 之后剩 %zu 字节（已经交出去了）\n\n", p2);

                /* ---- 行缓冲 _IOLBF：遇到 '\n' 就刷 ---- */
    memset(mybuf, '.', sizeof(mybuf));
    setvbuf(stdout, mybuf, _IOLBF, sizeof(mybuf));
    printf("IMU ok");                                /* 没有 \n：留着 */
    size_t p3 = pending_bytes(mybuf, sizeof(mybuf));
    fprintf(stderr, "行缓冲 _IOLBF ：printf(\"IMU ok\") 没有 \\n，积压 %zu 字节\n", p3);
    printf("\n");                                    /* 有 \n：整行刷出去 */
    size_t p4 = pending_bytes(mybuf, sizeof(mybuf));
    fprintf(stderr, "               -> 补一个 \\n 之后剩 %zu 字节（整行被交出）\n\n", p4);

                /* ---- 无缓冲 _IONBF：每个字符都立刻交出去 ---- */
    memset(mybuf, '.', sizeof(mybuf));
    setvbuf(stdout, mybuf, _IONBF, 0);
    printf("IMU ok");
    size_t p5 = pending_bytes(mybuf, sizeof(mybuf));
    fprintf(stderr, "无缓冲 _IONBF ：即使用户缓冲区也给了，里面只有 %zu 字节\n", p5);
    fprintf(stderr, "               -> 每个字符都立刻交出去，等价于 fputc 的调用频率\n\n");
    fflush(stdout);

                /* 恢复默认，后面的输出才正常 */
    setvbuf(stdout, NULL, _IONBF, 0);
}

/* 第二部分：两种重定向策略的调用次数与耗时（模拟嵌入式串口） 真机上 UART 每发一个字节要 87us（115200bps，8N1）。 这里用一个固定的自旋次数来模拟"每次发送调用的固定开销"， 用另一个自旋次数模拟"每个字节的传输时间"。 */
#define PER_CALL_SPIN   200000L      /* 模拟每次发送函数的固定开销 */
#define PER_BYTE_SPIN    20000L      /* 模拟每个字节的传输时间 */

static long g_calls = 0;
static long g_bytes = 0;
static volatile long g_sink = 0;

static void uart_send_blocking(const char *buf, size_t n)
{
    (void)buf;
    g_calls++;
    g_bytes += (long)n;
    for (long k = 0; k < PER_CALL_SPIN; k++) g_sink += k;              /* 固定开销 */
    for (size_t i = 0; i < n; i++)
        for (long k = 0; k < PER_BYTE_SPIN; k++) g_sink += k;                  /* 每字节 */
}

/* ---------- 路线 A：fputc 风格 —— printf 内部一个字符调一次 ---------- */
static void log_fputc_style(const char *msg, int a, int b)
{
    char tmp[96];
    int n = snprintf(tmp, sizeof(tmp), "%s a=%d b=%d", msg, a, b);
    for (int i = 0; i < n; i++) uart_send_blocking(&tmp[i], 1);          /* 一次一个字节 */
}

/* ---------- 路线 B：_write 风格 —— 先格式化到缓冲区，再整块交出去 ---------- */
static void log_write_style(const char *msg, int a, int b)
{
    char tmp[96];
    int n = snprintf(tmp, sizeof(tmp), "%s a=%d b=%d", msg, a, b);
    uart_send_blocking(tmp, (size_t)n);                                  /* 一次一整块 */
}

static double time_it(void (*fn)(const char *, int, int), const char *tag)
{
    g_calls = 0; g_bytes = 0;
    clock_t c0 = clock();
    for (int k = 0; k < 20; k++) fn(tag, k, k * 3);
    clock_t c1 = clock();
    printf("  %-10s : 发送函数被调用 %3ld 次，共 %4ld 字节，耗时 %.1f ms\n",
           tag, g_calls, g_bytes, 1000.0 * (double)(c1 - c0) / CLOCKS_PER_SEC);
    return (double)(c1 - c0);
}

int main(void)
{
    printf("=========== 第一部分：C 库的缓冲行为（实测）===========\n");
    show_buffering();

    printf("\n=========== 第二部分：两种重定向策略对比 ===========\n");
    printf("每行日志 19 字节，共打 20 行：\n");
    double ta = time_it(log_fputc_style, "fputc风格");
    double tb = time_it(log_write_style, "_write风格");
    printf("  结论：同样的总字节数，_write 风格少调用了大量函数，耗时约为 %.0f%%\n",
           100.0 * tb / ta);
    printf("        真机上差距更大：115200bps 下一个字节 87us，\n");
    printf("        60 个字符 = 5.2ms，而飞控的控制周期只有 4ms。\n");

    printf("\n=========== 真机上的重定向代码（原理示意，未在本机编译）===========\n");
    printf("· Keil / ARMCC 走 fputc：\n");
    printf("     int fputc(int ch, FILE *f) { HAL_UART_Transmit(&huart1,\n");
    printf("         (uint8_t *)&ch, 1, 2); return ch; }\n");
    printf("· GCC + newlib 走 _write：\n");
    printf("     int _write(int fd, char *buf, int len) { HAL_UART_Transmit(\n");
    printf("         &huart1, (uint8_t *)buf, len, 10); return len; }\n");
    printf("· 注意 _write 的返回值必须是「实际写出的字节数」，返回少了 printf 会重试；\n");
    printf("· 用 DMA 发送时，_write 里要先把缓冲区拷进自己的环形缓冲再启动 DMA，\n");
    printf("  不能直接把 printf 的临时缓冲区地址交给 DMA —— 函数一返回它就失效了。\n");
    return 0;
}
