/**
 * ch03_ex3_bitwise.c —— 位运算：置位、清位、翻转、读位、位段
 */
#include <stdio.h>
#include <stdint.h>

/**
 * 一个小工具：把 8 位二进制打印出来，比看十六进制直观
 */
static void print_bin8(const char *tag, uint8_t v)
{
    printf("%-22s = 0x%02X = 0b", tag, v);
    for (int i = 7; i >= 0; i--) {
        putchar((v & (1u << i)) ? '1' : '0');
    }
    putchar('\n');
}

/* 寄存器布局：位段写法，把一个字节拆成几个字段 */
typedef union {
    uint8_t raw;
    struct {
        uint8_t enable   : 1;                       /* bit0 */
        uint8_t dir      : 1;                       /* bit1 */
        uint8_t speed    : 2;                       /* bit2-3，取值 0..3 */
        uint8_t reserved : 4;                       /* bit4-7 */
    } f;
} ctrl_reg_t;

int main(void)
{
                /* ---- 1. 四种基本操作：置位 / 清位 / 翻转 / 读位 ---- */
    printf("---- 1. 四种基本操作 ----\n");
    uint8_t v = 0x00;

    v |= (1u << 3);                         /* 置位：把第 3 位置 1 */
    print_bin8("置位 v |= (1<<3)", v);

    v |= (1u << 5);                         /* 再置一位 */
    print_bin8("再置位 v |= (1<<5)", v);

    v = (uint8_t)(v & ~(1u << 3));          /* 清位：把第 3 位清 0 */
    print_bin8("清位 v &= ~(1<<3)", v);

    v ^= (1u << 5);                         /* 翻转：第 5 位取反 */
    print_bin8("翻转 v ^= (1<<5)", v);

    v ^= (1u << 5);                         /* 再翻一次，回来了 */
    print_bin8("再翻转回来", v);

    printf("读位 (v>>5)&1 = %u\n", (unsigned)((v >> 5) & 1u));
    printf("判断某位是否为 1：if (v & (1u<<5))  —— 不要写 == 1\n");

                /* ---- 2. 同时操作多位：掩码 ---- */
    printf("---- 2. 掩码（mask）----\n");
    uint8_t m = 0xF0;                       /* 高 4 位掩码 */
    uint8_t data = 0xAB;
    print_bin8("原始 data", data);
    print_bin8("取高 4 位 data & 0xF0", (uint8_t)(data & m));
    print_bin8("取低 4 位 data & 0x0F", (uint8_t)(data & 0x0Fu));
    print_bin8("高 4 位置 1 data | 0xF0", (uint8_t)(data | m));
    print_bin8("高 4 位清 0 data & ~0xF0", (uint8_t)(data & (uint8_t)~m));

                /* 交换高低 4 位：移位 + 或 */
    uint8_t swap = (uint8_t)((data << 4) | (data >> 4));
    print_bin8("高低 4 位互换", swap);

                /* ---- 3. 改寄存器里的一个字段：读-改-写 ---- */
    printf("---- 3. 读-改-写三部曲 ----\n");
    uint8_t reg = 0x00;

                /* 把 bit2-3 这个 2 位字段设成 0b10：先清掉这两位，再写入 */
    uint8_t field_mask = (uint8_t)(0x3u << 2);              /* 0b00001100 */
    reg = (uint8_t)(reg & (uint8_t)~field_mask);            /* 第一步：清 */
    reg = (uint8_t)(reg | (uint8_t)(0x2u << 2));            /* 第二步：写 */
    print_bin8("reg 的 bit2-3 设成 10", reg);

    printf("【为什么不能直接 reg = 0x08;】\n");
    printf("  那样会把其它位一起冲掉。真实寄存器里别的位可能控制着别的东西，\n");
    printf("  必须只改你要改的那几位。\n");

        /* 上面三行可以合成一句（常见写法，但可读性差，初学建议分开写）： reg = (uint8_t)((reg & ~(0x3u << 2)) | (0x2u << 2)); */

                /* ---- 4. 位段（bit-field）：让编译器帮你算移位 ---- */
    printf("---- 4. 位段 ----\n");
    ctrl_reg_t r;
    r.raw = 0;
    r.f.enable = 1;
    r.f.dir = 1;
    r.f.speed = 3;
    print_bin8("位段拼出来的字节", r.raw);
    printf("enable=%u dir=%u speed=%u\n",
           (unsigned)r.f.enable, (unsigned)r.f.dir, (unsigned)r.f.speed);
                /* 位段的好处：可读；坏处：位序和填充由实现定义， 所以在"必须和硬件手册完全一致的寄存器映射"里， 更稳的做法是手写掩码和移位（我自己的项目就用手写宏）。 */
    printf("手工读同一个字段：speed = (raw>>2)&0x3 = %u\n",
           (unsigned)((r.raw >> 2) & 0x3u));

                /* ---- 5. 异或的三个妙用 ---- */
    printf("---- 5. 异或的妙用 ----\n");
    uint8_t a = 0x0F, b = 0x3C;
    print_bin8("a", a);
    print_bin8("b", b);
    a ^= b; b ^= a; a ^= b;                 /* 不用临时变量交换（教学用，别在生产代码里炫技） */
    printf("异或交换后 a=0x%02X b=0x%02X\n", a, b);
    printf("a ^ a = %d   （自己异或自己是 0）\n", 0x5A ^ 0x5A);
    printf("x ^ 0 = x     （异或 0 不变）\n");
    printf("校验常用：把一串字节全异或起来做简单校验和\n");
    uint8_t frame[4] = { 0xA5, 0x01, 0x02, 0x03 };
    uint8_t cs = 0;
    for (int i = 0; i < 4; i++) {
        cs ^= frame[i];
    }
    printf("异或校验和 = 0x%02X\n", cs);

                /* ---- 6. 常见的位操作宏：嵌入式项目里几乎都会有一份 ---- */
    printf("---- 6. 位操作宏 ----\n");
#define BIT(n)        (1u << (n))
#define SET_BIT(r, n) ((r) |= BIT(n))
#define CLR_BIT(r, n) ((r) &= ~BIT(n))
#define TGL_BIT(r, n) ((r) ^= BIT(n))
#define GET_BIT(r, n) (((r) >> (n)) & 1u)
    uint8_t x = 0;
    SET_BIT(x, 1);
    SET_BIT(x, 4);
    print_bin8("SET_BIT(x,1); SET_BIT(x,4)", x);
    printf("GET_BIT(x,1)=%u  GET_BIT(x,2)=%u\n",
           (unsigned)GET_BIT(x, 1), (unsigned)GET_BIT(x, 2));
    TGL_BIT(x, 4);
    CLR_BIT(x, 1);
    print_bin8("TGL_BIT(x,4); CLR_BIT(x,1)", x);
    printf("注意宏参数都要加括号，否则遇到 x<<1 这种实参会算错。\n");
    return 0;
}
