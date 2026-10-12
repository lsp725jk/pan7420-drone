/**
 * ch09_ex22.c —— 【踩坑】fgets 遇到超长行，以及 while(!feof) 为什么是错的
 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>

int main(void)
{
    const char *path = "ch09_ex22_data.txt";

                /**
         * 造一个「有一行特别长」的文件
         */
    FILE *fp = fopen(path, "w");
    if (!fp) { perror("fopen"); return 1; }
    fputs("short\n", fp);
    fputs("this_is_a_very_long_line_that_exceeds_sixty_four_bytes_for_sure_1234567890\n", fp);
    fputs("last\n", fp);
    fclose(fp);

                /* ---------- 1) 用 16 字节的小缓冲区读：长行会被拆成几次 ---------- */
    printf("1) 缓冲区只有 16 字节，数一数 fgets 被调了几次：\n");
    fp = fopen(path, "r");
    char small[16];
    int calls = 0, lines = 0;
    while (fgets(small, (int)sizeof(small), fp) != NULL) {
        calls++;
        int has_nl = (strchr(small, '\n') != NULL);
        if (has_nl) lines++;
        printf("   第 %2d 次: %-2zu 字符  %s [%s]\n",
               calls, strlen(small),
               has_nl ? "有换行" : "无换行",
               small[strlen(small) - 1] == '\n' ? "" : "(被截断)");
    }
    fclose(fp);
    printf("   -> 6 次 fgets 调用，但文件里只有 3 行（lines = %d）\n", lines);
    printf("   结论：fgets 只保证「最多读 size-1 个字符」，不保证「读一整行」。\n");
    printf("         判断「这一行读完了没」要看最后一个字符是不是 '\\n'。\n\n");

                /* ---------- 2) 正确做法：缓冲区开够，或者用循环拼接 ---------- */
    printf("2) 正确做法：缓冲区按「最长可能的一行」来开\n");
    fp = fopen(path, "r");
    char big[128];
    int n = 0;
    while (fgets(big, (int)sizeof(big), fp)) {
        big[strcspn(big, "\r\n")] = '\0';
        printf("   第 %d 行（%zu 字符）: %s\n", ++n, strlen(big), big);
    }
    fclose(fp);
    printf("   -> 3 次调用读 3 行，一一对应\n\n");

                /* ---------- 3) 坑：while (!feof(fp)) ---------- */
    printf("3) 坑现场：while (!feof(fp)) 会多处理一次\n");
    fp = fopen(path, "r");
    int guard = 0;
    char line[128];
    while (!feof(fp)) {                            /* 错：feof 是"上次读撞到尾了吗" */
        line[0] = '\0';
        if (fgets(line, (int)sizeof(line), fp) == NULL) {
            printf("   第 %d 轮：fgets 返回 NULL（读失败了），但 feof 这时才刚变真\n",
                   ++guard);
            printf("   如果这里不判 NULL 就直接用 line，就会用上一次的旧内容 ——\n");
            printf("   症状：文件最后一行被处理了两遍。\n");
            break;
        }
        guard++;
    }
    fclose(fp);

                /* ---------- 4) 正确写法 ---------- */
    printf("\n4) 正确写法：把 fgets 的返回值当循环条件\n");
    printf("   while (fgets(line, sizeof(line), fp) != NULL) { ... }\n");
    printf("   循环退出后再用 feof / ferror 区分「正常读完」和「出错了」。\n");

    remove(path);
    printf("\n文件已删除，remove() 返回前请检查（这里忽略返回值只作演示）\n");
    return 0;
}
