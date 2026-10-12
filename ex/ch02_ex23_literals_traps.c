/**
 * ch02_ex23_literals_traps.c —— 字面量的三个真实坑
 */
#include <stdio.h>
#include <stdint.h>

/**
 * 把 0x0F 和 0b1111 这两种掩码写法都列出来对照
 */
#define MASK_HEX 0x0Fu
#define MASK_OCT 017u           /**
 * 注意：这是八进制！等于十进制 15
 */

int main(void)
{
                /**
         * ---- 1. 前导零 = 八进制 ----
         */
    printf("---- 1. 前导零是八进制 ----\n");
    printf("123      = %d   (十进制)\n", 123);
    printf("0123     = %d   <-- 八进制！1*64+2*8+3\n", 0123);
    printf("010      = %d   <-- 不是 10\n", 010);
    printf("MASK_HEX = 0x%X = %u\n", MASK_HEX, (unsigned)MASK_HEX);
    printf("MASK_OCT = 017  = %u   （八进制的 17 = 十进制的 15）\n",
           (unsigned)MASK_OCT);
    printf("【症状】想把 ID 写成 001、002 好看，结果 008 直接编译错误\n");
    printf("        error C2041: 对\"base 8\"无效的数字\"8\"。\n");
    printf("【正确做法】要补零就存字符串，或者用 %%03d 打印。\n");
    printf("        用 %%03d 打印 8 得到: %03d\n", 8);

                /* ---- 2. 大整数的类型阶梯 ---- */
    printf("---- 2. 没有后缀时大整数会自动升级 ----\n");
    printf("sizeof(2147483647) = %zu   (int 上限，还是 int)\n",
           sizeof(2147483647));
    printf("sizeof(2147483648) = %zu   (超出 int，升到 4 字节的无符号)\n",
           sizeof(2147483648));
    printf("sizeof(4294967295) = %zu\n", sizeof(4294967295));
    printf("sizeof(4294967296) = %zu   (再升到 8 字节)\n", sizeof(4294967296));
    printf("所以同一个字面量在不同平台上可能是不同类型。\n");
    printf("【对策】写常量池时明确加后缀：\n");
    printf("        0xFFFFFFFFu  /  4294967296LL  /  100UL\n");
    printf("  实测：0xFFFFFFFFu = %u, 4294967296LL = %lld\n",
           0xFFFFFFFFu, 4294967296LL);

                /* ---- 3. 浮点字面量默认是 double ---- */
    printf("---- 3. 浮点字面量的后缀 ----\n");
    printf("sizeof(1.0)   = %zu   <-- double\n", sizeof(1.0));
    printf("sizeof(1.0f)  = %zu   <-- float\n", sizeof(1.0f));
    printf("sizeof(1.0L)  = %zu   <-- 本机 long double == double\n", sizeof(1.0L));
    float f1 = 1.1f;                    /* 正确：直接是 float */
#pragma warning(push)
#pragma warning(disable : 4305)  /* 故意演示 double→float 的截断警告 */
    float f2 = 1.1;                     /* 这里本来会有 warning C4305/C4244 */
#pragma warning(pop)
    printf("float 存 1.1f = %.9g\n", (double)f1);
    printf("float 存 1.1  = %.9g   （值一样，但多了一次 double→float 转换）\n",
           (double)f2);
    printf("【症状】warning C4305: 从\"double\"到\"float\"截断\n");
    printf("        或 warning C4244: 从\"double\"转换到\"float\"，可能丢失数据。\n");
    printf("【为什么在意】在软浮点的单片机上，多一次转换就是多几十个周期。\n");

                /* ---- 4. 十六进制与位掩码的正确姿势 ---- */
    printf("---- 4. 掩码写法对照 ----\n");
    printf("0xF0       = 0x%02X   (十进制 %u)\n", 0xF0, 240u);
    printf("1u << 4    = 0x%02X   (更清楚地表达\"第 4 位\")\n", 1u << 4);
    printf("(1u<<7)|(1u<<6) = 0x%02X   (一次表达两位)\n", (1u << 7) | (1u << 6));
    printf("0b11000000 （C23 起：二进制字面量，本机 /std:c17 下也接受）\n");
    printf("  C17 等价写法：0xC0 = 0x%02X\n", 0xC0);
    printf("【建议】寄存器掩码用 (1u<<n) 或 0xXX 写，别用十进制。\n");
    return 0;
}
