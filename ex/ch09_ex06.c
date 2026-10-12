/**
 * ch09_ex06.c —— snprintf：把"格式化 + 拼字符串"变成不会溢出的操作
 */
#include <stdio.h>
#include <string.h>

int main(void)
{
    char buf[16];

                /**
         * 1) 装得下：返回值 = 实际写入长度
         */
    int n = snprintf(buf, sizeof(buf), "PWM=%d", 1500);
    printf("装得下 : buf = \"%s\" , 返回值 = %d , strlen = %zu\n",
           buf, n, strlen(buf));

                /* 2) 装不下：内容被截断，但返回值仍然是"本该有多长" */
    n = snprintf(buf, sizeof(buf), "PWM=%d,ARM=%d", 1500, 1);
    printf("装不下 : buf = \"%s\" , 返回值 = %d , 实际长度 = %zu  <-- 返回值 > 实际长度\n",
           buf, n, strlen(buf));

                /* 3) 用返回值判断有没有被截断 —— 这是唯一正确的判断方式 */
    if (n >= (int)sizeof(buf))
        printf("判定   : 被截断了，还差 %d 个字符\n", n - (int)sizeof(buf) + 1);

                /* 4) size = 0 也不崩（标准保证不写任何字节，但仍返回应有长度） */
    n = snprintf(NULL, 0, "abc%d", 12345);
    printf("size=0 : 返回值 = %d （可以先用它算需要多大缓冲区）\n", n);

                /* 5) 变式：安全的"逐段拼接" —— 记录还能写多少，指针往前走 */
    char msg[24];
    size_t used = 0;
    const char *parts[3] = { "imu", "ok", "500Hz" };
    for (int k = 0; k < 3; k++) {
        if (used >= sizeof(msg)) break;
        int w = snprintf(msg + used, sizeof(msg) - used, "%s%s",
                         (k == 0) ? "" : ",", parts[k]);
        if (w < 0) break;
        if ((size_t)w >= sizeof(msg) - used) { used = sizeof(msg); break; }                  /* 截断了 */
        used += (size_t)w;
    }
    printf("拼接   : msg = \"%s\"\n", msg);

                /* 6) 变式：格式化到"固定大小"的缓冲区里做日志前缀 */
    char logline[32];
    snprintf(logline, sizeof(logline), "[%05d]%s", 42, "boot");
    printf("日志   : %s\n", logline);
    return 0;
}
