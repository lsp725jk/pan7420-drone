/**
 * ch02_ex11_literals.c —— 字面量与后缀：类型由写法决定
 */
#include <stdio.h>
#include <stdint.h>

/* 怎么在运行时"看"字面量的类型？ C11 起有 _Generic 可以做编译期的类型分派，但它只在 C 里有，C++ 没有。 为了这份示例在 C 和 C++ 下都能编译，这里改用另一种通俗办法： 同一个值分别赋给各种类型，然后打印 sizeof —— 用"占几个字节"反推类型。 */
#define SHOW_SIZE(expr)  printf("sizeof(" #expr ") = %zu\n", sizeof(expr))

int main(void)
{
                /* ---- 1. 整数字面量的默认类型：用 sizeof 反推 ---- */
    printf("---- 1. 整数没有后缀时是什么类型（用 sizeof 反推）----\n");
    SHOW_SIZE(1);                               /* int        -> 4 */
    SHOW_SIZE(1L);                              /* long       -> 4 (Windows) */
    SHOW_SIZE(1LL);                             /* long long  -> 8 */
    SHOW_SIZE(1U);                              /* unsigned   -> 4 */
    SHOW_SIZE(2147483647);                      /* int        -> 4 */
    SHOW_SIZE(2147483648);                      /* 超出 int，升级到 8 字节 */
    SHOW_SIZE(4294967296);                      /* 也超出 unsigned int，继续升级 */

        /* _Generic 的写法（只在 C11/C17 下可用，C++ 下编译不过）： printf("%s\n", _Generic(1, int: "int", long: "long", default: "其他")); 它的好处是**在编译期**就得出类型名，运行时零开销。 */

                /* ---- 2. 后缀与类型的关系 ---- */
    printf("---- 2. 后缀一看就懂 ----\n");
    printf("1u   -> unsigned int       sizeof=%zu\n", sizeof(1u));
    printf("1l   -> long               sizeof=%zu\n", sizeof(1l));
    printf("1ul  -> unsigned long      sizeof=%zu\n", sizeof(1ul));
    printf("1ll  -> long long          sizeof=%zu\n", sizeof(1ll));
    printf("1ull -> unsigned long long sizeof=%zu\n", sizeof(1ull));
        /* 后缀大小写都行，但小写 l 和数字 1 太像，所以推荐写大写 L： "1l" 到底是 1L 还是 11？读代码的人会愣一下。 */

                /* ---- 3. 进制：同一个数的四种写法 ---- */
    printf("---- 3. 进制 ----\n");
    printf("十进制 123   = %d\n", 123);
    printf("八进制 0173  = %d   (0 开头！0*8^0... 1*64+7*8+3 = 123)\n", 0173);
    printf("十六进制 0x7B = %d\n", 0x7B);
    printf("十六进制 0X7B = %d   (X 大小写都行)\n", 0X7B);
    printf("用于位掩码：0xF0 = %d, 0b 还没有，得写 0xF0 或 (1<<7)|(1<<6)...\n", 0xF0);

                /* ---- 4. 浮点字面量的后缀（最容易踩的坑）---- */
    printf("---- 4. 浮点后缀 ----\n");
    printf("3.14    sizeof = %zu   <-- 默认是 double，不是 float！\n",
           sizeof(3.14));
    printf("3.14f   sizeof = %zu\n", sizeof(3.14f));
    printf("3.14F   sizeof = %zu\n", sizeof(3.14F));
    printf("3.14L   sizeof = %zu\n", sizeof(3.14L));
    printf("1e3     sizeof = %zu   (科学计数法也是 double)\n", sizeof(1e3));
    printf("1.0f    sizeof = %zu\n", sizeof(1.0f));

                /* ---- 5. 字符字面量与转义 ---- */
    printf("---- 5. 字符与转义 ----\n");
    printf("'A'  = %d   字符字面量的类型是 int（不是 char）\n", 'A');
    printf("'A' 的十六进制 = 0x%X\n", 'A');
    printf("'\\n' = %d   '\\t' = %d   '\\0' = %d\n", '\n', '\t', '\0');
    printf("'\\x41' = %c   '\\101' = %c   (十六进制 / 八进制转义)\n", '\x41', '\101');
    printf("多字符字面量 'AB' 的值是实现定义的（不要用）；");
    printf("sizeof('A') = %zu\n", sizeof('A'));

                /* 字符串字面量是"字符数组"，sizeof 会算上结尾的 '\0' */
    printf("sizeof(\"abc\") = %zu   strlen(\"abc\") 要用 string.h 才是 3\n",
           sizeof("abc"));

                /* ---- 6. C23 起的二进制字面量（本机需要 /std:c23）---- */
    printf("---- 6. C23 新写法（本文件用 /std:c17 编译，见正文说明）----\n");
    printf("C23 起可以写 0b1011 == 11，还可以用单引号分隔数字：1'000'000\n");
    printf("C17 下等价写法：0x0B = %d，1000000 = %d\n", 0x0B, 1000000);

                /* ---- 7. 嵌入式常用的掩码字面量 ---- */
    printf("---- 7. 位掩码写法 ----\n");
    uint32_t mask = 1u << 7;                        /* 第 7 位置 1：推荐写法 */
    printf("1u << 7  = 0x%08X   (用 1u 而不是 1，避免有符号移位问题)\n", mask);
    printf("0xFFu    = 0x%08X   (u 后缀：无符号)\n", (unsigned)0xFFu);
    printf("0x1UL    = 0x%08lX  (UL：无符号 long，寄存器操作常用)\n",
           0x1UL);
    return 0;
}
