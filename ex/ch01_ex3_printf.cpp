/**
 * ch01_ex3_printf.c —— printf 的格式化：宽度、精度、转换说明、长度修饰
 */
#include <stdio.h>
#include <stddef.h>

int main(void)
{
                /**
         * ---- 1. 整数的宽度与对齐：同一批数据，五种写法 ----
         */
    printf("---- 整数宽度对齐 ----\n");
    printf("[%d] [%5d] [%-5d] [%05d] [%+d]\n", 42, 42, 42, 42, 42);
    printf("[%d] [%5d] [%-5d] [%05d] [%+d]\n", -42, -42, -42, -42, -42);

                /* ---- 2. 进制：十进制 / 十六进制 / 八进制 ---- */
    printf("---- 进制 ----\n");
    printf("255 -> %%d=%d  %%x=%x  %%X=%X  %%o=%o\n", 255, 255, 255, 255);
    printf("带前缀：%%#x=%#x  %%#o=%#o\n", 255, 255);

                /* ---- 3. 字符与字符串 ---- */
    printf("---- 字符与字符串 ----\n");
    printf("%%c='%c'  %%s=\"%s\"  %%8s=[%8s]  %%-8s=[%-8s]|\n",
           'A', "abc", "abc", "abc");
    printf("只取前 2 个字符 %%.2s=%.2s\n", "abcdef");

                /* ---- 4. 浮点：默认 6 位小数、精度控制、科学计数法、自动选择 ---- */
    printf("---- 浮点 ----\n");
    printf("默认   %%f     = %f\n", 3.14159265358979);
    printf("两位   %%.2f   = %.2f\n", 3.14159265358979);
    printf("十位   %%.10f  = %.10f\n", 3.14159265358979);
    printf("科学   %%e     = %e\n", 3.14159265358979);
    printf("自动   %%g     = %g\n", 3.14159265358979);
    printf("宽度+精度      = [%10.3f]\n", 3.14159265358979);

                /* ---- 5. 指针与百分号本身 ---- W4 下 cl 会提示 %p 要配 void*，所以这里显式转换一下。 */
    int n = 7;
    printf("---- 指针 / 百分号 ----\n");
    printf("n 的地址 %%p = %p\n", (void *)&n);
    printf("百分号本身要写两个：完成度 100%%\n");

        /* ---- 6. 长度修饰：告诉 printf "这个实参有多宽" ---- %ld 配 long      %lld 配 long long %zu 配 size_t    %hu 配 unsigned short %hhd 配 signed char（按十进制打印） */
    printf("---- 长度修饰 ----\n");
    printf("long long %%lld   = %lld\n", -1234567890123LL);
    printf("unsigned %%llu     = %llu\n", 18446744073709551615ULL);
    printf("size_t    %%zu     = %zu\n", sizeof(double));
    printf("short     %%hu     = %hu\n", (unsigned short)65535);
    printf("signed ch %%hhd    = %hhd\n", (signed char)-5);
    printf("long      %%ld     = %ld  (sizeof(long)=%zu)\n",
           1234567L, sizeof(long));
    return 0;
}
