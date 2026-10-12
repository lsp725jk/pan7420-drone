/**
 * ch09_ex13.c —— string.h 里的"不安全"函数：事故现场
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
                /**
         * ---------- 1) strncpy 的著名陷阱：源串比 n 长时【不补 '\0'】 ----------
         */
    char dst[6];
    memset(dst, '#', sizeof(dst));                             /**
     * 先填满，方便看清有没有被覆盖
     */
    strncpy(dst, "hello world", sizeof(dst));                  /* 只拷 6 个字节，一个 '\0' 都没有 */
    printf("strncpy 后各字节：");
    for (int k = 0; k < (int)sizeof(dst); k++)
        printf("%02X ", (unsigned char)dst[k]);
    printf("\n  dst[5] = 0x%02X —— 不是 0，所以 dst 不是一个合法的字符串\n",
           (unsigned char)dst[5]);
    printf("  正确写法：dst[sizeof(dst)-1] = '\\0'; 或者直接用 snprintf\n");

    char ok[6];
    snprintf(ok, sizeof(ok), "%s", "hello world");             /* 一定以 '\0' 结尾 */
    printf("  snprintf 的结果 = \"%s\"（被安全截断）\n", ok);

                /* ---------- 2) strcat 累加：长度只增不减，稍不注意就溢出 ---------- */
    char line[16] = "IMU";
    printf("\nstrcat 累加演示：初始 \"%s\"，容量 %d\n", line, (int)sizeof(line));
    const char *add[] = { ",OK", ",500Hz", ",GYRO", ",ACCEL", ",MAG" };
    for (int k = 0; k < 5; k++) {
        size_t need = strlen(line) + strlen(add[k]) + 1;
        if (need > sizeof(line)) {
            printf("  第 %d 次追加 \"%s\" 需要 %zu 字节 > %zu，停手！否则就是栈溢出\n",
                   k + 1, add[k], need, sizeof(line));
            break;
        }
        strcat(line, add[k]);
        printf("  第 %d 次追加后 = \"%s\"（%zu 字节）\n", k + 1, line, strlen(line));
    }

                /* ---------- 3) strtok：会修改原字符串，而且不可重入 ---------- */
    char csv[] = "1,23,-456,7";
    printf("\nstrtok 解析前 : \"%s\"\n", csv);
    char *tok = strtok(csv, ",");
    while (tok) {
        printf("  字段 = %-5s (atoi=%d)\n", tok, atoi(tok));
        tok = strtok(NULL, ",");
    }
    printf("strtok 解析后 : \"%s\"  <-- 逗号被就地改成了 '\\0'，原串被破坏\n", csv);
    printf("  这就是为什么 strtok 不能作用在字符串字面量上（会崩）\n");

                /* ---------- 4) strlen 返回 size_t（无符号）：减法会"绕回去" ---------- */
    const char *a = "abc", *b = "abcdefgh";
    printf("\nstrlen(\"%s\") - strlen(\"%s\") = %zu\n", a, b, strlen(a) - strlen(b));
    printf("  %zu 不是 -5，而是 2^64-5 —— 无符号减法下溢（C 里这是定义良好的行为）\n",
           strlen(a) - strlen(b));
    printf("  正确写法：先转成 (int) 再减，或者写 if (strlen(a) < strlen(b))\n");
    return 0;
}
