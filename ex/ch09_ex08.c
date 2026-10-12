/**
 * ch09_ex08.c —— atoi / atol / atof 与 strtol / strtod：怎么把字符串变成数字
 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

int main(void)
{
                /**
         * ---------- 1) atoi 家族：简单，但"错在哪"完全看不出来 ----------
         */
    printf("atoi(\"123\")      = %d\n", atoi("123"));
    printf("atoi(\"  -45abc\")  = %d   （跳过空格，遇到非数字就停）\n", atoi("  -45abc"));
    printf("atoi(\"abc\")     = %d   （完全不是数字 -> 返回 0，和真的 0 分不清）\n", atoi("abc"));
    printf("atoi(\"0\")       = %d   （也是 0）\n", atoi("0"));
    printf("atoi(\"99999999999999\") = %d  （溢出 -> UB，这里给出的是 MSVC 的实际结果）\n",
           atoi("99999999999999"));

                /* ---------- 2) strtol：能告诉你"停在哪里"和"有没有出错" ---------- */
    const char *texts[] = { "123", "  -45abc", "abc", "0x1F", "99999999999999999999" };
    for (int k = 0; k < 5; k++) {
        char *end = NULL;
        errno = 0;                                                  /* 【必须】先清 errno 才能判断 */
        long v = strtol(texts[k], &end, 0);                         /* base=0 表示自动识别 0x / 0 */

        int consumed = (int)(end - texts[k]);                       /* end 指向"第一个没被转换的字符" */
        printf("strtol(%-22s) = %-20ld 转换了 %d 个字符", texts[k], v, consumed);
        if (end == texts[k]) printf("  <-- 一个都没转换（不是数字）");
        if (errno == ERANGE) printf("  <-- ERANGE：超出 long 范围，返回了 LONG_MAX/MIN");
        if (*end != '\0' && end != texts[k]) printf("  <-- 后面还有 \"%s\"", end);
        printf("\n");
    }
    printf("LONG_MAX = %ld\n", LONG_MAX);

                /* ---------- 3) strtod：浮点版，语法比 strtol 复杂（支持 1e-3、inf、nan） ---------- */
    const char *ft[] = { "3.14159", "1e-3", "inf", "nan", "12abc" };
    for (int k = 0; k < 5; k++) {
        char *end = NULL;
        errno = 0;
        double d = strtod(ft[k], &end);
        printf("strtod(%-10s) = %-12g 停了 %d 个字符\n", ft[k], d, (int)(end - ft[k]));
    }

                /* ---------- 4) 变式：strtoul / strtoll，以及"判错"的正确姿势 ---------- */
    char *end = NULL;
    errno = 0;
    unsigned long uv = strtoul("4000000000", &end, 10);
    printf("strtoul(\"4000000000\") = %lu （放进 unsigned long 才装得下）\n", uv);
    printf("判错三步：errno=0 -> 调用 -> 检查 errno 和 end\n");
    return 0;
}
