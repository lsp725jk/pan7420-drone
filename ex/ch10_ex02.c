/**
 * ch10_ex02.c —— 位段（bit-field）vs 位运算：同一个需求，两种写法
 */
#include <stdio.h>
#include <stdint.h>

/**
 * ============ 写法 A：位运算版（嵌入式主流，可移植） ============
 */
#define PRESC_POS   0
#define PRESC_MSK   (0x7u << PRESC_POS)
#define EN_POS      3
#define EN_MSK      (0x1u << EN_POS)
#define MODE_POS    4
#define MODE_MSK    (0xFu << MODE_POS)

static uint32_t cfg_a = 0;

static void a_set(uint32_t mask, unsigned pos, uint32_t v)
{
    cfg_a = (cfg_a & ~mask) | ((v << pos) & mask);
}

/* ============ 写法 B：位段版（C 语言原生语法，看着更直观） ============ */
struct ConfigBits {
    uint32_t presc : 3;
    uint32_t en    : 1;
    uint32_t mode  : 4;
    uint32_t rsvd  : 24;
};

int main(void)
{
                /* ---------- 位运算版 ---------- */
    a_set(PRESC_MSK, PRESC_POS, 5);
    a_set(EN_MSK,    EN_POS,    1);
    a_set(MODE_MSK,  MODE_POS,  9);
    printf("位运算版：cfg = 0x%08X ，二进制 ", cfg_a);
    for (int b = 15; b >= 0; b--) putchar((cfg_a >> b) & 1u ? '1' : '0');
    printf("\n");

                /* 只改 mode，其余位不受影响 */
    a_set(MODE_MSK, MODE_POS, 2);
    printf("改 mode=2 后 cfg = 0x%08X\n", cfg_a);

                /* ---------- 位段版 ---------- */
    struct ConfigBits cfg_b;
    cfg_b.presc = 5;
    cfg_b.en    = 1;
    cfg_b.mode  = 9;
    cfg_b.rsvd  = 0;
    printf("\n位段版  ：cfg = 0x%08X ，二进制 ", *(uint32_t *)&cfg_b);
    for (int b = 15; b >= 0; b--) putchar((*(uint32_t *)&cfg_b >> b) & 1u ? '1' : '0');
    printf("\n");

                /* ---------- 位段的问题 1：一点都不"省内存" ---------- */
    struct SmallBits {                      /* 只要 8 位就够了 */
        uint32_t a : 3;
        uint32_t b : 1;
        uint32_t c : 4;
    };
    printf("\n位段的问题：\n");
    printf("  1) struct SmallBits 只需要 3+1+4 = 8 位，"
           "但 sizeof = %zu 字节\n", sizeof(struct SmallBits));
    printf("     —— 位段按「存储单元」分配，装不满也要占一整个单元。\n");
    printf("     想真的省内存，得靠编译器扩展（GCC 的 __attribute__((packed))）\n");
    printf("     或者干脆手动做位运算塞进一个 uint8_t。\n");

                /* ---------- 位段的问题 2：不能取成员地址 ---------- */
    printf("  2) 不能写 &cfg_b.mode —— 位段成员没有独立地址，\n");
    printf("     所以传不进函数、也读不了寄存器（硬件寄存器必须要地址）。\n");

                /* ---------- 位段的问题 3：写入是整体读-改-写，且顺序不可移植 ---------- */
    printf("  3) 位段的赋值不是原子的（编译器生成 ldr/and/orr/str），\n");
    printf("     而且「谁在高位谁在低位」由编译器决定，换编译器就变。\n");

                /* ---------- 位段的问题 4：溢出/回绕行为随实现 ---------- */
    cfg_b.presc = 9;                          /* 3 位装不下 9 */
    printf("  4) 给 3 位位段赋 9，读回来是 %u （本机是把高位截掉）\n", cfg_b.presc);

                /* ---------- 结论 ---------- */
    printf("\n结论：\n");
    printf("  · 操作【硬件寄存器】-> 一律用位运算 + volatile 指针，因为需要地址。\n");
    printf("  · 只想在内存里紧凑存几个小字段（比如协议打包）-> 位段可以，\n");
    printf("    但跨平台协议必须自己做位运算，不能靠位段的布局。\n");
    return 0;
}
