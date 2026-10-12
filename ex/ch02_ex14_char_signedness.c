/**
 * ch02_ex14_char_signedness.c —— char 到底是有符号还是无符号
 */
#include <stdio.h>
#include <ctype.h>
#include <limits.h>

int main(void)
{
                /**
         * ---- 1. 本机的 char 是有符号的 ----
         */
    printf("---- 1. char 的符号性 ----\n");
    printf("CHAR_MIN = %d, CHAR_MAX = %d\n", CHAR_MIN, CHAR_MAX);
    printf("SCHAR_MIN = %d, SCHAR_MAX = %d\n", SCHAR_MIN, SCHAR_MAX);
    printf("0..UCHAR_MAX = %u\n", (unsigned)UCHAR_MAX);
    printf("本机 CHAR_MIN < 0，说明 char 是 **有符号** 的。\n");
    printf("但这只是本机的选择，标准允许编译器把它实现成无符号。\n");

                /* ---- 2. 一个 >127 的字节，用两种类型解释结果不同 ---- */
    printf("---- 2. 同一个字节，两种解释 ----\n");
                /* 用 volatile 中转一下，让 0xC8 变成"运行期的值"， 否则 MSVC /W4 会对常量截断报 warning C4310。 */
    volatile int raw = 0xC8;
    unsigned char ub = (unsigned char)raw;                  /* 200 */
    signed char   sb = (signed char)raw;                    /* -56 */
    printf("字节 0xC8: unsigned char = %u, signed char = %d\n",
           (unsigned)ub, (int)sb);
    printf("所以处理**字节流**（串口、协议、CRC）时用 unsigned char / uint8_t，\n");
    printf("避免出现负值导致比较、求和出错。\n");

                /* ---- 3. 比较和求和时的符号陷阱 ---- */
    printf("---- 3. 比较与求和 ----\n");
    unsigned char rx[4] = { 0xC8, 0x01, 0xFF, 0x80 };
    int sum_signed = 0;
    unsigned int sum_unsigned = 0;
    for (int i = 0; i < 4; i++) {
                                /* 下面这行如果按键入 signed char 来解释，0xC8 会变成 -56 */
        sum_signed   += (signed char)rx[i];
        sum_unsigned += (unsigned char)rx[i];
    }
    printf("把这 4 个字节当 signed char 求和   = %d\n", sum_signed);
    printf("把这 4 个字节当 unsigned char 求和 = %u\n", sum_unsigned);
    printf("校验和就该用无符号版本。\n");

                /* ---- 4. <ctype.h> 的坑 ---- */
    printf("---- 4. ctype 函数的参数要求 ----\n");
    char c = (char)raw;                         /* 本机是负数，且这次不会有 C4310 */
    printf("char c = 0xC8 时，c 的值是 %d（负数）\n", (int)c);
                /* toupper(c) 直接传负值是 UB！ 正确做法是先转成 unsigned char：toupper((unsigned char)c) */
    int safe = toupper((unsigned char)c);
    printf("toupper((unsigned char)c) = %d   <-- 这是安全写法\n", safe);
    printf("直接写 toupper(c) 是 UB（参数必须是 unsigned char 或 EOF）。\n");
    printf("正确写法示范：isalpha((unsigned char)ch) / tolower((unsigned char)ch)\n");
    return 0;
}
