/**
 * ch09_ex24.c —— 【踩坑】strtok 不可重入：一个函数把另一个函数的解析打乱
 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>

/**
 * ---------- 函数 B：内部也用 strtok 解析自己的字符串 ----------
 */
static void parse_hex(const char *s)
{
    char tmp[64];
    strncpy(tmp, s, sizeof(tmp) - 1);
    tmp[sizeof(tmp) - 1] = '\0';

    printf("      [B] 开始解析 \"%s\"\n", tmp);
    for (char *t = strtok(tmp, ":"); t != NULL; t = strtok(NULL, ":"))
        printf("      [B]   字段 = %s\n", t);
    printf("      [B] 解析结束\n");
}

/* ---------- 函数 A：也在用 strtok，中途调用了 B ---------- */
static void parse_csv(const char *s, int call_b)
{
    char tmp[64];
    strncpy(tmp, s, sizeof(tmp) - 1);
    tmp[sizeof(tmp) - 1] = '\0';

    printf("   [A] 开始解析 \"%s\"\n", tmp);
    for (char *t = strtok(tmp, ","); t != NULL; t = strtok(NULL, ",")) {
        printf("   [A]   字段 = %s\n", t);
        if (call_b) {
            parse_hex("AA:BB:CC");                               /* B 里也用了 strtok —— 灾难 */
            call_b = 0;                                          /* 只插一次，方便看输出 */
        }
    }
    printf("   [A] 解析结束\n");
}

int main(void)
{
                /* ---------- 1) 不嵌套调用：一切正常 ---------- */
    printf("1) A 自己解析（不调用 B）：\n");
    parse_csv("1,22,333,4444", 0);
    printf("   -> 4 个字段全都解析出来了，正确\n\n");

                /* ---------- 2) 嵌套调用：A 的循环被打乱 ---------- */
    printf("2) A 解析到第 2 个字段时调用了 B：\n");
    parse_csv("1,22,333,4444", 1);
    printf("   -> 注意 [A] 后面的字段：只剩下 2 个（本该 4 个）！\n");
    printf("      B 里的 strtok 把「上次位置」覆盖掉了，A 的循环直接跑偏。\n\n");

                /* ---------- 3) 用可重入版本 strtok_s，问题消失 ---------- */
    printf("3) 用 strtok_s（GCC 里叫 strtok_r）：状态放在 ctx 里，互不干扰\n");
    {
        char sa[64] = "1,22,333,4444";
        char sb[64] = "AA:BB:CC";
        char *ca = NULL, *cb = NULL;
        printf("   [A] ");
        for (char *t = strtok_s(sa, ",", &ca); t; t = strtok_s(NULL, ",", &ca)) {
            printf("%s ", t);
            if (ca && strcmp(t, "22") == 0) {
                printf("\n   [B] ");
                for (char *u = strtok_s(sb, ":", &cb); u; u = strtok_s(NULL, ":", &cb))
                    printf("%s ", u);
                printf("\n   [A] ");
            }
        }
        printf("\n   -> A 的 4 个字段一个不少\n");
    }

                /* ---------- 4) 变式：干脆不用 strtok，自己切 ---------- */
    printf("\n4) 最稳的写法：自己用一个指针扫描（无全局状态、可嵌套、可重入）\n");
    {
        char s[] = "10,20,30";
        char *p = s;
        printf("   字段：");
        while (*p) {
            char *comma = strchr(p, ',');
            if (comma) *comma = '\0';
            printf("[%s] ", p);
            if (!comma) break;
            p = comma + 1;
        }
        printf("\n");
    }

    printf("\n结论：\n");
    printf("  · strtok 不可重入、不可嵌套、不能在中断里用、不能多线程用；\n");
    printf("  · 工程里一律用 strtok_s / strtok_r，或者自己扫描。\n");
    printf("  · 同类「有隐藏全局状态」的函数还有：rand()、localtime()、strerror()。\n");
    return 0;
}
