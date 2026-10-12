/**
 * ch09_ex21.c —— 【变式】printf 家族的「成员」与它们的返回值
 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

/**
 * 变式：自己写一个「带长度限制」的日志函数，内部用 vsnprintf
 */
static int my_log(char *out, size_t out_size, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(out, out_size, fmt, ap);           /* vsnprintf：参数表已经打包好了 */
    va_end(ap);
    return n;
}

int main(void)
{
                /* ---------- 1) 输出到「流」的三个成员 ---------- */
    printf("1) printf         : 输出到 stdout\n");
    fprintf(stdout, "   fprintf(stdout) : 也是 stdout（可以换成 stderr 或文件）\n");
    fprintf(stderr, "   fprintf(stderr) : 这一行走 stderr（不带缓冲）\n");

                /* ---------- 2) 输出到「内存缓冲区」的两个成员 ---------- */
    char buf[32];
    int n1 = sprintf(buf, "%d-%s", 42, "ok");
    printf("\n2) sprintf  返回值 = %d ，buf = \"%s\" （无长度限制，危险）\n", n1, buf);
    int n2 = snprintf(buf, sizeof(buf), "%d-%s", 42, "ok");
    printf("   snprintf 返回值 = %d ，buf = \"%s\"\n", n2, buf);

                /* ---------- 3) 返回值的统一规则：都是"本该写多少个字符" ---------- */
    char small[6];
    int n3 = snprintf(small, sizeof(small), "abcdefghij");
    printf("\n3) snprintf 截断时：返回值 = %d ，实际 strlen = %zu\n",
           n3, strlen(small));
    printf("   sprintf 没有这个信息（它只会写爆缓冲区）\n");

                /* ---------- 4) 字符串专用的三个「更快」的函数 ---------- */
    printf("\n4) puts     : 自动补 '\\n'，等价于 printf(\"%%s\\n\", s)\n");
    puts("   puts 输出这一行");
    printf("   fputs    : 不补 '\\n'，可以指定流\n");
    fputs("   fputs 输出这一行（没有自动换行）\n", stdout);
    printf("   putchar  : 输出一个字符，返回该字符\n");
    int pc = putchar('A');
    printf(" <- putchar('A') 的返回值 = %d ('A')\n", pc);

                /* ---------- 5) 变式：用 vsnprintf 写自己的日志函数 ---------- */
    char line[24];
    int m1 = my_log(line, sizeof(line), "PWM=%d,ARM=%d", 1500, 1);
    printf("\n5) my_log 返回值 = %d ，内容 = \"%s\"\n", m1, line);
    int m2 = my_log(line, sizeof(line), "PWM=%d,ARM=%d,GYRO=%d", 1500, 1, 7);
    printf("   内容更长时：返回值 = %d （本该这么长），实际 = \"%s\"  -> 被截断\n",
           m2, line);
    printf("   vsnprintf 是「自己写 printf 风格函数」的唯一正确入口\n");

                /* ---------- 6) 返回值为负：出错了 ---------- */
    printf("\n6) 所有 printf 家族函数：输出/编码出错时返回【负数】\n");
    printf("   工程里如果只判 \"> 0\"，就会把\"截断\"和\"出错\"混在一起 ——\n");
    printf("   正确判法：n < 0 是出错；(size_t)n >= size 是截断。\n");
    return 0;
}
