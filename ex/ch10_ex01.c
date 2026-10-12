/**
 * ch10_ex01.c —— 寄存器编程：为什么必须用 |= 而不是 =
 */
#include <stdio.h>
#include <stdint.h>

/**
 * 模拟一个外设寄存器。真机写法大致是：
 */
static uint32_t REG = 0;

static void dump(const char *what)
{
    printf("%-22s REG = 0x%08X = ", what, REG);
    for (int b = 31; b >= 0; b--) putchar((REG >> b) & 1u ? '1' : '0');
    putchar('\n');
}

int main(void)
{
                /* 先假设寄存器里已经有别的外设配置好了（低 8 位全是 1） */
    REG = 0x000000FFu;
    dump("初始状态");

                /* ---------- 错：整字赋值 ---------- */
    REG = (1u << 3);                         /* 只想点亮第 3 位，结果把其它位全清零了 */
    dump("REG = (1u<<3)");

                /* ---------- 对：|= 只置位，不动别人 ---------- */
    REG = 0x000000FFu;
    REG |= (1u << 3);                        /* 读-改-写：只把第 3 位置 1 */
    dump("REG |= (1u<<3)");

                /* ---------- 清位：&= ~掩码 ---------- */
    REG = 0x000000FFu;
    REG &= ~(1u << 3);                       /* 只把第 3 位清 0 */
    dump("REG &= ~(1u<<3)");

                /* ---------- 翻位：^= ---------- */
    REG = 0x000000FFu;
    REG ^= (1u << 3);                        /* 第 3 位取反 */
    dump("REG ^= (1u<<3)");

                /* ---------- 一次改多位：先清掩码、再或新值 ---------- */
    REG = 0x000000FFu;
    const uint32_t MASK = 0x7u << 4;                 /* 第 4~6 位 */
    REG = (REG & ~MASK) | ((5u << 4) & MASK);        /* 把那 3 位设成 101 */
    dump("设置第4~6位 = 101");

                /* ---------- 变式：用宏把上面三件事封装起来（嵌入式标准做法） ---------- */
    #define REG_SET(reg, mask)      ((reg) |=  (mask))
    #define REG_CLR(reg, mask)      ((reg) &= ~(mask))
    #define REG_TOG(reg, mask)      ((reg) ^=  (mask))
    #define REG_GET(reg, mask)      ((reg) &   (mask))
    #define REG_PUT(reg, mask, v)   ((reg) = ((reg) & ~(mask)) | ((v) & (mask)))

    REG = 0x00000000u;
    REG_SET(REG, 1u << 5);
    REG_SET(REG, 1u << 9);
    dump("宏 SET 第5、9位");
    REG_PUT(REG, 0x7u << 12, 3u << 12);
    dump("宏 PUT 第12~14位=011");
    REG_CLR(REG, 1u << 5);
    dump("宏 CLR 第5位");
    printf("REG_GET(REG, 1u<<9) = 0x%X （非 0 即「该位为 1」）\n", REG_GET(REG, 1u << 9));
    return 0;
}
