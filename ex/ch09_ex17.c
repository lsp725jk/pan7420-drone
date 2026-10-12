/**
 * ch09_ex17.c —— ctype.h：字符分类与大小写转换
 */
#include <stdio.h>
#include <ctype.h>

/**
 * 手写版：不用区域设置（locale），而且更快
 */
static char my_upper(char c) { return (c >= 'a' && c <= 'z') ? (char)(c - 32) : c; }

int main(void)
{
    const char *s = "Gyro_X 42!\t\n";
    printf("逐字符分类 \"%s\"：\n", "Gyro_X 42!");
    for (const char *p = s; *p; p++) {
        unsigned char c = (unsigned char)*p;
        printf("  '%c' (0x%02X) : alpha=%d digit=%d alnum=%d space=%d punct=%d\n",
               isprint(c) ? *p : '?', c,
               !!isalpha(c), !!isdigit(c), !!isalnum(c), !!isspace(c), !!ispunct(c));
    }

                /* 1) 大小写转换：toupper / tolower 返回 int，不是 char */
    printf("\ntoupper('a') = %c , tolower('Z') = %c\n", toupper('a'), tolower('Z'));
    printf("整串转大写：");
    for (const char *p = "pitch roll yaw"; *p; p++) putchar(toupper((unsigned char)*p));
    putchar('\n');

                /* 2) 变式：手写就不用担心区域设置（locale），而且更快 */
    printf("手写 upper  ：");
    for (const char *p = "pitch roll yaw"; *p; p++) putchar(my_upper(*p));
    putchar('\n');

                /* 3) 十六进制字符 -> 数值：isxdigit + 手写换算 */
    const char *hex = "1aF9";
    unsigned v = 0;
    for (const char *p = hex; *p; p++) {
        if (!isxdigit((unsigned char)*p)) break;
        unsigned d = (unsigned)(isdigit((unsigned char)*p) ? *p - '0'
                                                           : (tolower((unsigned char)*p) - 'a' + 10));
        v = v * 16 + d;
    }
    printf("\n\"%s\" 解析成 = %u\n", hex, v);

                /* 4) 坑现场：把负的 char 直接传给 isalpha */
    unsigned int raw = 0xE9u;                       /* 某个高位字节 */
    char bad = (char)raw;                           /* char 有符号时，它就变成负数 */
    printf("\n(char)0xE9 的值 = %d\n", (int)bad);
    printf("isalpha(bad)                    = %d  <-- 参数越界，UB\n", isalpha(bad));
    printf("isalpha((unsigned char)bad)     = %d  <-- 唯一正确的写法\n",
           isalpha((unsigned char)bad));

                /* 5) 变式：只判断 ASCII 数字可以用手写，避免 UB 也更快 */
    printf("\n手写 is_digit('7') = %d , is_digit('x') = %d\n",
           ('7' >= '0' && '7' <= '9'), ('x' >= '0' && 'x' <= '9'));
    return 0;
}
