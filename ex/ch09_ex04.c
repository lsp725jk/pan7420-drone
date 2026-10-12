/**
 * 文件写在"当前工作目录"，程序结束前自己删掉，不留垃圾。
 */
#define _CRT_SECURE_NO_WARNINGS      /**
 * 让 MSVC 别再警告 fopen 不安全，本节就是讲它
 */
#include <stdio.h>
#include <string.h>

int main(void)
{
    const char *path = "ch09_ex04_data.txt";

                /**
         * ---------- 写 ----------
         */
    FILE *fp = fopen(path, "w");                    /**
     * "w" = 只写，文件不存在就建、存在就清空
     */
    if (fp == NULL) {                               /* 【必须】每次 fopen 都要检查 */
        perror("fopen 写失败");
        return 1;
    }
    fprintf(fp, "陀螺 X = %d\n", 123);
    fputs("陀螺 Y = -45\n", fp);
    printf("写入完成，检查返回值：\n");
    printf("  fprintf 返回写入的字符数 : %d\n", fprintf(fp, "abc\n"));
    fclose(fp);                                     /* 不关文件 = 缓冲区可能没落盘 + 句柄泄漏 */

                /* ---------- 读 ---------- */
    fp = fopen(path, "r");
    if (fp == NULL) { perror("fopen 读失败"); return 1; }
    char line[64];
    int  n = 0;
    printf("读回内容：\n");
    while (fgets(line, (int)sizeof(line), fp) != NULL) {           /* fgets 会连 '\n' 一起读进来 */
        line[strcspn(line, "\r\n")] = '\0';                                /* 去掉行尾换行（见 9.5 节） */
        printf("  第 %d 行: [%s]\n", ++n, line);
    }
                /* 区分"读完了"和"出错了"：feof 只在"读到文件尾之后"才为真 */
    printf("feof(fp) = %d , ferror(fp) = %d\n", feof(fp), ferror(fp));
    fclose(fp);

                /* ---------- 变式：各种打开模式 ---------- */
    static const char *modes[] = { "r", "w", "a", "r+", "w+", "a+" };
    for (int k = 0; k < 6; k++) {
        FILE *t = fopen(path, modes[k]);
        printf("  模式 %-2s : %s\n", modes[k], t ? "打开成功" : "打开失败");
        if (t) fclose(t);
    }

                /* ---------- 追加 ---------- */
    fp = fopen(path, "a");                          /* "a" = 追加，不清空 */
    if (fp) { fputs("追加的一行\n", fp); fclose(fp); }
    fp = fopen(path, "r");
    if (fp) {
        printf("追加后整个文件：\n");
        while (fgets(line, (int)sizeof(line), fp)) printf("  | %s", line);
        fclose(fp);
    }

                /* ---------- 删除 ---------- */
    printf("remove() 返回值 = %d （0 = 成功）\n", remove(path));
    return 0;
}
