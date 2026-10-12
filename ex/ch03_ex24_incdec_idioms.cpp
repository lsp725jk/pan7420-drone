/**
 * ch03_ex24_incdec_idioms.c —— 自增自减的四种惯用法与它们各自的边界
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>

int main(void)
{
    printf("---- 1. a[i++] ：写缓冲区最常用的手法 ----\n");
    char buf[8];
    int n = 0;
    const char *src = "hi";
    buf[n++] = 'A';
    buf[n++] = 'B';
    printf("  两次 buf[n++] 之后：buf[0]=%c buf[1]=%c, n=%d\n", buf[0], buf[1], n);
                /* 更紧凑的经典写法：把复制和判断合在一起 */
    n = 0;
    while ((buf[n] = src[n]) != '\0') {
        n++;
    }
    printf("  复制 \"%s\" 之后 buf = \"%s\"（含结尾的 '\\0'），n=%d\n", src, buf, n);

    printf("---- 2. while (n--) 的边界陷阱 ----\n");
    int m = 3;
    int loops = 0;
    while (m--) {                           /* m 从 3 数到 0，循环 3 次 */
        loops++;
    }
    printf("  int m = 3; while (m--) 循环了 %d 次，m 最终 = %d\n", loops, m);

                /* 用无符号时要注意：初值 0 会绕成最大值 */
    uint32_t u = 3;
    uint32_t u_loops = 0;
    while (u--) {                           /* 3 次；但如果 u 初值是 0，就会循环 4294967296 次 */
        u_loops++;
        if (u_loops > 10) { break; }                        /* 保险 */
    }
    printf("  uint32_t u = 3; while (u--) 循环了 %u 次，u 最终 = %u\n",
           (unsigned)u_loops, (unsigned)u);
    printf("  【危险】如果 u 初值是 0，while (u--) 会先判断 0（假）但 u 已经变成 4294967295，\n");
    printf("          所以「循环体不执行」这件事本身是对的，但 u 的值被改坏了。\n");
    printf("          更安全的写法：while (u-- > 0) 或者 while (u != 0) { ...; u--; }\n");

    printf("---- 3. 前缀与后缀：在只是推进时完全等价 ----\n");
    int a1 = 0, a2 = 0, sum1 = 0, sum2 = 0;
    for (int i = 0; i < 5; i++)  { a1++; sum1 += i; }
    for (int i = 0; i < 5; ++i)  { ++a2; sum2 += i; }
    printf("  i++  版本：a1=%d sum1=%d\n", a1, sum1);
    printf("  ++i  版本：a2=%d sum2=%d\n", a2, sum2);
    printf("  结果完全一致 —— 循环的第三段用哪个都行。\n");

    printf("---- 4. 指针遍历的两种写法 ----\n");
    const char *s = "abcd";
    int c1 = 0;
    for (const char *p = s; *p != '\0'; p++) { c1++; }
    int c2 = 0;
    for (const char *p = s; *p; ) { c2++; p++; }
    printf("  写法一（p++ 在 for 第三段）: %d\n", c1);
    printf("  写法二（p++ 在循环体里）    : %d\n", c2);
    printf("  【实践建议】把推进语句放 for 的第三段 —— 这样 continue 不会跳过它（见 04.3）。\n");

    printf("---- 5. 该避免的三种写法（都是 UB 或不可读）----\n");
    printf("  a) i = i++;            -> UB（同一表达式里既读又改）\n");
    printf("  b) printf(\"%%d\", i++, i++); -> 实参求值顺序未指定\n");
    printf("  c) sum += (*p)++;      -> 指针不动，反复读同一个元素（见 03.7 的实测）\n");
    printf("  替代方案：拆成多条语句，或者把「读」和「改」分到不同的表达式里。\n");
    return 0;
}
