/**
 * ch09_ex07.c —— 【坑】snprintf 返回值 + sprintf 的危险
 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>

int main(void)
{
                /**
         * ---------- 坑 1：返回值 >= size ----------
         */
    char small[8];
    int n = snprintf(small, sizeof(small), "VERY_LONG_STRING");
    printf("size=8 : 内容 = \"%s\" , 返回值 = %d , strlen = %zu\n",
           small, n, strlen(small));
    printf("         返回值 %d 比缓冲区 %d 还大 —— 它不是\"写了多少\"\n",
           n, (int)sizeof(small));

                /* ---------- 坑 2：拿返回值推进指针，越界 ---------- */
    char buf[16];
    size_t off = 0;
                /* 下面这个循环是"错误示范"：本意是逐段拼接，但一旦截断 off 就会超出 16 */
    for (int k = 0; k < 3; k++) {
        int w = snprintf(buf + off, sizeof(buf) - off, "[%d]", k * 111111);
        if (w < 0) break;
        off += (size_t)w;                                      /* 错：没检查是否截断 */
        if (off >= sizeof(buf)) {
            printf("坑 2  : 第 %d 次后 off = %zu 已经 >= %zu ——\n",
                   k + 1, off, sizeof(buf));
            printf("        再写下去 buf+off 就跑到数组外面了（真实程序在这里崩溃）\n");
            break;
        }
    }

                /* ---------- 正确写法：先检查再推进 ---------- */
    char ok[16];
    size_t used = 0;
    for (int k = 0; k < 3; k++) {
        size_t space = sizeof(ok) - used;
        int w = snprintf(ok + used, space, "[%d]", k * 111111);
        if (w < 0) break;
        if ((size_t)w >= space) {                              /* 截断了：停止，不再推进 */
            printf("正确  : 第 %d 次发生截断，主动停止。ok = \"%s\"\n", k + 1, ok);
            break;
        }
        used += (size_t)w;
        printf("正确  : 第 %d 次写完后 ok = \"%s\"\n", k + 1, ok);
    }

                /* ---------- 对比：sprintf 完全没有"大小"这个概念 ---------- */
    char danger[8];
    sprintf(danger, "%s", "1234567890");               /* 需要 11 字节，却只有 8 —— 栈被踩坏 */
    printf("sprintf 写 8 字节缓冲区：danger = \"%s\"  <-- 已经越界，结果是随机的\n", danger);
    printf("注意：这里\"看起来正常\"只是运气，实际已经破坏了后面的栈数据\n");
    return 0;
}
