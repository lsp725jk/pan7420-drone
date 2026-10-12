/**
 * ch09_ex16.c —— time.h：日历时间、格式化，以及"测代码跑多久"
 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <time.h>

static volatile long g_sink = 0;

static void busy_loop(long n)
{
    volatile long s = 0;
    for (long k = 0; k < n; k++) s += k;
    g_sink += s;
}

int main(void)
{
                /**
         * 1) time(NULL)：从 1970-01-01 UTC 起经过的秒数（Unix 时间戳）
         */
    time_t now = time(NULL);
    printf("time(NULL)      = %lld  （Unix 时间戳，秒）\n", (long long)now);



    struct tm *lt = localtime(&now);
    struct tm *gt = gmtime(&now);                   /* 这一句把 lt 指向的内容也覆盖了 */
    printf("\nlt = %p , gt = %p  -> %s\n", (void *)lt, (void *)gt,
           (lt == gt) ? "同一个缓冲区！" : "不同");
    printf("用 lt 打印 \"本地时间\" ：%02d:%02d:%02d  <-- 已经被 gmtime 改成 UTC 了\n",
           lt->tm_hour, lt->tm_min, lt->tm_sec);
    printf("用 gt 打印 \"UTC 时间\"  ：%02d:%02d:%02d\n",
           gt->tm_hour, gt->tm_min, gt->tm_sec);

                /* 正确做法：拿到之后【立刻拷贝一份】 */
    struct tm local_copy = *localtime(&now);
    struct tm utc_copy   = *gmtime(&now);
    printf("\n正确做法（各自拷贝一份）：\n");
    printf("  本地时间 = %04d-%02d-%02d %02d:%02d:%02d (星期%d)\n",
           local_copy.tm_year + 1900, local_copy.tm_mon + 1, local_copy.tm_mday,
           local_copy.tm_hour, local_copy.tm_min, local_copy.tm_sec, local_copy.tm_wday);
    printf("  UTC 时间 = %04d-%02d-%02d %02d:%02d:%02d\n",
           utc_copy.tm_year + 1900, utc_copy.tm_mon + 1, utc_copy.tm_mday,
           utc_copy.tm_hour, utc_copy.tm_min, utc_copy.tm_sec);
    printf("  tm_year 是「从 1900 起算」，tm_mon 是 0~11 —— 这两个最常忘\n");

                /* 3) strftime：自己控制格式 */
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S %A", &local_copy);
    printf("\nstrftime 结果   = %s\n", buf);
    strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", &local_copy);
    printf("当文件名用      = %s\n", buf);
    strftime(buf, sizeof(buf), "%j", &local_copy);
    printf("一年中的第几天  = %s\n", buf);

                /* 4) mktime：反过来把 struct tm 变成时间戳（会自动"进位"，比如 1 月 32 日 -> 2 月 1 日） */
    struct tm t;
    t.tm_year = 2026 - 1900;
    t.tm_mon  = 0;                    /* 0 = 一月 */
    t.tm_mday = 1;
    t.tm_hour = 12;
    t.tm_min  = 0;
    t.tm_sec  = 0;
    t.tm_isdst = -1;                  /* 让 mktime 自己去判断夏令时 */
    printf("\nmktime(2026-01-01 12:00) = %lld\n", (long long)mktime(&t));

                /* 5) difftime：算两个时间点差多少秒（返回 double，避免整数溢出） */
    time_t start = time(NULL);
    busy_loop(5000000);
    time_t end = time(NULL);
    printf("\n忙循环耗时 difftime = %.0f 秒（秒级精度不够看，用 clock 更细）\n",
           difftime(end, start));

                /* 6) clock()：CPU 时间，单位是 CLOCKS_PER_SEC 分之一秒。 注意它量的是"本进程占用的 CPU 时间"，不是墙上时钟 */
    clock_t c0 = clock();
    busy_loop(20000000);
    clock_t c1 = clock();
    printf("CPU 时间 = %.3f 秒 （CLOCKS_PER_SEC = %d）\n",
           (double)(c1 - c0) / CLOCKS_PER_SEC, (int)CLOCKS_PER_SEC);

                /* 7) 坑：time_t 是有符号整数，2038 年会溢出（32 位平台）。 嵌入式里如果不联网对时，通常根本不用 time.h， 而是用 SysTick 的毫秒计数当"相对时间"。 */
    printf("\n提示：32 位平台上 time_t 最大到 2038-01-19，这就是「2038 问题」\n");
    return 0;
}
