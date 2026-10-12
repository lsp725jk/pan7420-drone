/**
 * ch09_ex25.c —— 【变式】用 ctype 解析一行「KEY=VALUE」配置
 */
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

/**
 * ---------- 版本 A：标准库版 ----------
 */
static int parse_kv_lib(const char *line, char *key, size_t ksize, long *val)
{
                /**
         * 跳过前导空白
         */
    const char *p = line;
    while (*p && isspace((unsigned char)*p)) p++;
    if (*p == '\0' || *p == '#' || *p == ';') return 0;              /* 空行/注释行 */

                /* 取 KEY（到 '=' 或空白为止） */
    size_t k = 0;
    while (*p && *p != '=' && !isspace((unsigned char)*p)) {
        if (k + 1 < ksize) key[k++] = *p;
        p++;
    }
    key[k] = '\0';

                /* 找 '=' */
    while (*p && isspace((unsigned char)*p)) p++;
    if (*p != '=') return -1;                                         /* 没有等号：格式错 */
    p++;
    while (*p && isspace((unsigned char)*p)) p++;

                /* 取 VALUE（要求全是数字或正负号） */
    char num[32];
    size_t n = 0;
    while (*p && !isspace((unsigned char)*p) && n + 1 < sizeof(num)) num[n++] = *p++;
    num[n] = '\0';
    if (n == 0) return -1;

    char *end = NULL;
    *val = strtol(num, &end, 0);
    if (end == num || *end != '\0') return -2;                        /* 值不是数字 */
    return 1;                                                         /* 成功 */
}

/* ---------- 版本 B：手写版（不依赖 locale，通常更快、也更好移植） ---------- */
#define IS_WS(c)   ((c) == ' ' || (c) == '\t' || (c) == '\r' || (c) == '\n')
#define IS_DIG(c)  ((c) >= '0' && (c) <= '9')

static int is_num_str(const char *s)
{
    if (*s == '+' || *s == '-') s++;
    if (!IS_DIG(*s)) return 0;
    for (; *s; s++) if (!IS_DIG(*s)) return 0;
    return 1;
}

int main(void)
{
    static const char *cfg[] = {
        "  # 这是注释行",
        "",
        "  PWM_MAX = 1800",
        "\tTIMEOUT=500",
        "BAD_LINE_NO_EQUALS",
        "NAME = abc",
        "NEG = -42",
        "HEX = 0x1F",
    };

    printf("用标准库 ctype 解析：\n");
    for (int k = 0; k < 8; k++) {
        char key[32]; long val = 0;
        int r = parse_kv_lib(cfg[k], key, sizeof(key), &val);
        switch (r) {
        case 1:  printf("  [行%d] %-22s -> %s = %ld\n", k, cfg[k], key, val); break;
        case 0:  printf("  [行%d] %-22s -> 跳过（空行/注释）\n", k, cfg[k]); break;
        case -1: printf("  [行%d] %-22s -> 格式错（没有等号或值为空）\n", k, cfg[k]); break;
        default: printf("  [行%d] %-22s -> 值不是数字\n", k, cfg[k]); break;
        }
    }

                /* ---------- 对比：手写版只判断"是不是数字" ---------- */
    printf("\n手写版 is_num_str 的判断（用于快速过滤）：\n");
    static const char *vals[] = { "1800", "-42", "+7", "0x1F", "abc", "", "-" };
    for (int k = 0; k < 7; k++)
        printf("  is_num_str(%-6s) = %d\n", vals[k][0] ? vals[k] : "(空)",
               is_num_str(vals[k]));
    printf("  -> 注意 0x1F 被判为「不是纯数字」：手写版只认十进制，\n");
    printf("     要支持十六进制就用 strtol 的 base=0（见例 9-8）。\n");

                /* ---------- 变式：为什么嵌入式常用手写版 ---------- */
    printf("\n为什么嵌入式常用手写版：\n");
    printf("  1) 不会因 locale（区域设置）变化而改变行为；\n");
    printf("  2) 不用把 ctype 的分类表拉进来，省 Flash；\n");
    printf("  3) 没有「参数必须是 unsigned char」这个坑，编译器也更容易优化。\n");
    printf("  代价：只认 ASCII，处理 UTF-8 中文要另想办法（ctype 也一样做不到）。\n");
    return 0;
}
