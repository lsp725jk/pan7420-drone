/**
 * ch02_ex3_signed_unsigned.c —— 有符号 / 无符号、整型提升、隐式转换
 */
#include <stdio.h>
#include <limits.h>
#include <string.h>

int main(void)
{
                /**
         * ---- 1. 经典陷阱：负数竟然"大于"正数 ----
         */
    int          i = -1;
    unsigned int u = 1u;
    printf("---- 陷阱一：int 和 unsigned 比较 ----\n");
    printf("i = %d, u = %u\n", i, u);
        /* 下面这一行是故意的：MSVC /W4 会报 warning C4018 "有符号/无符号不匹配"。 为了保持示例输出干净，这里用 pragma 把这一条警告压掉； 你自己的代码里出现 C4018，就应该去检查，而不是压掉。 */
#pragma warning(push)
#pragma warning(disable : 4018)
    printf("i < u  ? %d   <-- 期望 1，实际 0！\n", i < u);
#pragma warning(pop)
    printf("(unsigned)i = %u   <-- -1 被重新解释成 4294967295\n", (unsigned)i);
    printf("把 i 显式转成 int 再比: %d\n", i < (int)u);

                /* ---- 2. 同样的坑：字符串长度和减法 ---- */
                /* strlen 返回 size_t（无符号）。strlen(s) - 3 在 s 很短时会变成一个 巨大的无符号数，于是"长度大于 3"这种判断就反了。 */
    const char *s = "ab";
    printf("---- 陷阱二：无符号减法下溢 ----\n");
    printf("strlen(\"%s\") = %zu\n", s, strlen(s));
    printf("(int)(2 - 3)        = %d\n", (int)(2 - 3));
    printf("(unsigned)(2u - 3u) = %u\n", (unsigned)(2u - 3u));
    printf("用无符号判断 2-3 > 0 : %d   <-- 也是 1，因为下溢后成了大正数\n",
           (unsigned)(2u - 3u) > 0u);
    printf("所以千万别写 if (strlen(s) - 3 > 0)，要写成 if (strlen(s) > 3)\n");

                /* ---- 3. 整型提升：窄类型运算前先变 int ---- */
    signed char  c1 = 100;
    signed char  c2 = 100;
    short        s1 = 30000;
    short        s2 = 30000;
    printf("---- 陷阱三：整型提升让中间结果变大 ----\n");
    printf("(signed char)100 + (signed char)100 = %d\n", c1 + c2);
    printf("上面这个结果是 int，所以没有溢出成 -56\n");
    printf("但存回 signed char 就会溢出:\n");
    signed char c3 = (signed char)(c1 + c2);
    printf("(signed char)(100+100) = %d\n", c3);
    printf("(short)30000 + (short)30000 = %d\n", s1 + s2);
    printf("存回 short 后 = %d\n", (short)(s1 + s2));

        /* 反过来：unsigned short 提升成 int 还是 unsigned int？ 在 int 是 32 位的机器上，unsigned short（16 位）能放进 int， 所以提升成 **int**，于是下面这个减法得到负数。 */
    unsigned short us = 1;
    printf("unsigned short 1 - 2 = %d   <-- 提升成 int 后是负数，不是 4294967295\n",
           us - 2);

                /* ---- 4. 赋值时的截断：宽 -> 窄 ---- */
    printf("---- 截断 ----\n");
    int    big  = 300;
    unsigned char uc = (unsigned char)big;              /* 300 = 0x12C，只留低 8 位 0x2C */
    printf("(unsigned char)300 = %u   (0x12C 砍掉高位剩 0x2C)\n", (unsigned)uc);

    unsigned int ubig = 0x12345678u;
    unsigned short ush = (unsigned short)ubig;
    printf("(unsigned short)0x12345678 = 0x%04X\n", (unsigned)ush);

    int neg = -1;
    unsigned char uc2 = (unsigned char)neg;
    printf("(unsigned char)(-1) = %u   (二进制全 1 的低 8 位)\n", (unsigned)uc2);

                /* ---- 5. 有符号 / 无符号的除法与取模也不一样 ---- */
    printf("---- 除法与取模 ----\n");
    printf("-7 / 2 = %d   -7 %% 2 = %d   (C99 起向零取整，商的符号跟着被除数)\n",
           -7 / 2, -7 % 2);
    printf("7 / -2 = %d   7 %% -2 = %d\n", 7 / -2, 7 % -2);
    unsigned int uu = (unsigned int)-7;
    printf("(unsigned)(-7) / 2u = %u   (当成大正数除)\n", uu / 2u);
    return 0;
}
