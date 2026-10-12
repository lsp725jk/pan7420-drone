/**
 * ch09_ex14.c —— string.h 的安全写法与 mem* 家族
 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>

/**
 * memcpy 最朴素的实现就是"正序逐字节拷贝"。把它写出来，
 */
static void naive_copy(void *dst, const void *src, size_t n)
{
    unsigned char       *d = (unsigned char *)dst;
    const unsigned char *s = (const unsigned char *)src;
    for (size_t i = 0; i < n; i++) d[i] = s[i];
}

int main(void)
{
                /* ---------- 1) 拷贝：snprintf 一把梭 ---------- */
    char dst[8];
    strncpy(dst, "1234567", sizeof(dst) - 1);
    dst[sizeof(dst) - 1] = '\0';                             /* 老写法：必须手动补 0 */
    printf("strncpy+手动补0 : \"%s\"\n", dst);
    snprintf(dst, sizeof(dst), "%s", "abcdefghij");          /* 新写法：自动截断并补 0 */
    printf("snprintf        : \"%s\"\n", dst);

                /* ---------- 2) 追加：strncat 的 n 是"最多再追加几个字符" ---------- */
    char buf[16] = "ID:";
    strncat(buf, "1234567890", sizeof(buf) - strlen(buf) - 1);          /* 16 - 已用 3 - 1 */
    printf("strncat 结果    : \"%s\"（容量 %d，写满了自动停并补 0）\n",
           buf, (int)sizeof(buf));

                /* ---------- 3) 重叠拷贝：为什么必须用 memmove ---------- */
    char a[16] = "abcdefghij";
    char b[16] = "abcdefghij";
    char c[16] = "abcdefghij";

    printf("\n原始            : \"%s\"\n", a);
    memmove(a + 2, a, 8);                /* memmove 会判断方向，重叠也正确 */
    printf("memmove(a+2,a,8): \"%s\"  <-- 正确\n", a);
    naive_copy(b + 2, b, 8);             /* 正序逐字节：源已经被前面的写操作改过了 */
    printf("正序逐字节拷贝  : \"%s\"  <-- 错！后面的字节被前面的覆盖污染了\n", b);
    memcpy(c + 2, c, 8);                 /* 标准说重叠时是 UB */
    printf("memcpy (a+2,a,8): \"%s\"  <-- 本机 MSVC 恰好也对了\n", c);
    printf("  注意：memcpy 这里\"看起来对\"完全不能依赖 —— 换编译器、换尺寸、\n");
    printf("        换优化等级都可能出错。标准规定重叠时是未定义行为。\n");

                /* ---------- 4) memcmp / memset / memchr ---------- */
    unsigned char pkt1[6] = { 0xAA, 0x01, 0x02, 0x03, 0x04, 0xBB };
    unsigned char pkt2[6] = { 0xAA, 0x01, 0x02, 0x03, 0x04, 0xCC };
    printf("\nmemcmp(pkt1,pkt2,5) = %d （比前 5 字节：相同）\n", memcmp(pkt1, pkt2, 5));
    printf("memcmp(pkt1,pkt2,6) = %d （比 6 字节：不同。标准只保证符号有意义）\n",
           memcmp(pkt1, pkt2, 6));
    printf("memchr 找 0x03      = 偏移 %d\n",
           (int)((unsigned char *)memchr(pkt1, 0x03, 6) - pkt1));
    unsigned char zero[6];
    memset(zero, 0, sizeof(zero));
    printf("memset 清零后首字节 = %d\n", zero[0]);
    printf("坑：memset(p, 1, n) 是把每个【字节】设成 1，不是把 int 设成 1；\n");
    printf("    想给 int 数组赋 1 只能用循环或循环展开。\n");

                /* ---------- 5) 查找与切分：strchr / strrchr / strstr / strspn / strcspn ---------- */
    const char *path = "cfg/imu/gyro.bin";
    printf("\nstrchr('/')   -> %s\n", strchr(path, '/'));
    printf("strrchr('/')  -> %s\n", strrchr(path, '/'));
    printf("strstr(\"gyro\") -> %s\n", strstr(path, "gyro"));
    printf("strspn(小写+斜杠) = %zu （开头连续属于集合的字符数）\n",
           strspn(path, "abcdefghijklmnopqrstuvwxyz/"));
    printf("strcspn(\".\")  = %zu （开头连续不属于集合的字符数；用它去掉行尾换行最方便）\n",
           strcspn(path, "."));

                /* ---------- 6) strtok 的可重入替代：strtok_s（GCC 里叫 strtok_r） ---------- */
    char csv[] = "10,20,30";
    char *ctx = NULL;
    printf("\nstrtok_s 解析：");
    for (char *t = strtok_s(csv, ",", &ctx); t != NULL; t = strtok_s(NULL, ",", &ctx))
        printf("[%s] ", t);
    printf("\n  带 _s / _r 的版本把「状态」放在 ctx 里，所以可以嵌套、可以多线程\n");
    return 0;
}
